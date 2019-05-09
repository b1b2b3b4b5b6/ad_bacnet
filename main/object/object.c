#include "object.h"
#include "get_data.h"

static const char *TAG = "object.c";

void object_init()
{
    Analog_Output_Init();
    Analog_Input_Init();
    Analog_Value_Init();
    Binary_Input_Init();
}

int get_device_timeout(device_t * device)
{
    return 0;
}
int get_device_upload_time(device_t * device, char *str)
{
    strcpy(str, device->upload_time);
    return 0; 
}
int get_device_description(device_t * device, char *str)
{
    sprintf(str, "%s", "show demo");
    return 0;    
}

#define DEVICE_CREATE(str) \
int check_change_##str(device_t * device)\
{\
    if(device->status_change.b_##str == true)\
        return 1;\
    else\
        return 0;\
}\
int clear_change_##str(device_t * device)\
{\
    device->status_change.b_##str = false;\
    return 0;\
}\
int get_device_name_##str(device_t * device, char *name)\
{\
    sprintf(name, "%s", #str);  \
    return 0;\
}\
float get_device_value_##str(device_t * device)\
{\
    return device->d_##str;\
}

#define DEIVCE_OBJECT(type, objest_type, id, str) \
    {\
        type,\
        objest_type,\
        id,\
        g_device_list,\
        0,\
        check_change_##str,\
        clear_change_##str,\
        get_device_name_##str,\
        get_device_description,\
        get_device_value_##str,\
        get_device_timeout,\
        get_device_upload_time,\
        NULL,\
    },\


DEVICE_CREATE(sw_1)
DEVICE_CREATE(sw_2)
DEVICE_CREATE(sw_3)
DEVICE_CREATE(sw_4)

object_t g_object_list[] = {

    DEIVCE_OBJECT(SW_1, OBJECT_BINARY_INPUT, 1000, sw_1)
    DEIVCE_OBJECT(SW_2, OBJECT_BINARY_INPUT, 2000, sw_2)
    DEIVCE_OBJECT(SW_3, OBJECT_BINARY_INPUT, 3000, sw_3)
    DEIVCE_OBJECT(SW_4, OBJECT_BINARY_INPUT, 4000, sw_4)

    {
        -1,
        0,
        0,
        NULL,
        0,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};
