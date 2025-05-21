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

void extract_proc_name(const char *path, char *proc_name) {
    const char *last_path = strrchr(path, '/'); // Find last '/' character
    if (last_path) {
        strcpy(proc_name, last_path + 1); // Copy substring after last '/'
    } else {
        strcpy(proc_name, path); //use full path_name
    }
}

int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
{
    char proc_name[100];        // Store the target process name
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
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    /* TODO: Traverse proclist to terminate the proc
     *       stcmp to check the process match proc_name
     */
    //caller->running_list
    //caller->mlq_ready_queue
    char proc_temp[100];
    
    /* TODO Maching and terminating 
     *       all processes with given
     *        name in var proc_name
     */

#ifdef MLQ_SCHED
    struct queue_t *ready_queue = caller->mlq_ready_queue;

    if (ready_queue){
        for(int prio = 0; prio < MAX_PRIO; prio++) {
            int i = 0;
            for (int j = 0; j < ready_queue[prio].size; j++) {
                struct pcb_t *proc = ready_queue[prio].proc[j];
                if (proc) {
                    extract_proc_name(proc->path, proc_temp);  // Get just the process name
                    if (strcmp(proc_temp, proc_name) == 0) {
                        printf("Terminating process %d with name %s from mlq_ready_queue.\n", proc->pid, proc->path);
#ifdef MM_PAGING
                        libfree(proc, memrg);  // Use libfree for paging mode
#else    
                        // Free process resources
                        free(proc->code->text);
                        free(proc->code);
                        free(proc->page_table);
                        free(proc->mm);
                        free(proc);
#endif
                    proc = NULL;
                    } else {
                        ready_queue[prio].proc[i++] = proc; // Keep non-matching processes
                    }
                }
            }
            ready_queue[prio].size = i;  // Update size after removal
        }
    }
#endif

    struct queue_t *running_list = caller->running_list;

    if (running_list) {
        int i = 0;
        for(int j = 0; j < running_list->size; j++) {
            struct pcb_t *proc = running_list->proc[j];
            if (proc) {
                extract_proc_name(proc->path, proc_temp);
                if (strcmp(proc_temp, proc_name) == 0) {
                    printf("Terminating process %d with name %s from running list.\n", proc->pid, proc->path);
#ifdef MM_PAGING
                    libfree(proc, memrg);
#else
                    // Free process resources
                    free(proc->code->text);
                    free(proc->code);
                    free(proc->page_table);
                    free(proc->mm);
                    free(proc);
#endif
                proc = NULL;
                } else {
                    running_list->proc[i++] = proc;
                }
            }
        }
        running_list->size = i;
    }

    return 0; 
}