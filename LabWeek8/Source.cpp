#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#define scount 7
#define screen_x 80
#define screen_y 25
HANDLE wHnd;
HANDLE rHnd;
DWORD fdwMode;
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD bufferSize = { screen_x,screen_y };
COORD characterPos = { 0,0 };
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };
DWORD aj = 7;
struct star
{
	bool active = true;
	int  x = 0, y = 0;
};

star comet[scount];

char cursor(int x, int y)
{
	HANDLE hStd = GetStdHandle(STD_OUTPUT_HANDLE);
	char buf[2]; COORD c = { x,y }; DWORD num_read;
	if (
		!ReadConsoleOutputCharacter(hStd, (LPTSTR)buf, 1, c, (LPDWORD)&num_read))
		return '\0';
	else
		return buf[0];
}

void setcursor(bool visible)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = 20;
	SetConsoleCursorInfo(console, &lpCursor);
}

void setcolor(int fg, int bg)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, bg * 16 + fg);
}

int setMode()
{
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT |
		ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}

int setConsole(int x, int y)
{
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}
void clear_buffer()
{
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 0;
		}
	}
}
void fill_buffer_to_console()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos,
		&windowSize);
}

void init_star()
{
	for (int i = 0; i < scount; i++) 
	{
			comet[i].x = rand() % screen_x;
			comet[i].y = rand() % screen_y;
	}
}


void fill_star_to_buffer()
{
	for (int i = 0; i < scount; ++i) {
			consoleBuffer[comet[i].x + screen_x * comet[i].y].Char.AsciiChar = '*';
			consoleBuffer[comet[i].x + screen_x * comet[i].y].Attributes = 7;
		}
}

void draw_ship(int x, int y)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	consoleBuffer[(c.X - 2) + screen_x * c.Y].Char.AsciiChar = '<';
	consoleBuffer[(c.X - 1) + screen_x * c.Y].Char.AsciiChar = '-';
	consoleBuffer[c.X + screen_x * c.Y].Char.AsciiChar = '0';
	consoleBuffer[(c.X + 1) + screen_x * c.Y].Char.AsciiChar = '-';
	consoleBuffer[(c.X + 2) + screen_x * c.Y].Char.AsciiChar = '>';
	
	consoleBuffer[(c.X - 2) + screen_x * c.Y].Attributes = aj;
	consoleBuffer[(c.X - 1) + screen_x * c.Y].Attributes = aj;
	consoleBuffer[c.X + screen_x * c.Y].Attributes = aj;
	consoleBuffer[(c.X + 1) + screen_x * c.Y].Attributes = aj;
	consoleBuffer[(c.X + 2) + screen_x * c.Y].Attributes = aj;
	
}

void draw_score(int x, int y, int score)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	setcolor(7, 0);
	printf("Life : %d", score);
}

int main()
{
	int count = 10;
	srand(time(NULL));
	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setcursor(0);
	setConsole(screen_x, screen_y);
	init_star();
	setMode();
	while (play)
	{
		draw_score(65,2,count);
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {
				if (eventBuffer[i].EventType == KEY_EVENT &&
					eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'c') {
						aj = rand() % 10 + 1;
					}
				}
				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					if (eventBuffer[i].Event.MouseEvent.dwButtonState &
						FROM_LEFT_1ST_BUTTON_PRESSED) {
						aj = rand() % 10 + 1;
					}
					else if (eventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
						clear_buffer();
						fill_star_to_buffer();
						if (cursor(posx, posy) != '*' && cursor(posx+1, posy) != '*' && cursor(posx+2, posy) != '*' && cursor(posx-1, posy) != '*' && cursor(posx-2, posy) != '*')
						{
							draw_ship(posx, posy);
						}
						else 
						{
							draw_ship(posx, posy);
							init_star();
							count = count - 1;
							draw_score(65, 2, count);
							if (count == 0)
							{
								play = false;
							}
						}
						fill_buffer_to_console();
					}
				}
			}
			delete[] eventBuffer;
		}
		Sleep(1);
	}
	return 0;
}