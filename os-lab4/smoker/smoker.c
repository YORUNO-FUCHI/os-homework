#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ipc.h"
#include "smoker.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <smoker_id (0-2)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int smoker_id = atoi(argv[1]);
    const char *materials[] = {"Tobacco", "Paper", "Glue"};
    const char *owned_material = materials[smoker_id];

    // 获取共享内存
    key_t shm_key = 1000;
    struct shared_data *shared = (struct shared_data*)set_shm(shm_key, sizeof(struct shared_data), IPC_CREAT | 0666);

    // 获取信号量
    int smoker_sem = set_sem(3000 + smoker_id, 0, IPC_CREAT | 0666);
    int done_sem = set_sem(4000 + smoker_id, 0, IPC_CREAT | 0666);

    while (1) {
        down(smoker_sem);  // 等待被唤醒

        // 卷烟并输出信息
        printf("[Smoker %d (%s)] Making cigarette with materials %d and %d\n",
               smoker_id, owned_material, shared->supplied[0], shared->supplied[1]);
        sleep(1);  // 模拟卷烟时间

        up(done_sem);  // 通知供应者完成
    }

    return 0;
}