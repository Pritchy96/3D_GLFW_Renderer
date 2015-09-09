#include "stdafx.h"
#include "QuadTree.h"
#include <glm.hpp>
#include <vector>

using namespace std;

QuadTree::QuadTree(){
};

QuadTree::QuadTree(glm::vec3 TopLeft, glm::vec3 TopRight, glm::vec3 BottomRight, glm::vec3 BottomLeft, int SubLevelCount)
{
	//Set augs to vars for internal use.
	subLevelCount = SubLevelCount;
	topLeft = TopLeft;
	topRight = TopRight;
	bottomLeft =  BottomLeft;
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
		//Splits this quad into four 'children'
		glm::vec3 topMid = GetMidPoint(topLeft, topRight),
			leftMid = GetMidPoint(topLeft, bottomLeft),
			center = GetMidPoint(topLeft, bottomRight),
			rightMid = GetMidPoint(topRight, bottomRight),
			bottomMid = GetMidPoint(bottomLeft, bottomRight);

		//sublevel is one level deeper than this, so it will have one less sublevels.
		int level = subLevelCount - 1;

		sublevels.push_back(QuadTree(topLeft, topMid, center, leftMid, level));
		sublevels.push_back(QuadTree(topMid, topRight, rightMid, center, level));
		sublevels.push_back(QuadTree(leftMid, center, bottomMid, bottomLeft, level));
		sublevels.push_back(QuadTree(center, rightMid, bottomRight, bottomMid, level));
	}


glm::vec3 QuadTree::GetMidPoint(glm::vec3 p1, glm::vec3 p2)
	{
		//Simply gets the halfway point between p1 and p2 by adding half of the difference on to p1.
		//Make sure p1 is the smaller value!
		return glm::vec3(p1.x + ((p2.x - p1.x) / 2), p1.y + ((p2.y - p1.y) / 2), p1.z + ((p2.z - p1.z) / 2));
	}

vector<float> QuadTree::GetVerts(int depthLevel, vector<float> vectors)
	{
		//If we're as deep as we need to be, return the Quad.
		if (depthLevel == subLevelCount) 
		{
			vectors.push_back(topLeft.x);
			vectors.push_back(topLeft.y);
			vectors.push_back(topLeft.z);

			vectors.push_back(topRight.x);
			vectors.push_back(topRight.y);
			vectors.push_back(topRight.z);

			vectors.push_back(bottomLeft.x);
			vectors.push_back(bottomLeft.y);
			vectors.push_back(bottomLeft.z);

			vectors.push_back(bottomRight.x);
			vectors.push_back(bottomRight.y);
			vectors.push_back(bottomRight.z);


		}
		else
		{
			//if we're not, see if it's children are deep enough.
			for (QuadTree subs : sublevels)
			{
				 vectors = subs.GetVerts(depthLevel, vectors);
			}
		}

		return vectors;
	}

//Not currently used.
glm::vec3 QuadTree::MakeSphere(glm::vec3 in)
{
	//'bends' cube section into sphere, see:
	//http://gamedev.stackexchange.com/questions/43741/how-do-you-turn-a-cube-into-a-sphere
	//https://www.youtube.com/watch?v=_8Bg1OH_9Bo
	return glm::vec3(
		(in.x * sqrtf(1.0 - (in.y * in.y / 2.0) - (in.z * in.z / 2.0) + (in.y * in.y * in.z * in.z / 3.0))),
		(in.y * sqrtf(1.0 - (in.z * in.z / 2.0) - (in.x * in.x / 2.0) + (in.z * in.z * in.x * in.x / 3.0))),
		(in.z * sqrtf(1.0 - (in.x * in.x / 2.0) - (in.y * in.y / 2.0) + (in.x * in.x * in.y * in.y / 3.0))));
}

//Not currently used.
void QuadTree::SetVerts(vector<float> vectors)
{
	//If the array is 12 floats, it's one Quad, so it must not have any sublevels.
	//Therefore set this level to those values.
	if (vectors.size() == 12)
	{
		topLeft.x = vectors[0];
		topLeft.y = vectors[1];
		topLeft.z = vectors[2];

		topRight.x = vectors[3];
		topRight.y = vectors[4];
		topRight.z = vectors[5];

		bottomRight.x = vectors[6];
		bottomRight.y = vectors[7];
		bottomRight.z = vectors[8];

		bottomLeft.x = vectors[9];
		bottomLeft.y = vectors[10];
		bottomLeft.z = vectors[11];
	}
	else
	{
		//If the aray has more than 12 floats, it must be referring to
		//this Quads sublevels, so split vector into 4 and give them to sublevel Quads.
		vector<float> subVerts;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < vectors.size() / 4; j++)
			{
				subVerts.push_back(vectors[(4 * i) + j]);
			}
			//Set values on this child.
			sublevels[i].SetVerts(subVerts);
			//Reset for next iteration/child.
			subVerts.clear();
		}
	}
}


