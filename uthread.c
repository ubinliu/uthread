/*
 
 * ============================================================
 
 *       Filename:  uthread.c
 *    Description:  
 *         Author:  liuyoubin@ubinliu.com
 *        Company:  Ubinliu.com, Inc
 *        Created:  2016-02-26 17:06:49
 
 * ============================================================
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
 #include <stdint.h>
#include "uthread.h"

static void default_schedule(uint32_t h32, uint32_t l32){
    uintptr_t ptr = (uintptr_t)h32 << 32 | (uintptr_t)l32;
    manager_t *mgr = (manager_t*)ptr;
    uthread_id_t id = mgr->run_id;
    uthread_t *uthread = mgr->threads[id];    

    uthread->func(mgr, uthread->data);

    uthread->status = UTHREAD_DEAD;
}

manager_t* uthread_open(schedule_t schedule){
    manager_t *mgr = (manager_t*)malloc(sizeof(manager_t));
    assert(mgr != NULL);

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
            free(mgr->threads[i]);
        }
    }
    return 0;
}

uthread_id_t uthread_create(manager_t* mgr, uthread_func_t func, void *data){
    uthread_t *uthread = (uthread_t *)malloc(sizeof(uthread_t));
    assert(uthread != NULL);
    memset(uthread, 0, sizeof(uthread_t));
    
    uthread->mgr = mgr;
    uthread->func = func;
    uthread->data = data;
    uthread->status = UTHREAD_READY;

    getcontext(&(uthread->ctx));
    uthread->ctx.uc_stack.ss_sp = malloc(UTHREAD_STACK_SIZE);
    assert(uthread->ctx.uc_stack.ss_sp != NULL);
    uthread->ctx.uc_stack.ss_size = UTHREAD_STACK_SIZE;
    uthread->ctx.uc_link = &mgr->ctx;
    
    uintptr_t ptr = (uintptr_t)mgr;
    makecontext(&(uthread->ctx), (void (*)(void))default_schedule, 2, (uint32_t) (ptr>>32), (uint32_t)ptr);

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

int uthread_resume(manager_t* mgr, uthread_id_t id){
    assert(id >= 0 && id < mgr->uthread_cap);
    uthread_t* uthread = mgr->threads[id];
    if (uthread == NULL){
        return -1;
    }

    mgr->run_id = id;
          
    uthread->status = UTHREAD_RUNNING;
    swapcontext(&mgr->ctx, &(uthread->ctx));

    if (uthread->status == UTHREAD_DEAD){
        free(uthread->ctx.uc_stack.ss_sp);
        free(uthread);        
        mgr->threads[id] = NULL;
        mgr->uthread_count--;
    }

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
    swapcontext(&(uthread->ctx), &mgr->ctx);
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
