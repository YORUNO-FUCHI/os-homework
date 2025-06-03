
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cstring>

class Replace {
public:
    Replace();
    ~Replace();
    void InitSpace(char* MethodName); // 初始化页号记录
    void Report(void);               // 报告算法执行情况
    void Fifo(void);                 // 先进先出算法
    void Lru(void);                  // 最近最旧未用算法
    void Clock(void);                // 时钟(二次机会)置换算法
    void Eclock(void);               // 增强二次机会置换算法
    void GenerateRandomReference(int length, int max_page, double write_ratio = 0.3); // 随机生成引用串
    void CompareAlgorithms();        // 算法比较功能
    void Lfu(void);                  // 最不经常使用置换算法
    void Mfu(void);                  // 最经常使用置换算法


private:
    int* ReferencePage;  // 存放要访问到的页号
    bool* ReferenceType; // 存放访问类型 (true = 写操作, false = 读操作)
    int* EliminatePage;  // 存放淘汰页号
    int* PageFrames;     // 存放当前正在实存中的页号 (用于FIFO/LRU)
    
    // 用于Clock/Eclock算法的数据结构
    struct FrameEntry {
        int pageNum;
        bool R; // 访问位
        bool M; // 修改位
    };
    FrameEntry* ClockFrames; // 用于Clock算法的帧
    FrameEntry* EclockFrames; // 用于Eclock算法的帧
    
     struct FreqEntry {
        int pageNum;
        int frequency; // 访问频率
        int lastUsed;  // 最近使用时间（用于打破平局）
    };
    FreqEntry* FreqFrames; // 用于LFU/MFU算法的帧

    int globalTime;        // 全局时间计数器
    int PageNumber;      // 访问页数
    int FrameNumber;     // 实存帧数
    int FaultNumber;     // 失败页数
    int currentPointer;  // 时钟指针
};