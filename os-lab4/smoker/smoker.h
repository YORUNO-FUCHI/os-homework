#ifndef SMOKER_H
#define SMOKER_H

#define TOBACCO 0
#define PAPER   1
#define GLUE    2

// 共享内存数据结构
struct shared_data {
    int current_combination;  // 当前供应的组合编号（0-2）
    int supplied[2];          // 供应的两种材料
};

#endif