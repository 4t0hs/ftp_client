#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ftp.h"

void main(char argc, char *argv[])
{
	printf("FTP client application start.\n");
	FTP_CONFIG conf;
	strcpy(conf.server_ip, "192.168.2.2");
	ftp_init(&conf);
	ftp_login("xilinx", "xilinx");
	ftpRecvFile("backup.cmd");
	ftpQuit();
}