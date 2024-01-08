#include <iostream>
#include <sstream>
#include <conio.h>
#include <vector>
#include "sockclient.h"
#include "sockserver.h"
#include "util.h"
#include "scrutil.h"
#include <wincon.h>
#include<fstream>
#include<time.h>
#include<random>


using namespace std;
const int maxClients = 2;
const size_t matrixSize = 5;
const int victoryBingoCount = 5;
const int losingBingoCount = -1;

//commands to slave from master
const int cmdUndefined = 0;
const int cmdSendMatrix = 1;
const int cmdGetUserInput = 2;
const int cmdRecalculateMatrix = 3;
const int cmdWon = 4;
const int cmdLost = 5;

//Initialize pointer to zero so that it can be initialized in first call to getInstance
Utils *Utils::instance = 0;
ScrUtils *ScrUtils::instance = 0;

void copyMatrix(Matrix& matrixOut, const Matrix& matrixIn);
void updateMatrixAndColor(int numInp, Matrix& matrixOut, Matrix& colorout);
unsigned __stdcall serverThread(void* ts);
int calculateBingoCount(Matrix& colorout);
void initializeMatrix( Matrix& matrix, size_t matrixSize );
vector<int> CreateRange(int , int);

typedef struct
{
    int clientId;
    int command;
    int userInput;
    int bingoCount;
    Matrix c; // init color
    Matrix m; // init matrix
    Socket *s;
    HANDLE masterWait;
    HANDLE slaveWait;
    size_t size;
    string clientInfo;
} ServThreadStruct;

ServThreadStruct sStruct[maxClients];


/*
const Matrix m1 { vector<int>{ 1, 2, 3, 4, 5 },
                  vector<int>{6, 7, 8, 9, 10 },
                  vector<int>{11, 12, 13, 14, 15 },
                  vector<int>{16, 17, 18, 19, 20 },
                  vector<int>{21, 22, 23, 24, 25 }
                };

const Matrix m2 { vector<int>{ 2, 4, 6, 8, 10 },
                  vector<int>{ 12, 14, 16, 18, 20 },
                  vector<int>{ 22, 24, 25, 23, 21 },
                  vector<int>{ 19, 17, 15, 13, 11 },
                  vector<int>{ 9, 7, 5, 3, 1 }
                };
*/
Matrix m1={CreateRange(1, 5), CreateRange(6, 10), CreateRange(11, 15), CreateRange(16, 20), CreateRange(21, 25)};
Matrix m2={CreateRange(1, 5), CreateRange(6, 10), CreateRange(11, 15), CreateRange(16, 20), CreateRange(21, 25)};

const Matrix initColor { vector<int>{ 0, 0, 0, 0, 0 },
                         vector<int>{ 0, 0, 0, 0, 0 },
                         vector<int>{ 0, 0, 0, 0, 0 },
                         vector<int>{ 0, 0, 0, 0, 0 },
                         vector<int>{ 0, 0, 0, 0, 0 }
                       };

Matrix m[] = {m1, m2};

//initializeMatrix()
void initializeMatrix( Matrix& matrix, size_t matrixSize )
{
    vector<int> tmp;
    for(size_t i = 0; i < matrixSize; i++)
    {
        for(size_t j = 0; j < matrixSize; j++)
        {
            tmp.push_back(0);
        }
        matrix.push_back(tmp);
        tmp.clear();
    }
}

//initializeThreadStruct()
void initializeThreadStruct(ServThreadStruct &ts, size_t matrixSize)
{
    /*
    vector<int> tmp;
    for(size_t i = 0; i < matrixSize; i++)
    {
        for(size_t j = 0; j < matrixSize; j++)
        {
            tmp.push_back(0);
        }
        ts.m.push_back(tmp);
        ts.c.push_back(tmp);
        tmp.clear();
    }*/
    initializeMatrix(ts.m, matrixSize);
    initializeMatrix(ts.c, matrixSize);
    ts.s = NULL;
    ts.masterWait = (HANDLE)NULL;
    ts.slaveWait = (HANDLE)NULL;
    ts.size = matrixSize;
    ts.clientId = 0;
    ts.command = cmdUndefined;
    ts.bingoCount = 0;
    ts.userInput = -1;
}

