# ESP-IDF Notes

## Tutorials:
* [BLE](https://github.com/espressif/esp-idf/tree/master/examples/bluetooth/bluedroid/ble/gatt_server)
* [WiFi](https://github.com/espressif/esp-idf/tree/master/examples/wifi)
* [Peripherals](https://github.com/espressif/esp-idf/tree/master/examples/peripherals)

## WiFi

Initialisation code is below, order must be followed strictly and called once only.

```C
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

void app_main(void)
{
   nvs_flash_init();
   tcpip_adapter_init();
   esp_event_loop_init(event_handler, NULL);
   wifi_init_config_t conf = WIFI_INIT_CONFIG_DEFAULT();
   esp_wifi_init(&conf);
   esp_wifi_set_mode(WIFI_MODE_STA);
   esp_wifi_start();
   wifi_scan_config_t scanConf;
   scanConf.ssid = NULL;
   scanConf.bssid = NULL;
   scanConf.channel = 0;
   scanConf.show_hidden = 1;
   scanConf.scan_type = WIFI_SCAN_TYPE_ACTIVE;
   esp_wifi_scan_start(&scanConf, 0);
}
``` 

Next, an `event_handler` function is needed. When the WiFi driver calls the `event_handler` function it passes all needed details in a struct `system_event_t`. The event handler below prints out the SSIDs of all available networks.

```C
esp_err_t event_handler(void *ctx, system_event_t *event)
{
   if (event->event_id == SYSTEM_EVENT_SCAN_DONE)
   {
   uint16_t APCnt = event->event_info.scan_done.number;
   if(APCnt == 0) return 0;
   printf("%d AP available\n",APCnt);
   wifi_ap_record_t *list = malloc(sizeof(wifi_ap_record_t)*APCnt);
   printf("\r\n|------------------------------|\r\n");
   esp_wifi_scan_get_ap_records(&APCnt,list);
   printf("        SSID              RSSI\r\n");    
   for (uint8_t i = 0;i < APCnt;i++)
   {
      printf("\t%s\t\t%d\r\n", list[i].ssid, list[i].rssi);
   }
   printf("|------------------------------|\r\n");
   free(list);
   }
   return ESP_OK;
}
```

### Connecting to WiFi

```C
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "string.h"

static const char *TAG = "scan";

static const char *WIFISSID = "SSID";
static const char *WIFIPWD = "PWD123456";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {

       case SYSTEM_EVENT_STA_START:

           ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");

           ESP_ERROR_CHECK(esp_wifi_connect());

           break;

       case SYSTEM_EVENT_STA_GOT_IP:

           ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");

           ESP_LOGI(TAG, "Got IP: %s\n",

                    ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

           break;

       case SYSTEM_EVENT_STA_DISCONNECTED:

           ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");

           ESP_ERROR_CHECK(esp_wifi_connect());

           break;

       default:

           break;

    }

    return ESP_OK;
}


void app_main(void)
{
   nvs_flash_init();  

   tcpip_adapter_init();  

   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

   esp_event_loop_init(event_handler, NULL); 

   wifi_config_t wifi_config;

   strcpy((char *)wifi_config.sta.ssid,(char *)WIFISSID);
   strcpy((char *)wifi_config.sta.password,(char *)WIFIPWD);

   wifi_config.sta.scan_method = WIFI_FAST_SCAN;
   wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
   wifi_config.sta.threshold.rssi = -100;
   wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

   esp_wifi_set_mode(WIFI_MODE_STA);
 
   esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config); 

   esp_wifi_start();

}
```

### Sending data over WiFi

ESP32 uses the LwIP stack. One of LwIP's application layers is the Socket API which is compatible with standard BSD-sockets. Sockets are 'virtual' endpoints of any network communications between two hosts. First thing to do is to call the `socket()` API method.

```C
int socket(int domain, int type, int protocol);
```
* domain: selects protocol family (e.g. `AF_INET6` for IPv6 internet protocol)
* type: `SOCK_STREAM` for TCP or `SOCK_DGRAM` for UDP
* protocol: particular protocol to be used with the socket. Normally only a single protocol is supported by a specific socket. Typically left as 0.

```C
int socket_desc;
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
     
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
```

### Server Side

Sockets need to be bound to an address and a port. Address and port can be specified using a `struct sockaddr_in` variable type.

```C
struct sockaddr_in  server

//Prepare the sockaddr_in structure
server.sin_family = AF_INET;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port = htons( 8888 );
     
if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
{
    printf("bind failed");
}
printf("bind done");
```

The `sockaddr_in` struct from LwIP is:

```C
struct sockaddr_in {
  u_short         sin_family;     /* always AF_INET                          */
  u_short         sin_port;       /* the service port                        */
  struct in_addr  sin_addr;       /* the IP address                          */
  char            sin_zero[8];    /* unused (reserved for expansion          */
};
```

Finally, call the `listen()` API method to start listening for connections on the selected socket. First argument is the socket descriptor and the second one specifies the maximum number of queued connections.

```C
listen(socket_desc , 3);
```

`accept()` API method checks the queue for pending connections related to the socket with the specified file descriptor and returns a new file descriptor referring to that socket. `addr` is a pointer to a `sockaddr` structure. 
```C
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

You can read or write data from/to that connection using `read()` or `write()` functions.

```C
while(1)
{
    printf("Waiting for incoming connections...");
    int new_socket, c = sizeof(struct sockaddr_in);
    new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (new_socket<0)
    {
        printf("accept failed");
    }
    else
    {
        char *message = "Hello Client , I have received your connection\n";
        write(new_socket , message , strlen(message));
    }
}
```

### Client Side

If the socket is used to connect to a remote address, the `connect()` API method is then used instead of `bind()` with the same arguments with no need to call `listen()` nor `accept()`.

## Configuring ESP32 as an AP

This is usually useful for TCP connection via telnet/PuTTY. The `wifi_config_t` struct is used to configure the AP. Previously we used the `sta` part of `wifi_config_t`, now we use `ap` part of it.

```C
wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
```

Initalising a SoftAP ESP32.

```C
void wifi_init_softap()
{

    tcpip_adapter_init();
    esp_event_loop_init(event_handler, NULL);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}
```

The event handler has to handle `SYSTEM_EVENT_AP_START`, `SYSTEM_EVENT_AP_STOP`, `SYSTEM_EVENT_AP_STACONNECTED`, `SYSTEM_EVENT_AP_STADISCONNECTED` events.

```C
esp_err_t event_handler(void *ctx, system_event_t *event)
{

    tcpip_adapter_ip_info_t ip;

    switch(event->event_id) {

    case SYSTEM_EVENT_AP_STACONNECTED:

               ESP_LOGI(TAG,"station:"MACSTR"join",MAC2STR(event->event_info.sta_connected.mac));
                tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ip);    
                ESP_LOGI(TAG, "SoftAP IP=%s", inet_ntoa(ip.ip.addr));
}
        
        break;

    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR"leave",
                 MAC2STR(event->event_info.sta_disconnected.mac));
        break;
    default:
        break;
    }
    return ESP_OK;
}
```

Information about the event is stored in the `system_event_t` structure, example access is `event->event_info.sta_connected.mac`. `tcpip_adapter_get_ip_info()` method used to get network IP. `inet_ntoa()` is a macro used to convert numeric IP address into decimal dotted ASCII representation.

## HTTP Request

The AP SSID and Password can be set in the `menuconfig` or in the code itself as plaintext. The example below is from [Yaha Tawil, ATADIAT](https://atadiat.com/en/e-all-about-esp32-part-3/). It is

```C
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"



