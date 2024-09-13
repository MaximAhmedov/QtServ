#include "myServer.h"

myServer::myServer(){
    this->startToListen();

};
myServer::~myServer(){
    this->stopServer();
};

void myServer::startToListen(){
    WIN(iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if(iResult != 0){
            std::cout << "WSAStartup failed: " << iResult << std::endl;
        }
        else
        std::cout << "WSAStartup is ok" << std::endl;);

    ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(ListenSocket == INVALID_SOCKET){
        std::cout << "Error at socket(): " <<  std::endl;
        WIN(WSACleanup());
    }
    else
        std::cout << "Socket() is ok" << std::endl;

    serveraddress.sin_family = AF_INET;
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddress.sin_port = htons(PORT);

    iResult = bind( ListenSocket,(struct sockaddr*) &serveraddress, sizeof(serveraddress));
    if(iResult == SOCKET_ERROR){
        std::cout << "bind failed with error: " << std::endl;
        WIN(closesocket(ClientSocket))NIX(close(ClientSocket));
        WIN(WSACleanup());
    }
    else
        std::cout << "bind is ok" << std::endl;

    iResult = listen(ListenSocket,5);
    if(iResult==SOCKET_ERROR){
        std::cout << "listen failed: " << std::endl;
        WIN(closesocket(ClientSocket))NIX(close(ClientSocket));
        WIN(WSACleanup());
    }
    else
        std::cout << "listen is ok" << std::endl;
}

void myServer::fdStart() {
    FD_ZERO(&(this->master));
    FD_SET(this->ListenSocket,&(this->master));
}

int myServer::selectFunc()
{
    this->copy = this->master;
    return select(0, &(this->copy), nullptr, nullptr, nullptr);;
}

void myServer::mainFunc(int i) {

    std::string response;

    selectSock = copy.fd_array[i];
    if(selectSock == ListenSocket){
        selectClSocket = accept(ListenSocket,nullptr,nullptr);
        FD_SET(selectClSocket,&master);
    }
    else{
        bzero(recvBuff, recvBuffLen);
        int bytesIn = recv(selectSock,recvBuff,recvBuffLen,0);
        if(bytesIn <= 0){
            WIN(closesocket(selectSock))NIX(close(selectSock));
            FD_CLR(selectSock,&master);
        }
        else{
            response.clear();
            std::string message = recvBuff;
            //std::cout << "Data received from client: " << message << std::endl;
            myMute.lock();
            switch(chat.getCommand(message)){
            case chat.ISBAN: {
                response = chat.userstatus(message);
                send(selectSock,response.c_str(),response.length(),0);
                myMute.unlock();
                break;
            }
            case chat.REGIS:{
                response = chat.regis(message);
                send(selectSock,response.c_str(),response.length(),0);
                myMute.unlock();
                break;
            }
            case chat.SIGNIN:{
                response = chat.auth(message);
                send(selectSock,response.c_str(),response.length(),0);
                myMute.unlock();
                break;
            }
            case chat.CHATW:{
                response = chat.chatw(message);
                send(selectSock,response.c_str(),response.length(),0);
                myMute.unlock();
                break;
            }
            case chat.ADDMES:{
                chat.addmes(message);
                //send(selectSock,response.c_str(),response.length(),0);
                myMute.unlock();
                break;
            }
            case chat.ALLUSER:{
                response = chat.alluser(message);
                send(selectSock,response.c_str(),response.length(),0);
                myMute.unlock();
                break;
            }
            case chat.ALLCHATS:{
                response = chat.allchats(message);
                send(selectSock,response.c_str(),response.length(),0);
                myMute.unlock();
                break;
            }
            default:{
                myMute.unlock();
                break;
            }
            }
        }
    }
}

void myServer::stopServer(){
    shutdown(ClientSocket, 1);
    WIN(closesocket(ClientSocket))NIX(close(ClientSocket));
    WIN(WSACleanup());
}

QString myServer::getIP()
{
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    host_entry = gethostbyname(hostbuffer);
    addr_list = (struct in_addr **)host_entry->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++) {
        std::string x = inet_ntoa(*addr_list[i]);
        if(x != "192.168.56.1"){
            //printf("IP address : %s\n",inet_ntoa(*addr_list[i]));
            QString tmp = "IP addres : " + QString::fromStdString(x);
            return tmp;
        }
    }
}

QVector<QString> myServer::showAllUsers()
{
    myMute.lock();
    QVector<QString> vctr = chat.showAllUsers();
    myMute.unlock();
    return vctr;
}

QVector<QString> myServer::showUsersStatus(QString command)
{
    myMute.lock();
    QVector<QString> vctr = chat.usersStatus(command);
    myMute.unlock();
    return vctr;
}


QVector<QString> myServer::showUserMessages(QString userLog)
{
    myMute.lock();
    QVector<QString> vctr = chat.showUserMessages(userLog);
    myMute.unlock();
    return vctr;
}

void myServer::banOrUnban(QString command)
{
    myMute.lock();
    chat.changeUsrStsatus(command);
    myMute.unlock();
}



void myServer::threadFunc()
{
    fdStart();
    while(true){
        int counter = selectFunc();
        for(int i = 0; i < counter; i++){
            mainFunc(i);
        }
    }
}