// usage: GameBingo -server
//        GameBingo -client -serverhost <hostname>
int main(int argc, char** argv)
{
    //create events for threads to wait on
    HANDLE ghEvent[2*maxClients];
    ghEvent[0] = CreateEvent(
        NULL,               // default security attributes
        FALSE,              // manual-reset event
        FALSE,              // initial state is nonsignaled
        TEXT("EvCli1")      // object name
    );
    ghEvent[1] = CreateEvent(
        NULL,               // default security attributes
        FALSE,              // manual-reset event
        FALSE,              // initial state is nonsignaled
        TEXT("EvCli2")      // object name
    );
    ghEvent[2] = CreateEvent(
        NULL,               // default security attributes
        FALSE,              // manual-reset event
        FALSE,              // initial state is nonsignaled
        TEXT("EvSer1")      // object name
    );
    ghEvent[3] = CreateEvent(
        NULL,               // default security attributes
        FALSE,              // manual-reset event
        FALSE,              // initial state is nonsignaled
        TEXT("EvSer2")      // object name
    );

    //create instances for utility functions
    Utils *util = Utils::getInstance();
    ScrUtils *scrutil = ScrUtils::getInstance();

    //clear screen
    scrutil->clrscr();

    //validate inputs
    string serverhost;
    Utils::platformType appType;
    appType = util->validateInput(argc, argv, serverhost);

    switch(appType)
    {
        case Utils::client:
            try
            {
                stringstream ss;
                Matrix matrixIn;
                Matrix colorIn;
                string hostname, sendStr, recvStr;
                BOOL toExit = FALSE;
                int bingoCnt;
                SockClient s(serverhost, 8888);
                while( !toExit )
                {
                    //wait for server command to initiate user command entry
                    scrutil->gotoxy(0,0);
                    recvStr.clear();
                    s.recvString(recvStr);

                    //wait to receive matrix from server
                    if( 0 == recvStr.compare("MATRIX"))
                    {
                        recvStr.clear();
                        s.recvString(recvStr);
                        util->destringifyMatrix(recvStr, 5, 5, matrixIn);

                        //wait to receive color attributes from server
                        recvStr.clear();
                        s.recvString(recvStr);
                        util->destringifyMatrix(recvStr, 5, 5, colorIn);
                        recvStr.clear();

                        // wait to receive bingo count from server
                        recvStr.clear();
                        s.recvString(recvStr);
                        ss.str().clear();
                        ss.clear();
                        ss << recvStr;
                        ss >> bingoCnt;

                        scrutil->box(40, 8, 95, 36, "B I N G O", bingoCnt);
                        scrutil->printMatrix(matrixIn, colorIn, 5, 5);
                        scrutil->gotoxy(0, 40);
                        matrixIn.clear();
                        colorIn.clear();
                    }
                    else if( 0 == recvStr.compare("USRINPUT"))
                    {
                        try
                        {
                            //enable key input and play Bingo
                            scrutil->gotoxy(0, 40);
                            cout << "Enter a number from the matrix to play, or type EXIT to exit program:  " ;
                            sendStr.clear();
                            getline(std::cin, sendStr);
                            if( 0 != sendStr.compare("EXIT"))
                            {
                                s.sendString(sendStr);
                                sendStr.clear();
                            }
                            else
                            {
                                int x=0;
                                throw x;
                                //toExit = TRUE;
                            }
                        }
                        catch(int x)
                        {
                            ofstream logf("exitlog.txt", ios::app);
                            logf<<"User exited game. No winners\n"<<endl;
                            logf.close();
                            toExit=TRUE;
                        }
                    }
                    else if( 0 == recvStr.compare("WON"))
                    {
                        scrutil->gotoxy(40, 5);
                        cout << "******BINGO!!!! I HAVE WON the GAME!!!*******" << endl;
                        toExit = TRUE;
                    }
                    else if( 0 == recvStr.compare("LOST"))
                    {
                        scrutil->gotoxy(40, 5);
                        cout << "!!!!!!!I HAVE LOST the BINGO GAME!!!!!!!" << endl;
                        toExit = TRUE;
                    }

                    //remove user input line and disable key input
                    scrutil->gotoxy(0, 40);
                    cout << "                                                                                 " ;
                }
            }
            catch (const char* s)
            {
                cerr << s << endl;
                ofstream err("error.txt", ios::app);
                err<<s<<endl;
                err.close();
            }
            catch (std::string s)
            {
                cerr << s << endl;
                ofstream err("error.txt", ios::app);
                err<<s<<endl;
                err.close();
            }
            catch (...)
            {
                cerr << "unhandled exception\n";
                ofstream err("error.txt", ios::app);
                err<<"Unhandled exception\n";
                err.close();
            }
            break;

        case Utils::server:
            try
            {
                stringstream ss;
                string sendStr, recvStr;

                Socket* new_sock;
                SockServer serv(8888,5);
                for(int cli = 0; cli < maxClients; cli++)
                {
                    cout << "waiting for client connection" << endl;
                    new_sock = serv.acceptConnect();
                    unsigned ret;
                    initializeThreadStruct(sStruct[cli], matrixSize);
                    copyMatrix(sStruct[cli].c, initColor);
                    copyMatrix(sStruct[cli].m, m[cli]);
                    sStruct[cli].slaveWait = ghEvent[cli*2];
                    sStruct[cli].masterWait = ghEvent[cli*2+1];
                    sStruct[cli].s = new_sock;
                    sStruct[cli].clientInfo = serv.clientInfo;
                    sStruct[cli].bingoCount = 0;
                    sStruct[cli].clientId = cli;
                    _beginthreadex(0, 0, serverThread, (void*)&sStruct[cli], 0, &ret);

                    //send command to slave to transmit matrix, color and bingo count
                    sStruct[cli].command = cmdSendMatrix;
                    SetEvent(ghEvent[cli*2]);

                    //wait for the signal from thread to continue to next
                    WaitForSingleObject(ghEvent[cli*2+1], INFINITE);
                }

                //signal server threads to receive user inputs from their clients
                BOOL gameOver = FALSE;
                int userInput, i, cnt;
                while(!gameOver)
                {
                    for(int cli = 0; !gameOver && (cli < maxClients); cli++)
                    {
                        //send command to slave to get user input
                        sStruct[cli].command = cmdGetUserInput;
                        SetEvent(ghEvent[cli*2]);

                        //wait for the signal from thread to continue to next
                        WaitForSingleObject(ghEvent[cli*2+1], INFINITE);
                        userInput = sStruct[cli].userInput;

                        //have all slave threads recalculate their respective matrices based on new user input
                        //identify victory client
                        int victoryClient;
                        gameOver = FALSE;
                        i = cli;
                        cnt = 0;
                        while(cnt < maxClients)
                        {
                            sStruct[i].userInput = userInput;
                            sStruct[i].command = cmdRecalculateMatrix;
                            SetEvent(ghEvent[i*2]);
                            WaitForSingleObject(ghEvent[i*2+1], INFINITE);
                            if(!gameOver && (sStruct[i].bingoCount >= victoryBingoCount))
                            {
                                gameOver = TRUE;
                                victoryClient = i;
                            }
                            i = (i+1) % maxClients;
                            cnt++;
                        }

                        //transmit new matrix based on user input to all clients
                        cnt = 0;
                        i = cli;
                        while(cnt < maxClients)
                        {
                            //Send updated matrix to client
                            sStruct[i].command = cmdSendMatrix;
                            SetEvent(ghEvent[i*2]);
                            WaitForSingleObject(ghEvent[i*2+1], INFINITE);

                            //check if the client has won
                            if(gameOver)
                            {
                                if(victoryClient == i)
                                {
                                    cout << "[ " << i << " ] " << "GAME OVER!!!! Client #" << i << " WON the BINGO GAME!!!" << endl;
                                    ofstream win("exitlog.txt", ios::app);
                                    win<<"Client "<<i<<" won, Client "<<(i+1)%maxClients<<" lost.\n";
                                    win.close();
                                    sStruct[i].command = cmdWon;
                                    SetEvent(ghEvent[i*2]);
                                    WaitForSingleObject(ghEvent[i*2+1], INFINITE);
                                }
                                else
                                {
                                    cout << "[ " << i << " ] " <<"GAME OVER!!!! Client #" << i << " LOST the BINGO GAME!!!" << endl;
                                    /*
                                    ofstream win("exitlog.txt", ios::app);
                                    win<<"Client "<<i<<" won, Client "<<(i+1)%maxClients<<" lost.\n";
                                    win.close();
                                    */
                                    sStruct[i].command = cmdLost;
                                    SetEvent(ghEvent[i*2]);
                                    WaitForSingleObject(ghEvent[i*2+1], INFINITE);
                                }
                            }
                            i = (i+1) % maxClients;
                            cnt++;
                        }
                   }
                }
               for(i = 0; i < maxClients; i++)
               {
                   delete sStruct[i].s;
               }
            }
            catch (const char* s)
            {
                cerr << s << endl;
                ofstream err("error.txt", ios::app);
                err<<s<<endl;
                err.close();
            }
            catch (std::string s)
            {
                cerr << s << endl;
                ofstream err("error.txt", ios::app);
                err<<s<<endl;
                err.close();
            }
            catch (...)
            {
                cerr << "unhandled exception\n";
                ofstream err("error.txt", ios::app);
                err<<"Unhandled exception\n";
                err.close();
            }
            break;

        default:
            cout << "Usage: GameBingo -server" << endl << "       GameBingo -client -serverhost <hostname>" << endl;
            break;
    }
    return 0;
}

