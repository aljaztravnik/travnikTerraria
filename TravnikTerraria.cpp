#include "TravnikTerraria.h"

void TravnikTerraria::enable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(0, TCSANOW, &term);
}

void TravnikTerraria::disable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

bool TravnikTerraria::kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

char TravnikTerraria::getch()
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

void TravnikTerraria::makeTerrain()
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

void TravnikTerraria::moveCursor(std::ostream& os, int row, int col)
{
  os << "\033[" << row << ";" << col << "H";
}

void TravnikTerraria::drawWholeMap()
{
	for(int i = 0; i < map.size(); i++)
	{
		for(int j = 0; j < map[0].size(); j++)
			std::cout << map[i][j];
		std::cout << '\n';
	}
}

void TravnikTerraria::drawMap_playerAndEmptyBlock()
{
	map[newEmptyBlockPos[0]][newEmptyBlockPos[1]] = " ";
	map[newPlayerPos[0]][newPlayerPos[1]] = PLAYER;

	moveCursor(std::cout, newEmptyBlockPos[0], newEmptyBlockPos[1]+1);
	std::cout << " ";
	moveCursor(std::cout, newPlayerPos[0], newPlayerPos[1]+1);
	std::cout << PLAYER << '\n';
}

void TravnikTerraria::drawMap_emptyBlock()
{
	map[newEmptyBlockPos[0]][newEmptyBlockPos[1]] = " ";

	moveCursor(std::cout, newEmptyBlockPos[0], newEmptyBlockPos[1]+1);
	std::cout << " \n";
}

TravnikTerraria::TravnikTerraria()
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	map.resize(size.ws_row, std::vector<std::string>(size.ws_col, " "));
	posX = map[0].size()/2;
	posY = 0;
	jump = false;
	playerMoved = false;
	emptyBlockChanged = false;
	blockChanged = false;
	jumpHeight = 3;
	delayFor = 0;
	smer = nikamor;
}

void TravnikTerraria::init()
{
	makeTerrain();
	drawWholeMap();
}

void TravnikTerraria::run()
{
	if(playerMoved && emptyBlockChanged) // player moved
	{
		drawMap_playerAndEmptyBlock();
		playerMoved = false;
		emptyBlockChanged = false;
	}
	if(emptyBlockChanged && !playerMoved) // player destroyed a block
	{
		drawMap_emptyBlock();
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