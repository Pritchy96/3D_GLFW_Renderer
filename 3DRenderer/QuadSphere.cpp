#include "stdafx.h"
#include "QuadSphere.h"
#include "QuadTree.h"
#include <vector>

//Default constructor (needed for vector<QuadSphere>)
QuadSphere::QuadSphere()
{
}

QuadSphere::QuadSphere(float size, int MaxLOD)
{
	maxLOD = MaxLOD;
	currentLOD = MaxLOD;
	//Positioned at -1 so it is between -1 and 1 (cubicSphere algorithm only works for that range)
	//Back
	faces.push_back(QuadTree(glm::vec3(-1, -1, size), glm::vec3(size, -1, size), glm::vec3(size, size, size), glm::vec3(-1, size, size), MaxLOD));
	////Front
	faces.push_back(QuadTree(glm::vec3(-1, size, -1), glm::vec3(size, size, -1), glm::vec3(size, -1, -1), glm::vec3(-1, -1, -1), MaxLOD));
	////Top
	faces.push_back(QuadTree(glm::vec3(size, size, -1), glm::vec3(-1, size, -1), glm::vec3(-1, size, size), glm::vec3(size, size, size), MaxLOD));
	////Bottom
	faces.push_back(QuadTree(glm::vec3(-1, -1, -1), glm::vec3(size, -1, -1), glm::vec3(size, -1, size), glm::vec3(-1, -1, size), MaxLOD));
	////Right
	faces.push_back(QuadTree(glm::vec3(size, -1, -1), glm::vec3(size, size, -1), glm::vec3(size, size, size), glm::vec3(size, -1, size), MaxLOD));
	////Left
	faces.push_back(QuadTree(glm::vec3(-1, -1, size), glm::vec3(-1, size, size), glm::vec3(-1, size, -1), glm::vec3(-1, -1, -1), MaxLOD));
}

int QuadSphere::GetMaxLOD()
{
	return maxLOD;
}

void QuadSphere::SetCurrentLOD(int value)
{
	currentLOD = value;
}

int QuadSphere::GetCurrentLOD()
{
	return currentLOD;
}

vector<float> QuadSphere::GetFaceVerts()
{
	vector<float> verts;

	for (QuadTree face : faces)
	{
		verts = face.GetVerts(currentLOD, verts);
	}
	return verts;
}


vector<float> QuadSphere::ConvertToSphere()
{
	vector<float> sphereFloats;

	for (QuadTree tree : faces)
	{
		vector<float> verts = tree.GetVerts(currentLOD, verts);

		for (int i = 0; i < verts.size() / 3; i++)
		{
			float dx = verts[(i * 3)] * sqrtf(1.0 - (verts[(i * 3) + 1] * verts[(i * 3) + 1] / 2.0) - (verts[(i * 3) + 2] * verts[(i * 3) + 2] / 2.0) + (verts[(i * 3) + 1] * verts[(i * 3) + 1] * verts[(i * 3) + 2] * verts[(i * 3) + 2] / 3.0));
			float dy = verts[(i * 3) + 1] * sqrtf(1.0 - (verts[(i * 3) + 2] * verts[(i * 3) + 2] / 2.0) - (verts[(i * 3)] * verts[(i * 3)] / 2.0) + (verts[(i * 3) + 2] * verts[(i * 3) + 2] * verts[(i * 3)] * verts[(i * 3)] / 3.0));
			float dz = verts[(i * 3) + 2] * sqrtf(1.0 - (verts[(i * 3)] * verts[(i * 3)] / 2.0) - (verts[(i * 3) + 1] * verts[(i * 3) + 1] / 2.0) + (verts[(i * 3)] * verts[(i * 3)] * verts[(i * 3) + 1] * verts[(i * 3) + 1] / 3.0));
			sphereFloats.push_back(dx);
			sphereFloats.push_back(dy);
			sphereFloats.push_back(dz);
		}
		
	}

	return sphereFloats;

	/*
	float dx = x * sqrtf(1.0 - (y*y / 2.0) - (z*z / 2.0) + (y*y*z*z / 3.0));
	float dy = y * sqrtf(1.0 - (z*z / 2.0) - (x*x / 2.0) + (z*z*x*x / 3.0));
	float dz = z * sqrtf(1.0 - (x*x / 2.0) - (y*y / 2.0) + (x*x*y*y / 3.0));
	*/
}

QuadSphere::~QuadSphere()
{
}


/*
//Not Used.
void QuadSphere::ConvertToSphere()
{
for (QuadTree tree : faces)
{
vector<float> verts = tree.GetVerts(0, verts), temp;

for (int i = 0; i < verts.size() / 3; i++)
{
float dx = verts[(i * 3)] * sqrtf(1.0 - (verts[(i * 3) + 1] * verts[(i * 3) + 1] / 2.0) - (verts[(i * 3) + 2] * verts[(i * 3) + 2] / 2.0) + (verts[(i * 3) + 1] * verts[(i * 3) + 1] * verts[(i * 3) + 2] * verts[(i * 3) + 2] / 3.0));
float dy = verts[(i * 3) + 1] * sqrtf(1.0 - (verts[(i * 3) + 2] * verts[(i * 3) + 2] / 2.0) - (verts[(i * 3)] * verts[(i * 3)] / 2.0) + (verts[(i * 3) + 2] * verts[(i * 3) + 2] * verts[(i * 3)] * verts[(i * 3)] / 3.0));
float dz = verts[(i * 3) + 2] * sqrtf(1.0 - (verts[(i * 3)] * verts[(i * 3)] / 2.0) - (verts[(i * 3) + 1] * verts[(i * 3) + 1] / 2.0) + (verts[(i * 3)] * verts[(i * 3)] * verts[(i * 3) + 1] * verts[(i * 3) + 1] / 3.0));
temp.push_back(dx);
temp.push_back(dy);
temp.push_back(dz);
}
tree.SetVerts(temp);
}
*/