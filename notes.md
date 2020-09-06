# FreeRTOS Notes

## Task structure

void aTaskFunction (void *pvParams)
{
    while (1)
    {
        // Task will normallbe implemented as an infinite loop
    }
    // If the task breaks out of the loop, then the task must be deleted before reaching the end of this function. The NULL parameter passed to the vTaskDelete() function indicates that the task to be deleted is the current task.
    vTaskDelete(NULL);
}

## Task creation

vTaskCreate( pvTaskCode, pcName, usStackDepth, pvParams, uxPriority, pxCreatedTask )

- pvTaskCode: pointer to the function
- pcName: name to the task, for debugging
- usStackDepth: length of memory stack allocated in words, depends on hardware. If stack is 32 bits / 4 bytes, and usStackDepth is 10, the 400 bytes will be allocated to the task
- pvParameters: pointer to arguments given to the task. Good practice to create a dedicated structure, instantiate and fill it, then give its pointer to the task
- uxPriority: priority given to the task, between 0 and MAX_PRIORITIES-1, bigger is higher priority
- pxCreatedTask: handler for the task to manipulate in the future, if not required leave as NULL

## Task Deletion

vTaskDelete( pxTask )

When a task is deleted, it is the responsibility of the idle task to free all allocated memory to this task by the kernel. All memory dynamically allocated has to be manually freed.

## Scheduling

FreeRTOS decides which task gets to run at every tick. The priority number given to a task can be changed manually with vTaskPriorityGet() and vTaskPrioritySet(). Higher numbers get higher priority. Equal priority tasks use a Round Robin implementation, which each quantum is the time between each clock tick. This value is available in TICK_RATE_HZ constant, in FreeRTOSConfig.h
