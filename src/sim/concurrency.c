#include "concurrency.h"
#include "common.h"
#include "particle.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static worker_t workers[THREAD_COUNT];
static int worker_state[THREAD_COUNT];
static particle_t * current_frames[THREAD_COUNT];
static particle_t * updated_frames[THREAD_COUNT];
static int flag = CONTINUE;

void * update_acc(void * worker_arg) {
    worker_t * worker = (worker_t *) worker_arg;
    
    pthread_mutex_lock(&worker->lock);
    
    while(flag == CONTINUE) {
        while(worker_state[worker->id] == IDLE && flag == CONTINUE)
            pthread_cond_wait(&worker->cond_v, &worker->lock);

        for(int i = 0; i < worker->n; i++) {
            float x = worker->current_frame[i].x,
                  y = worker->current_frame[i].y,
                 xv = worker->current_frame[i].xv,
                 yv = worker->current_frame[i].yv,
                 xa = 0, ya = 0;

            for(int j = 0; j < THREAD_COUNT; j++) {
                for(int k = 0; k < workers[j].n; k++) {
                    float x2 = workers[j].current_frame[k].x,
                          y2 = workers[j].current_frame[k].y,
                          m = workers[j].current_frame[k].m;
                    if(x == x2 && y == y2)
                        continue;
                    float len2 = pow(x2 - x, 2) + pow(y2 - y, 2);
                    float gxm = GRAVITATIONAL_CONSTANT * m;
                    xa += (x2 - x) * gxm / len2,
                    ya += (y2 - y) * gxm / len2;
                }
            }
               
            worker->updated_frame[i].x = x + xv;
            worker->updated_frame[i].y = y + yv;

            worker->updated_frame[i].xv = xv + xa;
            worker->updated_frame[i].yv = yv + ya;

            worker->updated_frame[i].m = worker->current_frame[i].m;
        }

        memcpy(worker->current_frame, ((void*) worker->updated_frame), PARTICLE_SIZE * worker->n);

        worker_state[worker->id] = IDLE;
        pthread_cond_signal(&worker->cond_v);
    }

    pthread_mutex_unlock(&worker->lock);

    return NULL;
}

/*void * update_cm(void * worker_arg) {
    worker_t * worker = (worker_t *) worker_arg;
    
    pthread_mutex_lock(&worker->lock);
    
    while(flag == CONTINUE) {
        float x_cm = 0, y_cm = 0;
        int t_m = 0;

        while(worker_state[worker->id] == IDLE && flag == CONTINUE)
            pthread_cond_wait(&worker->cond_v, &worker->lock);

        // add up particle positions
        for(int i = 0; i < THREAD_COUNT; i++) {
            for(int j = 0; j < workers[i].n; j++) {
                x_cm += workers[i].current_frame[j].x;
                y_cm += workers[i].current_frame[j].y;
                t_m += workers[i].current_frame[j].m;
            }
        }

        // calculate center of mass
        x_cm /= t_m; y_cm /= t_m;
        //printf("CM: %d : %d\n", (int) x_cm, (int) y_cm);

        for(int i = 0; i < worker->n; i++) {
            float x = worker->current_frame[i].x,
                  y = worker->current_frame[i].y,
                 xv = worker->current_frame[i].xv,
                 yv = worker->current_frame[i].yv;
            
            float len2 = pow(x_cm - x, 2) + pow(y_cm - y, 2);
            float gxt_m = GRAVITATIONAL_CONSTANT * t_m;
            float delta_xv = (x_cm - x) * gxt_m / len2;
            float delta_yv = (y_cm - y) * gxt_m / len2;

            set_xv(worker->updated_frame, i, xv + delta_xv);
            set_yv(worker->updated_frame, i, yv + delta_yv);

            set_x(worker->updated_frame, i, x + xv + delta_xv);
            set_y(worker->updated_frame, i, y + yv + delta_yv);

            set_mass(worker->updated_frame, i, get_mass(worker->current_frame, i));
        }

        memcpy(worker->current_frame, ((void*) worker->updated_frame), PARTICLE_SIZE * worker->n);

        worker_state[worker->id] = IDLE;
        pthread_cond_signal(&worker->cond_v);
    }

    pthread_mutex_unlock(&worker->lock);

    return NULL;
}*/

int init_workers(particle_t * particles, int particle_count, void *(* update_method)(void *)) {
    int ppt = particle_count / THREAD_COUNT,
       rppt = particle_count % THREAD_COUNT + ppt;
    
    flag = CONTINUE;
    
    int carry = 0;
    for(int i = 0; i < THREAD_COUNT; i++) {
        pthread_mutex_init(&workers[i].lock, NULL);
        pthread_cond_init(&workers[i].cond_v, NULL);

        int p = i == THREAD_COUNT - 1 ? rppt : ppt;
        current_frames[i] = PARTICLE_CALLOC(p);
        memcpy(current_frames[i], ((void *) particles) + carry * PARTICLE_SIZE, p * PARTICLE_SIZE);
        carry += p;
        updated_frames[i] = PARTICLE_CALLOC(p);
        workers[i].n = p;
        workers[i].id = i;

        workers[i].current_frame = current_frames[i];
        workers[i].updated_frame = updated_frames[i];
    
        worker_state[i] = BUSY;
        if(pthread_create(&workers[i].thread, NULL, update_method, &workers[i]) != 0)
            return -1;
    }

    return 0;
}

void export_frame(char * filepath) {
    FILE * fp = fopen(filepath, "a");

    if(fp == NULL) {
        ERROR(ERR_FOPEN(filepath), );
    }

    for(int i = 0; i < THREAD_COUNT; i++) {
        pthread_mutex_lock(&workers[i].lock);

        while(worker_state[i] == BUSY)
            pthread_cond_wait(&workers[i].cond_v, &workers[i].lock);

        fwrite(current_frames[i], PARTICLE_SIZE, workers[i].n, fp);

        worker_state[i] = BUSY;
        pthread_cond_signal(&workers[i].cond_v);
        
        pthread_mutex_unlock(&workers[i].lock);
    }

    if(fclose(fp) != 0) {
        ERROR(ERR_FCLOSE(filepath), );
    }
}

void join_workers() {
    flag = STOP;
    for(int i = 0; i < THREAD_COUNT; i++) {
        pthread_mutex_lock(&workers[i].lock);
        pthread_cond_signal(&workers[i].cond_v);
        pthread_mutex_unlock(&workers[i].lock);

        pthread_join(workers[i].thread, NULL);
        pthread_mutex_destroy(&workers[i].lock);
        pthread_cond_destroy(&workers[i].cond_v);
    }

    for(int i = 0; i < THREAD_COUNT; i++) {
        free(workers[i].current_frame);
        free(workers[i].updated_frame);
    }
}