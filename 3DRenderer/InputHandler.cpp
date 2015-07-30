#include "stdafx.h"
#include "InputHandler.h"
#include <GL/glew.h> //Makes sure we're using latest GL.
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>


glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 InputHandler::getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 InputHandler::getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3(0, 0, 5);
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;
int xmax, ymax;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

void InputHandler::setup(GLFWwindow *window)
{
	glfwSetKeyCallback(window, key_callback);


}

void InputHandler::update(GLFWwindow *window){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	//Get window size.
	glfwGetWindowSize(window, &xmax, &ymax);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, xmax / 2, ymax / 2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(xmax / 2 - xpos);
	verticalAngle += mouseSpeed * float(ymax / 2 - ypos);

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

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
		);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}


void InputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	{
		switch (key)
		{
		case(GLFW_KEY_ESCAPE) :
			exit(0);
			break;

		case(GLFW_KEY_V) :
			if (action == GLFW_PRESS)
			{
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
			}
			break;

		default:
			break;
		}
	}
}