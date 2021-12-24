/* echo_client.c
**데이터를 보낼 서버에 연결 요청을 보내고 수락되는 일반적인 클라이언트의 과정을 거치게 되고,
**연결이 완료되면 서버로 데이터를 전송하고 에코(echo)되어 돌아오는 데이터를 수신하는 과정을 거치게 된다.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#define BUFSIZE 1024

void error_handling(char *message);

int main(int argc, char **argv)
{
	int sock;
	struct sockaddr_in serv_addr;
	char message[BUFSIZE];
	int str_len;
	if (argc != 3)
	{
		printf("Usage : %s <IP> <port> \n", argv[0]);
		exit(1);
	}
	sock = socket(PF_INET, SOCK_STREAM, 0); /* 서버 접속을 위한 소켓 생성 */
	if (sock == -1)
		error_handling("socket() error");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");
	while (1)
	{
		/* 메시지 입력 전송*/
		fputs("전송할 메시지를 입력하세요(q to quit) : ", stdout);
		fgets(message, BUFSIZE, stdin);
		if (!strcmp(message, "q\n"))
			break;
		send(sock, message, strlen(message), 0);
		/* 메시지 수신 출력 */
		// str_len = recv(sock, message, BUFSIZE - 1, 0);
		// message[str_len] = 0;
		// printf("서버로부터 전송된 메시지 : %s \n", message);
	}
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}