#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

#define WEB_SERVER "httpbin.org"
#define WEB_PORT 80
#define WEB_URL "http://httpbin.org"

static const char *TAG = "HTTP Request";

static const char *REQUEST = "GET " "/" " HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

static void http_get_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int socket_desc, r;
    char recv_buf[64];

    while(1) 
    {
        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                            false, true, portMAX_DELAY);
        ESP_LOGI(TAG, "Connected to AP");

        socket_desc = socket(hints.ai_family, hints.ai_socktype, 0);
        
        if(socket_desc < 0) 
        { 
            ESP_LOGE(TAG, "... Failed to allocate socket."); 
            freeaddrinfo(res); 
            vTaskDelay(1000 / portTICK_PERIOD_MS); 
            continue; 
        } 
        ESP_LOGI(TAG, "... allocated socket"); 
        int err = getaddrinfo(WEB_SERVER, "80", &hints, &res); 
        if(err != 0 || res == NULL) 
        { 
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res); 
            vTaskDelay(1000 / portTICK_PERIOD_MS); 
            continue; 
        } 
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        if(connect(socket_desc, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(socket_desc);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);

        if (write(socket_desc, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG, "... socket send failed");
            close(socket_desc);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success");

        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(socket_desc, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) { putchar(recv_buf[i]); } } while(r > 0);

        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
        close(socket_desc);
        for(int countdown = 10; countdown >= 0; countdown--) {
            ESP_LOGI(TAG, "%d... ", countdown);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Starting again!");
    }
}

