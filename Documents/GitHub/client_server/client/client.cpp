#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class client{

    int sockfd = 0, connfd = 0;

    string sendBuff;

    struct sockaddr_in serv_addr;

public:

    int ini_sock(string protocol){
        if( (sockfd = socket(AF_INET, protocol == "tcp" ? SOCK_STREAM : SOCK_DGRAM, protocol == "tcp" ? IPPROTO_TCP : IPPROTO_UDP))  < 0 ) {
            cout <<"Не удалось создать сокет"<< endl;
            return -1;
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons( protocol == "tcp" ? 5000 : 5001);
    }

    int connect_serv(string ipAdress) {

        if(inet_pton(AF_INET, ipAdress.c_str(), &serv_addr.sin_addr) < 0) {
            cout << "Не верный ip-адрес" << endl;
            return -1;
        }

        if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof( serv_addr)) < 0) {
            cout << "Не удалось соединиться с сервером" << endl;
            return -1;
        }
    }

    int send_msg_tcp(string sendMsg) {
        if ( send( sockfd, sendMsg.c_str(),  sendMsg.size(), 0) < 0) {
            cout <<"Не удалось отправить сообщение"<< endl;
            return -1;
        }
        cout << "Отправленное сообщение tcp: " << endl << sendMsg << endl;
    }

    int send_msg_udp(string sendMsg) {
        if ( sendto( sockfd, sendMsg.c_str(), sendMsg.size(), 0, ( struct sockaddr* )&serv_addr, sizeof( serv_addr ) ) < 0) { //sizeof( sockaddr_in )
            cout <<"Не удалось отправить сообщение"<< endl;
            return -1;
        }
        cout << "Отправленное сообщение udp: " << endl << sendMsg << endl;
    }

    int recv_msg(){
        vector<char> recvBuff(1024);
        if( recv(sockfd, &recvBuff[0], recvBuff.size(), 0) > 0){
            cout << "Ответ:" << endl;
            for(auto i = recvBuff.begin(); i != recvBuff.end(); i++)
                cout << *i;
            cout << endl;
        } else
            cout << "Не удалось получить ответа" << endl;

    }

    void close_sock() {
        close(sockfd);
    }
};




int main(int argc, char *argv[]){

    //if( argc = 3 ) {

    client c;

    //cout << argc << argv[0] << endl;

    string sendMsg("10 apple, 20 banana, 25 orange");
    string protocol("tcp");//argv[2]);
    string ipAdress("127.0.0.1");//argv[1]);

    //getline(cin, sendMsg);


    for(int schet = 0; schet < 5; schet++){
        if(c.ini_sock(protocol) > 0)
            cout << "ini_sock" << endl;

        if(c.connect_serv(ipAdress) > 0)
            cout << "connect" << endl;

        if(protocol == "tcp"){
            if(c.send_msg_tcp(sendMsg) > 0)
                cout << "send_tcp" << endl;
        }
        else
            if(c.send_msg_udp(sendMsg) > 0)
                cout << "send_udp" << endl;
        if(c.recv_msg() > 0)
            cout << "recv  " << schet  <<endl;

        c.close_sock();
    }

    //}
    return 0;
}
