#include "vmrp.h"
#include <climits>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cstring>
using namespace std;

Replace::Replace() 
    : ReferencePage(nullptr), ReferenceType(nullptr), EliminatePage(nullptr), 
      PageFrames(nullptr), ClockFrames(nullptr), EclockFrames(nullptr),
      FreqFrames(nullptr), PageNumber(0), FrameNumber(0), FaultNumber(0), 
      currentPointer(0), globalTime(0) {
    
    srand(time(0)); // 初始化随机种子
    
    cout << "===== 内存页面置换算法模拟程序 =====" << endl;
    cout << "选择输入方式: " << endl;
    cout << "1. 手动输入" << endl;
    cout << "2. 随机生成" << endl;
    cout << "请选择: ";
    int choice;
    cin >> choice;
    
    if (choice == 1) {
        // 手动输入模式
        cout << "请输入页面引用数量: ";
        cin >> PageNumber;
        ReferencePage = new int[PageNumber];
        ReferenceType = new bool[PageNumber];
        EliminatePage = new int[PageNumber];
        
        cout << "请输入页面引用序列 (格式: 页号 类型[r/w], 如: 1 r, 2 w):" << endl;
        for (int i = 0; i < PageNumber; i++) {
            char type;
            cout << "引用 #" << i+1 << ": ";
            cin >> ReferencePage[i] >> type;
            ReferenceType[i] = (type == 'w' || type == 'W');
        }
    } else {
        // 随机生成模式
        cout << "请输入引用串长度: ";
        cin >> PageNumber;
        cout << "请输入最大页号: ";
        int max_page;
        cin >> max_page;
        cout << "请输入写操作比例 (0.0-1.0, 默认0.3): ";
        double write_ratio = 0.3;
        cin >> write_ratio;
        
        ReferencePage = new int[PageNumber];
        ReferenceType = new bool[PageNumber];
        EliminatePage = new int[PageNumber];
        GenerateRandomReference(PageNumber, max_page, write_ratio);
        
        cout << "生成的引用串: ";
        for (int i = 0; i < PageNumber; i++) {
            cout << ReferencePage[i] << (ReferenceType[i] ? "w" : "r") << " ";
        }
        cout << endl;
    }
    
    // 设定内存实页数(帧数)
    cout << "请输入内存页帧数: ";
    cin >> FrameNumber;
    PageFrames = new int[FrameNumber];
    ClockFrames = new FrameEntry[FrameNumber];
    EclockFrames = new FrameEntry[FrameNumber];
    FreqFrames = new FreqEntry[FrameNumber];
}

Replace::~Replace() {
    delete[] ReferencePage;
    delete[] ReferenceType;
    delete[] EliminatePage;
    delete[] PageFrames;
    delete[] ClockFrames;
    delete[] EclockFrames;
    delete[] FreqFrames;
}

void Replace::GenerateRandomReference(int length, int max_page, double write_ratio) {
    for (int i = 0; i < length; i++) {
        ReferencePage[i] = rand() % max_page + 1; // 页号从1开始
        ReferenceType[i] = (static_cast<double>(rand()) / RAND_MAX) < write_ratio;
    }
}

void Replace::InitSpace(char* MethodName) {
    int i;
    cout << endl << "===== " << MethodName << " 算法 =====" << endl;
    FaultNumber = 0;
    currentPointer = 0;
    globalTime = 0;
    
    // 引用还未开始，-1表示无引用页
    for (i = 0; i < PageNumber; i++)
        EliminatePage[i] = -1;
    
    // 初始化页面帧
    for (i = 0; i < FrameNumber; i++) {
        if (PageFrames) PageFrames[i] = -1;
        if (ClockFrames) {
            ClockFrames[i].pageNum = -1;
            ClockFrames[i].R = false;
            ClockFrames[i].M = false;
        }
        if (EclockFrames) {
            EclockFrames[i].pageNum = -1;
            EclockFrames[i].R = false;
            EclockFrames[i].M = false;
        }
        if (FreqFrames) {
            FreqFrames[i].pageNum = -1;
            FreqFrames[i].frequency = 0;
            FreqFrames[i].lastUsed = -1;
        }
    }
}

// 分析统计选择的算法对于当前输入的页面走向的性能
void Replace::Report(void) {
    // 报告淘汰页顺序
    cout << endl << "淘汰页面序列: ";
    bool first = true;
    for (int i = 0; i < PageNumber && EliminatePage[i] != -1; i++) {
        if (!first) cout << " -> ";
        cout << EliminatePage[i];
        first = false;
    }
    
    // 报告缺页数和缺页率
    cout << endl << "缺页次数 = " << FaultNumber << endl;
    cout << fixed << setprecision(2);
    cout << "缺页率 = " << 100 * static_cast<float>(FaultNumber) / PageNumber << "%" << endl;
}

