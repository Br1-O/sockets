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

    //creation of TCP socket for the server
    SOCKET serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(serverSocket == INVALID_SOCKET){
        cout << "Error creating server socket():" << WSAGetLastError() << endl;
        WSACleanup();
    }else{
        cout << "server socket creation was successful." << endl;
    }

    //info of server socket
    socketInfo serverSocketInfo{};
    serverSocketInfo.family = AF_INET; //family for TCP or UDP connections
    serverSocketInfo.port = htons(12345); //htons formats the port number into big-endian, TCP/IP network byter order
    serverSocketInfo.ip.s_addr = INADDR_ANY; //enables all connections from local addresses

    //binding of the server socket with the server socket info
    SOCKET binding = SOCKET_ERROR;
    binding = bind(serverSocket, (sockaddr*)&serverSocketInfo, sizeof(serverSocketInfo));

    if(binding == SOCKET_ERROR){
        cout << "binding of server socket failed! : " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
    }else{
        cout << "binding of server socket was successful." << endl;
    }

    //listen for incoming connection requests from clients
    SOCKET listening = SOCKET_ERROR;
    listening = listen(serverSocket, 5); //accepts up to 5 simultaneous client connections

    if(listening == SOCKET_ERROR){
        cout << "listening of server socket failed! : " << WSAGetLastError() << endl;
    }else{
        cout << "listening of server socket was successful. Server socket is now waiting for connections" << endl;
    }

    //struct to save client's data
    socketInfo clientSocketInfo{};
    socklen_t clientLen = sizeof(clientSocketInfo);

    //accept for incoming connections
    SOCKET clientServerConnectionSocket = INVALID_SOCKET;
    clientServerConnectionSocket = accept(serverSocket, (sockaddr*)&clientSocketInfo, &clientLen);

    if(clientServerConnectionSocket == INVALID_SOCKET){
        cout << "accept from server socket failed! : " << WSAGetLastError() << endl;
        WSACleanup();
    }

    //buffer to store messages as a char array of max length 1024
    char buffer[1024];
    
    //loop to receive and show messages
    while (true){
        //receive messages from client
        int clientMessageByteCount = recv(clientServerConnectionSocket, buffer, sizeof(buffer), 0);
        if(clientMessageByteCount <= 0){
            cout << "Client disconnected." << endl;
            break;
        }else{
        buffer[clientMessageByteCount] = '\0';
        cout << endl << "Cliente dijo: " << buffer << endl << endl;
        }

        cout << "Responde: " << endl;
        cin.getline(buffer, 1024);

        //send message to client
        send(clientServerConnectionSocket, buffer, sizeof(buffer), 0);
    }

    //closing of sockets instances
    closesocket(clientServerConnectionSocket);
    closesocket(serverSocket);

    //clean up for windows socket api
    WSACleanup();
    return 0;
}