#include "stdafx.h"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#include <algorithm>

#pragma warning(disable: 4996)
using namespace std;


//Split - делит полученную строку на слова
vector<string> Split(string& s) {
	vector<string> v;
	auto it = begin(s);
	while (it != end(s)) {
		auto its = find_if(it, end(s),
			[](const char c) {
				return c == '*';
			});
		string buf = "";
		for (auto item = it; item != its; ++item) {
			buf += *item;
		}
		v.push_back(buf);
		if (its == end(s)) {
			break;
		}
		it = ++its;
	}
	return v;
}

int main(int argc, char* argv[]) {
	string directory_server;
	int Port;

	if (argc > 1) {
		directory_server = argv[1];
		Port = stoi(argv[2]);
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
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(Port);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, 0);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

	if (newConnection == 0) {
		cout << "Client hasn't connected!";
	}
	else {
		cout << "Client connected!" << endl;

		//Получение сообщения от клиента
		char mesg[1024];
		recv(newConnection, mesg, sizeof(mesg), 0);

		//Разделение полученного сообщения на команду (comand), директорию файла (directory_file),
		// название файла (file_name)
		string req = string(mesg);
		vector<string> words = Split(req);

		string comand, directory_file, file_name;
		comand = words[0];
		directory_file = words[1];
		file_name = words[2];

		//path - путь к созданию/удалению файла
		string path = directory_server + directory_file + file_name;

		//Если необходимо добавить файл, то используем команду ADD
		if (comand == "ADD") {
			ofstream out(path);
			out.close();

			//Отправка сообщения клиенту о завершении команды
			char msg[256] = "File is created!";
			send(newConnection, msg, sizeof(msg), 0);

			cout << "File : " << file_name << " is created in " << directory_server + directory_file << endl;
		}
		//Если необходимо удалить файл, то используем команду DELETE
		else if (comand == "DELETE") {
			if (remove(path.c_str()) != 0) {
				//Отправка сообщения клиенту об ошибке (не существует файла)
				char msg[256] = "File is not found!";
				send(newConnection, msg, sizeof(msg), 0);

				cout << "File : " << file_name << " is not found in " << directory_server + directory_file << endl;
			}
			else {
				//Отправка сообщения клиенту о завершении команды
				char msg[256] = "File is deleted!";
				send(newConnection, msg, sizeof(msg), 0);

				cout << "File : " << file_name << " is deleted in " << directory_server + directory_file << endl;
			}
		}
	}

	system("pause");
	return 0;
}
