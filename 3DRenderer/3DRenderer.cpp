// 3DRenderer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"	//Must be at top.
#include <GL/glew.h> //Makes sure we're using latest GL.
#include <GLFW/glfw3.h>
#include "Logger.h"
#include <iostream>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "LoadShader.h"
#include "InputHandler.h"
#include "Island_Utils.h"
#include "Util.h"
using namespace std;


// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
double rotate_y = 0;
double rotate_x = 0;
double rotate_z = 0;
double scale = 0.01;

//Window size
int g_gl_width = 1024;
int g_gl_height = 768;

//Prototypes
void log_gl_params();



static void error_callback(int error, const char* description)
{
	fputs(description, stderr);	//Error logging stuff
}
//Window size change callback.
static void window_size_callback(GLFWwindow* window, int width, int height)
{
	g_gl_height = height;
	g_gl_width = width;

	//Update perspective matrices here
}


void update_fps_counter(GLFWwindow* window) {
	//Get time.
	static double previous_seconds = glfwGetTime();
	static int frame_count;
	//Get time again.
	double current_seconds = glfwGetTime();
	//Get time difference.
	double elapsed_seconds = current_seconds - previous_seconds;

	//If it's more than a quarter of a second, update fps. 
	//This is to stop it jumping around too much by averaging the fps over 0.25 of a second.
	if (elapsed_seconds > 0.25) {
		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];
		//Write formatted data to tmp string.
		sprintf_s(tmp, "opengl renderer @ fps: %.2f", fps);
		//Set window title to string.
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}
	frame_count++;
}

