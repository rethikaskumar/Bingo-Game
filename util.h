#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <iostream>
#include <vector>

using namespace std;

typedef vector< vector<int> > Matrix;

class Utils
{
private:
    static Utils* instance;

    // Private constructor so that no objects can be created.
    Utils() {};

public:
    enum platformType { client, server, invalid };

    static Utils* getInstance()
    {
        if (!instance)
        {
            instance = new Utils();
        }
        return instance;
    }

    platformType validateInput(int argc, char** argv, std::string& serverName);
    void stringifyMatrix(const Matrix& m, string& str); // parameter order: source, destination
    void destringifyMatrix(const string& str, int rows, int cols, Matrix& m);
};

#endif // UTIL_H_INCLUDED
