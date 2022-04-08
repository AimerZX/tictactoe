#include <iostream>
#include <WINSOCK2.H>
#include <string>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

#define PORT           6050
#define MsgSize        1024

static int checkerboard[9];
static bool turn = true;
static SOCKET sClient;
static unsigned long conTime;

void output();
bool checkWin();
void timer();
int main() {

    WSADATA wsaData;
    SOCKET sListen;

    SOCKADDR_IN local;
    SOCKADDR_IN client;
    char szMessage[MsgSize];
    int ret;
    int ADDRSize = sizeof(SOCKADDR_IN);
    WSAStartup(0x0202, &wsaData);

    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    local.sin_family = AF_INET;
    local.sin_port = htons(PORT);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    cout<<"bind status:"<<bind(sListen, (struct sockaddr *) &local, sizeof(SOCKADDR_IN));

    listen(sListen, 1);

    sClient = accept(sListen, (struct sockaddr *) &client, &ADDRSize);
    printf("Accepted client:%s:%d\n", inet_ntoa(client.sin_addr),
           ntohs(client.sin_port));

    int end = 0;
    output();
    while (TRUE) {
        if (turn) {
            while (TRUE) {
                timer();
                string loc;
                cin >> loc;
                if (loc.length() == 1 && loc[0] >= 48 && loc[0] <= 56 ) {
                    if(checkerboard[loc[0] - '0']==0){
                        conTime = 0;
                        send(sClient, loc.c_str(),(int)strlen(loc.c_str()), 0);
                        checkerboard[loc[0] - '0'] = 1;
                        if (checkWin()) {
                            end = 1;
                            cout << "You win!" << endl;
                        }
                        break;
                    }else  {
                        cout<<"There are already chess in this place"<<endl;
                        timer();
                    }
                } else {
                    cout << "Please enter the correct location!" << endl;
                    timer();
                }
            }
            turn = false;
        } else{
            cout<<"Waiting for the other side to play chess..."<<endl;
            ret = recv(sClient, szMessage, MsgSize, 0);
            szMessage[ret] = '\0';
            if(szMessage[0] - '0' == 9){
                cout<<"Rival timeout.You win!"<<endl;
                return 0;
            }
            checkerboard[szMessage[0] - '0'] = -1;
            turn = true;
            if(checkWin()){
                end = 1;
                cout<<"You lose!"<<endl;
            }
        }
        if (end ==1) break;
        output();
    }
    return 0;
}
void output(){
    for(int i=0;i<9;i++){
        if(checkerboard[i]==0){
            cout<<"__"<<i<<"__";
        }else if(checkerboard[i]==1) {
            cout <<"__"<<"X"<<"__";
        }else{
            cout<<"__"<<"*"<<"__";
        }
        if((i+1)%3!=0) cout<<"|";
        else cout<<endl;
    }
    cout<<"x=> your chess"<<'\n'<<"*=> opposite chess"<<'\n'<<"0~8=>Places you can place chess"<<endl;
}
bool checkWin(){
    if(
            (checkerboard[0]!=0&&checkerboard[0]==checkerboard[1]&&checkerboard[0]==checkerboard[2])||
            (checkerboard[3]!=0&&checkerboard[3]==checkerboard[4]&&checkerboard[3]==checkerboard[5])||
            (checkerboard[6]!=0&&checkerboard[6]==checkerboard[7]&&checkerboard[6]==checkerboard[8])||
            (checkerboard[0]!=0&&checkerboard[0]==checkerboard[3]&&checkerboard[0]==checkerboard[6])||
            (checkerboard[1]!=0&&checkerboard[1]==checkerboard[4]&&checkerboard[1]==checkerboard[7])||
            (checkerboard[2]!=0&&checkerboard[2]==checkerboard[5]&&checkerboard[2]==checkerboard[8])||
            (checkerboard[0]!=0&&checkerboard[0]==checkerboard[4]&&checkerboard[0]==checkerboard[8])||
            (checkerboard[2]!=0&&checkerboard[2]==checkerboard[4]&&checkerboard[2]==checkerboard[6])
            ) return true;
    else return false;
}
void  timer(){
    int timerFlag[6] ={0,0,0,0,0,0};
    DWORD begin = GetTickCount();
    unsigned long time = GetTickCount() - begin + conTime;
    while(time < 60000){
        time = GetTickCount() - begin + conTime;
        if(timerFlag[time/10000]==0){
            cout<<"Time remaining:"<<(6-time/10000)*10<<"s"<<endl;
            cout << "Please enter location(0~8):"<<endl;
            timerFlag[time/10000]=1;
        }
        if(_kbhit()) {
            conTime = time;
            return;
        }
    }
    cout<<"Time Out!You lose!";
    string endCode = "9";
    send(sClient, endCode.c_str(),(int)strlen(endCode.c_str()), 0);
    exit(0);
}
