#include "stdafx.h"
#include "QuadSphere.h"
#include "QuadTree.h"
#include <vector>

vector<vector<glm::vec3>> Newfaces;

//Default constructor (needed for vector<QuadSphere>)
QuadSphere::QuadSphere()
{
}

void QuadSphere::Initialise(float size)
{
	Newfaces.resize(6);
	//Front
	Newfaces[0] = vector<glm::vec3>{glm::vec3(-1, -1, size), glm::vec3(size, -1, size), glm::vec3(size, size, size), glm::vec3(-1, size, size)};
	//Back
	Newfaces[1] = vector<glm::vec3>{glm::vec3(-1, size, -1), glm::vec3(size, size, -1), glm::vec3(size, -1, -1), glm::vec3(-1, -1, -1)};
	Newfaces[2] = vector<glm::vec3>{glm::vec3(size, size, -1), glm::vec3(-1, size, -1), glm::vec3(-1, size, size), glm::vec3(size, size, size)};
	Newfaces[3] = vector<glm::vec3>{glm::vec3(-1, -1, -1), glm::vec3(size, -1, -1), glm::vec3(size, -1, size), glm::vec3(-1, -1, size)};
	Newfaces[4] = vector<glm::vec3>{glm::vec3(size, -1, -1), glm::vec3(size, size, -1), glm::vec3(size, size, size), glm::vec3(size, -1, size)};
	Newfaces[5] = vector<glm::vec3>{glm::vec3(-1, -1, size), glm::vec3(-1, size, size), glm::vec3(-1, size, -1), glm::vec3(-1, -1, -1)};

	//IncreaseDetail();
	//IncreaseDetail();
	//IncreaseDetail();
	//IncreaseDetail();
	//IncreaseDetail();
	//IncreaseDetail();
	//IncreaseDetail();
}

void QuadSphere::IncreaseDetail()
{
	for (int i = 0; i < Newfaces.size(); i++)
	{
		//Temp array of new points.
		vector<glm::vec3> newPoints;

		//calculate increased width.
		int width = sqrt(Newfaces[i].size()) + 1;
		//int newArraySize = pow(width, 2);
		
		for (int j = 0; j < Newfaces[i].size(); j++)
		{
			newPoints.push_back(Newfaces[i][j]);

			int y = (int)j / width;
			int x = j % width;

			//If i isn't at the end of a row/ isn't the last value in the array..
			if ((x != width - 1) && (j + 1 + 1 < Newfaces[i].size()))
			{

				//Add a new point which is the midpoint between the current and next point.
				newPoints.push_back(GetMidPoint(Newfaces[i][j], Newfaces[i][j + 1]));
			}
		}

		Newfaces[i].clear();
		Newfaces[i] = newPoints;
	}
}

glm::vec3 QuadSphere::GetMidPoint(glm::vec3 p1, glm::vec3 p2)
{
	//Simply gets the halfway point between p1 and p2 by adding half of the difference on to p1.
	//Make sure p1 is the smaller value!
	return glm::vec3(p1.x + ((p2.x - p1.x) / 2), p1.y + ((p2.y - p1.y) / 2), p1.z + ((p2.z - p1.z) / 2));
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

vector<float> QuadSphere::ReturnFaceVertices()
{
	vector<float> verts;

	for (int i = 0; i < Newfaces.size(); i++)
	{
		for (int j = 0; j < Newfaces[i].size(); j++)
		{
			verts.push_back(Newfaces[i][j].x);
			verts.push_back(Newfaces[i][j].y);
			verts.push_back(Newfaces[i][j].z);
		}
	}
	return verts;
}

vector<int> QuadSphere::ReturnFaceIndices(int gap)
{
	//Converts the array of points (left to right, moving down like a book)
	//to an array of faces (top left, top right, bottom right, bottom left).
	//gap is the size between these face compared to the max LOD.

	//Temp array of new points.
	vector<int> indices;

	//for each vert, if points[x+gap][y+gap] exists,make face
	for (int i = 0; i < Newfaces.size(); i++)
	{
		//calculate increased width.
		int width = sqrt(Newfaces[i].size());
		//Offset to increase the indices number by for each face, so to differentiate between each face.
		//this is because each face uses it's own array, and so they all have indices starting at zero.
		//The offset fixes that by ofsetting each subsequent face after the first by the number of verts in a face.
		int offset = (Newfaces[i].size())*i;

		for (int j = 0; j < Newfaces[i].size(); j += gap)
		{
			int y = (int)j / width;
			int x = j % width;

			//If points to the right and below the vert exist, we can make a face!
			if ((y + gap < width) && (x + gap < width))
			{
				//top left. (Remember, this is the position in the array of verts, not the vert data itself)
				indices.push_back((y * width) + x + offset);
				//top right
				indices.push_back((y * width) + x + gap + offset);
				//bottom right
				indices.push_back(((y + gap) * width) + x + offset);
				//bottom left
				indices.push_back(((y + gap) * width) + x + gap + offset);
			}
		}
	}
	return indices;
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