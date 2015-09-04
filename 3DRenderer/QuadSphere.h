#pragma once
#include "QuadTree.h"
#include <vector>

class QuadSphere
{
public:
	QuadSphere();
	QuadSphere(float size, int maxLOD);
	vector<float> GetFaceVerts();
	vector<float> ConvertToSphere();
	~QuadSphere();
	int GetMaxLOD();
	int GetCurrentLOD();
	void SetCurrentLOD(int value);
private:
	vector<QuadTree> faces;
	int maxLOD = 0, currentLOD;
};