// 最近最旧未用置换算法
void Replace::Lru(void) {
    int i, j, k, l = 0;
    InitSpace("LRU");
    
    // 循环装入引用页
    for (k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;
        
        // 检测引用页当前是否已在实存
        for (i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                // 找到将其调整到页记录栈顶
                int temp = PageFrames[i];
                for (j = i; j > 0; j--) 
                    PageFrames[j] = PageFrames[j - 1];
                PageFrames[0] = temp;
                found = true;
                break;
            }
        }
        
        if (found) {
            // 报告当前内存页号
            for (j = 0; j < FrameNumber; j++) {
                if (PageFrames[j] >= 0) 
                    cout << PageFrames[j] << " ";
            }
            cout << endl;
        } else {
            // 缺页处理
            FaultNumber++;
            EliminatePage[l] = PageFrames[FrameNumber - 1]; // 记录淘汰页
            
            // 向下压栈
            for (j = FrameNumber - 1; j > 0; j--)
                PageFrames[j] = PageFrames[j - 1];
                
            PageFrames[0] = next; // 新页放栈顶
            
            // 报告当前实存中页号
            for (j = 0; j < FrameNumber; j++) {
                if (PageFrames[j] >= 0) 
                    cout << PageFrames[j] << " ";
            }
            
            // 报告淘汰页
            if (EliminatePage[l] >= 0)
                cout << " -> " << EliminatePage[l++];
            cout << endl;
        }
    }
    Report();
}

// 先进先出置换算法
void Replace::Fifo(void) {
    int i, j = 0, k, l = 0;
    InitSpace("FIFO");
    
    for (k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];//要找的页面
        bool found = false;
        
        // 检测引用页是否在实存中
        for (i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                break;
            }
        }
        
        if (found) {
            // 报告当前内存页号
            for (i = 0; i < FrameNumber; i++) {
                if (PageFrames[i] >= 0) 
                    cout << PageFrames[i] << " ";
            }
            cout << endl;
        } else {
            // 缺页处理 j指向被淘汰的页面
            FaultNumber++;
            EliminatePage[l] = PageFrames[j]; // 记录淘汰页
            PageFrames[j] = next; // 新页放入
            j = (j + 1) % FrameNumber; // 指针移动
            
            // 报告当前实存页号
            for (i = 0; i < FrameNumber; i++) {
                if (PageFrames[i] >= 0) 
                    cout << PageFrames[i] << " ";
            }
            
            // 报告淘汰页
            if (EliminatePage[l] >= 0)
                cout << " -> " << EliminatePage[l++];
            cout << endl;
        }
    }
    Report();
}

