#include "globals.h"
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

const char *PORT = "5000";
const char *SERVER_IP = "127.0.0.1";

WSADATA wsaData;

bool setupWSA()
{
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    std::cout << "Initializing..." << "\n";
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return false;
    }
    return true;
}