# FreeRTOS Notes

## Task Structure

```C
void aTaskFunction (void *pvParams)
{
    while (1)
    {
        // Task will normallbe implemented as an infinite loop
    }
    // If the task breaks out of the loop, then the task must be deleted before reaching the end of this function. The NULL parameter passed to the vTaskDelete() function indicates that the task to be deleted is the current task.
    vTaskDelete(NULL);
}
```

## Task Creation

`vTaskCreate( pvTaskCode, pcName, usStackDepth, pvParams, uxPriority, pxCreatedTask )`

- pvTaskCode: pointer to the function
- pcName: name to the task, for debugging
- usStackDepth: length of memory stack allocated in words, depends on hardware. If stack is 32 bits / 4 bytes, and usStackDepth is 10, the 400 bytes will be allocated to the task
- pvParameters: pointer to arguments given to the task. Good practice to create a dedicated structure, instantiate and fill it, then give its pointer to the task
- uxPriority: priority given to the task, between 0 and `MAX_PRIORITIES-1`, bigger is higher priority
- pxCreatedTask: handler for the task to manipulate in the future, if not required leave as NULL

## Task Deletion

`vTaskDelete( pxTask )`

When a task is deleted, it is the responsibility of the idle task to free all allocated memory to this task by the kernel. All memory dynamically allocated has to be manually freed.

## Scheduling

FreeRTOS decides which task gets to run at every tick. The priority number given to a task can be changed manually with `vTaskPriorityGet()` and `vTaskPrioritySet()`. Higher numbers get higher priority. Equal priority tasks use a Round Robin implementation, which each quantum is the time between each clock tick. This value is available in `TICK_RATE_HZ` constant, in FreeRTOSConfig.h

There is no starvation mechanism in FreeRTOS, the programmer has to make sure that there is no higher priority task taking all the running time for itself.

## Queue Management

Queues are the underlying mechanism behind all task communication and synchronisation in FreeRTOS. They store a finite length of fixed-size data. FIFO buffer that can be used by any task. When a single task reads in a queue, it is moved to "Blocked" and moved back to "Ready" as soon as data has been written in the queue by another task or interrupt.

`xQueueReceive( xQueue, *pvBuffer, xTicksToWait )`

- `xQueue`: identifier of the queue to read
- `pvBuffer`: pointer to the buffer where the read value will be copied to. Memory must be allocated and large enough to handle the element read from the queue
- `xTicksToWait`: defines the maximum time to wait. 0 prevents the task from waiting, `MAX_DELAY`, task waits indefinitely until a value is written to the queue
- returns `pdPASS` if succesfully return a value, else returns `errQUEUE_EMPTY`

To read a value without removing it from the queue, use:

`xQueuePeek( xQueue, *pvBuffer, xTicksToWait )`

Writing to a queue. Task is blocked until another task reads the queue and frees some space.

`xQueueSend( xQueue, *pvItemToQueue, xTicksToWait )`

- `xQueue`: identifier of the queue to write on
- `pvItemToQueue`: pointer to an element which is wanted to be copied (by value) to the queue
- `xTicksToWait`: number of ticks to wait before the task gives up to write on the queue. 0 = won't wait, `MAX_DELAY` = wait indefinitely until space is free
- returns `pdPASS` if succesful, returns `errQUEUE_FULL` if maximum time elapsed before task could write on queue

`xQueueSendToBack( xQueue, pvItemToQueue, xTicksToWait ) [FIFO]`

`xQueueSendToFront( xQueue, pvItemToQueue, xTicksToWait ) [LIFO]`

### Creating a queue

`xQueueCreate( uxQueueLength, uxItemSize )`

- `uxQueueLength`: number of elements this queue will handle at any given time
- `uxItemSize`: size in bytes of any element stored in the queue
- returns `NULL` if failed to create due to lack of memory, handle if successful

## Binary Semaphores

Simple way to synchronise tasks, a queue with only one element.

`vSemaphoreCreateBinary( xSemaphore )`
- `xSemaphore`: semaphore to be created, treturns a handle

`xSemaphoreTake( xSemaphore, xTicksToWait )`
- `xSeamphore`: semaphore to take
- `xTicksToWait`: time in clock ticks for the task to wait before it gives up, if `MAX_DELAY` and `INCLUDE_vTaskSuspend = 1`, task waits indefinitely
- `pdPASS` if successful, `pdFAIL` if failed

`xSemaphoreGive( xSemaphore )`

## Mutexes

Designed to prevent mutual exclusion, similar to a binary semaphore except that the task that takes the semaphore must be the one to give it back. Used to block/protect a resource (e.g. writing to the same memory)

Priority inheritance is the main difference. When several tasks ask for a mutex, mutex holder's priority is set to the highest waiting task priority.

Counting semaphore is a semaphore that can be taken several (limited) times before it becomes unavailable, like a queue with a few elements.

