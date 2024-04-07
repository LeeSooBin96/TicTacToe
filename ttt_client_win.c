#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#define BUF_SIZE 30
void ClearBuffer(void);
void ErrorHandling(char* message);

char sel_ox(char n); //O, X 배치
void print_board(char* message);

int main(int argc, char* argv[])
{
	//서버 아이피, 포트번호 지정
	argc = 3; argv[1] = "192.168.0.14"; argv[2] = "9106";
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	char message[BUF_SIZE]; //통신에 사용할 버퍼
	//int strLen; //수신된 메시지 길이
	
	//게임에 필요한 요소들
	char pos[2]; //유저 입력 저장

	if (argc != 3)
	{
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //소켓 라이브러리 초기화
		ErrorHandling("WSAStartup() error!");

	hSocket = socket(PF_INET, SOCK_STREAM, 0); //본인 소켓 생성
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr)); //서버 주소 정보 설정
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) //서버에 연결요청
		ErrorHandling("connect() error!");
	else
		puts("waiting...");
	//게임 구현 부분
	while (1)
	{
		//서버에서 네 차례야 알려주는걸 만들어야할듯.
		if (recv(hSocket, message, sizeof(message), 0) != -1) //턴 대기
		{
			if (message[0] == 't') //본인 차례일때
			{
				while (1)
				{
					//보드판 데이터 수신
					if (recv(hSocket, message, sizeof(message), 0) == -1) ErrorHandling("read() error");
					//보드판 출력
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
						puts("잘못된 입력입니다. "); continue;
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


	closesocket(hSocket); //소켓 라이브러리 해제
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
	puts("┌─┬─┬─┐");
	for (i = 0;i < 3;i++) printf("│%c", sel_ox(message[i])); puts("│");
	puts("├─┼─┼─┤");
	for (i = 0;i < 3;i++) printf("│%c", sel_ox(message[i+3])); puts("│");
	puts("├─┼─┼─┤");
	for (i = 0;i < 3;i++) printf("│%c", sel_ox(message[i+6])); puts("│");
	puts("└─┴─┴─┘");
}
char sel_ox(char n) //O, X 배치
{
	switch (n)
	{
	case '0': return ' '; break;
	case '1': return 'O'; break;
	case '2': return 'X'; break;
	}
}