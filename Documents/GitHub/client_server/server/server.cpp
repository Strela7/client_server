#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <signal.h>

using namespace std;

class server{

    int sock_tcp = 0, sock_udp = 0, connfd = 0, namelen = 0;

    struct sockaddr_in serv_addr_tcp;
    struct sockaddr_in serv_addr_udp;

    socklen_t recv_size = sizeof( sockaddr_storage);

    struct sockaddr_in si_other;

    int slen = sizeof(si_other);

    char recvBuff[1024];

    string buff, sendBuff;

public:

    void socket_ini(){

        sock_tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sock_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        fcntl(sock_tcp, F_SETFL, O_NONBLOCK);
        fcntl(sock_udp, F_SETFL, O_NONBLOCK);

        serv_addr_tcp.sin_family = AF_INET;
        serv_addr_tcp.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr_tcp.sin_port = htons(5000);

        serv_addr_udp.sin_family = AF_INET;
        serv_addr_udp.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr_udp.sin_port = htons(5001);

        namelen = sizeof(serv_addr_tcp);
        bind(sock_tcp, (struct sockaddr*)&serv_addr_tcp, namelen);
        listen(sock_tcp, 10);

        namelen = sizeof(serv_addr_udp);
        bind(sock_udp, (struct sockaddr*)&serv_addr_udp, namelen);
        listen(sock_tcp, 10);
    }

    void tcp_udp_connect() {
        if ( (connfd = accept(sock_tcp, (struct sockaddr*)NULL,  0) ) > 0) {
            recv_send_tcp(connfd);
            close(connfd);
        }
        else{
            recv_send_udp(sock_udp);
        }
    }

    void parcer_recv() {
        int stop_i = 0;
        int sum = 0;
        for(int i = 0; recvBuff[i] != '\0'; i++)
            if(recvBuff[i] >= '0' && recvBuff[i] <= '9') {
                buff.push_back( recvBuff[i] );
                stop_i = 1;
            }
            else if(stop_i == 1) {
                sum += atoll(buff.c_str());
                sendBuff += buff;
                sendBuff += " ";
                buff.clear();
                stop_i = 0;
            }
        sendBuff += '\n';
        sendBuff += to_string(sum);

        buff.clear();
    }

    void recv_send_tcp(int conn_sock) {
        memset(recvBuff, '\0', sizeof(recvBuff));
        sendBuff.clear();

        if( recv( conn_sock, recvBuff, sizeof(recvBuff), 0) > 0) {

            parcer_recv();

            pid_t pid;
            if(pid = fork()){
                if (send( conn_sock, sendBuff.c_str(), sendBuff.size(), 0 ) < 0)
                    cout << "Не удалось отпарвить ответ tcp" << endl;
                kill(pid, SIGTERM);
            }   cout << "Не удалось создать процесс" << endl;
        }
    }


    void recv_send_udp(int conn_sock) {
        memset(recvBuff, '\0', sizeof(recvBuff));
        sendBuff.clear();

        if( recvfrom( conn_sock, recvBuff, sizeof(recvBuff), 0, ( struct sockaddr *) &si_other, &recv_size ) > 0) {

            parcer_recv();

            pid_t pid ;
            if(pid = fork()){
                if ( sendto( conn_sock, sendBuff.c_str(), sendBuff.size(), 0, (struct sockaddr *) &si_other, slen) < 0 )
                    cout << "Не удалось отпарвить ответ udp" << endl;
                kill(pid, SIGTERM);
            }   cout << "Не удалось создать процесс" << endl;
        }
    }

    void close_sock(){
        close(sock_tcp);
        close(sock_udp);
    }
};



int main(int argc, char *argv[]) {

    server s;

    s.socket_ini();

    while(true)
        s.tcp_udp_connect();

    s.close_sock();

    return 0;
}
