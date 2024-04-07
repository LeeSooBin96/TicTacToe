#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#define BUF_SIZE 30
void ClearBuffer(void);
void ErrorHandling(char* message);

char sel_ox(char n); //O, X ��ġ
void print_board(char* message);

int main(int argc, char* argv[])
{
	//���� ������, ��Ʈ��ȣ ����
	argc = 3; argv[1] = "192.168.0.14"; argv[2] = "9106";
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	char message[BUF_SIZE]; //��ſ� ����� ����
	//int strLen; //���ŵ� �޽��� ����
	
	//���ӿ� �ʿ��� ��ҵ�
	char pos[2]; //���� �Է� ����

	if (argc != 3)
	{
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //���� ���̺귯�� �ʱ�ȭ
		ErrorHandling("WSAStartup() error!");

	hSocket = socket(PF_INET, SOCK_STREAM, 0); //���� ���� ����
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr)); //���� �ּ� ���� ����
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) //������ �����û
		ErrorHandling("connect() error!");
	else
		puts("waiting...");
	//���� ���� �κ�
	while (1)
	{
		//�������� �� ���ʾ� �˷��ִ°� �������ҵ�.
		if (recv(hSocket, message, sizeof(message), 0) != -1) //�� ���
		{
			if (message[0] == 't') //���� �����϶�
			{
				while (1)
				{
					//������ ������ ����
					if (recv(hSocket, message, sizeof(message), 0) == -1) ErrorHandling("read() error");
					//������ ���
					print_board(message);
					printf("Your turn!\n(1 2 3\n 4 5 6\n 7 8 9)");
					fgets(pos, 2, stdin);
					ClearBuffer();
					if (pos[0] >= '1' && pos[0] <= '9')
					{
						if (send(hSocket, pos, sizeof(pos), 0) == SOCKET_ERROR) ErrorHandling("send() error");
						system("cls");
						puts("waiting...");
						break;
					}
					else
					{
						puts("�߸��� �Է��Դϴ�. "); continue;
					}
				}
			}
			else if (message[0] == 'w')
			{
				message[strlen(message)] = 0;
				printf("%s\n", message);
				continue;
			}
			else if (message[0] == 'u')
			{
				message[strlen(message)] = 0;
				printf("%s\n", message);
				break;
			}
		}
		else ErrorHandling("read() error!");
	}


	closesocket(hSocket); //���� ���̺귯�� ����
	WSACleanup();
	return 0;
}
void ClearBuffer(void) { while (getchar() != '\n'); }
void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
void print_board(char* message)
{
	int i;
	puts("��������������");
	for (i = 0;i < 3;i++) printf("��%c", sel_ox(message[i])); puts("��");
	puts("��������������");
	for (i = 0;i < 3;i++) printf("��%c", sel_ox(message[i+3])); puts("��");
	puts("��������������");
	for (i = 0;i < 3;i++) printf("��%c", sel_ox(message[i+6])); puts("��");
	puts("��������������");
}
char sel_ox(char n) //O, X ��ġ
{
	switch (n)
	{
	case '0': return ' '; break;
	case '1': return 'O'; break;
	case '2': return 'X'; break;
	}
}