#pragma once
#include "QuadTree.h"
#include <vector>

class QuadSphere
{
public:
	QuadSphere();
	void IncreaseDetail();
	vector<float> GetFaceVerts();
	vector<float> ConvertToSphere();
	~QuadSphere();
	void Initialise(float size);
	int GetMaxLOD();
	int GetCurrentLOD();
	void SetCurrentLOD(int value);
	glm::vec3 GetMidPoint(glm::vec3 p1, glm::vec3 p2);
	vector<int> ReturnFaceIndices(int gap);
	vector<float> ReturnFaceVertices();
private:
	vector<QuadTree> faces;
	int maxLOD = 0, currentLOD;
};