void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();
    xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
}
```

HTTP can get more complicated. HTTPS requires authentication. HTTP may have redirection, streaming GET/POST/DELETE requests. The ESP HTTP client API has three main steps.

1. Define a variable `esp_http_client_config_t` type and fill it with required infromation including URL.

For normal GET/POST requests:

```C
esp_http_client_config_t config = {
    .url = "http://httpbin.org/get",
    .event_handler = _http_event_handler,
};
```

For authentication by username and password:

```C
esp_http_client_config_t config = {
        .url = "http://user:passwd@httpbin.org/basic-auth/user/passwd",
        .event_handler = _http_event_handler,
        .auth_type = HTTP_AUTH_TYPE_BASIC,
    };
```

2. Calling the `esp_http_client_init()` API method:

```C
esp_http_client_handle_t client = esp_http_client_init(&config);
```

3. Calling `esp_http_client_perform()`, which opens the cnonection, sends data, download data, close the conncetion if necessary. Other functions are called according to the required operation. Example, `esp_http_client_set_post_field(client, post_data, strlen(post_data))` for POST operation.

```C
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "freertos/event_groups.h"

#include "esp_http_client.h"


#define MAX_HTTP_RECV_BUFFER 512
static const char *TAG = "HTTP_CLIENT";

static EventGroupHandle_t wifi_event_group;


const int CONNECTED_BIT = BIT0;


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            /* This is a workaround as ESP32 WiFi libs don't currently
               auto-reassociate. */
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}


esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // Write out data
                // printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

static void http_rest()
{
    esp_http_client_config_t config = {
        .url = "http://random-quote-generator.herokuapp.com/api/quotes/random",
        .event_handler = _http_event_handler,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int content_length =  esp_http_client_get_content_length(client);
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                content_length);
        char *buffer = malloc(MAX_HTTP_RECV_BUFFER);
        
        int read_len = esp_http_client_read(client, buffer, content_length);
        buffer[read_len] = 0;
        ESP_LOGI(TAG, "HTTP Stream reader Status = %d, content_length = %d, Buffer=%.*s",
                esp_http_client_get_status_code(client),
                read_len,
                read_len,
                buffer);
        free(buffer);
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }


    esp_http_client_cleanup(client);
}

static void http_test_task(void *pvParameters)
{
        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);

    ESP_LOGI(TAG, "Connected to AP, begin http example");
    http_rest();
    ESP_LOGI(TAG, "Finish http example");
    vTaskDelete(NULL);
}

void app_wifi_initialise(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

}

void app_main()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    app_wifi_initialise();

    xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 5, NULL);
}
```

## TCP Server

ESP32 as a SoftAP, create TCP server to listen to TCP requests and answer it with a welcome message. Since ESP32 acts as a server, Socket creation will include the `bind()` and `listen()` methods. Get the network IP by calling `tcpip_adapter_get_ip_info()`.

Print the IP address of connected stations:

```C
wifi_sta_list_t wifi_sta_lis;
tcpip_adapter_sta_list_t tcpip_sta_list;
esp_wifi_ap_get_sta_list(&wifi_sta_lis);
tcpip_adapter_get_sta_list(&wifi_sta_lis, &tcpip_sta_list);

