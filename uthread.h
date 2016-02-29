/*
 
 * ============================================================
 
 *       Filename:  uthread.h
 *    Description:  
 *         Author:  liuyoubin@ubinliu.com
 *        Company:  Ubinliu.com, Inc
 *        Created:  2016-02-26 17:06:16
 
 * ============================================================
 
 */

#ifndef __UTHREAD_H
#define __UTHREAD_H
#include <stdlib.h>
#include <ucontext.h>

#ifndef UTHREAD_SIZE
#define UTHREAD_SIZE 4
#endif

#ifndef UTHREAD_STACK_SIZE
#define UTHREAD_STACK_SIZE 1024*16
#endif

typedef enum uthread_status_s{
    UTHREAD_READY,
    UTHREAD_RUNNING,
    UTHREAD_SUSPEND,
    UTHREAD_DEAD
}uthread_status_t;


struct manager_s;
struct uthread_s;

typedef long uthread_id_t;

typedef void (*schedule_t)(struct manager_s *mgr);

typedef void (*uthread_func_t)(struct manager_s *mgr, void *data);

typedef struct uthread_s {
    ucontext_t ctx;
    struct manager_s *mgr;
    uthread_func_t func;
    void *data;
    uthread_status_t status;
}uthread_t;

typedef struct manager_s{
    ucontext_t ctx;    
    schedule_t schedule;
    uthread_t **threads;
    uthread_id_t run_id;
    long uthread_cap;
    long uthread_count; 
}manager_t;

manager_t * uthread_open(schedule_t schedule);
int uthread_close(manager_t *mgr);

uthread_id_t uthread_create(manager_t* mgr, uthread_func_t func, void *data);
int uthread_resume(manager_t* mgr, uthread_id_t id);
int uthread_suspend(manager_t* mgr);
uthread_status_t uthread_status(manager_t* mgr, uthread_id_t id);

#endif
