#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Definicije.h"
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

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

void makeTerrain(std::vector<std::vector<std::string>> &map, int posX, int& posY)
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
			if(height + 1 <= 49) 
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

void drawMap(std::vector<std::vector<std::string>> &map)
{
	system("clear");
	for(int i = 0; i < map.size(); i++)
	{
		for(int j = 0; j < map[0].size(); j++)
			std::cout << map[i][j];
		std::cout << '\n';
	}
}

int main()
{
	srand(time(NULL));
	std::vector<std::vector<std::string>> map(50, std::vector<std::string>(50, " "));
	int posX = map[0].size()/2, posY = 0;
	bool jump = false, up = true;
	uint8_t jumpHeight = 3;
	makeTerrain(map, posX, posY);
	while(1)
	{
		drawMap(map);
		
		enable_raw_mode();
		if(kbhit())
		{
			char inp = getch();
			switch(static_cast<int>(inp))
			{
				case 97:
					if(posX > 0)
					{
						map[posY][posX] = " ";
						posX--;
						map[posY][posX] = PLAYER;
					}
				case 100:
					if(posX < 49)
					{
						map[posY][posX] = " ";
						posX++;
						map[posY][posX] = PLAYER;
					}
				case 32:
					if(posY < 49 && posY > 0)
						if(map[posY-1][posX] == " " && map[posY+1][posX] != " ") jump = true;
			}
		}
		disable_raw_mode();
		

		if(posY > 0)
		{
			if(jump && map[posY-1][posX] == " ")
			{
				if(jumpHeight > 0)
				{
					jumpHeight--;
					map[posY][posX] = " ";
					posY--;
					map[posY][posX] = PLAYER;
				}
				else
				{
					jump = false;
					jumpHeight = 3;
				}
			}
		}
		else
		{
			jump = false;
			jumpHeight = 3;
		}

		if(!jump)
		{
			// padaj
			if(posY > 49)
				if(map[posY+1][posX] == " ")
				{
					map[posY][posX] = " ";
					posY++;
					map[posY][posX] = PLAYER;
				}
		}
	}
	//std::string output = BLACK + "a" + RED + "a" + GREEN + "a" + '\n';
	//std::cout << output;
	return 0;
}