// 时钟(二次机会)置换算法
void Replace::Clock(void) {
    int k, l = 0;
    InitSpace("Clock");
    
    for (k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool isWrite = ReferenceType[k];
        bool found = false;
        
        // 检查页面是否已在内存中
        for (int i = 0; i < FrameNumber; i++) {
            if (ClockFrames[i].pageNum == next) {
                ClockFrames[i].R = true; // 设置访问位
                found = true;
                break;
            }
        }
        
        if (found) {
            // 输出当前帧状态
            for (int i = 0; i < FrameNumber; i++) {
                if (ClockFrames[i].pageNum != -1) {
                    cout << ClockFrames[i].pageNum << "(" 
                         << ClockFrames[i].R << ") ";
                }
            }
            cout << endl;
            continue;
        }
        
        // 缺页处理
        FaultNumber++;
        bool pageReplaced = false;
        
        while (!pageReplaced) {
            if (ClockFrames[currentPointer].pageNum == -1) {
                // 空闲帧
                ClockFrames[currentPointer].pageNum = next;
                ClockFrames[currentPointer].R = true;
                pageReplaced = true;
                EliminatePage[l] = -1; // 表示没有淘汰页面
            }
            else if (ClockFrames[currentPointer].R == 0) {
                // 找到牺牲页
                EliminatePage[l] = ClockFrames[currentPointer].pageNum;
                ClockFrames[currentPointer].pageNum = next;
                ClockFrames[currentPointer].R = true;
                pageReplaced = true;
                l++;
            } else {
                // 给予第二次机会
                ClockFrames[currentPointer].R = 0;
            }
            
            // 移动指针
            currentPointer = (currentPointer + 1) % FrameNumber;
        }
        
        // 输出当前状态
        for (int i = 0; i < FrameNumber; i++) {
            if (ClockFrames[i].pageNum != -1) {
                cout << ClockFrames[i].pageNum << "(" 
                     << ClockFrames[i].R << ") ";
            }
        }
        if (EliminatePage[l-1] >= 0)
            cout << " -> " << EliminatePage[l-1];
        else
            cout << " -> (空闲帧)";
        cout << endl;
    }
    Report();
}
// 增强二次机会置换算法 - 修复版本
void Replace::Eclock(void) {
    int k, l = 0;
    InitSpace("Enhanced Clock");
    
    for (k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool isWrite = ReferenceType[k];
        bool found = false;
        
        // 检查页面是否已在内存中
        for (int i = 0; i < FrameNumber; i++) {
            if (EclockFrames[i].pageNum == next) {
                EclockFrames[i].R = true; // 设置访问位
                if (isWrite) EclockFrames[i].M = true; // 如果是写操作，设置修改位
                found = true;
                break;
            }
        }
        
        if (found) {
            for (int i = 0; i < FrameNumber; i++) {
                if (EclockFrames[i].pageNum != -1) {
                    cout << EclockFrames[i].pageNum << "(" 
                         << EclockFrames[i].R << EclockFrames[i].M << ") ";
                }
            }
            cout << endl;
            continue;
        }
        
        // 缺页处理
        FaultNumber++;
        int victim = -1;
        
        // 第一轮：寻找 (R=0, M=0) 的页面
        int startPointer = currentPointer;
        do {
            if (EclockFrames[currentPointer].pageNum == -1) {
                // 空闲帧，直接使用
                victim = currentPointer;
                break;
            }
            else if (EclockFrames[currentPointer].R == 0 && 
                     EclockFrames[currentPointer].M == 0) {
                // 找到类别1的页面 (R=0, M=0)
                victim = currentPointer;
                break;
            }
            currentPointer = (currentPointer + 1) % FrameNumber;
        } while (currentPointer != startPointer && victim == -1);
        
        // 第二轮：寻找 (R=0, M=1) 的页面，同时清除所有R位
        if (victim == -1) {
            startPointer = currentPointer;
            do {
                if (EclockFrames[currentPointer].R == 0 && 
                    EclockFrames[currentPointer].M == 1) {
                    // 找到类别2的页面 (R=0, M=1)
                    victim = currentPointer;
                }
                // 清除R位，为下次扫描做准备
                EclockFrames[currentPointer].R = 0;
                currentPointer = (currentPointer + 1) % FrameNumber;
            } while (currentPointer != startPointer && victim == -1);
        }
        
        // 第三轮：寻找 (R=0, M=0) 的页面（经过第二轮清除R位后）
        if (victim == -1) {
            startPointer = currentPointer;
            do {
                if (EclockFrames[currentPointer].R == 0 && 
                    EclockFrames[currentPointer].M == 0) {
                    // 找到类别3的页面，现在是 (R=0, M=0)
                    victim = currentPointer;
                    break;
                }
                currentPointer = (currentPointer + 1) % FrameNumber;
            } while (currentPointer != startPointer && victim == -1);
        }
        
        // 第四轮：寻找 (R=0, M=1) 的页面
        if (victim == -1) {
            startPointer = currentPointer;
            do {
                if (EclockFrames[currentPointer].R == 0 && 
                    EclockFrames[currentPointer].M == 1) {
                    // 找到类别4的页面 (R=0, M=1)
                    victim = currentPointer;
                    break;
                }
                currentPointer = (currentPointer + 1) % FrameNumber;
            } while (currentPointer != startPointer);
        }
        
        // 如果仍然没有找到，选择当前指针位置（理论上不会发生）
        if (victim == -1) {
            victim = currentPointer;
        }
        
        // 记录淘汰页
        EliminatePage[l] = EclockFrames[victim].pageNum;
        
        // 装入新页
        EclockFrames[victim].pageNum = next;
        EclockFrames[victim].R = true;
        EclockFrames[victim].M = isWrite;
        
        // 移动指针到下一个位置
        currentPointer = (victim + 1) % FrameNumber;
        
        // 输出当前状态
        for (int i = 0; i < FrameNumber; i++) {
            if (EclockFrames[i].pageNum != -1) {
                cout << EclockFrames[i].pageNum << "(" 
                     << EclockFrames[i].R << EclockFrames[i].M << ") ";
            }
        }
        
        // 报告淘汰页
        if (EliminatePage[l] >= 0)
            cout << " -> " << EliminatePage[l++];
        else
            cout << " -> (空闲帧)";
        cout << endl;
    }
    Report();
}

