#ifndef  SERVER_H
#define  SERVER_H
#include "defs.h"

#include "esp_http_server.h"

static volatile u64 num_of_visits = 0;

#define tag  "Server"

extern void server_init();

#ifdef SERVER_IMPLEMENTATION
 
esp_err_t _get_page_handler(httpd_req_t *req){
    static char page[1000]; 
    num_of_visits++;
    sprintf  (page, "<!DOCTYPE html><head><body><h1>Mali ako ovo ne radi najebo si!!!</h1></br></br><h2>Ovaj sajt je posijecen %llu</h2></body></head>"
                        , num_of_visits);
    

    httpd_resp_send(req, page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t _post_change_state(httpd_req_t *req){
   static char content[1000];
   i32 ret_time = httpd_req_recv(req, content, req->content_len);
   if(ret_time < 0){
    if(ret_time == HTTPD_SOCK_ERR_TIMEOUT){
        httpd_resp_send_408(req);
        //return ESP_FAIL
    }
    return ESP_FAIL;
   }
//   return _get_page_handler(req);

   return ESP_OK;
}



extern void server_init(){
    httpd_config_t server_config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server_handle = NULL;
    httpd_start(&server_handle, &server_config);
    if(server_handle == NULL){
        ERROR("We coude not init server!!!\n");
    }
    httpd_uri_t default_uri = {
        
        .uri = "/",
        .method = HTTP_GET,
        .handler = _get_page_handler,
        .user_ctx = NULL
    };
    httpd_uri_t post_uri = {
        .uri      = "/post",
        .method   = HTTP_POST,
        .handler  = _post_change_state,
        .user_ctx = NULL,
    };

    httpd_register_uri_handler(server_handle, &default_uri);
    httpd_register_uri_handler(server_handle, &post_uri);

}

#endif
#endif
