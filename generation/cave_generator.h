#ifndef CAVEGENERATOR_CLASS
#define CAVEGENERATOR_CLASS

#include <iostream>
#include <vector>
#include <time.h>

// Script is used to generate the raw cave layout. This class does not concern itself with generating the mesh itself, it only deals with the cellular automata.

class CaveGenerator
{
public:
	int width;
	int height;
	int randomFillPercent;
	int borderSize;
	int seed;
	std::vector<std::vector<int>> map;
	std::vector<std::vector<int>> borderedMap;

	CaveGenerator(int newWidth, int newHeight, int newRandomFillPercentage, int seed, int borderSize = 5)
	{
		CaveGenerator::width = newWidth;
		CaveGenerator::height = newHeight;
		CaveGenerator::randomFillPercent = newRandomFillPercentage;
		CaveGenerator::borderSize = borderSize;
		CaveGenerator::seed = seed;
		GenerateMap();
	}

private:
	void GenerateMap()
	{
		map = std::vector<std::vector<int>>(width, std::vector<int>(height, 0));
		RandomFillMap();

		for (int i = 0; i < 5; i++)
		{
			SmoothMap();
		}

		borderedMap = std::vector<std::vector<int>>(width + borderSize * 2, std::vector<int>(height + borderSize * 2, 0));

		for (int x = 0; x < borderedMap.size(); x++)
		{
			for (int y = 0; y < borderedMap[x].size(); y++)
			{
				if (x >= borderSize && x < width + borderSize && y >= borderSize && y < height + borderSize) 
				{
					borderedMap[x][y] = map[x - borderSize][y - borderSize];
				}
				else 
				{
					borderedMap[x][y] = 1;
				}
			}
		}
	} // Creates and smooths the cave map. Then, creates a few layers of border around this map. 

	void RandomFillMap()
	{
		if (seed == -1) 
		{
			seed = time(NULL);
			srand(seed);
		}
		else 
		{
			srand(seed);
		}
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
				{
					map[x][y] = 1;
				}
				else
				{
					int chance = rand() % 100 + 1;
					if (chance > randomFillPercent)
					{
						map[x][y] = 1;
					}
					else
					{
						map[x][y] = 0;
					}
				}
			}
		}
	} // Randomly fills the map vector, the density of the cave can be adjusted by the randomFillPercentage parameter. 

	void SmoothMap()
	{
		std::vector<std::vector<int>> smoothMap = std::vector<std::vector<int>>(width, std::vector<int>(height, 0));
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				int neighbourWallTiles = GetSurroundingWallCount(x, y);

				if (neighbourWallTiles > 4)
				{
					smoothMap[x][y] = 1;
				}
				else if (neighbourWallTiles < 4)
				{
					smoothMap[x][y] = 0;
				}
			}
		}
		map = smoothMap;
	} // Creates a new map where the the walls have been smoothened. If a empty square has more than 4 wall square adjacent to it then it will become a wall tile.

	int GetSurroundingWallCount(int gridX, int gridY)
	{
		int wallCount = 0;
		for (int neighbourX = gridX - 1; neighbourX <= gridX + 1; neighbourX++)
		{
			for (int neighbourY = gridY - 1; neighbourY <= gridY + 1; neighbourY++)
			{
				if (neighbourX >= 0 && neighbourX < width && neighbourY >= 0 && neighbourY < height)
				{
					if (neighbourX != gridX || neighbourY != gridY)
					{
						wallCount += map[neighbourX][neighbourY];
					}
				}
				else
				{
					wallCount++;
				}
			}
		}

		return wallCount;
	} // Returns the amount of wall squares directly adjacent to the inputted square.

};

#endif