// 最不经常使用置换算法
void Replace::Lfu(void) {
    int k, l = 0;
    InitSpace("LFU (最不经常使用)");
    globalTime = 0; // 重置全局时间计数器
    
    // 初始化频率帧
    for (int i = 0; i < FrameNumber; i++) {
        FreqFrames[i].pageNum = -1;
        FreqFrames[i].frequency = 0;
        FreqFrames[i].lastUsed = -1;
    }
    
    for (k = 0; k < PageNumber; k++) {
        globalTime++; // 增加全局时间
        int next = ReferencePage[k];
        bool found = false;
        
        // 检查页面是否已在内存中
        for (int i = 0; i < FrameNumber; i++) {
            if (FreqFrames[i].pageNum == next) {
                // 命中：增加访问频率，更新最后使用时间
                FreqFrames[i].frequency++;
                FreqFrames[i].lastUsed = globalTime;
                found = true;
                break;
            }
        }
        
        if (found) {
            // 输出当前帧状态
            for (int i = 0; i < FrameNumber; i++) {
                if (FreqFrames[i].pageNum != -1) {
                    cout << FreqFrames[i].pageNum << "(" 
                         << FreqFrames[i].frequency << ") ";
                }
            }
            cout << endl;
            continue;
        }
        
        // 缺页处理
        FaultNumber++;
        int victim = -1;
        int minFreq = INT_MAX;
        int oldestTime = INT_MAX;
        
        // 寻找频率最低且最久未使用的页面
        for (int i = 0; i < FrameNumber; i++) {
            if (FreqFrames[i].pageNum == -1) {

                victim = i;
                break;
            }
            
            // 优先选择频率最低的页面
            if (FreqFrames[i].frequency < minFreq) {
                minFreq = FreqFrames[i].frequency;
                oldestTime = FreqFrames[i].lastUsed;
                victim = i;
            } 
            // 频率相同则选择最久未使用的
            else if (FreqFrames[i].frequency == minFreq && 
                     FreqFrames[i].lastUsed < oldestTime) {
                oldestTime = FreqFrames[i].lastUsed;
                victim = i;
            }
        }
        
        // 记录淘汰页
        EliminatePage[l] = FreqFrames[victim].pageNum;
        
        // 装入新页
        FreqFrames[victim].pageNum = next;
        FreqFrames[victim].frequency = 1; // 初始频率为1
        FreqFrames[victim].lastUsed = globalTime;
        
        // 输出当前状态
        for (int i = 0; i < FrameNumber; i++) {
            if (FreqFrames[i].pageNum != -1) {
                cout << FreqFrames[i].pageNum << "(" 
                     << FreqFrames[i].frequency << ") ";
            }
        }
        
        // 报告淘汰页
        if (EliminatePage[l] >= 0)
            cout << " -> " << EliminatePage[l++];
        else
            cout << " -> (空闲帧)";
        cout << endl;
    }
    Report();
}

