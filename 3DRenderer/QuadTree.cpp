#include "stdafx.h"
#include "QuadTree.h"
#include <glm.hpp>
#include <vector>

using namespace std;



QuadTree::QuadTree(){






};

QuadTree::QuadTree(glm::vec3 TopLeft, glm::vec3 TopRight, glm::vec3 BottomRight, glm::vec3 BottomLeft, int SubLevelCount)
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
			bottomMid = GetMidPoint(bottomLeft, bottomRight);

		int level = subLevelCount - 1;

		sublevels[0] = QuadTree(topLeft, topMid, center, leftMid, level);
		sublevels[1] = QuadTree(topMid, topRight, rightMid, center, level);
		sublevels[2] = QuadTree(leftMid, center, bottomMid, bottomLeft, level);
		sublevels[3] = QuadTree(center, rightMid, bottomRight, bottomMid, level);
	}

glm::vec3 QuadTree::GetMidPoint(glm::vec3 p1, glm::vec3 p2)
	{
		return glm::vec3(p1.x + ((p2.x - p1.x) / 2), p1.y + ((p2.y - p1.y) / 2), p1.z + ((p2.z - p1.z) / 2));
	}



vector<float> QuadTree::GetVerts(int depthLevel, vector<float> vectors)
	{
		if (depthLevel == subLevelCount) 
		{
			vectors.push_back(topLeft.x);
			vectors.push_back(topLeft.y);
			vectors.push_back(topLeft.z);

			vectors.push_back(topRight.x);
			vectors.push_back(topRight.y);
			vectors.push_back(topRight.z);

			vectors.push_back(bottomRight.x);
			vectors.push_back(bottomRight.y);
			vectors.push_back(bottomRight.z);

			vectors.push_back(bottomLeft.x);
			vectors.push_back(bottomLeft.y);
			vectors.push_back(bottomLeft.z);
		}
		else
		{
			for (QuadTree subs : sublevels)
			{
				 vectors = subs.GetVerts(depthLevel, vectors);
			}
		}

		return vectors;
	}

