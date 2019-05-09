#ifndef GET_DATA_H
#define GET_DATA_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "mdf_json.h"
#include "common.h"

#include "pthread.h"
#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include "request.h"
#include "device.h"

extern device_t g_device_list[];

void get_data_init(char *path);
void python_clean();

void server_lock();
void server_unlock();
void cov_lock();
void cov_unlock();




#endif