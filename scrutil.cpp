#include <iostream>
#include <sstream>
#include <conio.h>
#include <vector>
#include <winsock2.h>
#include "scrutil.h"
#include <wincon.h>

using namespace std;

//clrscr()
void ScrUtils::clrscr()
{
    COORD coordScreen = { 0, 0 };
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwConSize, cCharsWritten;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}

//gotoxy
void ScrUtils::gotoxy( unsigned x, unsigned y )
{
    COORD coord;
    coord.X = x; coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

//setTextBkColorGreen()
void ScrUtils::setTextBkColorGreen()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),BACKGROUND_GREEN|BACKGROUND_INTENSITY);
}

void ScrUtils::SetConsoleColour(WORD* Attributes, DWORD Colour)
{
    CONSOLE_SCREEN_BUFFER_INFO Info;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout, &Info);
    *Attributes = Info.wAttributes;
    SetConsoleTextAttribute(hStdout, Colour);
}

void ScrUtils::ResetConsoleColour(WORD Attributes)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Attributes);
}

//getOriginalScreenAttr()
void ScrUtils::getOriginalScreenAttr(WORD& attr)
{
    CONSOLE_SCREEN_BUFFER_INFO Info;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout, &Info);
    attr = Info.wAttributes;
}

//resetScreenAttributes()
void ScrUtils::resetScreenAttributes(WORD attr)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attr);
}

//box()
void ScrUtils::box( unsigned x, unsigned y, unsigned sx, unsigned sy, const char* text, int textcolor )
{
    unsigned i,j,m;
    m=(sx-x);                       //differential
    j=m/8;                          //adjust
    j=j-1;                          //more adjustment
    gotoxy(x,y);cout << "É";        //Top left corner of box
    gotoxy(sx,y);cout << "»";      //Top right corner of box
    gotoxy(x,sy);cout << "È";      //Bottom left corner of box
    gotoxy(sx,sy);cout << "¼";     //Bottom right corner of box

    for (i=x+1;i<sx;i++)
    {
        gotoxy(i,y);cout << "Í";     // Top horizontal line
        gotoxy(i,sy);cout << "Í";    // Bottom Horizontal line
    }

    for (i=y+1;i<sy;i++)
    {
        gotoxy(x,i);cout << "º";     //Left Vertical line
        gotoxy(sx,i);cout << "º";    //Right Vertical Line
    }

    gotoxy(x+j,y);

    if( textcolor )
    {
        WORD origScrAttr;
        getOriginalScreenAttr(origScrAttr);
        setTextBkColorGreen();
        for(i = 0; i < (unsigned)textcolor; i++)
        {
            if( 0 != i )
            {
                cout << text[i*2-1];
            }
            cout << text[i*2];
        }
        resetScreenAttributes(origScrAttr);
        cout << &text[i*2-1]; //put Title
    }
    else
    {
        cout << text;
    }
    gotoxy(1,24);
}

//printMatrix()
void ScrUtils::printMatrix(const Matrix& m, const Matrix& color, int rows, int cols)
{
    WORD origScrAttr;
    unsigned x = 45, y = 12; //40,8,120,36    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),BACKGROUND_GREEN|BACKGROUND_INTENSITY);

    getOriginalScreenAttr(origScrAttr);
    for( int i = 0; i < rows; i++ )
    {
        for( int j = 0; j < cols; j++ )
        {
            gotoxy(x, y);
            resetScreenAttributes(origScrAttr);
            if(color[i][j])
            {
                setTextBkColorGreen();
            }
            cout << m[i][j];
            x += 10;
        }
        x = 45;
        y += 5;
    }
    resetScreenAttributes(origScrAttr);
}


