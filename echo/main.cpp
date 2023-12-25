#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <memory>
#include <string>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
using namespace std;
class critical_error:public std::runtime_error
{
public:
    critical_error(const std::string& s):std::runtime_error(s) {}
};
int main()
{
    try {
        int buff_size=1024;
        std::unique_ptr<char[]> buffer(new char[buff_size]);
        int sock = socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in self_addr;
        struct sockaddr_in serv_addr;
        self_addr.sin_family = AF_INET;
        self_addr.sin_port = htons(7);
        self_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(7);
        inet_aton("82.179.90.12",&serv_addr.sin_addr);
        int rc = connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        if (rc<0) {
            throw critical_error ("Ошибка при подсоединеии к серверу");
        }
        const char* message = "Проверка";
        strcpy(buffer.get(), message);
        int send_b=send(sock, buffer.get(), buff_size, 0);
        if (send_b<0) {
            throw critical_error ("Ошибка отправки собщения серверу");
        }
        int recv_b=send(sock, buffer.get(), buff_size, 0);
        if (recv_b<0) {
            throw critical_error ("Ошибка приема собщения от сервера");
        }
        std::string msg(buffer.get(), recv_b);
        cout<<"Работа службы echo: "+msg<<endl;
        close(sock);
    } catch(critical_error &e) {
        cout<<e.what()<<endl;
    }
    return 0;
}