// 最经常使用置换算法
void Replace::Mfu(void) {
    int k, l = 0;
    InitSpace("MFU (最经常使用)");
    globalTime = 0; // 重置全局时间计数器
    
    // 初始化频率帧
    for (int i = 0; i < FrameNumber; i++) {
        FreqFrames[i].pageNum = -1;
        FreqFrames[i].frequency = 0;
        FreqFrames[i].lastUsed = -1;
    }
    
    for (k = 0; k < PageNumber; k++) {
        globalTime++; // 增加全局时间
        int next = ReferencePage[k];
        bool found = false;
        
        // 检查页面是否已在内存中
        for (int i = 0; i < FrameNumber; i++) {
            if (FreqFrames[i].pageNum == next) {
                // 命中：增加访问频率，更新最后使用时间
                FreqFrames[i].frequency++;
                FreqFrames[i].lastUsed = globalTime;
                found = true;
                break;
            }
        }
        
        if (found) {
            // 输出当前帧状态
            for (int i = 0; i < FrameNumber; i++) {
                if (FreqFrames[i].pageNum != -1) {
                    cout << FreqFrames[i].pageNum << "(" 
                         << FreqFrames[i].frequency << ") ";
                }
            }
            cout << endl;
            continue;
        }
        
        // 缺页处理
        FaultNumber++;
        int victim = -1;
        int maxFreq = -1;
        int oldestTime = INT_MAX;
        
        // 寻找频率最高且最久未使用的页面
        for (int i = 0; i < FrameNumber; i++) {
            if (FreqFrames[i].pageNum == -1) {
                // 找到空闲帧，直接使用
                victim = i;
                break;
            }
            
            // 优先选择频率最高的页面
            if (FreqFrames[i].frequency > maxFreq) {
                maxFreq = FreqFrames[i].frequency;
                oldestTime = FreqFrames[i].lastUsed;
                victim = i;
            } 
            // 频率相同则选择最久未使用的
            else if (FreqFrames[i].frequency == maxFreq && 
                     FreqFrames[i].lastUsed < oldestTime) {
                oldestTime = FreqFrames[i].lastUsed;
                victim = i;
            }
        }
        
        // 记录淘汰页
        EliminatePage[l] = FreqFrames[victim].pageNum;
        
        // 装入新页
        FreqFrames[victim].pageNum = next;
        FreqFrames[victim].frequency = 1; // 初始频率为1
        FreqFrames[victim].lastUsed = globalTime;
        
        // 输出当前状态
        for (int i = 0; i < FrameNumber; i++) {
            if (FreqFrames[i].pageNum != -1) {
                cout << FreqFrames[i].pageNum << "(" 
                     << FreqFrames[i].frequency << ") ";
            }
        }
        
        // 报告淘汰页
        if (EliminatePage[l] >= 0)
            cout << " -> " << EliminatePage[l++];
        else
            cout << " -> (空闲帧)";
        cout << endl;
    }
    Report();
}

// 算法比较功能
void Replace::CompareAlgorithms() {
    int originalFrames = FrameNumber;
    vector<int> frameSizes;
    
    cout << "\n===== 算法性能比较 =====" << endl;
    cout << "请输入要测试的内存帧数序列 (以0结束): ";
    int frame;
    while (cin >> frame && frame != 0) {
        frameSizes.push_back(frame);
    }
    
    cout << "\n测试结果:\n";
    cout << "帧数\tFIFO\tLRU\tClock\tEclock\tLFU\tMFU\n";
    cout << "--------------------------------------------------\n";
    
    for (int frames : frameSizes) {
        FrameNumber = frames;
        int faults[6] = {0};
        
        // 保存原始FaultNumber
        int originalFaultNumber = FaultNumber;
        
        // FIFO
        Fifo();
        faults[0] = FaultNumber;
        FaultNumber = originalFaultNumber;
        
        // LRU
        Lru();
        faults[1] = FaultNumber;
        FaultNumber = originalFaultNumber;
        
        // Clock
        Clock();
        faults[2] = FaultNumber;
        FaultNumber = originalFaultNumber;
        
        // Eclock
        Eclock();
        faults[3] = FaultNumber;
        FaultNumber = originalFaultNumber;
        
        // LFU
        Lfu();
        faults[4] = FaultNumber;
        FaultNumber = originalFaultNumber;
        
        // MFU
        Mfu();
        faults[5] = FaultNumber;
        FaultNumber = originalFaultNumber;
        
        // 输出结果
        cout << frames << "\t";
        for (int i = 0; i < 6; i++) {
            cout << faults[i] << "\t";
        }
        cout << endl;
    }
    
    // 恢复原始帧数
    FrameNumber = originalFrames;
}

int main(int argc, char* argv[]) {
    Replace* vmpr = new Replace();
    
    while (true) {
        cout << "\n===== 页面置换算法模拟 =====" << endl;
        cout << "1. FIFO 算法" << endl;
        cout << "2. LRU 算法" << endl;
        cout << "3. Clock 算法" << endl;
        cout << "4. 增强二次机会算法" << endl;
        cout << "5. LFU 算法" << endl;
        cout << "6. MFU 算法" << endl;
        cout << "7. 算法性能比较" << endl;
        cout << "8. 退出" << endl;
        cout << "请选择: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                vmpr->Fifo();
                break;
            case 2:
                vmpr->Lru();
                break;
            case 3:
                vmpr->Clock();
                break;
            case 4:
                vmpr->Eclock();
                break;
            case 5:
                vmpr->Lfu();
                break;
            case 6:
                vmpr->Mfu();
                break;
            case 7:
                vmpr->CompareAlgorithms();
                break;
            case 8:
                delete vmpr;
                return 0;
            default:
                cout << "无效选择，请重新输入！" << endl;
        }
    }
}