for (int i=0; i<wifi_sta_lis.num; i++)
{
    printf("StaInfo:"MACSTR","IPSTR"",MAC2STR(wifi_sta_lis.sta[i].mac),IP2STR(&tcpip_sta_list.sta[i].ip));
}
```

Full code:

```C
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include <sys/socket.h>
#include "lwip/err.h"
#include "lwip/sys.h"
#include <netdb.h>


#define EXAMPLE_ESP_WIFI_MODE_AP   CONFIG_ESP_WIFI_MODE_AP //TRUE:AP FALSE:STA
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_MAX_STA_CONN       CONFIG_MAX_STA_CONN

static EventGroupHandle_t wifi_event_group;

const int CONNECTED_BIT = BIT0;


static const char *TAG = "simple wifi";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
        wifi_sta_list_t wifi_sta_lis;
    tcpip_adapter_sta_list_t tcpip_sta_list;
    tcpip_adapter_ip_info_t ip;
    switch(event->event_id) {

    case SYSTEM_EVENT_AP_STACONNECTED:

                ESP_LOGI(TAG,"station:"MACSTR"join",MAC2STR(event->event_info.sta_connected.mac));
                
                 esp_wifi_ap_get_sta_list(&wifi_sta_lis);
                  tcpip_adapter_get_sta_list(&wifi_sta_lis, &tcpip_sta_list);

                  for (int i=0; i<wifi_sta_lis.num; i++){ printf("StaInfo:"MACSTR","IPSTR"",MAC2STR(wifi_sta_lis.sta[i].mac),IP2STR(&tcpip_sta_list.sta[i].ip)); tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ip); ESP_LOGI(TAG, "IP=%s", inet_ntoa(ip.ip.addr)); } xEventGroupSetBits(wifi_event_group, CONNECTED_BIT); break; case SYSTEM_EVENT_AP_STADISCONNECTED: ESP_LOGI(TAG, "station:"MACSTR"leave",MAC2STR(event->event_info.sta_disconnected.mac));
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void http_get_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *addr;
    int socket_desc,new_socket,c;
    struct sockaddr_in  server,client;

            xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                            false, true, portMAX_DELAY);
        ESP_LOGI(TAG, "SoftAP Started");

        socket_desc = socket(hints.ai_family, hints.ai_socktype, 0);
        
        if(socket_desc < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
        }
        ESP_LOGI(TAG, "... allocated socket");
        
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons( 8888 );
        if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
        {
            ESP_LOGI(TAG,"bind failed");
        }
        ESP_LOGI(TAG,"bind done");
        
        //Listen
        listen(socket_desc , 3);
        
    while(1) {

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (new_socket<0)
    {
        perror("accept failed");
        return ;
    }
    else
    {
    ESP_LOGI(TAG,"Connection accepted");
     
    char *message = "Hello Client , I have received your connection. But I have to go now, bye\n";
    write(new_socket , message , strlen(message));


    close(socket_desc);

    }
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void wifi_init_softap()
{
    wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}


void app_main()
{
    nvs_flash_init();  
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
    wifi_init_softap();
}
```

## Bluetooth Low Energy (BLE)

The typically BLE system architecture consists the following:

* Controller: combines LE PHY and Link Layer for radio connection, transaction etc.
* Host: combines L2CAP, SMP, GAP, ATT and GATT that manage access to real data, BLE device discovery
* Host-Controller Interface (HCI): link layer between Host and Controller, standard method of accessing Bluetooth capabilities
* Application: real code on main MCU

BLE Software Stack manages BLE layers, effectively abstracts the radio hardware into an API. Softdevice for nRFxxxxxx, Bluedroid for ESP32, Bluegiga for SiLabs.

* Advertising: broadcast presence to other devices, receiving is called scanner. Similar to APs in WiFi. Up to 31 bytes, formatted as follows:
    * 1st byte: length excluding this byte
    * 2nd byte: AD type, by Bluetooth Specification
    * AD data with variable length according to AD type

![BLE Advertising](https://atadiat.com/wp-content/uploads/2018/09/AD-example-GAP.png)

The AD packet also contains the PDU header, describing connectability type of the device.

![BLE Connectability](https://atadiat.com/wp-content/uploads/2018/09/AD-Packet-Type.png)

There is also a connection-less protocol like iBeacon from Apple or Eddystone from Google.

The basic building block of communications are characteristics, with is own UUID standardised by Bluetooth SIG. This defines the structure of how data is sent.

![Bluetooth V4.2](https://atadiat.com/wp-content/uploads/2018/09/ATT-BLE-entry.png)

It is also possible to use a custom UUID to send custom data, as long as it fits within the packet length of the Bluetooth specification.

### BLE GATT Server Example

This example defines two profiles and broadcast advertisements under `ESP_GATTS_DEMO`. Each profile contains one service with one characteristic. Debug using nRF Connect application for iOS or Android.

Initialising the Bluetooth stack:

```C
// Initialize NVS.
ret = nvs_flash_init();
if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
}

esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
esp_bt_controller_init(&bt_cfg);

// ESP32 can run in multiple modes, IDLE/BLE/CLASSIC_BT/BTDM (dual mode)
esp_bt_controller_enable(ESP_BT_MODE_BLE);

esp_bluedroid_init();

// Bluedroid is ESP32's BLE Stack
esp_bluedroid_enable();
```

Next, add callback functions to serve GATT and GAP events, and BLE profiles described in the beginning.

```C
esp_ble_gatts_register_callback(gatts_event_handler);
    
esp_ble_gap_register_callback(gap_event_handler);
    
esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    
esp_ble_gatts_app_register(PROFILE_B_APP_ID);
    
esp_ble_gatt_set_local_mtu(500);
```

GAP is used for advertising and broadcasting, GATT is used for storing and exchanging real data through a set of services consisting of characteristics that hold the concerning data. An event handler is used for each of these BLE layers. 

Then, the two profiles are registered for this example, and Maximum Transmission Unit (MTU) to 500. The MTU defines the size of the Attribute PDU (entire packet). ATT packets with a size larger than MTU cannot be transferred (have to be broken up and parsed accordingly).

Starting broadcasting/advertisement. When the profile is registered, `ESP_GATTS_REG_EVT` is triggered. The list of GATT events is:

```C
typedef enum 
{     
    ESP_GATTS_REG_EVT                 = 0,       /*!< When register application id, the event comes */
    ESP_GATTS_READ_EVT                = 1,       /*!< When gatt client request read operation, the event comes */
    ESP_GATTS_WRITE_EVT               = 2,       /*!< When gatt client request write operation, the event comes */
    ESP_GATTS_EXEC_WRITE_EVT          = 3,       /*!< When gatt client request execute write, the event comes */
    ESP_GATTS_MTU_EVT                 = 4,       /*!< When set mtu complete, the event comes */ 
    ESP_GATTS_CONF_EVT                = 5,       /*!< When receive confirm, the event comes */ 
    ESP_GATTS_UNREG_EVT               = 6,       /*!< When unregister application id, the event comes */ 
    ESP_GATTS_CREATE_EVT              = 7,       /*!< When create service complete, the event comes */ 
    ESP_GATTS_ADD_INCL_SRVC_EVT       = 8,       /*!< When add included service complete, the event comes */ 
    ESP_GATTS_ADD_CHAR_EVT            = 9,       /*!< When add characteristic complete, the event comes */
    ESP_GATTS_ADD_CHAR_DESCR_EVT      = 10,      /*!< When add descriptor complete, the event comes */ 
    ESP_GATTS_DELETE_EVT              = 11,      /*!< When delete service complete, the event comes */ 
    ESP_GATTS_START_EVT               = 12,      /*!< When start service complete, the event comes */ 
    ESP_GATTS_STOP_EVT                = 13,      /*!< When stop service complete, the event comes */
    ESP_GATTS_CONNECT_EVT             = 14,      /*!< When gatt client connect, the event comes */ 
    ESP_GATTS_DISCONNECT_EVT          = 15,      /*!< When gatt client disconnect, the event comes */ 
    ESP_GATTS_OPEN_EVT                = 16,      /*!< When connect to peer, the event comes */ 
    ESP_GATTS_CANCEL_OPEN_EVT         = 17,      /*!< When disconnect from peer, the event comes */
    ESP_GATTS_CLOSE_EVT               = 18,      /*!< When gatt server close, the event comes */ 
    ESP_GATTS_LISTEN_EVT              = 19,      /*!< When gatt listen to be connected the event comes */
    ESP_GATTS_CONGEST_EVT             = 20,      /*!< When congest happen, the event comes */
     /* following is extra event */ 
    ESP_GATTS_RESPONSE_EVT            = 21,      /*!< When gatt send response complete, the event comes */ 
    ESP_GATTS_CREAT_ATTR_TAB_EVT      = 22,      /*!< When gatt create table complete, the event comes */ 
    ESP_GATTS_SET_ATTR_VAL_EVT        = 23,      /*!< When gatt set attr value complete, the event comes */ 
    ESP_GATTS_SEND_SERVICE_CHANGE_EVT = 24,      /*!< When gatt send service change indication complete, the event comes */ 
} esp_gatts_cb_event_t;
```

The `gatts_event_handler`:

```C
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        } else {
            ESP_LOGI(GATTS_TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    /* If the gatts_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gatts_if == gl_profile_tab[idx].gatts_if) {
                if (gl_profile_tab[idx].gatts_cb) {
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}
```

Part of `gatts_profile_a_handler` will set advertisement data using `esp_ble_gap_config_adv_data(&adv_data)` and `esp_ble_gap_set_device_name(TEST_DEVICE_NAME)`. Then `gap_event_handler` will start the advertisement using `esp_ble_gap_start_advertising` when the event type is `ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT`. 

Create a service inside the profile using the `esp_ble_gatts_create_service` function. The function takes three parameters; one of which is the number of handlers/ATT to allocate for the service as the service may contain more than one characteristic. The minimum attributes for a service are: service declaration, characteristic declaration, characteristic value attribute handle, and the real value.

Defining the type of advertising packets:

```C
static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};
```

Now when something connects to our device, service of profile A, the connection parameters will be updated to new values making the timeout for the connection without any activity equal to 4000ms.

```C
case ESP_GATTS_CONNECT_EVT: {
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
        conn_params.latency = 0;
        conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;     // timeout = 400*10ms = 4000ms
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                 param->connect.conn_id,
                 param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                 param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = param->connect.conn_id;
        // start sent the update connection parameters to the peer device.
        esp_ble_gap_update_conn_params(&conn_params);
        break;
```

For the read request, it will send a response containing dummy four bytes as described in the code:

```C
case ESP_GATTS_READ_EVT: {
        ESP_LOGI(GATTS_TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 4;
        rsp.attr_value.value[0] = 0xde;
        rsp.attr_value.value[1] = 0xed;
        rsp.attr_value.value[2] = 0xbe;
        rsp.attr_value.value[3] = 0xef;
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
                                    ESP_GATT_OK, &rsp);
        break;
```

A detailed tutorial from Espressif for the BLE GATT Server is available [here](https://github.com/espressif/esp-idf/tree/master/examples/bluetooth/bluedroid/ble/gatt_server).

## Logging Messages

ESP-IDF has a specific serial monitor, using `make monitor`, that has options for ESP32 like automatically decoding addresses, pausing the application etc. The logging system automatically highlights different messages in different colours, and what level of alerts are displayed can be chosen in `make menuconfig`.

* No output
* Error (`ESP_LOGE`)
* Info (`ESP_LOGI`)
* Debug (`ESP_LOGD`)
* Verbose (`ESP_LOGV`)

Each source file has its own TAG name, so `esp_log_level_set()` has two arguments, the tag name and level.

`ESP_ERROR_CHECK` checks the return value from API functions with `esp_err_t` return type, and if value is not `ESP_OK`, it prints out an error indicating the line number, source file name and the function name.


```C
#define ESP_ERROR_CHECK(x) do {                                        \
       esp_err_t __err_rc = (x);                                       \
       if (__err_rc != ESP_OK) {                                       \
           _esp_error_check_failed(__err_rc, __FILE__, __LINE__,       \
                                   __ASSERT_FUNC, #x);                 \
       }                                                               \
    } while(0);
```

## References
[All about ESP32 by Yahya Tawil](https://atadiat.com/en/e-all-about-esp32-part-1/)