//serverThread()
unsigned __stdcall serverThread(void* ts)
{
    ServThreadStruct *tStruct = (ServThreadStruct *)ts;
    Utils *util = Utils::getInstance();

    try
    {
        Matrix matrixOut; //matrix sent to client
        Matrix colorout; //color attributes sent to client
        stringstream ss;
        string sendStr, recvStr;

        // receive matrices from master thread
        initializeMatrix(matrixOut, tStruct->size);
        initializeMatrix(colorout, tStruct->size);
        copyMatrix(matrixOut, tStruct->m);
        copyMatrix(colorout, tStruct->c);

        BOOL continueLoop = TRUE;
        while(continueLoop)
        {
            //wait for the signal from master thread to continue
            WaitForSingleObject(tStruct->slaveWait, INFINITE);

            if(cmdSendMatrix == tStruct->command)
            {
                //send command code to client
                sendStr = "MATRIX";
                tStruct->s->sendString(sendStr);

                //send matrix to client
                sendStr.clear();
                util->stringifyMatrix(matrixOut, sendStr);
                tStruct->s->sendString(sendStr);

                //send color attributes to client
                sendStr.clear();
                util->stringifyMatrix(colorout, sendStr);
                tStruct->s->sendString(sendStr);

                //send bingo count to client
                tStruct->bingoCount = calculateBingoCount(colorout);
                tStruct->bingoCount = tStruct->bingoCount > victoryBingoCount ? victoryBingoCount : tStruct->bingoCount;
                sendStr.clear();
                ss.str().clear();
                ss.clear();
                ss << tStruct->bingoCount;
                ss >> sendStr;
                tStruct->s->sendString(sendStr);

                //Signal master thread
                SetEvent(tStruct->masterWait);
            }
            else if(cmdRecalculateMatrix == tStruct->command)
            {
                updateMatrixAndColor(tStruct->userInput, matrixOut, colorout);
                tStruct->bingoCount = calculateBingoCount(colorout);
                SetEvent(tStruct->masterWait); //Signal master thread
            }
            else if(cmdGetUserInput == tStruct->command)
            {
                //request client to get user input
                sendStr = "USRINPUT";
                tStruct->s->sendString(sendStr);

                //receive number data from client
                recvStr.clear();
                tStruct->s->recvString(recvStr);
                cout << "RECV{" << tStruct->clientInfo << "}: " << recvStr << endl;

                //update matrix and color attributes accordingly
                ss.str().clear();
                ss.clear();
                ss << recvStr;
                ss >> tStruct->userInput;
                updateMatrixAndColor(tStruct->userInput, matrixOut, colorout);

                //Signal master thread
                tStruct->bingoCount = calculateBingoCount(colorout);
                SetEvent(tStruct->masterWait);
            }
            else if(cmdWon == tStruct->command)
            {
                sendStr = "WON";
                tStruct->s->sendString(sendStr);
                continueLoop = FALSE;
                Sleep(1000);    // 1 sec wait for the client to exit
                SetEvent(tStruct->masterWait);
            }
            else if(cmdLost == tStruct->command)
            {
                //inform client that it has won
                sendStr = "LOST";
                tStruct->s->sendString(sendStr);
                continueLoop = FALSE;
                Sleep(1000);    // 1 sec wait for the client to exit
                SetEvent(tStruct->masterWait);
            }
        }
    }
    catch (const char* s)
    {
        cerr << s << endl;
        ofstream err("error.txt", ios::app);
        err<<s<<endl;
        err.close();
    }
    catch (std::string s)
    {
        cerr << s << endl;
        ofstream err("error.txt", ios::app);
        err<<s<<endl;
        err.close();
    }
    catch (...)
    {
        cerr << "unhandled exception\n";
        ofstream err("error.txt", ios::app);
        err<<"Unhandled exception\n";
        err.close();
    }

    return 0;
}

