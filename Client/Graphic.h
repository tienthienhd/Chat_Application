#pragma once

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0502

#include <windows.h>
#include <iostream>
#define BLACK 0
#define LIGHTGRAY 8
#define BLUE 9
#define GREEN 10
#define RED 12
#define YELLOW 14
#define WHITE 15


void SetSizeWindow(void)
{
	system("MODE CON COLS=33");

  	HWND console = GetConsoleWindow();
  	RECT r;
  	GetWindowRect(console, &r); //stores the console's current dimensions

  	//MoveWindow(window_handle, x, y, width, height, redraw_window);
  	MoveWindow(console, r.left, r.top, 297, 360, TRUE);
}

void textcolor(WORD color)
{
	HANDLE hConsoleOutput;
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	GetConsoleScreenBufferInfo(hConsoleOutput, &screen_buffer_info);

	WORD wAttributes = screen_buffer_info.wAttributes;
	color &= 0x000f;
	wAttributes &= 0xfff0;
	wAttributes |= color;

	SetConsoleTextAttribute(hConsoleOutput, wAttributes);
}
void textbackground(WORD color)
{
	HANDLE hConsoleOutput;
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	GetConsoleScreenBufferInfo(hConsoleOutput, &screen_buffer_info);

	WORD wAttributes = screen_buffer_info.wAttributes;
	color &= 0x000f;
	color <<= 4;
	wAttributes &= 0xff0f;
	wAttributes |= color;

	SetConsoleTextAttribute(hConsoleOutput, wAttributes);
}
void gotoxy(short x, short y)
{
	HANDLE hConsoleOutput;
	COORD Cursor_an_Pos = { x,y };
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsoleOutput, Cursor_an_Pos);
}
void clrscr(int c = BLACK, int top = 0, int down = 24)
{
	textbackground(c);
	gotoxy(0, top);
	for (int i = top; i < down; i++)
		std::cout << "                                                                                 ";
	gotoxy(0, top);
}
void HidePointer()
{
	HANDLE hOut;
	CONSOLE_CURSOR_INFO ConCurInf;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	ConCurInf.dwSize = 10;
	ConCurInf.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &ConCurInf);
}
void ShowPointer()
{
	HANDLE hOut;
	CONSOLE_CURSOR_INFO ConCurInf;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	ConCurInf.dwSize = 10;
	ConCurInf.bVisible = TRUE;
	SetConsoleCursorInfo(hOut, &ConCurInf);
}

int wherex(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
	{
		return -1;
	}

	return csbi.dwCursorPosition.X;
}

int wherey(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
	{
		return -1;
	}

	return csbi.dwCursorPosition.Y;
}