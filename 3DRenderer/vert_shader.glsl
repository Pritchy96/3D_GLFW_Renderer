#version 330 core

// Input vertex data, notice it's from "location 0" of the VBO
layout(location = 0) in vec3 vertexPosition_modelspace;
//Input vertex colour, Location 1 of VBO as set in VBO init code.
layout(location = 1) in vec3 vertex_colour;

// Output data to next stage of the gfx pipeline (the fragment shader).
out vec3 fragment_colour;
// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){	

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	// The color of each vertex will be interpolated
	// to produce the color of each fragment
	fragment_colour = vertex_colour;
}

