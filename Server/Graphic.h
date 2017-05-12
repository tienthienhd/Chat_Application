#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <Windows.h>
#define BLACK 0
#define LIGHTGRAY 8
#define BLUE 9
#define GREEN 10
#define RED 12
#define YELLOW 14
#define WHITE 15

void SetSizeWindow(void);
void TextColor(WORD color);
void TextBackGround(WORD color);
void Gotoxy(short x, short y);
void HidePointer(void);
int Wherex(void);
int Wherey(void);

#endif