#ifndef __JDY31_SYSFS_H__
#define __JDY31_SYSFS_H__

#include "jdy31_serdev.h"

int jdy31_sysfs_add(struct jdy31_priv *priv);
void jdy31_sysfs_remove(struct jdy31_priv *priv);

#endif