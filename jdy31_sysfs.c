#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/module.h>

#include "jdy31_sysfs.h"

ssize_t version_attr_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	ssize_t len = 0;

	jdy31_get_cmd_resp(priv, AT_VERSION, buf, &len);

	return len;
}

ssize_t reset_attr_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	ssize_t len = 0;

	jdy31_get_cmd_resp(priv, AT_RESET, buf, &len);

	return len;
}

ssize_t reset_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	struct cmd_record *record;

	record = at_alloc_record_param(AT_RESET, NULL, 0);
	if (record) {
		jdy31_add_cmd_work(priv, record);
		return count;
	}

	return -ENOMEM;
}

ssize_t disconnect_attr_show(struct kobject *kobj, struct kobj_attribute *attr,
			     char *buf)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	ssize_t len = 0;

	jdy31_get_cmd_resp(priv, AT_DISC, buf, &len);

	return len;
}

ssize_t disconnect_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
			      const char *buf, size_t count)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	struct cmd_record *record;

	record = at_alloc_record_param(AT_DISC, NULL, 0);
	if (record) {
		jdy31_add_cmd_work(priv, record);
		return count;
	}

	return -ENOMEM;
}

ssize_t addr_attr_show(struct kobject *kobj, struct kobj_attribute *attr,
		       char *buf)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	ssize_t len = 0;

	jdy31_get_cmd_resp(priv, AT_LADDR, buf, &len);

	return len;
}

ssize_t addr_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	struct cmd_record *record;

	if (!buf || !count || !at_addr_valid(buf, count)) {
		return -EINVAL;
	}

	record = at_alloc_record_param(AT_LADDR, buf, count);
	if (record) {
		jdy31_add_cmd_work(priv, record);
		return count;
	}

	return -ENOMEM;
}

ssize_t pin_attr_show(struct kobject *kobj, struct kobj_attribute *attr,
		      char *buf)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	ssize_t len = 0;

	jdy31_get_cmd_resp(priv, AT_PIN, buf, &len);

	return len;
}

ssize_t pin_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
		       const char *buf, size_t count)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	struct cmd_record *record;

	if (!buf || !count || !at_pin_valid(buf, count)) {
		return -EINVAL;
	}

	record = at_alloc_record_param(AT_PIN, buf, count);
	if (record) {
		jdy31_add_cmd_work(priv, record);
		return count;
	}

	return -ENOMEM;
}

ssize_t baudrate_attr_show(struct kobject *kobj, struct kobj_attribute *attr,
			   char *buf)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	ssize_t len = 0;

	jdy31_get_cmd_resp(priv, AT_BAUD, buf, &len);

	return len;
}

ssize_t baudrate_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
			    const char *buf, size_t count)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	struct cmd_record *record;

	if (!buf || !count || !at_baudrate_valid(buf, count)) {
		return -EINVAL;
	}

	record = at_alloc_record_param(AT_BAUD, buf, count);
	if (record) {
		jdy31_add_cmd_work(priv, record);
		return count;
	}

	return -ENOMEM;
}

ssize_t name_attr_show(struct kobject *kobj, struct kobj_attribute *attr,
		       char *buf)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	ssize_t len = 0;

	jdy31_get_cmd_resp(priv, AT_NAME, buf, &len);

	return len;
}

ssize_t name_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	struct cmd_record *record;

	if (!buf || !count || !at_name_valid(buf, count)) {
		return -EINVAL;
	}

	record = at_alloc_record_param(AT_NAME, buf, count);
	if (record) {
		jdy31_add_cmd_work(priv, record);
		return count;
	}

	return -ENOMEM;
}

ssize_t default_attr_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	ssize_t len = 0;

	jdy31_get_cmd_resp(priv, AT_DEFAULT, buf, &len);

	return len;
}

ssize_t default_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t count)
{
	struct jdy31_priv *priv = container_of(kobj, struct jdy31_priv, kobj);
	struct cmd_record *record;

	record = at_alloc_record_param(AT_DEFAULT, NULL, 0);
	if (record) {
		jdy31_add_cmd_work(priv, record);
		return count;
	}

	return -ENOMEM;
}

static ssize_t jdy31_attr_show(struct kobject *kobj, struct attribute *attr,
			       char *buf)
{
	struct kobj_attribute *jdy31_attr =
		container_of(attr, struct kobj_attribute, attr);

	if (jdy31_attr->show)
		return jdy31_attr->show(kobj, jdy31_attr, buf);

	return -EIO;
}

static ssize_t jdy31_attr_store(struct kobject *kobj, struct attribute *attr,
				const char *buf, size_t count)
{
	struct kobj_attribute *jdy31_attr =
		container_of(attr, struct kobj_attribute, attr);

	if (jdy31_attr->store)
		return jdy31_attr->store(kobj, jdy31_attr, buf, count);

	return -EIO;
}

static void jdy31_release(struct kobject *kobj)
{
}

struct kobj_attribute version_attr =
	__ATTR(version, 0444, version_attr_show, NULL);
struct kobj_attribute reset_attr =
	__ATTR(reset, 0644, reset_attr_show, reset_attr_store);
struct kobj_attribute disconnect_attr =
	__ATTR(disconnect, 0644, disconnect_attr_show, disconnect_attr_store);
struct kobj_attribute addr_attr =
	__ATTR(addr, 0644, addr_attr_show, addr_attr_store);
struct kobj_attribute pin_attr =
	__ATTR(pin, 0644, pin_attr_show, pin_attr_store);
struct kobj_attribute baudrate_attr =
	__ATTR(baudrate, 0644, baudrate_attr_show, baudrate_attr_store);
struct kobj_attribute name_attr =
	__ATTR(name, 0644, name_attr_show, name_attr_store);
struct kobj_attribute default_attr =
	__ATTR(default, 0644, default_attr_show, default_attr_store);

static struct attribute *attrs[] = {
	&version_attr.attr,
    &reset_attr.attr,
    &disconnect_attr.attr,
	&addr_attr.attr,
    &pin_attr.attr,
    &baudrate_attr.attr,
	&name_attr.attr,
    &default_attr.attr,
    NULL
};

static struct attribute_group attr_group = {
	.attrs = attrs,
	.is_visible = NULL,
};

struct sysfs_ops jdy31_sysfs_ops = {
	.show = jdy31_attr_show,
	.store = jdy31_attr_store,
};

static const struct kobj_type jdy31_ktype = {
	.release = jdy31_release,
	.sysfs_ops = &jdy31_sysfs_ops,
};

int jdy31_sysfs_add(struct jdy31_priv *priv)
{
	int ret = 0;

	priv->kobj.kset = priv->serdev->dev.kobj.kset;
	kobject_init(&priv->kobj, &jdy31_ktype);
	ret = kobject_add(&priv->kobj, NULL, "%sjdy31",
			  priv->serdev->ctrl->dev.of_node->full_name);
	if (ret) {
		printk(KERN_ERR "Failed to add jdy31 kobject\n");
		return ret;
	}

	ret = sysfs_create_group(&priv->kobj, &attr_group);
	if (ret) {
		printk(KERN_ERR "Failed to create jdy31 sysfs group\n");
		kobject_del(&priv->kobj);
		return ret;
	}

	return 0;
}

void jdy31_sysfs_remove(struct jdy31_priv *priv)
{
	kobject_del(&priv->kobj);
}
