# ESP32

## esp_log.h

Enable using CONFIG_LOG_MASTER_LEVEL or 

```c
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

```

```c
esp_log_level_set("*", ESP_LOG_VERBOSE);

```
ESP_LOG"LEVEL"  ` ESP_LOGI(const char* tag, const char* other);`

## Timing control 

#include "freertos/timers.h"

` port_TICK_PERIOD_MS `

`esp_timer_get_time() micro sec`

```c
#include "freertos/timers.h"
#define MS(x) (x) / port_TICK_PERIOD_MS
vTaskDelay(MS(x));
```

port_TICK_PERIOD_MS