`xSemaphoreCreateCounting( uxMaxCount, uxInitialCount )`
- `uxMaxCount`: max capacity of semaphore
- `uxInitialCount`: initial availability after creation
- return `NULL` if failed, handler if successful

Give and take are the same functions as normal semaphores.

## Handling Interrupts

An interrupt is a mechanism fully implemented and handled by hardware. Interrupt priorities are not related to task priorities, and will always preempt them. A function defined as an interrupt handler cannot freely use the FreeRTOS API, must use specialised functions (e.g xSemaphoreGiveFromISR() ), which some differences to allow the interrupt to be preemped by a higher priority interrupt.

Interrupt management can be configured in FreeRTOSConfig.h
- `configKERNEL_INTERRUPT_PRIORITY` sets the interrupt priority level for the tick interrupt
- `configMAX_SYSCALL_INTERRUPT_PRIORITY` defines highest level interrupt available to interrupts that use interrupt-safe FreeRTOS API functions. If not defined, any interrupt handler function must execute at `configKERNEL_INTERRUPT_PRIORITY`

Any interrupt whose priority level is greater than `configMAX_SYSCALL_INTERRUPT_PRIORITY` will never be preempted by the kernel, but are forbidden to use FreeRTOS API functions.

Interrupt handlers are run by the microcontroller and not FreeRTOS, potential problems with memory access.  Make interrupt handlers' execution as short as possible. A solution is to make the ISR "give" a semaphore and unblock a Handler task to handle the ISR and execute code. This reduces ISR execution time.

FreeRTOS provides two mechanisms to protect certain portions of code from changes. Protect any context change from a scheduler operation or interrupt event, or just prevent scheduler from preempting the task. This is important for instructions that are not atomic, require several hardware instructions to execute fully.

### Suspend Interrupts

`taskENTER_CRITICAL()` ensures this section of code cannot be interrupted, `taskEXIT_CRITICAL()` ends the critical section. Suspend interrupts should be as short as possible since it suspends the whole system, any other portion of code cannot be executed.

Such a critical section is not protected from interrupts which priority is greater than `configMAX_SYSCALL_INTERRUPT_PRIORITY` defined in FreeRTOSConfig.h.

### Stop the Scheduler

A less drastic method prevents any scheduler tasks from preempting the section, but allows hardware interrupts. This is achieved by preventing any task to leave "Ready" state to "Running". FreeRTOS API functions must not be called when the scheduler is stopped.

`vTaskSuspendAll()` stops the scheduler, `xTaskResumeAll()` resumes it.

## Memory Management

In embedded systems, `malloc()` and `free()` can cause problems: preemption while allocating some memory, memory allocation and free can be nondeterministic operations. Once compiled, they coneusme a lot of space or suffer from memory fragmentation. FreeRTOS provides three different ways to allocate memory.

### Prototypes

All implementations respect the same allocation/free memory function prototypes.

```C
void *pvPortMalloc( size_t xWantedSize )
void pvPortFree( void *pv )
```

- `xWantedSize`: size in bytes to be allocated
- `pv`: pointer to the memory allocated / to be freed

A call to `pvPortMalloc` requires 20 bytes to be free and returns back to its reference. 

### Memory allocated once for all

It is possible to allocate all tasks, queues and semaphores, then start the scheduler and run the entire application without reallocating any of the structures or new ones. Only `pvPortMalloc()` is used. FreeRTOS adds a Task Control Block (TCB) then all the memory it needs, for each task.

### Constant sized and numbered memory

An application can allocate and deallocate memory. If the number of variables and its size remains constant, this mechanism can be set up. The algorithm always chooses the smallest free space where it can fit the requested size. This can generate a lot of fragmentation in memory if allocations are not regular, but fits if allocations remain constant in size and number.

### Free memory allocation and deallocation

This allows any manipulation but suffers the same drawbacks as `malloc()` and `free()`: large compiled code or nondeterministic execution. This implementation wraps the two functions, but makes them thread safe by suspenidng the scheduler while allocating or deallocating.

```C
void *pvPortMalloc( size_t xWantedSize ) {
    void *pvReturn;
    vTaskSuspendAll();
    {
        pvReturn = malloc( xWantedSize );
    }
    xTaskResumeAll();
    return pvReturn;
}

void *vPortFree( void *pv ) {
    if ( pv != NULL) {
        vTaskSuspendAll();
        {
            free( pv );
        }
        xTaskResumeAll();
    }
}
```

## Conclusion

FreeRTOS is designed for embedded systems with its small memory footprint. It has no support for threading, minimalist memory management, no driver to handle resources on USB or PCI, no IP stack communication support. This information is taken from [**Study of an operating system: FreeRTOS**](http://wiki.csie.ncku.edu.tw/embedded/FreeRTOS_Melot.pdf) by **Nicolas Melot**.
