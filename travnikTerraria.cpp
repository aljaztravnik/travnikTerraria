#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Definicije.h"

// [19, 41)

void makeTerrain(std::vector<std::vector<char>> &map)
{
	int height = rand() % (40 - 19) + 19;
	map[height][0] = 'X';
	for(int i = 1; i < map[0].size(); i++)
	{
		int upDown = rand() % (3-1)+1;
		if(upDown == 1)
			if(height - 1 >= 9)
			{
				height--; // Actually -- because arrays start with 0 and we have to go up.
				std::cout << '[' << i+1 << "] gremo gor\n";
			}
		else
			if(height + 1 <= 49) 
			{
				height++;
				std::cout << '[' << i+1 << "] gremo dol\n";
			}
		map[height][i] = 'X';
	}
	for(int i = 10; i < map.size(); i++)
		for(int j = 0; j < map[i].size(); j++)
			if(map[i-1][j] == 'X') map[i][j] = 'X';
}

void drawMap(std::vector<std::vector<char>> &map)
{
	//system("clear");
	for(int i = 0; i < map.size(); i++)
	{
		for(int j = 0; j < map[i].size(); j++)
			std::cout << map[i][j];
		std::cout << '\n';
	}
}

int main()
{
	srand(time(NULL));
	std::vector<std::vector<char>> map(50, std::vector<char>(50, ' '));
	makeTerrain(map);
	drawMap(map);
	//std::string output = BLACK + "a" + RED + "a" + GREEN + "a" + '\n';
	//std::cout << output;
	return 0;
}