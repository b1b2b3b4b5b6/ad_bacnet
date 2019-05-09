#include "get_data.h"

const char *TAG = "get_data.c";

#define HEART_BEAT_TIME 60*10

#define DEVICE_MAX_COUNT 2000

#define REFRESH_PERIOD 1

#define PATH "usr/share/show_demo"


device_t g_device_list[DEVICE_MAX_COUNT] = {0};
int g_device_list_count = 0;

int fresh_seq;
sem_t server_sem;
sem_t cov_sem;
FILE *g_log = NULL;

static void open_log()
{
    if(opendir(PATH"/log") == NULL)
    {
        if(mkdir(PATH"/log", 0777) != 0)
        {
            BAC_LOGE("mkdir "PATH"/log fail");
            return;
        }

    }

    g_log = fopen(PATH"/log/log.txt","w+");
    if(g_log == NULL)
        BAC_LOGE("open "PATH"/log.txt fail");

}

static void print_info(char *str)
{
    BAC_LOGD("%s",str);
    char temp[100] = {0};
  
    for(int n = 0; n < g_device_list_count; n++)
    {
        // device_t *device = &g_device_list[n];
        // BAC_LOGD("id: %3d | temp: %f | humidity: %f | pir: %6d | smoke: %6d | plug: %6d | kitchen: %6d | energy: %6f | upload_time: %s",
        //     device->object_id, device->temperature, device->humidity, device->pir,
        //     device->smoke, device->plug, device->kitchen, device->energy,
        //     device->upload_time);
        // if(g_log != NULL)
        // {
        //     fprintf(g_log, "id: %3d | temp: %6d | humidity: %f | pir: %6d | smoke: %6d | plug: %6d | kitchen: %6d | energy: %f | upload_time: %s",
        //     device->object_id, device->temperature, device->humidity, device->pir,
        //     device->smoke, device->plug, device->kitchen, device->energy,
        //     device->upload_time);
        // }

    }
    // if(g_log != NULL)
    // {
	// 	fseek(g_log, 0, SEEK_SET);
    //     fprintf(g_log, "\r\n\r\n");
    //     fflush(g_log);
    // }

}

static char *get_request(char *mac)
{
    char *packet = NULL;
    char *body = "get no body";
    char *temp;

    packet = request_server(INFO, body);

    if(packet == NULL)
    {
        BAC_LOGE("request falut");
        return NULL;
    }

    int err_code = -1;
    json_parse_object(packet, "ErrCode", &err_code);
    if(err_code != 0)
    {
        char *message;
        json_parse_object(packet, "Message", &message);
        BAC_LOGE("request error: %s", message);   
        free(message);
        free(packet);
        return NULL;      
    }

    return packet;
}

static device_t *get_device_by_object_id(int object_id)
{
    for(int n = 0; n < g_device_list_count; n++)
    {
        if(g_device_list[n].object_id == object_id)
        return &g_device_list[n];
    }
    BAC_LOGE("no object id match");
    return NULL;
}

static int parse_device(char *str, device_t *device)
{   
    int len;
    char *temp;
    BAC_LOGV("parse_device start");


    json_parse_object(str, "BacnetID", &(device->object_id));
    BAC_LOGV("id: %d", device->object_id);
    if(device->object_id <= 0)
        BAC_LOGE("handle illegal object id: %d", device->object_id);

    json_parse_object(str, "SW_1", &device->d_sw_1);
    BAC_LOGV("SW_1: %d", device->d_sw_1);

    json_parse_object(str, "SW_1", &device->d_sw_1);
    BAC_LOGV("SW_1: %d", device->d_sw_1);

    json_parse_object(str, "SW_1", &device->d_sw_1);
    BAC_LOGV("SW_1: %d", device->d_sw_1);

    json_parse_object(str, "SW_1", &device->d_sw_1);
    BAC_LOGV("SW_1: %d", device->d_sw_1);
    
    // json_parse_object(str, "humidity", &device->humidity);
    // BAC_LOGV("humidity: %f", device->humidity);


    // json_parse_object(str, "socketConsumption", &device->energy);
    // BAC_LOGV("energy: %f", device->energy);

    // json_parse_object(str, "pir", &(device->pir));
    // BAC_LOGV("pir: %d", device->pir);

    // json_parse_object(str, "smoke", &(device->smoke));
    // BAC_LOGV("smoke: %d", device->smoke);

    // json_parse_object(str, "socketState", &(device->plug));
    // BAC_LOGV("plug: %d", device->plug);

    // json_parse_object(str, "kitchen", &(device->kitchen));
    // BAC_LOGV("kitchen: %d", device->kitchen);


    // json_parse_object(str, "update_time", &temp);
    // BAC_LOGV("upload_time: %s", temp);
    // memcpy(device->upload_time, temp, strlen(temp) + 1);
    // free(temp);


    BAC_LOGV("parse_device finish");
    return 0;
}

