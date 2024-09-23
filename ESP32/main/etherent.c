//#define ETHERNET_IMPLEMENTATION
#define ETHERNET_IMPLEMENTATNION
#include "defs.h"
#include "ethernet.h"
#define SERVER_IMPLEMENTATION
#include "server.h"

void app_main(){
    ethernet_init();
    BLOCK_TASK(ETHERNET, 100);
    ESP_LOGI("main", "This is logeded last\n\n");
    server_init();

}