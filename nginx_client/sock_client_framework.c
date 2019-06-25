#include "sock_client_framework.h"



SOCKET serv_sock;
char nick[20];

void ErrorHandling(char* message) {
	WSACleanup();
	fputs(message, stderr);
	fputc('\n', stderr);
	char c = _getch();
	exit(1);
}
void __cdecl RecvThread(void* p)
{
	SOCKET sock = (SOCKET)p;
	char buf[256];
	while (1)
	{
		//Recive wait From Server(_beginThread)
		int recvsize = recv(sock, buf, sizeof(buf), 0);
		if (recvsize <= 0)
		{
			printf("��������\n");
			break;
		}
		//------------------------------------------------
		buf[recvsize] = '\0';
		printf("\r%s\n>>", buf);
	}
}

void __cdecl sendMessage(const char* message) {

	char buf[256] = { 0 };
	sprintf_s(buf, sizeof(buf), "[%s] %s",nick, message);

	//send Packet Data (Client To Server)
	int sendsize = send(serv_sock, buf, (int)strlen(buf), 0);
	//if (sendsize <= 0)
	//------------------------------------------
}

void bind_sock_clnt(void) {

	//-------���� ���̺귯�� �ҷ�����(?)--------
	WSADATA wsaData;
	int retval = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (retval != 0)
	{
		printf("WSAStartup() Error\n");
	}
	//------------------------------------------

	//---------���ϻ���-------- 
	serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//TCP�� �̿��� ����
	//-------------------------
	
	//---------���� ���� �Է�--------------------
	SOCKADDR_IN serv_addr;
	serv_addr.sin_family = AF_INET;						
	serv_addr.sin_port = htons(2513);					

	char* server_type;
	server_type = "local";
	if (server_type == "remote") {
		struct addrinfo hints;
		struct addrinfo* servinfo; // ����� ������ ����
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET; // IPv4�� �ްڴ�.
		hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
		int status = getaddrinfo("foretion.codns.com", "http", &hints, &servinfo);

		struct sockaddr_in* saddr = (struct sockaddr_in*)servinfo->ai_addr;
		char* ip_addr = inet_ntoa(saddr->sin_addr);
		serv_addr.sin_addr = saddr->sin_addr; //SOCKADDR_IN::sin_addr�� �Ҵ�
		freeaddrinfo(servinfo);
	}
	else if (server_type == "local") {
		serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	}


	retval = connect(serv_sock, (SOCKADDR*)& serv_addr, sizeof(SOCKADDR));
	if (retval == SOCKET_ERROR)
	{
		printf("connect() Error\n");
	}
	//-------------------------------

	//printf("�г��� >> ");
	//gets_s(nick,sizeof(nick));
	//_beginthread(RecvThread, 0, (void*)serv_sock);
	strcpy_s(nick,sizeof(nick), "TEST_NICK");
	/*
	while (1)
	{
		char buf[256] = { 0 };
		char str[256];
		printf(">> ");
		gets_s(str,sizeof(str));
		sprintf_s(buf, sizeof(buf),"[%s] %s", nick, str);

		//---------������ �޽��� ����---------------
		int sendsize = send(serv_sock, buf, (int)strlen(buf), 0);
		if (sendsize <= 0)
			break;
		//------------------------------------------
	}
	*/
	//----------���� ����------------------
	//closesocket(serv_sock);
	//-------------------------------------

	//---------���̺귯�� ����(?)---------
	//WSACleanup();
	//-------------------------------------

}
