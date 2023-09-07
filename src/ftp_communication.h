

void sendCommand(int socket, char *p);
int32_t ftp_connection(uint8_t *ip);
int32_t ftp_data_connection(int32_t cmd_socket, int32_t data_waiting_socket);
int32_t ftp_pre_data_connection(int32_t cmd_socket);
int32_t ftpPort(uint64_t local_ip, int cmd_socket, int data_waiting_socket);
int32_t ftp_data_close(int data_waiting_socket);
