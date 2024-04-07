#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 30
void ErrorHandling(char* message);

int match(char* board); 

int main(int argc, char* argv[])
{
	//��Ʈ��ȣ ����
	argc = 2; argv[1] = "9106";
	WSADATA wsaData; //���Ӱ��� ����ü
	SOCKET hServSock, hClntSock, hClntSock2; //���� �ڵ�(��ũ����)
	SOCKADDR_IN servAddr, clntAddr, clntAddr2; //�ּ� ����

	int szClntAddr, szClntAddr2; //�ּ� ũ��(accept�� ���޵Ǿ����)
	char buf[BUF_SIZE]; //��ſ� ����� ����
	char board[10] = "000000000"; //������
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //���� ���̺귯�� �ʱ�ȭ
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0); //���� ���� ����
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr)); //�ּ����� �ʱ�ȭ
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) //���� ���Ͽ� IP�ּ�, PORT��ȣ �Ҵ�
		ErrorHandling("bind() error");

	if (listen(hServSock, 2) == SOCKET_ERROR) // ���� ��� ���� (���� �������� �ϼ�)
		ErrorHandling("listen() error");

	szClntAddr = sizeof(clntAddr); szClntAddr2 = sizeof(clntAddr2);
	while(1)
	{
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr); //Ŭ���̾�Ʈ ���� ��û ����
		if (hClntSock == INVALID_SOCKET) ErrorHandling("accept() error");
		else puts("Client 1 connected....");
		hClntSock2 = accept(hServSock, (SOCKADDR*)&clntAddr2, &szClntAddr2); //Ŭ���̾�Ʈ ���� ��û ����
		if (hClntSock2 == INVALID_SOCKET) ErrorHandling("accept() error");
		else puts("Client 2 conneced....");
		//���� ���� �κ�
		while (1)
		{
			//������ 't'�� ������ Ŭ���̾�Ʈ �����ϰ�!
			if (send(hClntSock, "t", 2, 0) == SOCKET_ERROR) ErrorHandling("send() error");
			//������ ������ ����
			if (send(hClntSock, board, sizeof(board), 0) == SOCKET_ERROR) ErrorHandling("send() error");
			//���� 1 �Է�
			if (recv(hClntSock, buf, sizeof(buf), 0) == -1) ErrorHandling("read() error!");
			if (buf[0] == 'E') continue;
			else if(board[atoi(&buf[0])-1]=='0') board[atoi(&buf[0]) - 1] = '1';
			else //�������� �����Ϸ��� �Ҷ�
			{
				strcpy(buf, "wrong pos!");
				if (send(hClntSock, buf, sizeof(buf), 0) == SOCKET_ERROR) ErrorHandling("send() error");
				Sleep(1000);continue;
			}
			if (match(board) == 1)
			{
				strcpy(buf, "user1 is win!");
				break;
			}
			else if (match(board) == 2)
			{
				strcpy(buf, "Draw!!!");
				break;
			}

			for (int i = 0;i < 1;i++)
			{
				//������ 't'�� ������ Ŭ���̾�Ʈ �����ϰ�!
				if (send(hClntSock2, "t", 2, 0) == SOCKET_ERROR) ErrorHandling("send() error");
				//������ ������ ����
				if (send(hClntSock2, board, sizeof(board), 0) == SOCKET_ERROR) ErrorHandling("send() error");
				//���� 2 �Է�
				if (recv(hClntSock2, buf, sizeof(buf), 0) == -1) ErrorHandling("read() error!");
				if (buf[0] == 'E') i--; //�߸��� �Է�
				else if (board[atoi(&buf[0]) - 1] == '0') board[atoi(&buf[0]) - 1] = '2';
				else //�������� �����Ϸ��� �Ҷ�
				{
					strcpy(buf, "wrong pos!");
					if (send(hClntSock2, buf, sizeof(buf), 0) == SOCKET_ERROR) ErrorHandling("send() error");
					Sleep(1000); i--;
				}
			}
			if (match(board) == 1)
			{
				strcpy(buf, "user2 is win!");
				break;
			}
			else if (match(board) == 2)
			{
				strcpy(buf, "Draw!!!");
				break;
			}
			
		}
		if (send(hClntSock, buf, sizeof(buf), 0) == SOCKET_ERROR) ErrorHandling("send() error");
		if (send(hClntSock2, buf, sizeof(buf), 0) == SOCKET_ERROR) ErrorHandling("send() error");
		
		closesocket(hClntSock); closesocket(hClntSock2);
		for (int i = 0;i < 10;i++) board[i] = '0'; //������ �ʱ�ȭ
	}
	closesocket(hServSock);
	WSACleanup(); //���� ���̺귯�� ����
	return 0;
}
void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
int match(char* board)
{
	int blank=0;
	for (int i = 0;i < 9;i+=3)
	{
		if (board[i] != '0' && board[i] == board[i + 1] && board[i] == board[i + 2])//���� �˻�
			return 1;
		else if (board[i] != '0' && board[i] == board[i + 3] && board[i] == board[i + 6])//���� �˻�
			return 1;
		else if (board[i] == '0' || board[i + 1] == '0' || board[i + 2] == '0') blank++;
	}
	if (blank == 0) return 2;
	else if (board[0] != '0' && board[0] == board[4] && board[0] == board[8]) return 1; //�밢�� �˻�
	else if (board[2] != '0' && board[2] == board[4] && board[2] == board[6]) return 1;
	else return 0;
}