int main(int argc, char* argv[]){
	{

		//Logging
		Logger::log("Log Start:", true, true);

		if (!glfwInit()){
			Logger::log("ERROR: could not initiate GLFW", false, true);
			return 1;
		}

		GLFWwindow* window = glfwCreateWindow(g_gl_width, g_gl_height, "Renderer", NULL, NULL);

		if (!window) {
			Logger::log("ERROR: could not open window using GLFW", false, true);
			glfwTerminate();
			return 1;
		}

		//linking callbacks
		glfwSetErrorCallback(error_callback);
		glfwSetWindowSizeCallback(window, window_size_callback);

		//Setup input handler callbacks.
		InputHandler::setup(window);

		glfwMakeContextCurrent(window);

		//Start GLEW extension handler.
		glewExperimental = GL_TRUE;
		glewInit();

		// get and log version info
		string renderer = (const char*)glGetString(GL_RENDERER);
		string version = (const char*)glGetString(GL_VERSION);
		Logger::log(("Renderer: %s", renderer), false, true);
		Logger::log(("OpenGL version supported %s", version), false, true);

		//NOT WORKING
		//log_gl_params();

		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		glm::mat4 Projection = glm::perspective(45.0f, ((float)g_gl_width) / ((float)g_gl_height), 0.1f, 100.0f);
		// Camera matrix
		glm::mat4 View = glm::lookAt(
			glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);

		//Enable GL Depth tests.
		glEnable(GL_DEPTH_TEST); // enable depth-testing
		glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"


		//Anti Aliasing
		glfwWindowHint(GLFW_SAMPLES, 4);

#pragma region reading island array
		vector<vector<int>> islandVector = Island_Utils::MakeIsland();

		int arraySize = islandVector.size()*islandVector[0].size();
		vector<float> position_array, colour_array;
		position_array.resize(arraySize * 12);
		colour_array.resize(arraySize * 12);

		for (int y = 0; y < islandVector[0].size(); y++)
		{
			for (int x = 0; x < islandVector.size(); x++)
			{
				int arrayPosition = Util::To1DArray(x, y, islandVector.size()) * 12;

				//vector<int> colour = Island_Utils::GetBiomeColour();

				//Top Left point
				position_array[arrayPosition] = x*scale;
				position_array[arrayPosition + 1] = y*scale;
				position_array[arrayPosition + 2] = (islandVector[x][y])*(scale/10);

				colour_array[arrayPosition] = 1;
				colour_array[arrayPosition + 1] = 2;
				colour_array[arrayPosition + 2] = 3;
				
				//Top Right Point.
				position_array[arrayPosition + 3] = (x + 1.0f)*scale;
				position_array[arrayPosition + 4] = y*scale;

				if (x + 1 < islandVector.size())	
				{
					position_array[arrayPosition + 5] = (islandVector[x + 1][y])*(scale / 10);
				}
				else
				{
					//If the next value in the position_array doesn't exist, just make it the same height as the top left point.
					position_array[arrayPosition + 5] = (islandVector[x][y])*(scale / 10);
				}

				//Bottom Right Point
				position_array[arrayPosition + 6] = (x + 1.0f)*scale;
				position_array[arrayPosition + 7] = (y + 1.0f)*scale;

				if (y + 1 < islandVector[0].size() && x + 1 < islandVector.size())
				{
					position_array[arrayPosition + 8] = (islandVector[x + 1][y + 1])*(scale / 10);
				}
				else
				{
					position_array[arrayPosition + 8] = (islandVector[x][y])*(scale / 10);
				}
				
				
				//Bottom Left Point
				position_array[arrayPosition + 9] = x*scale;
				position_array[arrayPosition + 10] = (y + 1.0f)*scale;

				if (y + 1 < islandVector[0].size())
				{
					position_array[arrayPosition + 11] = (islandVector[x][y + 1])*(scale / 10);
				}
				else
				{
					position_array[arrayPosition + 11] = (islandVector[x][y])*(scale / 10);
				}	



			}
		}

#pragma endregion

#pragma region Setting up VAO

		//Create a vertex buffer object (VBO), which basically moves the position_array[] to the GPU Memory
		GLuint position_vbo = 0;
		glGenBuffers(1, &position_vbo);
		//Bind VBO, makes it the current buffer in OpenGL's state machine
		glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
		//Tells GL that the GL_ARRAY_BUFFER is the size of the vector * the size of a float, and "gives it the address of the first value"
		glBufferData(GL_ARRAY_BUFFER, position_array.size() * sizeof(float), position_array.data(), GL_STATIC_DRAW);

		//Create a vertex buffer object (VBO), which basically moves the array[] to the GPU Memory
		GLuint colour_vbo = 0;
		glGenBuffers(1, &colour_vbo);
		//Bind VBO, makes it the current buffer in OpenGL's state machine
		glBindBuffer(GL_ARRAY_BUFFER, colour_vbo);
		//Tells GL that the GL_ARRAY_BUFFER is the size of the vector * the size of a float, and "gives it the address of the first value"
		//NOTE: Here we are using the same data for position as well as colour! This is not normally the case!
		glBufferData(GL_ARRAY_BUFFER, position_array.size() * sizeof(float), position_array.data(), GL_STATIC_DRAW);


		//vertex attribute object (VAO) remembers all of the vertex buffers (VBO's) that you want to use, and the memory layout of each one. 
		//Integer to indentify VAO with later.
		GLuint vao = 0;
		//"bind it, to bring it in to focus in the state machine."
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//Enable the first attribute, 0, Position.
		glEnableVertexAttribArray(0);
		//Bind GL state to the VBO
		glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
		//Defining layout of our first VBO within the VAO: ""0" means define the layout for attribute number 0. "3" means that the variables are vec3 made from every 3 floats (GL_FLOAT) in the buffer."
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		//Enable the Second attribute, 1, Colour.
		glEnableVertexAttribArray(1);
		//Bind GL state to the VBO
		glBindBuffer(GL_ARRAY_BUFFER, colour_vbo);
		//Defining layout of our first VBO within the VAO: ""1" means define the layout for attribute number 1. "3" means that the variables are vec3 made from every 3 floats (GL_FLOAT) in the buffer."
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

#pragma endregion

#pragma region Shader



		GLuint shader_programme = LoadShader::LoadShaders("vert_shader.glsl", "frag_shader.glsl");
		glLinkProgram(shader_programme);
#pragma endregion


		// Get a handle for our "MVP" uniform.
		// Only at initialisation time.
		GLuint MatrixID = glGetUniformLocation(shader_programme, "MVP");
		


		//Set colour that glClear sets screen to.
		glClearColor(0.6f, 0.7f, 0.6f, 1.0f);
		glViewport(0, 0, g_gl_width, g_gl_height);

		// Cull triangles which normal is not towards the camera
		glEnable(GL_CULL_FACE);

		//Set point size used for GL_POINTS rendering mode
		glPointSize(2);



		while (!glfwWindowShouldClose(window))
		{

			// Model matrix : an identity matrix (model will be at the origin)
			//Just giving mat4 one param will make it an identity matrix
			//This means that the coords (of the model) will be zero, as they are the top four rows of the last column.

			// Compute the MVP matrix from keyboard and mouse input
			InputHandler::update(window);
			glm::mat4 ProjectionMatrix = InputHandler::getProjectionMatrix();
			glm::mat4 ViewMatrix = InputHandler::getViewMatrix();
			glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

			// Send our transformation to the currently bound shader,
			// in the "MVP" uniform
			// For each model you render, since the MVP will be different (at least the M part)
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

			//FPS Counter
			update_fps_counter(window);
			// wipe the drawing surface clear
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, g_gl_width, g_gl_height);
			glUseProgram(shader_programme);
			glBindVertexArray(vao);
			// draw all the array in the array (remember 3 entries in the array make up one point, XYZ, so divide size by 3), from the currently bound VAO with current in-use shader
			glDrawArrays(GL_QUADS, 0, position_array.size() / 3);
			//Update other events like input handling 
			glfwPollEvents();
			//Put the stuff we've been drawing onto the display
			glfwSwapBuffers(window);
		}

		// close GL context and any other GLFW resources
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(EXIT_SUCCESS);
	}
}


