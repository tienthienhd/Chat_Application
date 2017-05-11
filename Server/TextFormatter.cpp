#include "TextFormatter.h"
#include "Graphic.h"
#include <iostream>
#include <string>

using namespace std;

int g_PosOutput = 3;
int g_PosInput;
int g_Columns, g_Rows;

int IsWhitespace(int ch)
{
	return (ch == ' ') || (ch == '\t');
}

int ReadWord(char *word, int &wordLen)
{
	int ch, pos = 0;
	ch = getchar();
	while (IsWhitespace(ch))
		ch = getchar();
	while (!IsWhitespace(ch) && (ch != '\n'))
	{
		if (pos < MAX_WORD_LEN)
		{
			word[pos] = (char)ch;
			pos++;
		}
		ch = getchar();
	}
	word[pos] = '\0';
	wordLen = pos;

	if (ch == '\n')
	{
		return -1;
	}

	return 0;
}

void ClearLine(char *line, int *lineLen, int *numWords)
{
	line[0] = '\0';
	*lineLen = 0;
	*numWords = 0;
}

void AddWord(const char *word, char *line, int *lineLen)
{
	if (*lineLen > 0)
	{
		line[*lineLen] = ' ';
		line[*lineLen + 1] = '\0';
		(*lineLen)++;
	}
	
	strcat_s(line, MAX_LINE_LEN + 1, word);
	(*lineLen) += strlen(word);
}

void FormatText(char *buf, int &numLine)
{
	buf[0] = '\0';

	char word[MAX_WORD_LEN + 1];
	int wordLen, flag = 0;
	char line[MAX_LINE_LEN + 1];
	int lineLen = 0;
	int numWords = 0;
	ClearLine(line, &lineLen, &numWords);

	while (1)
	{
		if (ReadWord(word, wordLen) == -1)
		{
			flag = 1;
		}

		if ((wordLen + 1 + lineLen) > MAX_LINE_LEN)
		{
			strcat_s(buf, DEFAULT_BUFLEN, line);
			strcat_s(buf, DEFAULT_BUFLEN, "\n");
			numLine++;
			ClearLine(line, &lineLen, &numWords);
		}

		AddWord(word, line, &lineLen);
		numWords++;

		if (flag == 1)
		{
			numLine++;
			strcat_s(buf, DEFAULT_BUFLEN, line);
			break;
		}
	}
}

void GetSizeWindow(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	g_Columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	g_Rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	g_PosInput = g_Rows - 2;
}

/*	Ve hop thoai de nhap doan van ban chat,
dong thoi di chuyen con tro den vi tri nhap doan van ban chat*/
void DrawInputBox(void)
{
	Gotoxy(0, g_PosInput);
	TextColor(WHITE);
	TextBackGround(BLACK);
	cout << "->";
	TextBackGround(WHITE);
	TextColor(BLACK);
	cout << string(g_Columns - 2, ' ');
	cout << string(g_Columns, ' ');
	Gotoxy(2, g_PosInput);
}

/*	Di chuyen hop thoai de nhap doan van ban chat lui xuong 1 dong,
dong thoi di chuyen con tro den vi tri nhap doan van ban chat*/
void MoveInputBox(void)
{
	g_PosInput = g_PosOutput + 2;
	Gotoxy(0, g_PosInput - 1);
	TextBackGround(BLACK);
	cout << string(g_Columns, ' ');
	DrawInputBox();
}

void FillLineBackGround(int fromPos, int toPos)
{
	Gotoxy(fromPos, g_PosOutput);
	cout << string(toPos - fromPos, ' ');
	Gotoxy(fromPos, g_PosOutput);
}

void InputSendMessage(int &numLine, char* sendbuf)
{
	numLine = 0;

	if (g_PosOutput < g_Rows - 3) {
		DrawInputBox();
		FormatText(sendbuf, numLine);
		Gotoxy(0, 0);//Can thiet de giu nguyen trang thai man hinh
	}
	else {
		MoveInputBox();
		FormatText(sendbuf, numLine);
		Gotoxy(0, g_PosInput - g_Rows);//Can thiet de giu nguyen trang thai man hinh
	}
}

void PrintSendMessage(const int numLine, const char* sendbuf)
{
	TextBackGround(BLUE);
	TextColor(WHITE);
	g_PosOutput++;

	if (g_PosOutput >= g_Rows - 3) {
		MoveInputBox();
		TextBackGround(BLUE);
		TextColor(WHITE);
	}

	if (numLine == 1) {
		Gotoxy(short(g_Columns - strlen(sendbuf)), g_PosOutput);
		cout << sendbuf;
	}
	else {
		FillLineBackGround(4, g_Columns);
		const char *p = sendbuf;
		while (*p != '\0')
		{
			putchar(*p);
			if (*p == '\n') {
				g_PosOutput++;

				if (g_PosOutput >= g_Rows - 3) {
					MoveInputBox();
					TextBackGround(BLUE);
					TextColor(WHITE);
				}

				FillLineBackGround(4, g_Columns);
			}
			p++;
		}
	}

	/* In them mot dong trong giua cac tin nhan*/
	g_PosOutput++;

	if (g_PosOutput >= g_Rows - 3) {
		MoveInputBox();
		TextBackGround(BLUE);
		TextColor(WHITE);
	}
}

void PrintReceiveMessage(const int lenMess, const char* recvbuf)
{
	TextBackGround(LIGHTGRAY);
	TextColor(BLACK);

	++g_PosOutput;

	if (g_PosOutput >= g_Rows - 3) {
		MoveInputBox();
		TextBackGround(LIGHTGRAY);
		TextColor(BLACK);
	}

	Gotoxy(0, g_PosOutput);

	if (lenMess < g_Columns - 4) {
		FillLineBackGround(0, lenMess);
	}
	else {
		FillLineBackGround(0, g_Columns - 4);
	}

	const char *p = recvbuf;
	while (*p != '\0') {
		putchar(*p);

		if (*p == '\n') {
			g_PosOutput++;

			if (g_PosOutput >= g_Rows - 3) {
				MoveInputBox();
				TextBackGround(LIGHTGRAY);
				TextColor(BLACK);
			}
			FillLineBackGround(0, g_Columns - 4);
		}
		p++;
	}

	g_PosOutput = Wherey();

	//Tao khoang trong giua cac tin nhan
	g_PosOutput++;
	if (g_PosOutput >= g_Rows - 3)
	{
		MoveInputBox();
		TextBackGround(LIGHTGRAY);
		TextColor(BLACK);
	}
}

void BackToInputBox(void)
{
	if (g_PosOutput < g_Rows - 3) {
		DrawInputBox();
	}
	else {
		MoveInputBox();
	}
}