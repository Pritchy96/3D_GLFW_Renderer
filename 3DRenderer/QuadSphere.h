#pragma once
#include "QuadTree.h"
#include <vector>

class QuadSphere
{
public:
	QuadSphere();
	QuadSphere(float size, int maxLOD);
	vector<float> GetFaceVerts(int currentLOD);
	void ConvertToSphere();
	~QuadSphere();
	int GetMaxLOD();
private:
	vector<QuadTree> faces;
	int maxLOD = 0;
};

