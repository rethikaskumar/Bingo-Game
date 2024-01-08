#ifndef SCRUTIL_H_INCLUDED
#define SCRUTIL_H_INCLUDED

#include <iostream>
#include <vector>

using namespace std;

typedef vector< vector<int> > Matrix;

class ScrUtils
{
private:
    static ScrUtils* instance;

    // Private constructor so that no objects can be created.
    ScrUtils() {};

public:

    static ScrUtils* getInstance()
    {
        if (!instance)
        {
            instance = new ScrUtils();
        }
        return instance;
    }

    void clrscr();
    void gotoxy( unsigned x, unsigned y );
    void setTextBkColorGreen();
    void SetConsoleColour(WORD* Attributes, DWORD Colour);
    void ResetConsoleColour(WORD Attributes);
    void getOriginalScreenAttr(WORD& attr);
    void resetScreenAttributes(WORD attr);
    void box( unsigned x, unsigned y, unsigned sx, unsigned sy, const char* text, int textcolor );
    void printMatrix(const Matrix& m, const Matrix& color, int rows, int cols);
};

#endif // SCRUTIL_H_INCLUDED