//copyMatrix()
void copyMatrix(Matrix& matrixOut, const Matrix& matrixIn)
{
    size_t rows = matrixIn.size();
    size_t cols = matrixIn[0].size();
    for( size_t i = 0; i < rows; i++ )
    {
        for( size_t j = 0; j < cols; j++ )
        {
            matrixOut[i][j] = matrixIn[i][j];
        }
    }
}

//updateMatrixAndColor()
void updateMatrixAndColor(int numInp, Matrix& matrixOut, Matrix& colorout)
{
    size_t rows = matrixOut.size();
    size_t cols = matrixOut[0].size();
    for( size_t i = 0; i < rows; i++ )
    {
        for( size_t j = 0; j < cols; j++ )
        {
            if( numInp == matrixOut[i][j])
            {
                colorout[i][j] = 1;
            }
        }
    }
}

//calculateBingoCount()
int calculateBingoCount(Matrix& colorout)
{
    int bingoCount = 0;
    BOOL isFilled;
    size_t rowsize = colorout.size();
    size_t colsize = colorout[0].size();

    // check rows 1 by 1
    for( size_t i = 0; i < rowsize; i++ )
    {
        isFilled = TRUE;
        for( size_t j = 0; isFilled && (j < colsize); j++ )
        {
            if( 0 == colorout[i][j] )
            {
                isFilled = FALSE;
            }
        }
        if(isFilled)
        {
            bingoCount++;
        }
    }

    //check columns 1 by 1
    for( size_t j = 0; j < colsize; j++ )
    {
        isFilled = TRUE;
        for( size_t i = 0; isFilled && (i < rowsize); i++ )
        {
            if( 0 == colorout[i][j] )
            {
                isFilled = FALSE;
            }
        }
        if(isFilled)
        {
            bingoCount++;
        }
    }

    //check diagnal (top->down)
    isFilled = TRUE;
    for( size_t i = 0; isFilled && (i < rowsize); i++ )
    {
        if( 0 == colorout[i][i] )
        {
            isFilled = FALSE;
        }
    }
    if(isFilled)
    {
        bingoCount++;
    }

    //check diagnal (down->top)
    size_t j = rowsize-1;
    isFilled = TRUE;
    for( size_t i = 0; isFilled && (i < colsize); i++, j-- )
    {
        if( 0 == colorout[j][i] )
        {
            isFilled = FALSE;
        }
    }
    if(isFilled)
    {
        bingoCount++;
    }

    return bingoCount;
}

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
