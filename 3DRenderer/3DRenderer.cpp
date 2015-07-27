// 3DRenderer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"	//Must be at top.
#include <GL/glew.h> //Makes sure we're using latest GL.
#include <GLFW/glfw3.h>
#include "Logger.h"
#include <iostream>
#include <vector>

using namespace std;


// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
double rotate_y = 0;
double rotate_x = 0;
double rotate_z = 0;
double scale = 0.01;

//Window size
int g_gl_width = 640;
int g_gl_height = 480;

//Prototypes
void log_gl_params(Logger logger);


static void error_callback(int error, const char* description)
{
	fputs(description, stderr);	//Error logging stuff
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
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
		Logger logger;

		logger.log("Log Start:", true, true);

		if (!glfwInit()){
			logger.log("ERROR: could not initiate GLFW", false, true);
			return 1;
		}

		GLFWwindow* window = glfwCreateWindow(640, 480, "Renderer", NULL, NULL);

		if (!window) {
			logger.log("ERROR: could not open window using GLFW", false, true);
			glfwTerminate();
			return 1;
		}

		//linking callbacks
		glfwSetErrorCallback(error_callback);
		glfwSetWindowSizeCallback(window, window_size_callback);


		glfwMakeContextCurrent(window);

		//Start GLEW extension handler.
		glewExperimental = GL_TRUE;
		glewInit();

		// get and log version info
		string renderer = (const char*)glGetString(GL_RENDERER);
		string version = (const char*)glGetString(GL_VERSION);
		logger.log(("Renderer: %s", renderer), false, true);
		logger.log(("OpenGL version supported %s", version), false, true);

		//NOT WORKING
		//log_gl_params(logger);

		//Enable GL Depth tests.
		glEnable(GL_DEPTH_TEST); // enable depth-testing
		glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

		//Anti Aliasing
		glfwWindowHint(GLFW_SAMPLES, 4);


		vector<float> points = {
			0.0f, 0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f
		};

#pragma region Setting up VAO

		//Create a vertex buffer object (VBO), which basically moves the array[] to the GPU Memory
		GLuint vbo = 0;
		glGenBuffers(1, &vbo);
		//Bind VBO, makes it the current buffer in OpenGL's state machine
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//Tells GL that the GL_ARRAY_BUFFER is the size of the vector * the size of a float, and "gives it the address of the first value"
		glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

		//vertex attribute object (VAO) remembers all of the vertex buffers (VBO's) that you want to use, and the memory layout of each one. 
		//Integer to indentify VAO with later.
		GLuint vao = 0;
		//"bind it, to bring it in to focus in the state machine."
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		//Enable the first attribute, 0. 
		glEnableVertexAttribArray(0);
		//Bind GL state to the VBO
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//Defining layout of our first VBO within the VAO: ""0" means define the layout for attribute number 0. "3" means that the variables are vec3 made from every 3 floats (GL_FLOAT) in the buffer."
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
#pragma endregion

#pragma region Shader
		const char* vertex_shader =
			"#version 400\n"
			"in vec3 vp;"
			"void main () {"
			"  gl_Position = vec4 (vp, 1.0);"
			"}";
		const char* fragment_shader =
			"#version 400\n"
			"out vec4 frag_colour;"
			"void main () {"
			"  frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
			"}";
		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vertex_shader, NULL);
		glCompileShader(vs);
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fragment_shader, NULL);
		glCompileShader(fs);

		GLuint shader_programme = glCreateProgram();
		glAttachShader(shader_programme, fs);
		glAttachShader(shader_programme, vs);
		glLinkProgram(shader_programme);
#pragma endregion

		//Set colour that glClear sets screen to.
		glClearColor(0.6f, 0.7f, 0.6f, 1.0f);
		glViewport(0, 0, g_gl_width, g_gl_height);
		//Wireframe
		//glPolygonMode(GL_FRONT, GL_LINE);

		//Set point size used for GL_POINTS rendering mode
		glPointSize(2);

		while (!glfwWindowShouldClose(window))
		{
			//FPS Counter
			update_fps_counter(window);
			// wipe the drawing surface clear
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, g_gl_height, g_gl_height);
			glUseProgram(shader_programme);
			glBindVertexArray(vao);
			// draw all the array in the array (remember 3 entries in the array make up one point, XYZ, so divide size by 3), from the currently bound VAO with current in-use shader
			glDrawArrays(GL_LINE_LOOP, 0, points.size() / 3);
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


void log_gl_params(Logger logger) {
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

	logger.log(("GL Context Params:\n"), false, true);

	char msg[256];
	// integers - only works if the order is 0-10 integer return types
	for (int i = 0; i < 10; i++) {
		int v = 0;
		glGetIntegerv(params[i], &v);
		logger.log(("%s %i\n", names[i], (char*)v), false, true);
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