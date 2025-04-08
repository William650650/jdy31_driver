#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include "jdy31_sysfs.h"
#include "jdy31_serdev.h"

#define POLL_TIMER_VAL_MS 10000
#define RESP_TIMEOUT_MS 200

static enum at_cmd poll_at_cmd[] = {
	AT_VERSION,
	AT_LADDR,
	AT_PIN,
	AT_BAUD,
	AT_NAME
};

inline void jdy31_get_cmd_resp(struct jdy31_priv *priv, int at_cmd, char *buf,
			       int *len)
{
	mutex_lock(&priv->cmd_record_lock);
	if (priv->cmd_record[at_cmd] &&
	    0 < priv->cmd_record[at_cmd]->resp_len) {
		memcpy(buf, priv->cmd_record[at_cmd]->resp_str,
		       priv->cmd_record[at_cmd]->resp_len);
		*len = priv->cmd_record[at_cmd]->resp_len;
	}
	mutex_unlock(&priv->cmd_record_lock);
}

inline void jdy31_add_cmd_work(struct jdy31_priv *priv, struct cmd_record *rec)
{
	spin_lock_bh(&priv->cmd_lock);
	list_add_tail(&rec->list, &priv->cmd_list);
	spin_unlock_bh(&priv->cmd_lock);

	schedule_work(&priv->cmd_work);
}

static void poll_timer_cb(struct timer_list *timer)
{
	struct jdy31_priv *priv = from_timer(priv, timer, poll_timer);
	struct serdev_device *serdev = priv->serdev;
	struct cmd_record *cmd_record;

	dev_info(&serdev->dev, "poll_timer_cb\n");
	for (int i = 0; i < sizeof(poll_at_cmd) / sizeof(poll_at_cmd[0]); i++) {
		cmd_record = at_alloc_record_param(poll_at_cmd[i], NULL, 0);

		spin_lock_bh(&priv->cmd_lock);
		list_add_tail(&cmd_record->list, &priv->cmd_list);
		spin_unlock_bh(&priv->cmd_lock);
	}

	schedule_work(&priv->cmd_work);

	mod_timer(&priv->poll_timer,
		  jiffies + msecs_to_jiffies(POLL_TIMER_VAL_MS));
}

static void cmd_work_cb(struct work_struct *work)
{
	struct jdy31_priv *priv =
		container_of(work, struct jdy31_priv, cmd_work);
	struct serdev_device *serdev = priv->serdev;
	struct cmd_record *first;

	dev_info(&serdev->dev, "cmd_work_cb\n");
	while (1) {
		spin_lock_bh(&priv->cmd_lock);
		first = list_first_entry_or_null(&priv->cmd_list,
						 struct cmd_record, list);
		if (!first) {
			spin_unlock_bh(&priv->cmd_lock);
			return;
		}
		list_del_init(&first->list);
		spin_unlock_bh(&priv->cmd_lock);

		mutex_lock(&priv->cmd_wait_lock);
		list_add_tail(&first->list, &priv->cmd_wait_list);
		serdev_device_write(serdev, first->cmd_str, first->cmd_len,
				    MAX_SCHEDULE_TIMEOUT);
		mutex_unlock(&priv->cmd_wait_lock);

		if (!wait_for_completion_timeout(
			    &priv->resp_complete,
			    msecs_to_jiffies(RESP_TIMEOUT_MS))) {
			mutex_lock(&priv->cmd_wait_lock);
			first = list_first_entry_or_null(
				&priv->cmd_wait_list, struct cmd_record, list);
			list_del_init(&first->list);
			mutex_unlock(&priv->cmd_wait_lock);
			at_free_record(first);
		}
		reinit_completion(&priv->resp_complete);
	}
}

static int jdy31_receive_buf(struct serdev_device *serdev,
			     const unsigned char *buf, size_t count)
{
	struct jdy31_priv *priv = serdev_device_get_drvdata(serdev);
	char *resp_start = 0, *resp_end = 0;
	struct cmd_record *rec;
	int ret = count;

	// dev_info(&serdev->dev, "jdy31_receive_buf, receive_buf = %s, receive_len = %d\n", priv->receive_buf, priv->receive_len);
	// dev_info(&serdev->dev, "jdy31_receive_buf, buf = %s, count = %d\n", buf, count);

	if (priv->receive_len + count > RECEIVE_BUF_LEN) {
		ret = RECEIVE_BUF_LEN - priv->receive_len;
	}

	memcpy(priv->receive_buf + priv->receive_len, buf, ret);
	priv->receive_len += ret;

	resp_start = strstr(priv->receive_buf, "+");
	if (!resp_start) {
		return ret;
	}
	resp_end = strstr(resp_start, "\r\n");
	if (!resp_end) {
		return ret;
	}
	resp_end += 2;

	mutex_lock(&priv->cmd_wait_lock);
	rec = list_first_entry_or_null(&priv->cmd_wait_list, struct cmd_record,
				       list);
	if (!rec) {
		mutex_unlock(&priv->cmd_wait_lock);
		dev_warn(&serdev->dev,
			 "jdy31_receive_buf, cmd_wait_list is empty\n");
		goto clean_receive_buf;
	}
	list_del_init(&rec->list);
	mutex_unlock(&priv->cmd_wait_lock);

	complete(&priv->resp_complete);

	rec->resp_len = resp_end - resp_start;
	memcpy(rec->resp_str, resp_start, rec->resp_len);

	mutex_lock(&priv->cmd_record_lock);
	if (priv->cmd_record[rec->cmd]) {
		at_free_record(priv->cmd_record[rec->cmd]);
	}
	priv->cmd_record[rec->cmd] = rec;
	mutex_unlock(&priv->cmd_record_lock);
	dev_info(&serdev->dev,
		 "jdy31_receive_buf, cmd = %d, cmd_str %s, resp_str %s\n",
		 rec->cmd, rec->cmd_str, rec->resp_str);

clean_receive_buf:
	memset(resp_start, 0, rec->resp_len);
	memcpy(priv->receive_buf, resp_end, ret - rec->resp_len);
	priv->receive_len -= rec->resp_len;

	return ret;
}

