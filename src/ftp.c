#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "ftp.h"

typedef struct {
	int32_t cmd_socket;
	FTP_CONFIG conf;
} FTP_CTX;
FTP_CTX _ftp_ctx;
#define get_myself()	(&_ftp_ctx)

static char *makeCommand(char *cmd, char *arg, char *p)
{
	if (arg != NULL) {
		sprintf(p, "%s %s\n", cmd, arg);
	} else {
		sprintf(p, "%s\n", cmd);
	}
	return p;
}
static void ftpSendCmd(int sock, int8_t *cmd, int8_t *arg)
{
	char msg[255] = {0};

	sendCommand(sock, makeCommand(cmd, arg, msg));
}
void ftp_init(FTP_CONFIG *p_conf)
{
	FTP_CTX *this = get_myself();

	memset(this, 0, sizeof(FTP_CTX));

	this->conf = *p_conf;
}

int32_t ftp_login(char *usr, char *pass)
{
	FTP_CTX *this = get_myself();
	char cmd[255] = {0};

	this->cmd_socket = ftp_connection(this->conf.server_ip);
	printf("FTP connection socket=%d\n", this->cmd_socket);
	// user ---
	sendCommand(this->cmd_socket, makeCommand(FTP_CMD_USER, usr, cmd));

	// pass ---
	sendCommand(this->cmd_socket, makeCommand(FTP_CMD_PASS, pass, cmd));
}

int32_t ftpRecvFile(uint8_t *path)
{
	FTP_CTX *this = get_myself();
	int data_waiting_socket;
	data_waiting_socket = ftp_pre_data_connection(this->cmd_socket);
	ftpSendCmd(this->cmd_socket, FTP_CMD_RETR, path);
	ftp_data_connection(this->cmd_socket, data_waiting_socket);
	ftp_data_close(data_waiting_socket);
}

void ftpQuit(void)
{
	FTP_CTX *this = get_myself();
	char cmd[255] = {0};

	sendCommand(this->cmd_socket, makeCommand(FTP_CMD_QUIT, NULL, cmd));
}
