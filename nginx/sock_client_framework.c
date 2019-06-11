#include "sock_client_framework.h"


void __cdecl RecvThread(void* p)
{
	SOCKET sock = (SOCKET)p;
	char buf[256];
	while (1)
	{
		//-----------�����κ��� ����------------
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


void bind_sock_clnt(void) {

	//-------���� ���̺귯�� �ҷ�����(?)--------
	WSADATA wsaData;
	int retval = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (retval != 0)
	{
		printf("WSAStartup() Error\n");
		return 0;
	}
	//------------------------------------------

	//---------���ϻ���-------- 
	SOCKET serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//TCP�� �̿��� ����
	//-------------------------

	//---------���� ���� �Է�--------------------
	SOCKADDR_IN serv_addr;
	serv_addr.sin_family = AF_INET;						// IP�ּҸ� �̿��ϰ�
	serv_addr.sin_port = htons(2513);					// ������ 4000����
	serv_addr.sin_addr.s_addr = inet_addr("172.16.136.223");	// ������ ip �ּҴ� 127.0.0.1
	//--------------------------------------------

	//---------���� ����------------
	retval = connect(serv_sock, (SOCKADDR*)& serv_addr, sizeof(SOCKADDR));
	if (retval == SOCKET_ERROR)
	{
		printf("connect() Error\n");
		return 0;
	}
	//-------------------------------

	char nick[20];
	printf("�г��� >> ");
	gets(nick);

	_beginthread(RecvThread, 0, (void*)serv_sock);
	/*
	while (1)
	{
		char buf[256] = { 0 };
		char str[256];
		printf(">> ");
		gets(str);
		sprintf_s(buf, sizeof(buf),"[%s] %s", nick, str);

		//---------������ �޽��� ����---------------
		int sendsize = send(serv_sock, buf, strlen(buf), 0);
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


void ErrorHandling(char* message) {
	WSACleanup();
	fputs(message, stderr);
	fputc('\n', stderr);
	_getch();
	exit(1);
}