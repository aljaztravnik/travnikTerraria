#pragma once
#include <iostream>
#include "Definicije.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

enum Smer
{
	levo,
	desno,
	gor,
	dol,
	nikamor
};

class TravnikTerraria
{
private:
	struct winsize size;
	std::vector<std::vector<std::string>> map;
	std::vector<std::string> inventar;
	int newPlayerPos[2];
	int newEmptyBlockPos[2];
	int newBlockPos[2];
	int posX, posY;
	bool jump, playerMoved, emptyBlockChanged, blockChanged;
	uint8_t jumpHeight;
	uint32_t delayFor;
	Smer smer;


	void enable_raw_mode();
	void disable_raw_mode();
	bool kbhit();
	char getch();
	void drawWholeMap();
	void drawMap_playerAndEmptyBlock();
	void drawMap_emptyBlock();
	void moveCursor(std::ostream& os, int row, int col);
	void makeTerrain();
public:
	TravnikTerraria();
	void init();
	void run();
};