static void assign_device(device_t *old, device_t *new)
{  
    if(old->d_sw_1 != new->d_sw_1)
        new->status_change.b_sw_1 = true;


    memcpy(old, new, sizeof(device_t));
}

static int rebuild_device_list()
{
    char *packet;
    long temp;
    char **devcies_str;
    device_t device_real = {0};
    for(int i = 0; i< DEVICE_MAX_COUNT; i++)
    {
        memset(&g_device_list[i], 0, sizeof(device_t));
    }

    packet = get_request("");
    if(packet == NULL)
    {
        BAC_LOGE("rebuild_device_list get fail");
        return -1;
    }
    temp = json_parse_object(packet, "data", &devcies_str);
    free(packet);

    if(temp <= 0)
    {
        BAC_LOGE("rebuild_device_list parse fail or device num is 0");
        return -1;
    }

    g_device_list_count = 0;
    for(int n = 0; n < temp; n++)
    {
        parse_device(devcies_str[n], &device_real);
        free(devcies_str[n]);
        if(device_real.object_id >= 1)
        {
            assign_device(&g_device_list[g_device_list_count], &device_real);
            memset(&g_device_list[g_device_list_count].status_change, 1, sizeof(g_device_list[g_device_list_count].status_change));
            g_device_list_count++;
        }
    }
    free(devcies_str); 
    int object_count = 0;
    for(;;)
    {
        object_t  *object = &g_object_list[object_count];
        if(object->device_type == -1)
            break;
        object->object_count = g_device_list_count;
        object_count++;
    }

    BAC_LOGI("g_device_list_count : %d", g_device_list_count);
	
    return 0;
}

//if post fail or device struct has changed
static int refresh_devices()
{
    char *data = get_request("");
    long temp;
    char **devcies_str;
    device_t *device;
    bool restart_flag = false;
    int actual_count = 0;
    device_t device_real = {0};
    if(data == NULL)
    {
        BAC_LOGE("refresh_devices get fail");
        return -1;
    }

    BAC_LOGV("get_devices start");
    temp = json_parse_object(data, "data", &devcies_str);
    free(data);

    for(int n = 0; n < temp; n++)
    {
        parse_device(devcies_str[n], &device_real);
        
        free(devcies_str[n]);

        if(device_real.object_id > 0)
        {
			device = get_device_by_object_id(device_real.object_id);
            if(device == NULL)
            {
                BAC_LOGE("handle undefined object id: %d, device struct has changed", device_real.object_id);
                restart_flag = true;
            }

            if(restart_flag == false)
            {
                actual_count++;
                assign_device(device, &device_real);
            }
        }
    }   
    free(devcies_str);

    if(actual_count != g_device_list_count)
    {
        BAC_LOGE("device conunt has changed, %d---->%d", g_device_list_count, actual_count);
        restart_flag = true;
    }
        
    if(restart_flag == true)
    {
        BAC_LOGE("rebuild device list!");
        server_lock();
        cov_lock();
        while(rebuild_device_list() != 0)
        {
            BAC_LOGE("fail retry");
            sleep(5);
        }
        cov_unlock();
        server_unlock();
        object_init();
    }

    print_info("refresh after");
    return 0;

}

void server_lock()
{
    int res = sem_wait(&server_sem);
    if(res != 0)
        BAC_LOGE("sem_wait fault");
}

void server_unlock()
{
    int res = sem_post(&server_sem);
    if(res != 0)
        BAC_LOGE("sem_post fault");
}

void cov_lock()
{
    int res = sem_wait(&cov_sem);
    if(res != 0)
        BAC_LOGE("sem_wait fault");
}

void cov_unlock()
{
    int res = sem_post(&cov_sem);
    if(res != 0)
        BAC_LOGE("sem_post fault");
}

void *refresh_task(void *argc)
{
    while (1)
    {
        sleep(REFRESH_PERIOD);
        refresh_devices();
    }
    return NULL;
}
 
void get_data_init(char *path)
{
    char buf[100];
    if(path == NULL)
        strcpy(buf, "http://192.168.1.117:3428");
    else
        strcpy(buf, path);
    sem_init(&server_sem, 0, 1);
    sem_init(&cov_sem, 0, 1);
    BAC_LOGI("server path: %s", buf);
    request_init(buf);

    int peer = 3349;
    fresh_seq = msgget(peer, IPC_CREAT|0777);
    while((fresh_seq = msgget(888, IPC_CREAT|0777) <= 0))
    {
        BAC_LOGE("get fresh seq fail, retary %d", peer);
        peer++;
    }
    BAC_LOGI("create fresh seq success");

    while(rebuild_device_list() != 0)
    {
        BAC_LOGI("fail retry");
        sleep(10);
    }
    //open_log();
    pthread_t thread_refresh;
    pthread_create(&thread_refresh, NULL, refresh_task, NULL);
}





