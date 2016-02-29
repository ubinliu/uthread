/*
 
 * ============================================================
 
 *       Filename:  base.cpp
 *    Description:  
 *         Author:  liuyoubin@ubinliu.com
 *        Company:  Ubinliu.com, Inc
 *        Created:  2015-05-25 10:32:35
 
 * ============================================================
 
 */

#include <stdio.h>
#include <unistd.h>
#include "uthread.h"

typedef struct user_data_s{
    int count;
}user_data_t;

void user_func(manager_t *mgr, void *data){
    user_data_t *ud = (user_data_t*) data;
    int i = 0;
    int count = ud->count;
    for (; i < 3; ++i){
        printf("uthread:%ld, %d suspend\n", mgr->run_id, count++);
        uthread_suspend(mgr);
        printf("uthread:%ld, suspend done\n", mgr->run_id);
    }     
}

void test_uthread(manager_t *mgr){
    user_data_t *ud = (user_data_t*)malloc(sizeof(user_data_t));
    ud->count = 1;
    int total = 100;
    uthread_id_t ids[total];
    int i = 0;
    for (; i < total; ++i){
        ids[i] = uthread_create(mgr, user_func, (void*)ud);
        printf("uthread_create:%ld\n", ids[i]);
    }


    while (uthread_status(mgr, ids[0]) != UTHREAD_DEAD){
        i = 0;
        for (; i < total; ++i){
            printf("uthread_resume:%d begin\n", i);
            uthread_resume(mgr, ids[i]);
            printf("uthread_resume:%d end\n", i);
        }
    }
}

int main(int argc, char *argv[]){

    (void)argc;
    (void)argv;

    manager_t *mgr = uthread_open(NULL);
    printf("test_uthread begin\n");
    test_uthread(mgr);
    printf("test_uthread end\n");

    sleep(10000);
    uthread_close(mgr);

    return 0;

}
