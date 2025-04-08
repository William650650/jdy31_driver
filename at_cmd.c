#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>

#include "at_cmd.h"

const char *at_cmd_str[AT_MAX] = {
    "AT+VERSION",
    "AT+RESET",
    "AT+DISC",
    "AT+LADDR",
    "AT+PIN",
    "AT+BAUD",
    "AT+NAME",
    "AT+DEFAULT",
    "AT+ENLOG"
};

static int at_cmd(enum at_cmd cmd, char *buf)
{
	int cmd_len = strlen(at_cmd_str[cmd]);

	strncpy(buf, at_cmd_str[cmd], cmd_len);
	buf[cmd_len++] = '\r';
	buf[cmd_len++] = '\n';

	return cmd_len;
}

static int at_cmd_param(enum at_cmd cmd, char *buf, const char *param, int len)
{
	int cmd_len = strlen(at_cmd_str[cmd]);

	strncpy(buf, at_cmd_str[cmd], cmd_len);
	memcpy(buf + cmd_len, param, len);
	cmd_len += len;
	buf[cmd_len++] = '\r';
	buf[cmd_len++] = '\n';

	return cmd_len;
}

struct cmd_record *at_alloc_record_param(enum at_cmd cmd, const char *param,
					 int len)
{
	struct cmd_record *rec = NULL;

	rec = kzalloc(sizeof(struct cmd_record), GFP_ATOMIC);
	if (!rec) {
		return NULL;
	}

	rec->cmd = cmd;
	if (param && len > 0)
		rec->cmd_len = at_cmd_param(cmd, rec->cmd_str, param, len);
	else
		rec->cmd_len = at_cmd(cmd, rec->cmd_str);

	return rec;
}

void at_free_record(struct cmd_record *rec)
{
	kfree(rec);
}

int at_addr_valid(const char *addr, int len)
{
	return true;
}

int at_pin_valid(const char *addr, int len)
{
	return true;
}

int at_baudrate_valid(const char *addr, int len)
{
	return true;
}

int at_name_valid(const char *addr, int len)
{
	return true;
}
