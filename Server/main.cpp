#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include "../globals.h"

void echoClientMessages(SOCKET clientSocket)
{
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iResult;
    int iSendResult;

    do
    {
        iResult = recv(clientSocket, recvbuf, recvbuflen, 0);

        if (iResult > 0)
        {
            printf("Bytes received: %d\n", iResult);

            iSendResult = send(clientSocket, recvbuf, iResult, 0);

            if (iSendResult == SOCKET_ERROR)
            {
                printf("send failed: %d\n", WSAGetLastError());
                return;
            }

            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
        {
            printf("Connection closing...\n");
        }
        else
        {
            printf("recv failed: %d\n", WSAGetLastError());
        }

    } while (iResult > 0);
}

SOCKET createListenSocket()
{
    int iResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    iResult = getaddrinfo(NULL, PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    std::cout << "Address info attained" << "\n";

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    std::cout << "Opened Socket" << "\n";
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);

    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Socket Bound" << "\n";
    return ListenSocket;
}

int main()
{
    SOCKET ClientSocket;
    int iResult;

    if (!setupWSA())
    {
        return 1;
    }
    SOCKET ListenSocket = createListenSocket();
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("listen socket error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    echoClientMessages(ClientSocket);

    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}