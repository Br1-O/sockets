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
        return 1;
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

    //creation of UDP socket for the server
    SOCKET serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(serverSocket == INVALID_SOCKET){
        cout << "Error creating server socket():" << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }else{
        cout << "server socket creation was successful." << endl;
    }

    //info of server socket
    socketInfo serverSocketInfo{};
    serverSocketInfo.family = AF_INET; //family for TCP or UDP connections
    serverSocketInfo.port = htons(12345); //htons formats the port number into big-endian
    serverSocketInfo.ip.s_addr = inet_addr("127.0.0.1");

    //binding of the server socket with the server socket info
    SOCKET binding = SOCKET_ERROR;
    binding = bind(serverSocket, (sockaddr*)&serverSocketInfo, sizeof(serverSocketInfo));

    if(binding == SOCKET_ERROR){
        cout << "binding of server socket failed! : " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }else{
        cout << "binding of server socket was successful." << endl;
    }

    //struct to save client's data
    socketInfo clientSocketInfo{};
    int clientLen = sizeof(clientSocketInfo);

    //buffer to store messages as a char array of max length 1024
    char buffer[1024];
    
    //loop to receive and show messages
    while (true){
        //receive messages from client
        int clientMessageByteCount = recvfrom(serverSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientSocketInfo, &clientLen);
        if(clientMessageByteCount == SOCKET_ERROR){
            cout << "Client disconnected." << endl;
            break;
        }

        buffer[clientMessageByteCount] = '\0';
        cout << endl << "Cliente dijo: " << buffer << endl << endl;

        cout << "Responde: " << endl;
        cin.getline(buffer, 1024);

        //send message to client
        sendto(serverSocket, buffer, strlen(buffer), 0, (sockaddr*)&clientSocketInfo, clientLen);
    }

    //closing of sockets instances
    closesocket(serverSocket);

    //clean up for windows socket api
    WSACleanup();
    return 0;
}