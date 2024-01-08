#include<iostream>
#include<vector>
#include<random>
#include<time.h>


using namespace std;

typedef vector< vector<int> > Matrix;

vector<int> CreateRange(int low, int high)
{
    vector<int> r1;
    int c=0;
    while(c!=5)
    {
        int flag=1;
        int range = high - low + 1;
        int num = rand() % range + low;
        for (auto i = r1.begin(); i != r1.end(); ++i)
        {
            if(*i==num)
                flag=0;
        }
        if(flag==1)
        {
            //cout<<num<<" ";
            r1.push_back(num);
            c++;
        }
    }
    return r1;
}

int main()
{
    /*
    Matrix m1 { vector<int> r1={ 1, 2, 3, 4, 5 },
                  vector<int> r2={6, 7, 8, 9, 10 },
                  vector<int> r3={11, 12, 13, 14, 15 },
                  vector<int> r4={16, 17, 18, 19, 20 },
                  vector<int> r5={21, 22, 23, 24, 25 }
                };

    Matrix m2 { vector<int>{ 2, 4, 6, 8, 10 },
                  vector<int>{ 12, 14, 16, 18, 20 },
                  vector<int>{ 22, 24, 25, 23, 21 },
                  vector<int>{ 19, 17, 15, 13, 11 },
                  vector<int>{ 9, 7, 5, 3, 1 }
                };
    //cout<<m1*/
    Matrix m1{CreateRange(1, 5), CreateRange(6, 10), CreateRange(11, 15), CreateRange(16, 20), CreateRange(21, 25)};
    Matrix m2{CreateRange(1, 5), CreateRange(6, 10), CreateRange(11, 15), CreateRange(16, 20), CreateRange(21, 25)};

    for (int i = 0; i < m1.size(); i++)
    {
        for (int j = 0; j < m1[i].size(); j++)
            cout << m1[i][j] << " ";
        cout << endl;
    }

    for (int i = 0; i < m2.size(); i++)
    {
        for (int j = 0; j < m2[i].size(); j++)
            cout << m2[i][j] << " ";
        cout << endl;
    }
}
