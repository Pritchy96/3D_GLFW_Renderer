#pragma once
#include "stdafx.h"
#include "LoadShader.h"
#include <GL/glew.h> //Makes sure we're using latest GL.
#include <GLFW/glfw3.h>
#include "Logger.h"
#include <iostream>

class LoadShader
{
public:
	LoadShader();
	~LoadShader();
	static 	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
};

