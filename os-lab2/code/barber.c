#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 3          // 等待区座位数
#define CUSTOMERS 5  // 模拟顾客总数

// 全局变量与信号量定义
int available_seats = N;  // 当前可用等待座位数
sem_t mutex;              // 保护共享变量 available_seats
sem_t customers;          // 等待的顾客数量（理发师等待此信号）
sem_t barber_ready;       // 理发师就绪信号（顾客等待此信号）

// 理发师线程函数
void* barber(void* arg) {
    while (1) {
        sem_wait(&customers);   
        sem_wait(&mutex);      
        available_seats += 1;   
        sem_post(&barber_ready); 
        sem_post(&mutex);       

        printf("理发师：开始理发\n");
        sleep(1);
        printf("理发师：完成理发\n");
    }
    return NULL;
}

// 顾客线程函数
void* customer(void* arg) {
    sem_wait(&mutex);           
    if (available_seats > 0) {
        available_seats -= 1;   
        printf("顾客%ld：坐下等待（剩余座位：%d）\n", (long)arg, available_seats);
        sem_post(&customers);   
        sem_post(&mutex);       

        sem_wait(&barber_ready); 
        printf("顾客%ld：开始理发\n", (long)arg);
    } else {
        sem_post(&mutex);       
        printf("顾客%ld：无空位，离开\n", (long)arg);
    }
    return NULL;
}

int main() {
    pthread_t barber_tid;
    pthread_t customer_tids[CUSTOMERS];


    sem_init(&mutex, 0, 1);        
    sem_init(&customers, 0, 0);     
    sem_init(&barber_ready, 0, 0); 

    pthread_create(&barber_tid, NULL, barber, NULL);

    for (long i = 0; i < CUSTOMERS; i++) {
        sleep(1); 
        pthread_create(&customer_tids[i], NULL, customer, (void*)i);
    }

    for (int i = 0; i < CUSTOMERS; i++) {
        pthread_join(customer_tids[i], NULL);
    }

  
    pthread_cancel(barber_tid);
    pthread_join(barber_tid, NULL);
    sem_destroy(&mutex);
    sem_destroy(&customers);
    sem_destroy(&barber_ready);

    return 0;
}