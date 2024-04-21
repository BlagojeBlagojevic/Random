 CONFIG_FREERTOS_UNICORE - setup only one core

 GENERALY 2 CORES WITCH ACCESS ALL MEMORY INDEPENDETLY

2 INTERUPT PERIPHERAL

xTaskCreate()
xTaskCreateStatic()

xTaskCreatePinToCore()   //Use this two
xTaskCreateStaticPinToCore()

task state -> Running, Ready, Blocked, or Suspended.
vTaskeDelete()
Scheduler -> timer for control of time 
Incrementing the scheduler's tick count

Unblocking any blocked tasks that have timed out

Checking if time slicing is required, i.e., triggering a context switch

Executing the application tick hook  
Idle task -> controls task priority alocation and similar

vTaskSuspend() -> for only one core
vTaskResume()  -> same
taskDISABLE_INTERRUPTS
taskENABLE_INTERRUPTS

taskENTER_CRITICAL() enters a critical section by disabling interrupts

taskEXIT_CRITICAL() exits a critical section by reenabling interrupts

taskENTER_CRITICAL_FROM_ISR() enters a critical section from an ISR by disabling interrupt nesting

taskEXIT_CRITICAL_FROM_ISR() exits a critical section from an ISR by reenabling interrupt nesting

However, in an SMP system, merely disabling interrupts does not constitute a critical section as the presence of other cores means that a shared resource can still be concurrently accessed. Therefore, critical sections in IDF FreeRTOS are implemented using spinlocks. To accommodate the spinlocks, 
the IDF FreeRTOS critical section APIs contain an additional spinlock parameter as shown below:
Spinlocks are of portMUX_TYPE (not to be confused to FreeRTOS mutexes)

taskENTER_CRITICAL(&spinlock) enters a critical from a task context

taskEXIT_CRITICAL(&spinlock) exits a critical section from a task context

taskENTER_CRITICAL_ISR(&spinlock) enters a critical section from an interrupt context

taskEXIT_CRITICAL_ISR(&spinlock) exits a critical section from an interrupt context

