#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN


#include<Windows.h>
#include<iphlpapi.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iostream>
using namespace std;

#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_PORT			"27015"
#define	DEFAULT_BUFFER_LENGHT	1500

void main()
{
	setlocale(LC_ALL, "");
	//1)
	WSAData wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//2)
	addrinfo* result = NULL;
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo() failed with code: " << iResult << endl;
		WSACleanup();
		return;
	}

	//3)
	SOCKET connect_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (connect_socket == INVALID_SOCKET)
	{
		cout << "Socket creation with code: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//4)
	iResult = connect(connect_socket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Unable to connect to Server" << endl;
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//5)
	//int recvbuflen = DEFAULT_BUFFER_LENGHT;
	CHAR send_buffer[DEFAULT_BUFFER_LENGHT] = "Hello Server, i am Client";
	CHAR recvbuffer[DEFAULT_BUFFER_LENGHT]{};
	do
	{
		iResult = send(connect_socket, send_buffer, strlen(send_buffer), 0);
		if (iResult == SOCKET_ERROR)
		{
			cout << "Send data failed with " << WSAGetLastError() << endl;
			closesocket(connect_socket);
			freeaddrinfo(result);
			WSACleanup();
			return;
		}
		cout << iResult << " Bytes sent" << endl;

		//6)

		iResult = recv(connect_socket, recvbuffer, DEFAULT_BUFFER_LENGHT, 0);
		if (iResult > 0)cout << "Bytes received: " << iResult << ",Message: " << recvbuffer << endl;
		else if (iResult == 0)cout << "Connection close" << endl;
		else cout << "Received failed with code" << WSAGetLastError() << endl;
		ZeroMemory(send_buffer, sizeof(send_buffer));
		ZeroMemory(recvbuffer, sizeof(recvbuffer));
		cout << "Add message:";
		SetConsoleCP(1251);
		cin.getline(send_buffer, DEFAULT_BUFFER_LENGHT);
		SetConsoleCP(866);
		//for (int i = 0; send_buffer[i]; i++)send_buffer[i] = tolower(send_buffer[i]);
	} while (iResult > 0 && strcmp(send_buffer, "Exit"));

	//7) 
	iResult = shutdown(connect_socket, SD_SEND);
	closesocket(connect_socket);
	freeaddrinfo(result);
	WSACleanup();

	system("PAUSE");
}