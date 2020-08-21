#include "TravnikTerraria.cpp"
#include <chrono>

int main()
{
	srand(time(NULL));
	auto prevTime = std::chrono::high_resolution_clock::now();
	TravnikTerraria game;
	game.init();
	while(1)
	{
		auto timeNow = std::chrono::high_resolution_clock::now();
		if(std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - prevTime).count() > 33)
		{
			game.run();
			prevTime = timeNow;
		}
	}
	return 0;
}