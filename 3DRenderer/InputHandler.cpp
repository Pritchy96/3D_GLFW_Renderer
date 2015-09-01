#include "stdafx.h"
#include "InputHandler.h"
#include <GL/glew.h> //Makes sure we're using latest GL.
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Logger.h"

// Initial position : on +Z
glm::vec3 position = glm::vec3(0, 0, 15),
defaultPosition = glm::vec3(0);	//Default position used when resetting camera.
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Field of View
float FoV = 45.0f;
double xInitial, yInitial;
float speed = 5; // 30 units / second
float mouseSpeed = 0.005f;
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
bool renderAsSphere = true;

glm::mat4 InputHandler::getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 InputHandler::getProjectionMatrix(){
	return ProjectionMatrix;
}

bool InputHandler::getRenderAsSphere() {
	return renderAsSphere;
}

void InputHandler::setup(GLFWwindow *window)
{
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	defaultPosition = position;
}



void InputHandler::update(GLFWwindow *window){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	double xpos, ypos;

	//Get mouse state for left click
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	//While left click is pressed..
	if (state == GLFW_PRESS)
	{
		// Get mouse position for this tick.
		glfwGetCursorPos(window, &xpos, &ypos);

		// Compute new orientation: difference between last position and current positon 
		//Multiplied by a coefficient to make it a nicer speed.
		horizontalAngle +=  mouseSpeed * float(xInitial - xpos);
		verticalAngle +=  mouseSpeed * float(yInitial - ypos);

		//Set last x and y pos to current x and y pos for next update/tick.
		xInitial = xpos;
		yInitial = ypos;
	}
	

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
		);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 10000.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
		);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

void InputHandler::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//Set initial position of mouse to current mouse pos.
		//This is so we can find out how far we've dragged on the next tick.
		glfwGetCursorPos(window, &xInitial, &yInitial);
	}
		
}

void InputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case(GLFW_KEY_ESCAPE) :
			exit(0);
			break;

		case(GLFW_KEY_V) :

			GLint  mode[2];
			glGetIntegerv(GL_POLYGON_MODE, mode);
			if (mode[0] == GL_LINE)
			{
				//Sets up wirefram by setting the FRONT facing faces to GL_LINE.
				glPolygonMode(GL_FRONT, GL_FILL);
			}
			else
			{
				//Sets up wirefram by setting the FRONT facing faces to GL_LINE.
				glPolygonMode(GL_FRONT, GL_LINE);
			}
			break;

		case(GLFW_KEY_TAB) :
			//Flip render as sphere bool.
			renderAsSphere = !renderAsSphere;
			break;


		case(GLFW_KEY_SPACE) :
			//Reset Camera rotation and position to initial vals.
			position = defaultPosition;
			horizontalAngle = 3.14f;
			verticalAngle = 0.0f;
			break;

		default:
			break;
		}
	}
}

void InputHandler::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//Normal mouse just has yoffset.
	Logger::log(to_string(yoffset), false, true);
	FoV -= yoffset/10;
}