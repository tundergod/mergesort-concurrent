#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "threadpool.h"
#include "list.h"

#define CLOCK_ID CLOCK_MONOTONIC_RAW
#define ONE_SEC 1000000000.0

pthread_mutex_t mutex;
static tpool_t *pool = NULL;
static int thread_count = 0, data_count = 0, max_cut = -1;

list_t *merge_together(list_t *a, list_t *b)
{
    list_t *compare = malloc(sizeof(list_t));
    list_t *cur = compare;
    while(a || b) {
        if(a->data >= b->data) {
            cur->next = b;
            cur = cur->next;
        } else {
            cur->next = a;
            cur = cur->next;
        }
        cur = cur->next;
    }
    cur->next = (a)?a:b;
    return compare->next;
}

list_t *merge_one2two(list_t *list)
{
    if (!list || !list->next)	return list;
    list_t *mid = list_mid(list);
    list_t *second = mid->next;
    mid->next = NULL;
    return merge_together(merge_one2two(list),merge_one2two(second));
}

void merge(void *data)
{
    list_t *tmp_list;
    list_t *list = (list_t *) data;
    if (list_size(list) < data_count) {
        pthread_mutex_lock(&mutex);
        list_t *second = tmp_list;
        if (!second) {
            tmp_list = list;
            pthread_mutex_unlock(&mutex);
        } else {
            tmp_list = NULL;
            pthread_mutex_unlock(&mutex);
            task_t *_task = (task_t *) malloc(sizeof(task_t));
            _task->func = merge;
            _task->arg = merge_together(list, second);
            tqueue_push(pool->queue, _task);
        }
    } else {
        head_list = list;
        task_t *_task = (task_t *) malloc(sizeof(task_t));
        _task->func = NULL;
        tqueue_push(pool->queue, _task);
        list_print(list);
    }
}

void cut_list(void *data)
{
    list_t *list = (list_t *) data;
    int cut_count;
    if (list && list->next && cut_count < max_cut) {
        cut_count++;

        //cut list
        list_t *mid = list_mid(list);
        list_t *second = mid->next;
        mid->next = NULL;

        // create new task: left
        task_t *_task = (task_t *) malloc(sizeof(task_t));
        _task->func = cut_func;
        _task->arg = list;
        tqueue_push(pool->queue, _task);

        //create new task: right
        _task = (task_t *) malloc(sizeof(task_t));
        _task->func = cut_func;
        _task->arg = second;
        tqueue_push(pool->queue, _task);
    } else {
        merge(merge_sort(list));
    }
}

static void *task_run(void *data __attribute__ ((__unused__)))
{
    while (1) {
        task_t *_task = tqueue_pop(pool->queue);
        if (_task)
            _task->func(_task->arg);
    }
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    struct timespec start = {0, 0};
    struct timespec end = {0, 0};

    thread_num = atoi(argv[1]);
    data_num = atoi(argv[2]);

    if(data_num < 2 || thread_num < 1)	return 0;

    //max_cut(always = 2^n)
    while(thread_num > data_num) {
        thread_num /= 2;
    }
    max_cut = thread_num - 1;

    /* Read data */
    list_t *list_head = malloc(sizeof(list_t));
    list_head->next = NULL;
    list_t *e = list_head;

    for(int i = 0; i < data_num; i++) {
        long int data;
        scanf("%ld", &data);
        e = list_add(e, data);
    }

    clock_gettime(CLOCK_ID, &start);
    /* initialize tasks inside thread pool */
    pthread_mutex_init(&mutex, NULL);
    pool = (tpool_t *) malloc(sizeof(tpool_t));
    pool = tpool_init(pool, thread_num, task_run);

    /* launch the first task */
    task_t *_task = (task_t *) malloc(sizeof(task_t));
    _task->func = cut_list;
    _task->arg = e->next;
    tqueue_push(pool->queue, _task);

    /* release thread pool */
    tpool_free(pool);

    clock_gettime(CLOCK_ID,&end);
    printf("%lf",(double) (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec)/ONE_SEC);

    return 0;
}
