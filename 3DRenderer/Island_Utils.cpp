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


const int width = 600, height = 600, waterLevel = 300;
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

	int GetMinValue(vector<vector<int>> *array)
	{
		int minVal = 9999;
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if ((*array)[x][y] < minVal)
					minVal = (*array)[x][y];
			}
		}
		return minVal;
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

		elevationMap = FlattenWater(&elevationMap);

		Logger::log("Generating Moisture Fractal", false, true);
		moistureMap = maker.MakeFractal(width, height, 120, 255, 0);
		//Logger::log("Interpolating Rain Fractal with Gradient Map to make Moisture Map", false, true);
		//moistureMap = InterpolateBitmaps(&elevationMap, &gradientMap, 1.0f, 1.5f, 0, 10000);

		Logger::log("Calculating Biomes", false, true);
		islandColoured = Island_Utils::CalculateBiomes(&elevationMap, &moistureMap);

		Logger::log("Saving Biome Image", false, true);
		SaveBiomeImage(&islandColoured, "colour.bmp");
		SaveImage(&moistureMap, "moisture.bmp");
	}

	vector<vector<int>> FlattenWater(vector<vector<int>> *elevationMap)
	{

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				//If map value is low enough to be considered water...
				if ((*elevationMap)[x][y] < 300)
				{
					//Make the elevation a flat level
					(*elevationMap)[x][y] = 300;
				}

			}
		}

		return *elevationMap;
	}



	vector<vector<int>> CalculateBiomes(vector<vector<int>> *elevationMap, vector<vector<int>> *moistureMap)
	{
		int width = (*elevationMap).size(), height = (*elevationMap)[0].size();
		//Whittaker diagram has four rows for Elevation, 6 for Moisture. This is converting the 0-x to 1-6 and 1-4.
		float maxHeight = (float)GetMaxValue(elevationMap), maxMoisture = (float)GetMaxValue(moistureMap), minMoisture = (float)GetMinValue(moistureMap);
		float heightZone = maxHeight / 4, moistureZone = (maxMoisture - minMoisture) / 6;

		vector<vector<int>>  colouredIsland = vector<vector<int>>();
		//Resizing array and setting all values to 0.
		colouredIsland.resize(width, vector<int>(height, 0));

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				int moistureVal = maxMoisture - ((*moistureMap)[x][y] - minMoisture);
				if ((*elevationMap)[x][y] > waterLevel)  //Land
				{
					if ((*elevationMap)[x][y] <= 1 * heightZone)
					{
						if (moistureVal <= 1 * moistureZone)
						{
							colouredIsland[x][y] = 0;
						}
						else if (moistureVal <= 2 * moistureZone)
						{
							colouredIsland[x][y] = 1;
						}
						else if (moistureVal <= 4 * moistureZone)
						{
							colouredIsland[x][y] = 2;
						}
						else //if (moistureVal<= 6 * moistureZone)
						{
							colouredIsland[x][y] = 3;
						}
					}
					else if ((*elevationMap)[x][y] <= 2 * heightZone)
					{
						if (moistureVal <= 1 * moistureZone)
						{
							colouredIsland[x][y] = 4;
						}
						else if (moistureVal <= 3 * moistureZone)
						{
							colouredIsland[x][y] = 1;
						}
						else if (moistureVal <= 5 * moistureZone)
						{
							colouredIsland[x][y] = 5;
						}
						else //if (moistureVal<= 6 * moistureZone)
						{
							colouredIsland[x][y] = 6;
						}
					}
					else if ((*elevationMap)[x][y] <= 3 * heightZone)
					{
						if (moistureVal <= 2 * moistureZone)
						{
							colouredIsland[x][y] = 4;
						}
						else if (moistureVal <= 4 * moistureZone)
						{
							colouredIsland[x][y] = 7;
						}
						else //if (moistureVal<= 6 * moistureZone)
						{
							colouredIsland[x][y] = 8;
						}
					}
					else //if ((*elevationMap)[x][y] <= 4 * heightZone)   //Snow and mountains
					{
						if (moistureVal <= 1 * moistureZone)
						{
							colouredIsland[x][y] = 9;
						}
						else if (moistureVal <= 2 * moistureZone)
						{
							colouredIsland[x][y] = 10;
						}
						else if (moistureVal <= 3 * moistureZone)
						{
							colouredIsland[x][y] = 11;
						}
						else //if (moistureVal<= 6 * moistureZone)
						{
							colouredIsland[x][y] = 12;
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

	vector<float> GetBiomeColour(int biome)
	{
		vector<float> colour = vector<float>();
		colour.resize(3);

		switch (biome)
		{
		case -2:
		{
			//Deep Sea
			colour[0] = 0.110f;
			colour[1] = 0.196f;
			colour[2] = 0.247f;
			break;
		}
		case -1:
		{
			//Shallow Sea.
			colour[0] = 0.149f;
			colour[1] = 0.235f;
			colour[2] = 0.286f;
			break;
		}
		case 0:
		{
			//SubTropical Desert
			colour[0] = 0.851f;
			colour[1] = 0.745f;
			colour[2] = 0.553f;
			break;
		}
		case 1:
		{
			//Grassland
			colour[0] = 0.525f;
			colour[1] = 0.667f;
			colour[2] = 0.298f;
			break;
		}
		case 2:
		{
			//Tropical Seasonal Forest/Savanna
			colour[0] = 0.294f;
			colour[1] = 0.396f;
			colour[2] = 0.396f;
			break;
		}
		case 3:
		{
			//Tropical Rain Forest
			colour[0] = 0.173f;
			colour[1] = 0.447f;
			colour[2] = 0.286f;

			break;
		}
		case 4:
		{
			//Temperate Desert
			colour[0] = 0.808f;
			colour[1] = 0.839f;
			colour[2] = 0.576f;
			break;
		}
		case 5:
		{
			//Temperate Deciduous Forest
			colour[0] = 0.384f;
			colour[1] = 0.569f;
			colour[2] = 0.286f;
			break;
		}
		case 6:
		{
			//Temperate Rain Forest
			colour[0] = 0.243f;
			colour[1] = 0.525f;
			colour[2] = 0.278f;
			break;
		}
		case 7:
		{
			//Shrubland
			colour[0] = 0.525f;
			colour[1] = 0.596f;
			colour[2] = 0.447f;
			break;
		}
		case 8:
		{
			//Taiga
			colour[0] = 0.596f;
			colour[1] = 0.667f;
			colour[2] = 0.447f;
			break;
		}
		case 9:
		{
			//Scorched
			colour[0] = 0.149f;
			colour[1] = 0.149f;
			colour[2] = 0.149f;
			break;
		}
		case 10:
		{
			//Bare
			colour[0] = 0.447f;
			colour[1] = 0.447f;
			colour[2] = 0.447f;
			break;
		}
		case 11:
		{
			//Tundra
			colour[0] = 0.745f;
			colour[1] = 0.745f;
			colour[2] = 0.447f;
			break;
		}
		case 12:
		{
			//Snow
			colour[0] = 0.980f;
			colour[1] = 0.980f;
			colour[2] = 0.980f;
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
		int width = (*array).size(), height = (*array)[0].size(), maxVal = GetMaxValue(array);
		image.SetSize(width, height);
		image.SetBitDepth(32);

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if ((*array)[x][y] > 255)
				{
					int val = (int)((((double)(*array)[x][y]) / (double)maxVal) * 255);
					//vector<int> colour = Island_Utils::GetBiomeColour((*array)[x][y]);
					image(x, y)->Red = val;
					image(x, y)->Green = val;
					image(x, y)->Blue = val;

				}
				else
				{
					//vector<int> colour = Island_Utils::GetBiomeColour((*array)[x][y]);
					image(x, y)->Red = (*array)[x][y];
					image(x, y)->Green = (*array)[x][y];
					image(x, y)->Blue = (*array)[x][y];
				}
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
				vector<float> colour = Island_Utils::GetBiomeColour((*array)[x][y]);
				image(x, y)->Red = colour[0] * 255;
				image(x, y)->Green = colour[1] * 255;
				image(x, y)->Blue = colour[2] * 255;
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
