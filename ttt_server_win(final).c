#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 30
void ErrorHandling(char* message);

int match(char* board); 

int main(int argc, char* argv[])
{
	//포트번호 지정
	argc = 2; argv[1] = "9106";
	WSADATA wsaData; //윈속관련 구조체
	SOCKET hServSock, hClntSock, hClntSock2; //소켓 핸들(디스크립터)
	SOCKADDR_IN servAddr, clntAddr, clntAddr2; //주소 정보

	int szClntAddr, szClntAddr2; //주소 크기(accept에 전달되어야함)
	char buf[BUF_SIZE]; //통신에 사용할 버퍼
	char board[10] = "000000000"; //보드판
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //소켓 라이브러리 초기화
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0); //서버 소켓 생성
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr)); //주소정보 초기화
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) //서버 소켓에 IP주소, PORT번호 할당
		ErrorHandling("bind() error");

	if (listen(hServSock, 2) == SOCKET_ERROR) // 연결 대기 상태 (서버 소켓으로 완성)
		ErrorHandling("listen() error");

	szClntAddr = sizeof(clntAddr); szClntAddr2 = sizeof(clntAddr2);
	while(1)
	{
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr); //클라이언트 연결 요청 수락
		if (hClntSock == INVALID_SOCKET) ErrorHandling("accept() error");
		else puts("Client 1 connected....");
		hClntSock2 = accept(hServSock, (SOCKADDR*)&clntAddr2, &szClntAddr2); //클라이언트 연결 요청 수락
		if (hClntSock2 == INVALID_SOCKET) ErrorHandling("accept() error");
		else puts("Client 2 conneced....");
		//게임 구현 부분
		while (1)
		{
			//서버가 't'을 보내면 클라이언트 실행하게!
			if (send(hClntSock, "t", 2, 0) == SOCKET_ERROR) ErrorHandling("send() error");
			//보드판 데이터 전송
			if (send(hClntSock, board, sizeof(board), 0) == SOCKET_ERROR) ErrorHandling("send() error");
			//유저 1 입력
			if (recv(hClntSock, buf, sizeof(buf), 0) == -1) ErrorHandling("read() error!");
			if (buf[0] == 'E') continue;
			else if(board[atoi(&buf[0])-1]=='0') board[atoi(&buf[0]) - 1] = '1';
			else //놓은곳에 접근하려고 할때
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
				//서버가 't'을 보내면 클라이언트 실행하게!
				if (send(hClntSock2, "t", 2, 0) == SOCKET_ERROR) ErrorHandling("send() error");
				//보드판 데이터 전송
				if (send(hClntSock2, board, sizeof(board), 0) == SOCKET_ERROR) ErrorHandling("send() error");
				//유저 2 입력
				if (recv(hClntSock2, buf, sizeof(buf), 0) == -1) ErrorHandling("read() error!");
				if (buf[0] == 'E') i--; //잘못된 입력
				else if (board[atoi(&buf[0]) - 1] == '0') board[atoi(&buf[0]) - 1] = '2';
				else //놓은곳에 접근하려고 할때
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
		for (int i = 0;i < 10;i++) board[i] = '0'; //보드판 초기화
	}
	closesocket(hServSock);
	WSACleanup(); //소켓 라이브러리 해제
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
		if (board[i] != '0' && board[i] == board[i + 1] && board[i] == board[i + 2])//가로 검사
			return 1;
		else if (board[i] != '0' && board[i] == board[i + 3] && board[i] == board[i + 6])//세로 검사
			return 1;
		else if (board[i] == '0' || board[i + 1] == '0' || board[i + 2] == '0') blank++;
	}
	if (blank == 0) return 2;
	else if (board[0] != '0' && board[0] == board[4] && board[0] == board[8]) return 1; //대각선 검사
	else if (board[2] != '0' && board[2] == board[4] && board[2] == board[6]) return 1;
	else return 0;
}