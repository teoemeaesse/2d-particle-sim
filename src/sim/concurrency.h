#ifndef __CONCURRENCY_H__
#define __CONCURRENCY_H__

#include <pthread.h>

typedef struct {
  pthread_t thread;
  pthread_mutex_t lock;
  pthread_cond_t cond_v;
  float * current_frame, * updated_frame;
  int id, n;
} worker_t;

/* Update particle positions based on individual interactions. Most accurate, but slow
    worker_t (void *): respective worker
*/
void * update_acc(void * worker_arg);

/* Update particle positions based on an overall center of mass. Less accurate, much faster
    worker_t (void *): respective worker
*/
void * update_cm(void * worker_arg);

/* Start up all worker threads
    return 0 for success, -1 for error
    particles (int): total number of particles to simulate
    update_method (void * func(void *)): algorithm to update particle positions
*/
int init_workers(float * particles, int particle_count, void *(*update_method)(void *));

/* Exports frame and signals workers to process the next frame
    filepath (char *): path to export file
*/
void export_frame(char * filepath);

/* Close down all worker threads.
*/
void join_workers();

#endif