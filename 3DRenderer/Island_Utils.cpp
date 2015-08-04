// ----------------------------------------------------------
// Includes 
// ----------------------------------------------------------
#include "stdafx.h"
#include "Island_Utils.h"
#include <vector>
#include <cstdlib>
#include <math.h>
#include "Util.h"
#include "EasyBMP.h"
#include "Fractal_Creator.h"
#include "Logger.h"
using namespace std;
// ----------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------C:\Windows\SysWOW64


const int width = 600, height = 600;
vector<vector<int>> islandFractal, islandShape, elevationMap, moistureMap, islandColoured, gradientMap;
int maxHeight = 0;



namespace Island_Utils
{
	vector<vector<int>> GetIslandFractal()
	{
		return elevationMap;
	}

	vector<vector<int>> GetIslandColoured()
	{
		return islandColoured;
	}

	int GetMaxValue(vector<vector<int>> *array)
	{
		int maxVal = 0;
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if ((*array)[x][y] > maxVal)
					maxVal = (*array)[x][y];
			}
		}
		return maxVal;
	}

	void MakeIsland()
	{
		Util::SeedGenerator(0);

		Fractal_Creator maker = Fractal_Creator();

		Logger::log("Generating Gradient Map", false, true);
		gradientMap = Island_Utils::MakeCircularGradient(width, height, 255, 0);

		Logger::log("Generating Height Fractal", false, true);
		elevationMap = maker.MakeFractal(width, height, 12);
		Logger::log("Interpolating Height Fractal with Gradient Map to make Elevation Map", false, true);
		elevationMap = InterpolateBitmaps(&elevationMap, &gradientMap, 1.0f, 1.3f, 0, 10000);

		Logger::log("Generating Island Shape", false, true);
		islandShape = Island_Utils::ShapeIsland(&elevationMap);


		Logger::log("Generating Rain Fractal", false, true);
		moistureMap = maker.MakeFractal(width, height, 4, 100, 0);
		Logger::log("Interpolating Rain Fractal with Gradient Map to make Moisture Map", false, true);
		moistureMap = InterpolateBitmaps(&elevationMap, &gradientMap, 1.0f, 1.3f, 0, 10000);

		Logger::log("Calculating Biomes", false, true);
		islandColoured = Island_Utils::CalculateBiomes(&islandShape, &elevationMap, &moistureMap);

		Logger::log("Saving Biome Image", false, true);
		//SaveBiomeImage(&islandColoured, "colour.bmp");
	}

	vector<vector<int>> ShapeIsland(vector<vector<int>> *elevationMap)
	{
		vector<vector<int>>  shape = vector<vector<int>>();
		//Resizing array and setting all values to 0.
		shape.resize(width, vector<int>(height, 0));

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				int finalValue = 0;

				//If map value is low enough to be considered water...
				if ((*elevationMap)[x][y] < 300)
				{
					//Make the elevation a flat level
					(*elevationMap)[x][y] = 300;
					//Make it water
					finalValue = 0;
				}

				else    //Otherwise, make it land.
				{
					finalValue = 255;
				}

				shape[x][y] = finalValue;
			}
		}

		return shape;
	}



	vector<vector<int>> CalculateBiomes(vector<vector<int>> *islandShape, vector<vector<int>> *elevationMap, vector<vector<int>> *moistureMap)
	{
		int width = (*islandShape).size(), height = (*islandShape)[0].size();
		//Whittaker diagram has four rows for Elevation, 6 for Moisture. This is converting the 0-x to 1-6 and 1-4.
		float maxHeight = (float)GetMaxValue(elevationMap), maxMoisture = (float)GetMaxValue(moistureMap);
		float heightZone = maxHeight / 4, moistureZone = maxMoisture / 6;

		vector<vector<int>>  colouredIsland = vector<vector<int>>();
		//Resizing array and setting all values to 0.
		colouredIsland.resize(width, vector<int>(height, 0));

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{

				if ((*islandShape)[x][y] == 255)  //Land
				{
					if ((*elevationMap)[x][y] <= 1 * heightZone)
					{
						if ((*moistureMap)[x][y] <= 1 * moistureZone)
						{

						}
					}
				}
				else //Sea
				{
					if ((*elevationMap)[x][y] < 58) //Deep areas
					{
						colouredIsland[x][y] = -2;
					}
					else
					{
						//Inner sea
						colouredIsland[x][y] = -1;
					}
				}
			}
		}

		return colouredIsland;
	}

	vector<int> GetBiomeColour(int biome)
	{
		vector<int> colour = vector<int>();
		colour.resize(3);

		switch (biome)
		{
		case -2:
		{
			//Deep Sea
			colour[0] = 28;
			colour[1] = 50;
			colour[2] = 63;
			break;
		}
		case -1:
		{
			//Shallow Sea.
			colour[0] = 38;
			colour[1] = 60;
			colour[2] = 73;
			break;
		}
		case 0:
		{
			//SubTropical Desert
			colour[0] = 250;
			colour[1] = 148;
			colour[2] = 24;
			break;
		}
		case 1:
		{
			//Grassland
			colour[0] = 250;
			colour[1] = 219;
			colour[2] = 7;
			break;
		}
		case 2:
		{
			//Tropical Seasonal Forest/Savanna
			colour[0] = 250;
			colour[1] = 219;
			colour[2] = 7;
			break;
		}
		case 3:
		{
			//Tropical Rain Forest
			colour[0] = 155;
			colour[1] = 224;
			colour[2] = 35;

			break;
		}
		case 4:
		{
			//Temperate Desert
			colour[0] = 46;
			colour[1] = 177;
			colour[2] = 83;
			break;
		}
		case 5:
		{
			//Temperate Deciduous Forest
			colour[0] = 7;
			colour[1] = 249;
			colour[2] = 162;
			break;
		}
		case 6:
		{
			//Temperate Rain Forest
			colour[0] = 76;
			colour[1] = 102;
			colour[2] = 0;
			break;
		}
		case 7:
		{
			//Shrubland
			colour[0] = 5;
			colour[1] = 102;
			colour[2] = 33;
			break;
		}
		case 8:
		{
			//Taiga
			colour[0] = 85;
			colour[1] = 235;
			colour[2] = 249;
			break;
		}
		case 9:
		{
			//Scorched
			colour[0] = 85;
			colour[1] = 235;
			colour[2] = 249;
			break;
		}
		case 10:
		{
			//Bare
			colour[0] = 85;
			colour[1] = 235;
			colour[2] = 249;
			break;
		}
		case 11:
		{
			//Tundra
			colour[0] = 85;
			colour[1] = 235;
			colour[2] = 249;
			break;
		}
		case 12:
		{
			//Snow
			colour[0] = 85;
			colour[1] = 235;
			colour[2] = 249;
			break;
		}


		}

		return colour;
	}

	vector<vector<int>> MakeCircularGradient(int width, int height, int maxValue, int minValue)
	{
		vector<vector<int>> map = vector<vector<int>>();
		//Resizing array and setting all values to 0.
		map.resize(width, vector<int>(height, 0));

		double maxDistance = 0, centerX = width / 2, centerY = height / 2;
		maxDistance = sqrt((pow(centerX, 2)) + (pow(centerY, 2)));

		for (int x = 0; x < width; x++)
		{

			for (int y = 0; y < height; y++)
			{

				double distX = abs(x - centerX), distY = abs(y - centerY);    //Distance fron center in x and y.
				double distance = sqrt(pow(distX, 2) + pow(distY, 2));   //Distance from center.

				map[x][y] = (int)255 - ((distance / maxDistance) * 255);
			}
		}
		return map;
	}

	vector<vector<int>> InterpolateBitmaps(vector<vector<int>> *bmp1, vector<vector<int>> *bmp2, double bmp1Coeff,
		double bmp2Coeff, int offset, int maxValue, int minValue)
	{
		int width = (*bmp1).size(), height = (*bmp1)[0].size();

		vector<vector<int>>  newBmp = vector<vector<int>>();
		//Resizing array and setting all values to 0.
		newBmp.resize(width, vector<int>(height, 0));

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				double val = (((*bmp1)[x][y] * bmp1Coeff) + ((*bmp2)[x][y] * bmp2Coeff));

				if (val > maxValue)
				{
					val = maxValue;
				}
				else if (val < minValue)
				{
					val = minValue;
				}

				newBmp[x][y] = (int)(val);
			}
		}
		return newBmp;
	}

	void SaveImage(vector<vector<int>>*  array, string imageName)
	{
		BMP image;
		int width = (*array).size(), height = (*array)[0].size();
		image.SetSize(width, height);
		image.SetBitDepth(32);

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				//vector<int> colour = Island_Utils::GetBiomeColour((*array)[x][y]);
				image(x, y)->Red = (*array)[x][y];
				image(x, y)->Green = (*array)[x][y];
				image(x, y)->Blue = (*array)[x][y];
			}
		}

		const char *cstr = imageName.c_str();
		image.WriteToFile(cstr);
	}

	void SaveBiomeImage(vector<vector<int>>*  array, string imageName)
	{
		BMP image;
		int width = (*array).size(), height = (*array)[0].size();
		image.SetSize(width, height);
		image.SetBitDepth(32);

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				vector<int> colour = Island_Utils::GetBiomeColour((*array)[x][y]);
				image(x, y)->Red = colour[0];
				image(x, y)->Green = colour[1];
				image(x, y)->Blue = colour[2];
			}
		}

		const char *cstr = imageName.c_str();
		image.WriteToFile(cstr);
	}
}








/*
#pragma region Removing Fractal (blue for land, black for sea).
//Keep value between 0 and 255
if (finalValue > 109 || distance > maxDistance - 50)    //If we're high enough to be considered ocean or close to the edge.
{
elevationMap[x][y] = 109;
//If we're close to the center and it's going to be water, instead make it land.
if (distance < 170)
{
finalValue = 255;
//finalValue = (byte)Math.Min(255, (finalValue + (int)(((float)finalValue / 255) * rand.Next(-5, 5))));
}
else //Otherwise make it sea.
{
finalValue = 0;
}
}

else
{
finalValue = 255;
//finalValue = (byte)(Math.Min(255, (finalValue + (int)(((float)finalValue / 255) * rand.Next(170-5, 5))));
}
#pragma endregion
*/
