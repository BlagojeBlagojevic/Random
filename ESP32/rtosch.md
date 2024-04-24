FreeRTOS Cheat Sheet

1. Task Creation:
   - Create a task using `xTaskCreate()` function.
   - Specify the task function, task name, stack size, priority, and handle.
   - Example: `xTaskCreate(taskFunction, "Task Name", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);`

2. Task Synchronization:
   - Use `xSemaphoreCreateBinary()` to create a binary semaphore.
   - Use `xSemaphoreTake()` to acquire the semaphore.
   - Use `xSemaphoreGive()` to release the semaphore.

3 . Task Delay:
   - Use `vTaskDelay()` to delay a task for a specific time period.
   - Example: `vTaskDelay(pdMS_TO_TICKS(1000));` (delays the task for 1 second)

4. Queue Creation:
   - Create a queue using `xQueueCreate()` function.
   - Specify the queue length and item size.
   - Example: `xQueueCreate(10, sizeof(int));`

5. Queue Send/Receive:
   - Use `xQueueSend()` to send an item to the queue.
   - Use `xQueueReceive()` to receive an item from the queue.
   - Example: `xQueueSend(queueHandle, &data, portMAX_DELAY);`

6. Interrupt Handling:
   - Use `xSemaphoreCreateBinary()` to create a binary semaphore.
   - Use `xSemaphoreGiveFromISR()` to give the semaphore from an ISR.
   - Use `xSemaphoreTakeFromISR()` to take the semaphore from an ISR.

7. Memory Management:
   - Use `pvPortMalloc()` to allocate dynamic memory.
   - Use `vPortFree()` to free allocated memory.
   - Example: `int* ptr = (int*)pvPortMalloc(sizeof(int));`

8. Software Timers:
   - Create a software timer using `xTimerCreate()` function.
   - Specify the timer period, auto-reload, and timer ID.
   - Example: `xTimerCreate("Timer", pdMS_TO_TICKS(1000), pdTRUE, NULL, timerCallback);`

9. Tick Conversion:
   - Use `pdMS_TO_TICKS()` to convert milliseconds to ticks.
   - Use `pdMS_TO_TICKS()` to convert ticks to milliseconds.
   - Example: `vTaskDelay(pdMS_TO_TICKS(1000));`

10. Error Handling:
    - Use `configASSERT()` to handle critical errors.
    - Example: `configASSERT(x != NULL);` (asserts if x is NULL)

Remember to include the necessary FreeRTOS header files and configure the FreeRTOS kernel according to your application requirements.
