#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/uio.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>


void recv_line(int32_t socket, char *p)
{
	char *org_p = p;
	int32_t ret;

	while (1){
		if ( (ret = recv(socket, p, 1, 0)) < 0 ) {
			perror("recv");
			break;
		}
		if (ret == 0) {
			printf("recv ret 0.");
			break;
		}
		if ( *p == '\n' ) break;
		p++;
	}
	*(++p) = '\0';
}

int32_t getReply(int sock, char *p)
{
	uint8_t res[1024] = {0};

	do { 
		recv_line(sock, res);
		fprintf(stderr, "<-- %s", res);
	} while (isdigit(res[0]) && isdigit(res[1]) && isdigit(res[2]) && res[3]=='-' );
}

/*--------------------------------------------------
 * 指定されたソケット socket に文字列 p を送信。
 * 文字列 p の終端は \0 で terminate されている
 * 必要がある
 */
void sendCommand(int socket, char *p)
{
    fprintf(stderr, "--> %s", p);
    write(socket, p, strlen(p));
	getReply(socket, NULL);
}

void error( char *message )
{
	fprintf(stderr, message);
	exit(1);
}

int32_t ftp_connection(uint8_t *ip)
{
	int cmd_socket = 0;
	struct sockaddr_in server;

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_port = getservbyname("ftp", "tcp")->s_port;

	cmd_socket = socket(AF_INET, SOCK_STREAM, 0);
	printf("create socket=%d, server try connect.\n", cmd_socket);
	connect(cmd_socket, (struct sockaddr *)&server, sizeof(server));
	getReply(cmd_socket, NULL);

	return cmd_socket;
}

int32_t ftpPort(uint64_t local_ip, int cmd_socket, int data_waiting_socket)
{
	struct sockaddr_in sin;
	socklen_t len;
	uint8_t send_mesg[255] = {0};

	/* ポート番号を取得 */
	if ( getsockname(data_waiting_socket,
					(struct sockaddr *)&sin, &len) < 0 ){
		perror("getsockname(data_waiting_socket) failed.");
	}
	sin.sin_port = htons((uint16_t)100050);
	sprintf(send_mesg, "PORT %d,%d,%d,%d,%d,%d\n",
			(int)(local_ip >> 24) & 0xff,
			(int)(local_ip >> 16) & 0xff,
			(int)(local_ip >>  8) & 0xff,
			(int)(local_ip)       & 0xff,
			(ntohs(sin.sin_port) >>  8) & 0xff,
			ntohs(sin.sin_port)        & 0xff);
			/* PORT・RETR を送信 */
	sendCommand(cmd_socket, send_mesg);
}

int32_t ftp_pre_data_connection(int32_t cmd_socket)
{
	int32_t data_waiting_socket = 0;
	struct sockaddr_in sin;
	socklen_t len;

	data_waiting_socket = socket(AF_INET, SOCK_STREAM, 0);

	sin.sin_family = AF_INET;
	sin.sin_port = 0;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	if ( bind(data_waiting_socket, (struct sockaddr *)&sin, sizeof sin) < 0 ){
	    perror("bind failed.");
	}
	if ( listen(data_waiting_socket, SOMAXCONN) == -1 ){
	    perror("listen failed.\n");
	}
	
	len = sizeof(sin);
	if ( getsockname(cmd_socket,
					(struct sockaddr *)&sin, &len) < 0 ){
		perror("getsockname(cmd_socket) failed.");
	}
	// sin.sin_addr.s_addr = inet_addr("192.168.2.1");
	ftpPort(ntohl(sin.sin_addr.s_addr), cmd_socket, data_waiting_socket);

	return data_waiting_socket;
}

#define BUF_LEN	(10 * 1024 * 1024)
uint8_t buf[BUF_LEN];
int32_t ftp_data_connection(int32_t cmd_socket, int32_t data_waiting_socket)
{
	struct sockaddr_in sin;
	socklen_t len;
	int32_t data_socket;

	len = sizeof(sin);
	if ( getsockname(cmd_socket,
					(struct sockaddr *)&sin, &len) < 0 ){
		perror("getsockname(cmd_socket) failed.");
	}

	data_socket = accept(data_waiting_socket, (struct sockaddr *)&sin, &len);
	if ( data_socket == -1 ){
		perror("accept failed.");
	}
	int read_size;
	while (1){
	    read_size = read(data_socket, buf, BUF_LEN);
	    if ( read_size > 0 ){
	        write(1, buf, read_size);
	    } else {
	        break;
	    }
	}
	printf("received data:\n%.*s^n\n", read_size, buf);
	getReply(cmd_socket, NULL);
	close(data_socket);
}

int32_t ftp_data_close(int data_waiting_socket)
{
	close(data_waiting_socket);
}


