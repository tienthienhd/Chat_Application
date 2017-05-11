#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <stdio.h>
#include <string.h>

enum { MAX_WORD_LEN = 20 };
enum { MAX_LINE_LEN = 29 };
#define DEFAULT_BUFLEN 0x10240

int IsWhitespace(int ch);
int ReadWord(char *word, int &wordLen);
void ClearLine(char *line, int *lineLen, int *numWords);
void AddWord(const char *word, char *line, int *lineLen);
void FormatText(char *buf, int &numLine);
void GetSizeWindow(void);
void DrawInputBox(void);
void MoveInputBox(void);
void FillLineBackGround(int fromPos, int toPos);
void InputSendMessage(int &numLine, char* sendbuf);
void PrintSendMessage(const int numLine, const char* sendbuf);
void PrintReceiveMessage(const int lenMess, const char* recvbuf);
void BackToInputBox(void);

#endif // !TEXTFORMATTER_H
