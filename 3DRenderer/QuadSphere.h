#pragma once
#include "QuadTree.h"
#include <vector>

class QuadSphere
{
public:
	QuadSphere();
	void IncreaseDetail();
	vector<float> QuadSphere::ConvertToSphere(vector<float> input);
	~QuadSphere();
	void Initialise(float size, int MaxLOD);
	int GetMaxLOD();
	int GetCurrentLOD();
	int GetHighestMadeLOD();
	void SetCurrentLOD(int value);
	glm::vec3 GetMidPoint(glm::vec3 p1, glm::vec3 p2);
	vector<int> ReturnFaceIndices(int gap);
	vector<float> ReturnFaceVertices();
private:
	int maxLOD = 2, currentLOD = 1, HighestMadeLOD = 1;
};

