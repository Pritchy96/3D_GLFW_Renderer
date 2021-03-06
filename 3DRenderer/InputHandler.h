#pragma once
#include <GL/glew.h> //Makes sure we're using latest GL.
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class InputHandler
{
public:
	static void update(GLFWwindow *window);
	static glm::mat4 getViewMatrix();
	static glm::mat4 getProjectionMatrix();
	static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void setup(GLFWwindow *window);
};

