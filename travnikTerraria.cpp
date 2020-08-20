#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Definicije.h"
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fstream>

void enable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(0, TCSANOW, &term);
}

void disable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

bool kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

char getch()
{
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

// [19, 41)

void makeTerrain(std::vector<std::vector<std::string>> &map, int posX, int& posY, winsize &size)
{
	int height = rand() % (40 - 19) + 19;
	map[height][0] = "X";
	for(int i = 1; i < map[0].size(); i++)
	{
		bool upDown = rand() % 2;
		if(upDown)
			if(height - 1 >= 9)
				height--; // Actually -- because arrays start with 0 and we have to go up.
		else
			if(height + 1 <= size.ws_col-1)
				height++;
		map[height][i] = "X";
		if(i == map[0].size()/2) 
		{
			posY = height-1;
			map[posY][posX] = PLAYER;
		}
	}
	for(int i = 10; i < map.size(); i++)
		for(int j = 0; j < map[0].size(); j++)
			if(map[i-1][j] == "X") map[i][j] = "X";

}

void moveCursor(std::ostream& os, int row, int col)
{
  os << "\033[" << row << ";" << col << "H";
}

void drawMap(std::vector<std::vector<std::string>> &map)
{
	//system("clear");
	for(int i = 0; i < map.size(); i++)
	{
		for(int j = 0; j < map[0].size(); j++)
			std::cout << map[i][j];
		std::cout << '\n';
	}
}

void drawMap(int newEmptyBlockPos[], int newPlayerPos[], std::ofstream& myfile, std::vector<std::vector<std::string>> &map)
{
	map[newEmptyBlockPos[0]][newEmptyBlockPos[1]] = " ";
	map[newPlayerPos[0]][newPlayerPos[1]] = PLAYER;

	myfile << "premikam kursor na " << newEmptyBlockPos[0] << ", " << newEmptyBlockPos[1]+1 << '\n';
	moveCursor(std::cout, newEmptyBlockPos[0], newEmptyBlockPos[1]+1);
	std::cout << " ";
	myfile << "premikam kursor na " << newPlayerPos[0] << ", " << newPlayerPos[1]+1 << '\n';
	moveCursor(std::cout, newPlayerPos[0], newPlayerPos[1]+1);
	std::cout << PLAYER << '\n';
}

void drawMap(int newEmptyBlockPos[], std::vector<std::vector<std::string>> &map)
{
	map[newEmptyBlockPos[0]][newEmptyBlockPos[1]] = " ";

	moveCursor(std::cout, newEmptyBlockPos[0], newEmptyBlockPos[1]+1);
	std::cout << " \n";
}

enum Smer
{
	levo,
	desno,
	gor,
	dol,
	nikamor
};

int main()
{
	std::ofstream myfile;
	myfile.open("out.txt");
	srand(time(NULL));
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size); // size.ws_row is the number of rows, size.ws_col is the number of columns.
	std::vector<std::vector<std::string>> map(size.ws_row, std::vector<std::string>(size.ws_col, " "));
	std::vector<std::string> inventar;
	int newPlayerPos[2];
	int newEmptyBlockPos[2];
	int newBlockPos[2];
	int posX = map[0].size()/2, posY = 0;
	bool jump = false, playerMoved = false, emptyBlockChanged = false, blockChanged = false;
	uint8_t jumpHeight = 3;
	uint32_t delayFor = 0;
	Smer smer = nikamor;
	makeTerrain(map, posX, posY, size);
	drawMap(map);

	while(1)
	{
		if(playerMoved && emptyBlockChanged) // player moved
		{
			myfile << "PLAYER MOVED\n";
			drawMap(newEmptyBlockPos, newPlayerPos, myfile, map);
			playerMoved = false;
			emptyBlockChanged = false;
		}
		if(emptyBlockChanged && !playerMoved) // player destroyed a block
		{
			drawMap(newEmptyBlockPos, map);
			emptyBlockChanged = false;
		}
		
		enable_raw_mode();
		if(kbhit())
		{
			char inp = getch();
			switch(inp)
			{
				case 'w':
					smer = gor;
					break;
				case 's':
					smer = dol;
					break;
				case 'a':
					if(posX > 0)
						if(map[posY][posX-1] == " ")
						{
							myfile << "sem v a\n";
							newEmptyBlockPos[0] = posY;
							newEmptyBlockPos[1] = posX;
							--posX;
							newPlayerPos[0] = posY;
							newPlayerPos[1] = posX;
							playerMoved = true;
							emptyBlockChanged = true;
						}
					smer = levo;
					break;
				case 'd':
					if(posX < size.ws_col-1)
						if(map[posY][posX+1] == " ")
						{
							newEmptyBlockPos[0] = posY;
							newEmptyBlockPos[1] = posX;
							++posX;
							newPlayerPos[0] = posY;
							newPlayerPos[1] = posX;
							playerMoved = true;
							emptyBlockChanged = true;
						}
					smer = desno;
					break;
				case ' ':
					if(posY <= size.ws_row-1 && posY > 0)
						if(map[posY-1][posX] == " " && map[posY+1][posX] != " ")jump = true; //std::cout << "JUMP!";
					break;
				case 'e':
					switch(smer)
					{
						case gor:
							if(posY > 0)
								if(map[posY-1][posX] != " ")
								{
									inventar.push_back(map[posY-1][posX]);
									newEmptyBlockPos[0] = posY-1;
									newEmptyBlockPos[1] = posX;
									emptyBlockChanged = true;
								}
							break;
						case dol:
							if(posY < size.ws_row-1)
								if(map[posY+1][posX] != " ")
								{
									inventar.push_back(map[posY+1][posX]);
									newEmptyBlockPos[0] = posY+1;
									newEmptyBlockPos[1] = posX;
									emptyBlockChanged = true;
								}
							break;
						case levo:
							if(posX > 0)
								if(map[posY][posX-1] != " ")
								{
									inventar.push_back(map[posY][posX-1]);
									newEmptyBlockPos[0] = posY;
									newEmptyBlockPos[1] = posX-1;
									emptyBlockChanged = true;
								}
							break;
						case desno:
							if(posX < size.ws_col-1)
								if(map[posY][posX+1] != " ")
								{
									inventar.push_back(map[posY][posX+1]);
									newEmptyBlockPos[0] = posY;
									newEmptyBlockPos[1] = posX+1;
									emptyBlockChanged = true;
								}
					}
			}
		}
		disable_raw_mode();
		

		if(posY > 0)
			if(jump && map[posY-1][posX] == " ")
			{
				if(jumpHeight > 0)
				{
					if(delayFor >= 10000)
					{
						jumpHeight--;
						newEmptyBlockPos[0] = posY;
						newEmptyBlockPos[1] = posX;
						--posY;
						newPlayerPos[0] = posY;
						newPlayerPos[1] = posX;
						emptyBlockChanged = true;
						playerMoved = true;
						delayFor = 0;
					}
					else delayFor++;
				}
				else
				{
					jump = false;
					jumpHeight = 3;
				}
			}
		else
		{
			jump = false;
			jumpHeight = 3;
		}

		if(!jump && !playerMoved && !emptyBlockChanged)
			if(posY < size.ws_col-1)
				if(map[posY+1][posX] == " ")
				{
					newEmptyBlockPos[0] = posY;
					newEmptyBlockPos[1] = posX;
					++posY;
					newPlayerPos[0] = posY;
					newPlayerPos[1] = posX;
					emptyBlockChanged = true;
					playerMoved = true;
				}
	}
	//std::string output = BLACK + "a" + RED + "a" + GREEN + "a" + '\n';
	//std::cout << output;
	return 0;
}