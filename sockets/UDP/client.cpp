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

    //creation of UDP socket for the client
    SOCKET clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(clientSocket == INVALID_SOCKET){
        cout << "Error creating client socket():" << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }else{
        cout << "client socket creation was successful." << endl;
    }

    //struct to save server's data
    socketInfo serverSocketInfo{};
    serverSocketInfo.family = AF_INET;
    serverSocketInfo.port = htons(12345);
    serverSocketInfo.ip.s_addr = inet_addr("127.0.0.1");
    int serverLen = sizeof(serverSocketInfo);

    //buffer to store messages as a char array of max length 1024
    char buffer[1024];
    
    //loop to receive and show messages
    while (true){
        cout << "Responde: " << endl;
        cin.getline(buffer, 1024);

        //send message to server
        sendto(clientSocket, buffer, strlen(buffer), 0, (sockaddr*)&serverSocketInfo, serverLen);

        //receive messages from server
        int serverMessageByteCount = recvfrom(clientSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&serverSocketInfo, &serverLen);
        if(serverMessageByteCount == SOCKET_ERROR){
            cout << "Server disconnected." << endl;
            break;
        }

        buffer[serverMessageByteCount] = '\0';
        cout << endl << "Server dijo: " << buffer << endl << endl;
    }

    //closing of sockets instances
    closesocket(clientSocket);

    //clean up for windows socket api
    WSACleanup();
    return 0;
}