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
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL;
    struct addrinfo hints;
    const char* sendBuffer1 = "Первое сообщение";
    const char* sendBuffer2 = "Второе сообщение";

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed: " << iResult << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo("127.0.0.1", "6565", &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo failed: " << iResult << endl;
        WSACleanup();
        return 1;
    }

    ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        cout << "Unable to connect to server" << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (iResult == SOCKET_ERROR) {
        cout << "Send failed: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    cout << "Первое сообщение отправлено: " << sendBuffer1 << endl;
    
    iResult = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (iResult == SOCKET_ERROR) {
        cout << "Send failed: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    cout << "Второе сообщение отправлено: " << sendBuffer2 << endl;

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        cout << "Shutdown failed: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}