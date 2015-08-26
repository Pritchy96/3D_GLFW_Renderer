#pragma once
#include <glm.hpp>


class QuadTree
{
public:
	QuadTree(glm::vec3 TopLeft, glm::vec3 TopRight, glm::vec3 BottomLeft, glm::vec3 BottomRight, int SubLevelCount = 0);

	QuadTree::QuadTree();
	glm::vec3 GetMidPoint(glm::vec3 p1, glm::vec3 p2);
	void Subdivide();
};

