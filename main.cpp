#include <iostream>
#include "TravnikTerraria.cpp"

int main()
{
	srand(time(NULL));
	TravnikTerraria game;
	game.init();
	while(1)
	{
		game.run();
	}
	return 0;
}