#include "stdafx.h"
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>

#pragma warning(disable: 4996)
using namespace std;

int main(int argc, char* argv[]) {
	string Inert_Addr, comand, directory, file_name;
	int Port;

	if (argc > 1) {
		Inert_Addr = argv[1];
		Port = stoi(argv[2]);
		comand = argv[3];
		directory = argv[4];
		file_name = argv[5];
	}

	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "Error" << endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(Inert_Addr.c_str());
	addr.sin_port = htons(Port);
	addr.sin_family = AF_INET;

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cout << "Error: failed connect to server." << endl;
		return 1;
	}
	cout << "Successfully connected!" << endl;

	//req - строка, состоящая из команды, директории к файлу и названии файла
	string req = comand + '*' + directory + '*' + file_name;

	//отправка серверу req
	send(Connection, req.c_str(), sizeof(req), 0);

	//получение сообщения о выполненой команде
	char msg[256];
	recv(Connection, msg, sizeof(msg), 0);
	cout << msg << endl;

	system("pause");
	return 0;
}
