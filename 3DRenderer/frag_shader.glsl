#version 330 core

// Interpolated values inputted from the vertex shaders
in vec3 fragment_colour;

//Var to output in main
out vec3 colour;

void main(){

	// Output colour = colour specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
	colour = fragment_colour;
}