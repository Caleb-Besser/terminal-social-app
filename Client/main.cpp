#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include "../globals.h"

SOCKET connectToServer()
{
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int iResult;
    SOCKET ConnectSocket = INVALID_SOCKET;
    iResult = getaddrinfo(SERVER_IP, PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    ptr = result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }
    freeaddrinfo(result);

    return ConnectSocket;
}

bool sendMessage(SOCKET ConnectSocket, char *message)
{
    int recvbuflen = DEFAULT_BUFLEN;
    int iResult;

    char recvbuf[DEFAULT_BUFLEN];
    iResult = send(ConnectSocket, message, (int)strlen(message), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return false;
    }

    printf("Bytes Sent: %ld\n", iResult);
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return false;
    }
    do
    {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    } while (iResult > 0);
    return true;
}

int main(int argc, char *argv[])
{

    SOCKET ConnectSocket = INVALID_SOCKET;
    int iResult;

    if (!setupWSA())
    {
        return 1;
    }
    ConnectSocket = connectToServer();
    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    if (!sendMessage(ConnectSocket, "this is a test."))
    {
        printf("Error sending message: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}