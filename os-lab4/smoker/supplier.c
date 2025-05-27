#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ipc.h"
#include "smoker.h"

int main() {
    // 获取共享内存
    key_t shm_key = 1000;
    int shm_size = sizeof(struct shared_data);
    struct shared_data *shared = (struct shared_data*)set_shm(shm_key, shm_size, IPC_CREAT | 0666);

    // 初始化共享数据
    shared->current_combination = 0;
    shared->supplied[0] = TOBACCO;
    shared->supplied[1] = PAPER;

    // 获取信号量
    int mutex = set_sem(2000, 1, IPC_CREAT | 0666);          // 互斥锁
    int smoker_sem[3] = {
        set_sem(3000, 0, IPC_CREAT | 0666),  // 抽烟者0信号量
        set_sem(3001, 0, IPC_CREAT | 0666),  // 抽烟者1信号量
        set_sem(3002, 0, IPC_CREAT | 0666)   // 抽烟者2信号量
    };
    int done_sem[3] = {
        set_sem(4000, 0, IPC_CREAT | 0666),  // 组合0完成信号量
        set_sem(4001, 0, IPC_CREAT | 0666),  // 组合1完成信号量
        set_sem(4002, 0, IPC_CREAT | 0666)   // 组合2完成信号量
    };

    while (1) {
        down(mutex);  // 进入临界区

        // 更新供应组合（循环选择0-2）
        int current = shared->current_combination;
        shared->current_combination = (current + 1) % 3;

        // 设置供应的材料
        switch (current) {
            case 0:
                shared->supplied[0] = TOBACCO;
                shared->supplied[1] = PAPER;
                break;
            case 1:
                shared->supplied[0] = PAPER;
                shared->supplied[1] = GLUE;
                break;
            case 2:
                shared->supplied[0] = GLUE;
                shared->supplied[1] = TOBACCO;
                break;
        }

        printf("[Supplier] Supply: %d and %d\n", shared->supplied[0], shared->supplied[1]);
        up(smoker_sem[current]);  // 唤醒对应抽烟者
        up(mutex);                // 离开临界区

        down(done_sem[current]);  // 等待抽烟者完成
    }

    return 0;
}
//smoker 0: 有glue 2 需要0，1