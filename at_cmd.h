#ifndef __AT_CMD_H__
#define __AT_CMD_H__

#include <linux/list.h>

#define AT_CMD_LEN 128
#define AT_RESP_LEN 128

enum at_cmd {
	AT_VERSION,
	AT_RESET,
	AT_DISC,
	AT_LADDR,
	AT_PIN,
	AT_BAUD,
	AT_NAME,
	AT_DEFAULT,
	AT_ENLOG,
	AT_MAX
};

struct cmd_record {
	struct list_head list;
	enum at_cmd cmd;
	char cmd_str[AT_CMD_LEN];
	int cmd_len;
	char resp_str[AT_RESP_LEN];
	int resp_len;
};

struct cmd_record *at_alloc_record_param(enum at_cmd cmd, const char *param,
					 int len);
void at_free_record(struct cmd_record *rec);

int at_addr_valid(const char *addr, int len);
int at_pin_valid(const char *addr, int len);
int at_baudrate_valid(const char *addr, int len);
int at_name_valid(const char *addr, int len);

#endif