static int of_open_serdev(struct serdev_device *serdev)
{
	struct device_node *node = serdev->dev.of_node;
	int ret = 0;
	int baudrate = 0;
	int has_flow_control = 0;

	if (of_property_read_u32(node, "baudrate", &baudrate))
		baudrate = 9600;

	if (of_property_read_bool(node, "flow-control"))
		has_flow_control = 0;

	ret = serdev_device_open(serdev);
	if (ret) {
		dev_err(&serdev->dev, "failed to open serdev\n");
		return ret;
	}
	serdev_device_set_baudrate(serdev, baudrate);
	serdev_device_set_flow_control(serdev, has_flow_control);
	// serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);
	// serdev_device_set_tiocm(serdev, 0, 0);
	dev_info(&serdev->dev,
		 "of_open_serdev, baudrate = %d, has_flow_control = %d\n",
		 baudrate, has_flow_control);

	return 0;
}

static struct serdev_device_ops jdy31_ops = {
	.receive_buf = jdy31_receive_buf,
	.write_wakeup = serdev_device_write_wakeup,
};

static int jdy31_probe(struct serdev_device *serdev)
{
	struct jdy31_priv *priv;
	int ret = 0;

	dev_info(&serdev->dev, "jdy31 probe\n");
	priv = devm_kzalloc(&serdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->serdev = serdev;
	serdev_device_set_drvdata(serdev, priv);
	serdev_device_set_client_ops(serdev, &jdy31_ops);

	ret = of_open_serdev(serdev);
	if (ret) {
		goto out;
	}

	ret = jdy31_sysfs_add(priv);
	if (ret) {
		goto close_serdev;
	}

	INIT_WORK(&priv->cmd_work, cmd_work_cb);

	INIT_LIST_HEAD(&priv->cmd_list);
	spin_lock_init(&priv->cmd_lock);

	INIT_LIST_HEAD(&priv->cmd_wait_list);
	mutex_init(&priv->cmd_wait_lock);
	init_completion(&priv->resp_complete);

	mutex_init(&priv->cmd_record_lock);

	timer_setup(&priv->poll_timer, poll_timer_cb, 0);
	mod_timer(&priv->poll_timer, jiffies + msecs_to_jiffies(1000));

	return 0;

close_serdev:
	serdev_device_close(serdev);
out:
	devm_kfree(&serdev->dev, priv);

	return ret;
}

static void jdy31_remove(struct serdev_device *serdev)
{
	struct jdy31_priv *priv = serdev_device_get_drvdata(serdev);

	del_timer_sync(&priv->poll_timer);
	cancel_work_sync(&priv->cmd_work);
	jdy31_sysfs_remove(priv);
	serdev_device_close(serdev);
	serdev_device_set_drvdata(serdev, NULL);
	devm_kfree(&serdev->dev, priv);
}

static const struct of_device_id jdy31_of_match[] = {
	{ .compatible = "vendor,jdy-31" },
	{}
};

static struct serdev_device_driver jdy31_serdev_driver = {
    .probe = jdy31_probe,
    .remove = jdy31_remove,
    .driver = {
        .name = "jdy-31",
        .of_match_table	= jdy31_of_match,
    },
};

static int __init jdy_init(void)
{
	int ret;

	ret = serdev_device_driver_register(&jdy31_serdev_driver);
	if (ret) {
		printk(KERN_ERR "jdy31 register serdev driver failed\n");
		return ret;
	}

	return 0;
}

static void __exit jdy_exit(void)
{
	serdev_device_driver_unregister(&jdy31_serdev_driver);
}

module_init(jdy_init);
module_exit(jdy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Wenlin Liu");
MODULE_DESCRIPTION("jdy-31 driver");
MODULE_VERSION("0.1");