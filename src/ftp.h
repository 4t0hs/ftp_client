
#include "ftp_cmd.h"
#include "ftp_communication.h"

typedef struct _FTP_CONFIG {
	uint8_t server_ip[16];
} FTP_CONFIG;

void ftp_init(FTP_CONFIG *p_conf);
int32_t ftp_login(char *usr, char *pass);
int32_t ftpRecvFile(uint8_t *path);
void ftpQuit(void);