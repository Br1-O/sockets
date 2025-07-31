#include <winsock2.h>
#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib") // Link with Winsock library

using namespace std;

int main() {
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);

    //initialize Winsock
    int wsaStart = WSAStartup(wVersionRequested, &wsaData);
    if (wsaStart != 0) {
        std::cerr << "WSAStartup failed: " << wsaStart << std::endl;
    }else{
        cout << "Winsock dll found!" << endl;
        cout << "The status:" << wsaData.szSystemStatus << endl;
    }

    //struct for socket info (family, port, ip)
    struct socketInfo {
        short family;         // AF_INET
        unsigned short port;  // Puerto en red byte order
        struct in_addr ip;  // Dirección IP
        char sin_zero[8]; //padding to make struct the same size as sockaddr
    };

    //creation of TCP socket for the client
    SOCKET clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(clientSocket == INVALID_SOCKET){
        cout << "Error creating client socket():" << WSAGetLastError() << endl;
        WSACleanup();
    }else{
        cout << "client socket creation was successful." << endl;
    }

    //info of server socket
    socketInfo serverSocketInfo{};
    serverSocketInfo.family = AF_INET; //family for TCP or UDP connections
    serverSocketInfo.port = htons(12345); //htons formats the port number into big-endian, TCP/IP network byter order
    serverSocketInfo.ip.s_addr = inet_addr("127.0.0.1"); //ip from localhost

    //connect of the client socket with the server socket info
    SOCKET connection = SOCKET_ERROR;
    connection = connect(clientSocket, (sockaddr*)&serverSocketInfo, sizeof(serverSocketInfo));

    if(connection == SOCKET_ERROR){
        cout << "connection of client socket failed! : " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
    }else{
        cout << "connection of client socket was successful." << endl;
    }

    //buffer to store messages as a char array of max length 1024
    char buffer[1024];

    //sending and receiving loop
    while (true){
        //input of user to store in the buffer to be sent as the message
        cout << "Ingrese su mensaje: " << endl;
        cin.getline(buffer, 1024);
        
        //send messages to server
        int clientMessageByteCount = send(clientSocket, buffer, sizeof(buffer), 0);

        if(clientMessageByteCount == SOCKET_ERROR){
            cout << "Sending of message from client failed! : " << WSAGetLastError() << endl;
            break;
        }

        //receive messages from client
        int serverMessageByteCount = recv(clientSocket, buffer, sizeof(buffer), 0);
        if(serverMessageByteCount > 0){
            buffer[serverMessageByteCount] = '\0';
            cout << endl << "Servidor dijo: " << buffer << endl << endl;
        }else{
            cout << "Server disconnected." << endl;
            break;
        }
    }

    //closing of sockets instances
    closesocket(clientSocket);

    //clean up for windows socket api
    WSACleanup();
    return 0;
}