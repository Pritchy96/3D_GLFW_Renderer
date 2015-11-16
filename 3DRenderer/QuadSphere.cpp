#include "stdafx.h"
#include "QuadSphere.h"
#include "QuadTree.h"
#include <vector>
#include "Fractal_Creator.h"

vector<vector<glm::vec3>> verts;
vector<vector<int>> fractal;
QuadSphere::QuadSphere()
{
}

void QuadSphere::Initialise(float size, int MaxLOD)
{
	//Front
	verts.push_back(vector<glm::vec3>{glm::vec3(size, -1, size), glm::vec3(size, size, size), glm::vec3(-1, -1, size), glm::vec3(-1, size, size)});
	//Back
	verts.push_back(vector<glm::vec3>{glm::vec3(-1, size, -1), glm::vec3(size, size, -1), glm::vec3(-1, -1, -1), glm::vec3(size, -1, -1)});
	verts.push_back(vector<glm::vec3>{glm::vec3(size, size, -1), glm::vec3(-1, size, -1), glm::vec3(size, size, size), glm::vec3(-1, size, size)});
	verts.push_back(vector<glm::vec3>{glm::vec3(-1, -1, -1), glm::vec3(size, -1, -1), glm::vec3(-1, -1, size), glm::vec3(size, -1, size)});
	verts.push_back(vector<glm::vec3>{glm::vec3(size, -1, -1), glm::vec3(size, size, -1), glm::vec3(size, -1, size), glm::vec3(size, size, size)});
	verts.push_back(vector<glm::vec3>{glm::vec3(-1, -1, size), glm::vec3(-1, size, size), glm::vec3(-1, -1, -1), glm::vec3(-1, size, -1)});

	maxLOD = MaxLOD;
	while (HighestMadeLOD < maxLOD)
	{
		IncreaseDetail();
	}

	Fractal_Creator test;
	fractal = test.MakeFractal(1000, 1000, 10, 100, -100);

}

void QuadSphere::IncreaseDetail()
{
	for (int i = 0; i < verts.size(); i++)
	{
		//Temp array of new points.
		vector<glm::vec3> tempVec;
		//calculate increased newWidth.
		int newWidth = (sqrt(verts[i].size())*2) - 1;
		int oldWidth = sqrt(verts[i].size());
		int newArraySize = pow(newWidth, 2);
		tempVec.resize(newArraySize);

		//Loop through this face.
		for (int j = 0; j < verts[i].size(); j++)
		{
			int y = (int)j / oldWidth;
			int x = j % oldWidth;

			//Add in existing values to their corresponding slots.
			tempVec[((y * 2) * newWidth) + (x * 2)] = verts[i][j];

			//Filling in spaces between two values.
			//If x is odd, it's not been filled by an old value.
			if (x + 1 % 2 && x > 0)
			{
				tempVec[((y * 2) * newWidth) + (x * 2) - 1] = GetMidPoint(tempVec[((y * 2) * newWidth) + ((x * 2))], tempVec[((y * 2) * newWidth) + ((x * 2) - 2)]);
			}
		}

		//Filling in entire new rows
		//Loop through the new array.
		for (int k = 0; k < tempVec.size() - 1; k++)
		{
			if (tempVec[k].x == 0 && tempVec[k].y == 0 && tempVec[k].z == 0)
			{
				//Add in new vert, taken from vertical points if we can, if not (at edge), from horizontal.
				int y = (int)k / newWidth;
				int x = k % newWidth;
				//We are at the top or bottomof the array, so we can't take points from those sides. 
				//Get points from left and right instead.
				int above = (k + newWidth), below = (k - newWidth);
				tempVec[k] = GetMidPoint(tempVec[below], tempVec[above]);
			}
		}
		verts[i] = tempVec;

		tempVec.clear();
	}
	//New highest LOD achieved!
		HighestMadeLOD = sqrt(verts[0].size()) - 1;
}

glm::vec3 QuadSphere::GetMidPoint(glm::vec3 p1, glm::vec3 p2)
{
	//Simply gets the halfway point between p1 and p2 by adding half of the difference on to p1.
	//Make sure p1 is the smaller value!
	return glm::vec3((p1.x + ((p2.x - p1.x) / 2)), (p1.y + ((p2.y - p1.y) / 2)), (p1.z + ((p2.z - p1.z) / 2)));
}

int QuadSphere::GetMaxLOD()
{
	return maxLOD;
}

int QuadSphere::GetHighestMadeLOD()
{
	return HighestMadeLOD;
}

