using namespace std;
#include <sstream>
#include "util.h"

Utils::platformType Utils::validateInput(int argc, char** argv, string& serverName)
{
    platformType type = invalid;
    if( (2 == argc) || (4 == argc))
    {
        if(2 == argc) // server
        {
            string argument = "-server";
            if( 0 == argument.compare(argv[1]))
            {
                type = server;
            }
        }
        else // client
        {
            string argument = "-client";
            if( 0 == argument.compare(argv[1]))
            {
                argument = "-serverhost";
                if( 0 == argument.compare(argv[2]))
                {
                    serverName = argv[3];
                    type = client;
                }
            }
        }
    }
    return type;
}

//stringifyMatrix()
void Utils::stringifyMatrix(const Matrix& m, string& str)
{
    stringstream ss;
    str.clear();
    for( size_t i = 0; i < m.size(); i++ )
    {
        for( size_t j = 0; j < m[i].size(); j++)
        {
            ss << m[i][j] << " ";
        }
    }
    str = ss.str();
}

//destringifyMatrix()
void Utils::destringifyMatrix(const string& str, int rows, int cols, Matrix& m)
{
    stringstream ss(str);
    stringstream sstemp;
    string intermediate;

    // Tokenizing w.r.t. space ' '
    vector <int> rowVector;
    int j = 0;
    int num;
    rowVector.clear();
    while(getline(ss, intermediate, ' '))
    {
        sstemp << intermediate;
        sstemp >> num;
        rowVector.push_back(num);
        if( ++j == cols )
        {
            m.push_back(rowVector);
            rowVector.clear();
            j = 0;
        }
        sstemp.clear();
    }
}

