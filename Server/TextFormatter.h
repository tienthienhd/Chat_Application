#include <stdio.h>
#include <string.h>

enum { MAX_WORD_LEN = 20 };
enum { MAX_LINE_LEN = 29 };

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

    if(ch == '\n')
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
    strcat(line, word);
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

    while(1)
    {
        if(ReadWord(word, wordLen) == -1)
        {
			flag = 1;
        }

        if ((wordLen + 1 + lineLen) > MAX_LINE_LEN)
        {
            strcat(buf, line);
            strcat(buf, "\n");
			numLine++;
            ClearLine(line, &lineLen, &numWords);
        }

        AddWord(word, line, &lineLen);
        numWords++;

		if (flag == 1)
		{
			numLine++;
			strcat(buf, line);
			break;
		}
    }
}