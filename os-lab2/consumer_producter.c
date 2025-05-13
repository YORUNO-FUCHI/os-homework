#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5      
#define PRODUCER_NUM 2     
#define CONSUMER_NUM 2     
#define ITEMS_PER_THREAD 5 

int buffer[BUFFER_SIZE];   
int in = 0;               
int out = 0;               

sem_t empty;               
sem_t full;               
pthread_mutex_t mutex;     

// 生产者线程函数
void* producer(void* arg) {
    int thread_id = *(int*)arg;
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int item = thread_id * 100 + i; 
        
        sem_wait(&empty);// wait 检测》0 确保是缓冲区有空闲槽位 empty是信号量 >0表示有空 ==满了         
        pthread_mutex_lock(&mutex);    
        
        buffer[in] = item;            
        printf("生产者%d 生产: %d (位置: %d)\n", thread_id, item, in);
        in = (in + 1) % BUFFER_SIZE;  
        
        pthread_mutex_unlock(&mutex);
        sem_post(&full);              
    }
    return NULL;
}

// 消费者线程函数
void* consumer(void* arg) {
    int thread_id = *(int*)arg;
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        sem_wait(&full);              
        pthread_mutex_lock(&mutex);   
        
        int item = buffer[out];        
        printf("消费者%d 消费: %d (位置: %d)\n", thread_id, item, out);
        out = (out + 1) % BUFFER_SIZE; 
        
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);              
    }
    return NULL;
}

int main() {
    pthread_t producers[PRODUCER_NUM];
    pthread_t consumers[CONSUMER_NUM];
    int producer_ids[PRODUCER_NUM];
    int consumer_ids[CONSUMER_NUM];
    

    sem_init(&empty, 0, BUFFER_SIZE); //==0表示满了
    sem_init(&full, 0, 0);             
    pthread_mutex_init(&mutex, NULL);  


    for (int i = 0; i < PRODUCER_NUM; i++) {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    for (int i = 0; i < CONSUMER_NUM; i++) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }
    
    // 等待所有生产者线程结束
    for (int i = 0; i < PRODUCER_NUM; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < CONSUMER_NUM; i++) {
        pthread_join(consumers[i], NULL);
    }

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    
    printf("所有生产消费操作完成！\n");
    return 0;
}