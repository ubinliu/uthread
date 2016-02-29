/*
 
 * ============================================================
 
 *       Filename:  uthread.c
 *    Description:  
 *         Author:  liuyoubin@baidu.com
 *        Company:  Baidu.com, Inc
 *        Created:  2016-02-26 17:06:49
 
 * ============================================================
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "uthread.h"

manager_t* uthread_open(schedule_t schedule){
    manager_t *mgr = (manager_t*)malloc(sizeof(manager_t));
    assert(mgr != NULL);

    mgr->schedule = default_schedule;
    if (schedule != NULL){
        mgr->schedule = schedule;
    }

    mgr->run_id = -1;
    mgr->uthread_count = 0;
    mgr->uthread_cap = UTHREAD_SIZE;

    mgr->threads = (uthread_t**) malloc(mgr->uthread_cap * sizeof(uthread_t *));
    assert(mgr->threads != NULL);

    memset(mgr->threads, 0, sizeof(uthread_t *) * mgr->uthread_cap);

    return mgr;
}

int uthread_close(manager_t *mgr){
    int i = 0;
    for (; i < mgr->uthread_cap; ++i){
        if (mgr->threads[i] != NULL){
            uthread_t *uthread = mgr->threads[i];
            free(uthread->ctx.uc_stack.ss_sp);
            free(uthread);
        }
    }
    return 0;
}

uthread_id_t uthread_create(manager_t* mgr, uthread_func_t func, void *data){
    uthread_t *uthread = (uthread_t *)malloc(sizeof(uthread_t));
    assert(uthread != NULL);

    uthread->mgr = mgr;
    uthread->func = func;
    uthread->data = data;
    uthread->status = UTHREAD_READY;
    getcontext(&(uthread->ctx));
    uthread->stack = malloc(UTHREAD_STACK_SIZE * sizeof(char));
    uthread->stack_size = UTHREAD_STACK_SIZE;
    uthread->ctx.uc_stack.ss_sp = uthread->stack;
    assert(uthread->ctx.uc_stack.ss_sp != NULL);
    uthread->ctx.uc_stack.ss_size = UTHREAD_STACK_SIZE;
    uthread->ctx.uc_link = &(mgr->ctx);
    makecontext(&(uthread->ctx), (void (*)(void))default_schedule, 1, mgr);

    uthread_id_t id = -1;

    if (mgr->uthread_count == mgr->uthread_cap){
        mgr->threads = realloc(mgr->threads, mgr->uthread_cap * 2 * sizeof(uthread_t*));
        memset(mgr->threads+mgr->uthread_cap, 0, mgr->uthread_cap * sizeof(uthread_t*));
        assert(mgr->threads != NULL);
        
        mgr->uthread_cap = mgr->uthread_cap * 2;
    }
    int i = 0;
    for (; i < mgr->uthread_cap; ++i){
        if (mgr->threads[i] == NULL){
            id = i;
            mgr->threads[i] = uthread;
            mgr->uthread_count++;
            break;
        }
    }

    return id;
}

void default_schedule(manager_t *mgr){
    uthread_id_t id = mgr->run_id;
    uthread_t *uthread = mgr->threads[id];
    printf("default_schedule, uthread:%ld\n", id);
    uthread->func(mgr, uthread->data);

    free(uthread->stack);
    free(uthread);
    printf("free:%ld done\n", id);

    mgr->threads[id] = NULL;
    mgr->uthread_count--;
}

int uthread_resume(manager_t* mgr, uthread_id_t id){
    assert(id >= 0 && id < mgr->uthread_cap);
    uthread_t* uthread = mgr->threads[id];
    if (uthread == NULL){
        return -1;
    }

    mgr->run_id = id;
          
    uthread->status = UTHREAD_RUNNING;
    printf("uthread_resume,uthread:%ld\n", id);
    swapcontext(&(mgr->ctx), &(uthread->ctx));
    printf("uthread_resume,uthread:%ld done\n", id);
    return 0;
}

int uthread_suspend(manager_t* mgr){
    uthread_id_t id = mgr->run_id;
    if (id < 0 || id > mgr->uthread_cap){
        return -1;
    }

    uthread_t *uthread = mgr->threads[id];
    if (uthread == NULL){
        return -1;
    }

    mgr->run_id = -1;
    uthread->status = UTHREAD_SUSPEND;
    printf("uthread_suspend,uthread:%ld\n", id);
    swapcontext(&(uthread->ctx), &(mgr->ctx));
    printf("uthread_suspend,uthread:%ld done\n", id);
    return 0;
}

uthread_status_t uthread_status(manager_t* mgr, uthread_id_t id){
    if (id < 0 || id > mgr->uthread_cap){
        return UTHREAD_DEAD;
    }

    uthread_t *uthread = mgr->threads[id];
    if (uthread == NULL){
        return UTHREAD_DEAD;
    }

    return uthread->status;
}
