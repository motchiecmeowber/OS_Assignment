/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

#include "common.h"
#include "syscall.h"
#include "stdio.h"
#include "libmem.h"

#include "string.h"
#include "queue.h"
#include "sched.h"

#include <pthread.h>
#include <stdlib.h>

// extern pthread_mutex_t queue_lock;

int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
{
    char proc_name[100];
    uint32_t data;

    //hardcode for demo only
    uint32_t memrg = regs->a1;
    
    /* TODO: Get name of the target proc */
    //proc_name = libread..
    int i = 0;
    data = 0;
    while(data != -1){
        libread(caller, memrg, i, &data);
        proc_name[i]= data;
        if(data == -1) proc_name[i]='\0';
        i++;
    }
    // while(i < 99) {
    //     if(libread(caller, memrg, i, &data) != 0 || data == (uint32_t)(-1)) {
    //         break;
    //     }
    //     proc_name[i] = (char)data;
    //     i++;
    // }
    // proc_name[i] = '\0';
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    /* TODO: Traverse proclist to terminate the proc
     *       stcmp to check the process match proc_name
     */
    //caller->running_list
    //caller->mlq_ready_queue
    struct queue_t *running_list = caller->running_list;
    struct queue_t *ready_queue = caller->mlq_ready_queue;

    if (ready_queue == NULL || running_list == NULL) {
        printf("Error: mlq_ready_queue or running_list is not initialized.\n");
        return 1;
    }
    
    /* TODO Maching and terminating 
     *       all processes with given
     *        name in var proc_name
     */
    struct pcb_t* proc;
    struct queue_t temp_queue = {{0}, 0};

    // pthread_mutex_lock(&queue_lock);

    for(int prio = 0; prio < MAX_PRIO; prio++) {
        while((proc = dequeue(&ready_queue[prio])) != NULL) {
            if(strcmp(proc->path, proc_name) == 0) {
                printf("Terminating process %d with name %s from mlq_ready_queue.\n", proc->pid, proc->path);
                free(proc->code->text);
                free(proc->code);
                free(proc->page_table);
                free(proc->mm);
                free(proc);
            } else {
                enqueue(&temp_queue, proc);
            }
        }

        while((proc = dequeue(&temp_queue)) != NULL) {
            enqueue(&ready_queue[prio], proc);
        }
    }

    while((proc = dequeue(running_list)) != NULL) {
        if(strcmp(proc->path, proc_name) == 0) {
            printf("Terminating process %d with name %s from running list.\n", proc->pid, proc->path);
            free(proc->code->text);
            free(proc->code);
            free(proc->page_table);
            free(proc->mm);
            free(proc);
        } else {
            enqueue(&temp_queue, proc);
        }
    }
    while((proc = dequeue(&temp_queue)) != NULL) {
        enqueue(running_list, proc);
    }

    // pthread_mutex_unlock(&queue_lock);
    return 0; 
}
