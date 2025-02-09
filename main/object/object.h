#ifndef OBJECT_H
#define OBJECT_H

#include "ao.h"
#include "av.h"
#include "bi.h"
#include "ai.h"

#define SITE_INFO_LEN 100
#define DEVICE_MODE_LEN 40
#define NAME_LEN 200

typedef struct {
    bool b_sw_1;
    bool b_sw_2;
    bool b_sw_3;
    bool b_sw_4;
}status_change_t;

typedef struct {
    int object_id;
    status_change_t status_change;
    int d_sw_1;
    int d_sw_2;
    int d_sw_3;
    int d_sw_4;
    char upload_time[100];
}device_t;

typedef enum {
    SW_1 = 1,
    SW_2 = 2,
    SW_3 = 3,
    SW_4 = 4,
}sensor_type_t;

typedef struct {
    int device_type;
    int object_type;
    long object_offset;
    device_t *device_list;
    long object_count;
    int (*check_sensor_change)(device_t *);
    int (*clear_sensor_change)(device_t *);
    int (*get_device_name)(device_t *, char *name);
    int (*get_device_description)(device_t *, char *);
    float (*get_device_value)(device_t *);
    int (*get_device_timeout)(device_t *);
    int (*get_device_upload_time)(device_t *, char *);
    int (*set_device_value)(device_t *, float);
}object_t;

typedef struct {
    object_t *object;
    device_t *device;
    uint32_t object_instance;
}device_key_t;

extern object_t g_object_list[];

void object_init();

#define OBJECT_INIT do{\
    int object_conut = 0;\
    g_device_list_count = 0;\
    object_t *object;\
    for(;;)\
    {   \
        object = &g_object_list[object_conut];\
        if(object->device_type == -1)\
            break;\
        if(object->object_type == TYPE)\
        {\
            for(int n = 0; n < (object->object_count); n++)\
            {\
                g_local_list[g_device_list_count].object = object;\
                g_local_list[g_device_list_count].device = &object->device_list[n];\
                g_local_list[g_device_list_count].object_instance = object->object_offset + object->device_list[n].object_id;\
                g_device_list_count++;\
            }\
        }\
        object_conut++;\
    }\
    BAC_LOGI("init success, count :%d", g_device_list_count);\
}while(0)



#endif