void log_gl_params() {
	GLenum params[] = {
		GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		GL_MAX_DRAW_BUFFERS,
		GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
		GL_MAX_TEXTURE_IMAGE_UNITS,
		GL_MAX_TEXTURE_SIZE,
		GL_MAX_VARYING_FLOATS,
		GL_MAX_VERTEX_ATTRIBS,
		GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
		GL_MAX_VERTEX_UNIFORM_COMPONENTS,
		GL_MAX_VIEWPORT_DIMS,
		GL_STEREO,
	};
	const char* names[] = {
		"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
		"GL_MAX_CUBE_MAP_TEXTURE_SIZE",
		"GL_MAX_DRAW_BUFFERS",
		"GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
		"GL_MAX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_TEXTURE_SIZE",
		"GL_MAX_VARYING_FLOATS",
		"GL_MAX_VERTEX_ATTRIBS",
		"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_VERTEX_UNIFORM_COMPONENTS",
		"GL_MAX_VIEWPORT_DIMS",
		"GL_STEREO",
	};

	Logger::log(("GL Context Params:\n"), false, true);

	char msg[256];
	// integers - only works if the order is 0-10 integer return types
	for (int i = 0; i < 10; i++) {
		int v = 0;
		glGetIntegerv(params[i], &v);
		Logger::log(("%s %i\n", names[i], (char*)v), false, true);
	}
	/*
	// others
	int v[2];
	v[0] = v[1] = 0;
	glGetIntegerv(params[10], v);
	gl_log("%s %i %i\n", names[10], v[0], v[1]);
	unsigned char s = 0;
	glGetBooleanv(params[11], &s);
	gl_log(m"%s %u\n", names[11], (unsigned int)s);
	gl_log("-----------------------------\n");
	*/
}