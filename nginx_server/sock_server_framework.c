#include "sock_server_framework.h"
SOCKET client_list[10];
int client_idx = 0;
void __cdecl RecvThread(void* p)
{
	SOCKET sock = (SOCKET)p;
	char buf[256];
	while (1)
	{
		//Recive From Client
		int recvsize = recv(sock, buf, sizeof(buf), 0);
		if (recvsize <= 0)		break;
		//------------------------------------------------
		buf[recvsize] = '\0';
		printf("%s\n", buf);
		//Send Data To Clientx
		for (int i = 0; i < client_idx; i++)
		{
			if (client_list[i] != sock)
			{
				int sendsize = send(client_list[i], buf, (int)strlen(buf), 0);
			}
		}
		//-----------------------------------------------
	}
	printf("���� ����\n");
	//------------vector�� �ִ� ������ �����-----------
	for (int i = 0; i < client_idx; i++)
	{
		if (client_list[i] == sock)
		{
			client_list[i] = NULL;
			break;
		}
	}
	//---------------------------------------------------

	//------------���� ����---------------------
	closesocket(sock);
	//----------------------------------------
}
void bind_sock_srv(void) {
	//-------���� ���̺귯�� �ҷ�����(?)--------
	WSADATA wsaData;
	int retval = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (retval != 0)
	{
		printf("WSAStartup() Error\n");
	}
	//-------------------------------------------

	//----------���� ����--------------
	SOCKET serv_sock;
	serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock == SOCKET_ERROR)
	{
		printf("socket() Error\n");
	}
	//-----------------------------------

	//--------����(�ڽ�)�� ���� ���� �Է�------------
	SOCKADDR_IN serv_addr = { 0 };					// �ʱ�ȭ
	serv_addr.sin_family = AF_INET;					// IP ���
	serv_addr.sin_port = htons(2513);				// ��Ʈ 4000��
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// �ּҴ� �˾Ƽ� ã��
	//------------------------------------------------

	//-----------���ͳݿ� ����---------------------
	retval = bind(serv_sock, (SOCKADDR*)& serv_addr, sizeof(SOCKADDR));
	if (retval == SOCKET_ERROR)
	{
		printf("bind() Error\n");
	}
	//--------------------------------------------

	//-----------����ο� ����-----------------
	listen(serv_sock, 5);		// 5������� ����� �� �ְ� ��.
	//-------------------------------------------
	SOCKADDR_IN clnt_addr = { 0 };
	int size = sizeof(SOCKADDR_IN);

	while (1)
	{
		//-------------Ŭ���̾�Ʈ ���� ���, connect�� �ϸ� ������-------------
		SOCKET clnt_sock = accept(serv_sock, (SOCKADDR*)& clnt_addr, &size);
		if (clnt_sock == SOCKET_ERROR)
		{
			printf("accept() Error\n");
			continue;
		}
		//----------------------------------------------------------------------

		//----------------vector �� ������ �ֱ�------------
		client_list[client_idx++] = clnt_sock;
		//--------------------------------------------------

		printf("Ŭ���̾�Ʈ ����\n");
		printf("IP : %s, Port : %d\n", inet_ntoa(clnt_addr.sin_addr), clnt_addr.sin_port);
		//-----------���� ������ ����-------------
		_beginthread(RecvThread, 0, (void*)clnt_sock);
		//-----------------------------------------
	}
	//----------���� ����---------------
	closesocket(serv_sock);
	//-----------------------------------

	//-------���̺귯�� ����(?)---------
	WSACleanup();
	//----------------------------------

}
void ErrorHandling(char* message) {
	WSACleanup();
	fputs(message, stderr);
	fputc('\n', stderr);
	char c = _getch();
	exit(1);
}