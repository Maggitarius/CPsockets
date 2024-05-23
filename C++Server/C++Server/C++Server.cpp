#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>

using namespace std;

int main() {
    setlocale(LC_ALL, "Rus");
    SetConsoleCP(1251);

    WSADATA wsaData;
    ADDRINFOA hints;
    ADDRINFO* addrResult = nullptr;
    char recvBuffer[512];
    SOCKET ConnectSocket = INVALID_SOCKET;
    SOCKET ServerSocket = INVALID_SOCKET;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed: " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, "6565", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed: " << result << endl;
        WSACleanup();
        return 1;
    }

    ServerSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ServerSocket == INVALID_SOCKET) {
        cout << "Socket creation failed: " << WSAGetLastError() << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = bind(ServerSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Socket binding failed: " << WSAGetLastError() << endl;
        freeaddrinfo(addrResult);
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addrResult);

    result = listen(ServerSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cout << "Socket listening failed: " << WSAGetLastError() << endl;
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }

    cout << "Ожидание подключения клиента..." << endl;
    ConnectSocket = accept(ServerSocket, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket accepting failed: " << WSAGetLastError() << endl;
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }

    cout << "Клиент подключен!" << endl;
    closesocket(ServerSocket);

    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Получено " << result << " байт: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Соединение закрыто" << endl;
        }
        else {
            cout << "Ошибка получения данных: " << WSAGetLastError() << endl;
        }
    } while (result > 0);

    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}