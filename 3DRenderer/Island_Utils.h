//#pragma once
#include <vector>
#include "EasyBMP.h"
#include "Logger.h"
using namespace std;


namespace Island_Utils
{
	vector<vector<int>> ShapeIsland(vector<vector<int>> *elevationMap);
	vector<vector<int>> CalculateBiomes(vector<vector<int>> *elevationMap, vector<vector<int>> *moistureMap);
	vector<int> GetBiomeColour(int biome);
	vector<vector<int>> MakeCircularGradient(int width, int height, int maxValue = 255, int minValue = 0);
	vector<vector<int>> InterpolateBitmaps(vector<vector<int>> *bmp1, vector<vector<int>> *bmp2, double bmp1Coeff = 0.5,
		double bmp2Coeff = 0.5, int offset = 0, int maxValue = 255, int minValue = 0);
	void SaveImage(vector<vector<int>>*  array, string imageName);
	void SaveBiomeImage(vector<vector<int>>*  array, string imageName);
	void MakeIsland();
	vector<vector<int>> GetIslandFractal();
	vector<vector<int>> GetIslandColoured();
	int GetMaxValue(vector<vector<int>> *array);
	int GetMinValue(vector<vector<int>> *array);
};
