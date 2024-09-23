//#define ETHERNET_IMPLEMENTATION


//#define ETHERNET_IMPLEMENTATNION
#include "defs.h"
//#include "ethernet.h"
//#define SERVER_IMPLEMENTATION
//#include "server.h"
#define CLIENT_IMPLEMNTATION
#include "client.h"
//#define WIFI_IMPLEMENTATION
#define WIFI_IMPLEMENTATION

#include "wifi.h"

void app_main(){
    //ethernet_init();
    wifi_init();
    //BLOCK_TASK(ETHERNET, 100);
    BLOCK_TASK(WIFI, 100);
    ESP_LOGI("main", "This is logeded last\n\n");
    //server_init();
    rest_get("http://8.8.8.8");
    rest_post("http://8.8.8.8");
}
