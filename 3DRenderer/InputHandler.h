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
	static int getCurrentLOD();
	static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void setup(GLFWwindow *window, int maxLOD);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void IncreaseLOD();
	static void DecreaseLOD();
};

