#pragma once
#include "stdafx.h"
#include "Util.h"
#include <vector>
#include <cstdlib>
#include <glm.hpp>
using namespace std;

class QuadTree
{
private:
	int subLevelCount = 0;
	glm::vec3 topLeft = glm::vec3(), topRight = glm::vec3(), bottomLeft = glm::vec3(), bottomRight = glm::vec3();
	float width, height, depth;
	vector<QuadTree> sublevels;
public:
	QuadTree();
	QuadTree(glm::vec3 TopLeft, glm::vec3 TopRight, glm::vec3 BottomLeft, glm::vec3 BottomRight, int SubLevelCount);
	glm::vec3 GetMidPoint(glm::vec3 p1, glm::vec3 p2);
	void Subdivide();
	vector<float> GetVerts(int depthLevel, vector<float> vectors);
	void SetVerts(vector<float> vectors);

	glm::vec3 MakeSphere(glm::vec3 in);
};