void QuadSphere::SetCurrentLOD(int value)
{
	currentLOD = value;
}

int QuadSphere::GetCurrentLOD()
{
	return currentLOD;
}

vector<float> QuadSphere::ReturnFaceVertices()
{
	vector<float> floatVerts;
	
	for (int i = 0; i < verts.size(); i++)
	{
		for (int j = 0; j < verts[i].size(); j++)
		{
			//Create normalised vector.
			float x = verts[i][j].x, y = verts[i][j].y, z = verts[i][j].z;
			float mod = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
			float dx = x / mod;
			float dy = y / mod;
			float dz = z / mod;

			int width = sqrt(verts[i].size());
			int yArrayPos = (int)j / width;
			int xArrayPos = j % width;
			float displace = (((float)fractal[xArrayPos + 20][(xArrayPos*2) + 20]) - 100) / 100;

			//Add noise.
			float scale = 2;
			x += (displace * dx) / scale;
			y += (displace * dy) / scale;
			z += (displace * dz) / scale;

			floatVerts.push_back(x);
			floatVerts.push_back(y);
			floatVerts.push_back(z);
		}
	}
	return floatVerts;
}

vector<int> QuadSphere::ReturnFaceIndices(int gap)
{
	//Converts the array of points (left to right, moving down like a book)
	//to an array of faces (top left, top right, bottom right, bottom left).
	//gap is the size between these face compared to the max LOD.

	//Temp array of new points.
	vector<int> indices;
	//for each vert, if points[x+gap][y+gap] exists,make face
	for (int i = 0; i < verts.size(); i++)
	{
		//calculate Width.
		int newWidth = sqrt(verts[i].size());
		//Offset to increase the indices number by for each face, so to differentiate between each face.
		//this is because each face uses it's own array, and so they all have indices starting at zero.
		//The offset fixes that by ofsetting each subsequent face after the first by the number of verts in a face.


		int offset = (verts[i].size())*i;


		for (int y = 0; y < sqrt(verts[i].size()); y += gap)
		{
			for (int x = 0; x < sqrt(verts[i].size()); x += gap)
			{
				//If points to the right and below the vert exist, we can make a face!
				if ((y + gap < newWidth) && (x + gap < newWidth))
				{
					//top left. (Remember, this is the position in the array of verts, not the vert data itself)

					// (y * width) + x
					int topLeft = ((y * newWidth) + x + offset),
						topRight = (((y * newWidth) + x + gap + offset)),
						bottomRight = (((y + gap) * newWidth) + x + gap + offset),
						bottomLeft = (((y + gap) * newWidth) + x + offset);

					//top left
					indices.push_back(topLeft);
					//top right
					indices.push_back(topRight);
					//bottom right
					indices.push_back(bottomRight);
					//bottom left
					indices.push_back(bottomLeft);
				}
			}
		}

		/*
		for (int j = 0; j < verts[i].size(); j += gap)
		{
			int y = (int)j / newWidth;
			int x = j % newWidth;
		}
		*/
	}
	return indices;
}

vector<float> QuadSphere::ConvertToSphere(vector<float> input)
{
	vector<float> sphereFloats;

	for (int i = 0; i < input.size() / 3; i++)
	{
	float dx = input[(i * 3)] * sqrtf(1.0 - (input[(i * 3) + 1] * input[(i * 3) + 1] / 2.0) - (input[(i * 3) + 2] * input[(i * 3) + 2] / 2.0) + (input[(i * 3) + 1] * input[(i * 3) + 1] * input[(i * 3) + 2] * input[(i * 3) + 2] / 3.0));
	float dy = input[(i * 3) + 1] * sqrtf(1.0 - (input[(i * 3) + 2] * input[(i * 3) + 2] / 2.0) - (input[(i * 3)] * input[(i * 3)] / 2.0) + (input[(i * 3) + 2] * input[(i * 3) + 2] * input[(i * 3)] * input[(i * 3)] / 3.0));
	float dz = input[(i * 3) + 2] * sqrtf(1.0 - (input[(i * 3)] * input[(i * 3)] / 2.0) - (input[(i * 3) + 1] * input[(i * 3) + 1] / 2.0) + (input[(i * 3)] * input[(i * 3)] * input[(i * 3) + 1] * input[(i * 3) + 1] / 3.0));
	sphereFloats.push_back(dx);
	sphereFloats.push_back(dy);
	sphereFloats.push_back(dz);
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