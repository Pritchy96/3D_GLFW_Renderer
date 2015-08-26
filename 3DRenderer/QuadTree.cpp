#include "stdafx.h"
#include "QuadTree.h"
#include <glm.hpp>
#include <vector>

using namespace std;

int subLevelCount;
glm::vec3 topLeft = glm::vec3(), topRight = glm::vec3(), bottomLeft = glm::vec3(), bottomRight = glm::vec3();
float width, height, depth;
vector<QuadTree> sublevels;


QuadTree::QuadTree()
{};
QuadTree::QuadTree(glm::vec3 TopLeft, glm::vec3 TopRight, glm::vec3 BottomLeft, glm::vec3 BottomRight, int SubLevelCount)
{
	subLevelCount = SubLevelCount;
	topLeft = TopLeft;
	topRight = TopRight;
	bottomLeft = BottomLeft;
	bottomRight = BottomRight;

	width = topRight.x - topLeft.x;
	height = bottomLeft.y - topLeft.y;
	depth = bottomLeft.z - topLeft.z;

	//If this level has sublevels.
	if (subLevelCount > 0)
	{
		//Make them.
		Subdivide();
	}
}

void QuadTree::Subdivide()
	{
		sublevels.resize(4);
		glm::vec3 topMid = GetMidPoint(topLeft, topRight),
			leftMid = GetMidPoint(topLeft, bottomLeft),
			center = GetMidPoint(topLeft, bottomRight),
			rightMid = GetMidPoint(topRight, bottomRight),
			bottomMid =GetMidPoint(bottomLeft, bottomRight);

		sublevels[0] = *(new QuadTree(topLeft, topMid, leftMid, center, subLevelCount - 1));
		sublevels[1] = *(new QuadTree(topMid, topLeft, center, rightMid, subLevelCount - 1));
		sublevels[2] = *(new QuadTree(leftMid, center, bottomLeft, bottomMid, subLevelCount - 1));
		sublevels[3] = *(new QuadTree(center, rightMid, bottomMid, bottomRight, subLevelCount - 1));
	}

glm::vec3 QuadTree::GetMidPoint(glm::vec3 p1, glm::vec3 p2)
	{
		return glm::vec3(p1.x + ((p2.x - p1.x) / 2), p1.y + ((p2.y - p1.y) / 2), p1.z + ((p2.z - p1.z) / 2));
	}



