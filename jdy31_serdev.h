#ifndef __JDY31_SERDEV_H__
#define __JDY31_SERDEV_H__

#include <linux/serdev.h>
#include <linux/of.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/mod_devicetable.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include "at_cmd.h"

#define RECEIVE_BUF_LEN 1024

struct jdy31_priv {
	struct serdev_device *serdev;

	struct timer_list poll_timer;

	struct work_struct cmd_work;
	struct list_head cmd_list;
	struct spinlock cmd_lock;

	struct mutex cmd_wait_lock;
	struct list_head cmd_wait_list;
	struct completion resp_complete;

	struct mutex cmd_record_lock;
	struct cmd_record *cmd_record[AT_MAX];

	struct kobject kobj;

	char receive_buf[RECEIVE_BUF_LEN];
	int receive_len;
};

void jdy31_get_cmd_resp(struct jdy31_priv *priv, int at_cmd, char *buf,
			int *len);
void jdy31_add_cmd_work(struct jdy31_priv *priv, struct cmd_record *rec);

#endif