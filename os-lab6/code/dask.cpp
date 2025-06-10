#include "dask.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <climits> 

// 实现文件
// 实现部分
DiskArm::DiskArm(){
    std::cout << "=== 磁盘调度算法模拟器 ===" << std::endl;
    std::cout << "Please input current Cylinder: ";
    std::cin >> CurrentCylinder;
    OriginalCurrentCylinder = CurrentCylinder;
    
    std::cout << "Please input current direction (0: decrease, 1: increase): ";
    std::cin >> SeekDirection;
    OriginalSeekDirection = SeekDirection;
    
    std::cout << "Please input disk min cylinder (default 0): ";
    std::cin >> MinCylinder;
    std::cout << "Please input disk max cylinder (default 199): ";
    std::cin >> MaxCylinder;
    
    char choice;
    std::cout << "Generate requests randomly? (y/n): ";
    std::cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        std::cout << "Please input request numbers: ";
        std::cin >> RequestNumber;
        GenerateRandomRequests(RequestNumber, true);
    } else {
        std::cout << "Please input request numbers: ";
        std::cin >> RequestNumber;
        GenerateRandomRequests(RequestNumber, false);
    }
}

DiskArm::~DiskArm(){
    delete[] Request;
    delete[] Cylinder;
}

void DiskArm::GenerateRandomRequests(int count, bool useRandom) {
    Request = new int[count];
    Cylinder = new int[count];
    
    if (useRandom) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(MinCylinder, MaxCylinder);
        
        std::cout << "Generated random request sequence: ";
        for (int i = 0; i < count; i++) {
            Request[i] = dis(gen);
            std::cout << Request[i] << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "Please input request cylinder string: ";
        for (int i = 0; i < count; i++) {
            std::cin >> Request[i];
        }
    }
}

void DiskArm::ResetState() {
    CurrentCylinder = OriginalCurrentCylinder;
    SeekDirection = OriginalSeekDirection;
}

void DiskArm::InitSpace(const char *MethodName) {
    std::cout << std::endl << "=== " << MethodName << " ===" << std::endl;
    SeekNumber = 0;
    SeekChange = 0;
    for (int i = 0; i < RequestNumber; i++)
        Cylinder[i] = Request[i];
    ResetState();
}

void DiskArm::Report() {
    std::cout << std::endl;
    std::cout << "Total Seek Distance: " << SeekNumber << std::endl;
    std::cout << "Direction Changes: " << SeekChange << std::endl;
    std::cout << "Average Seek Distance: " << std::fixed << std::setprecision(2) 
              << (double)SeekNumber / RequestNumber << std::endl;
    std::cout << std::endl;
}

double DiskArm::CalculateAverageSeekTime() {
    return (double)SeekNumber / RequestNumber;
}

void DiskArm::FCFS() {
    int Current = CurrentCylinder;
    int Direction = SeekDirection;
    InitSpace("FCFS (First Come First Serve)");
    
    std::cout << "Seek sequence: " << Current;
    
    for (int i = 0; i < RequestNumber; i++) {
        bool needChangeDirection = ((Cylinder[i] >= Current) && !Direction) || 
                                 ((Cylinder[i] < Current) && Direction);
        
        if (needChangeDirection) {
            Direction = !Direction;
            SeekChange++;
            std::cout << std::endl << "Direction changed: " << Current << " -> " << Cylinder[i];
        } else {
            std::cout << " -> " << Cylinder[i];
        }
        
        SeekNumber += abs(Current - Cylinder[i]);
        Current = Cylinder[i];
    }
    
    Report();
}

void DiskArm::SSTF(){
    int Current = CurrentCylinder;
    int Direction = SeekDirection;
    InitSpace("SSTF (Shortest Seek Time First)");
    
    bool *visited = new bool[RequestNumber];
    for (int i = 0; i < RequestNumber; i++)
        visited[i] = false;
    
    std::cout << "Seek sequence: " << Current;
    
    for (int count = 0; count < RequestNumber; count++) {
        int minDistance = INT_MAX;
        int nextIndex = -1;
        
        for (int i = 0; i < RequestNumber; i++) {
            if (!visited[i]) {
                int distance = abs(Current - Cylinder[i]);
                if (distance < minDistance) {
                    minDistance = distance;
                    nextIndex = i;
                }
            }
        }
        
        if (nextIndex != -1) {
            visited[nextIndex] = true;
            
            bool needChangeDirection = false;
            if (Direction == 1 && Cylinder[nextIndex] < Current) {
                needChangeDirection = true;
                Direction = 0;
                SeekChange++;
            } else if (Direction == 0 && Cylinder[nextIndex] > Current) {
                needChangeDirection = true;
                Direction = 1;
                SeekChange++;
            }
            
            if (needChangeDirection) {
                std::cout << std::endl << "Direction changed: " << Current << " -> " << Cylinder[nextIndex];
            } else {
                std::cout << " -> " << Cylinder[nextIndex];
            }
            
            SeekNumber += abs(Current - Cylinder[nextIndex]);
            Current = Cylinder[nextIndex];
        }
    }
    
    delete[] visited;
    Report();
}

void DiskArm::SCAN(){
    int Current = CurrentCylinder;
    int Direction = SeekDirection;
    InitSpace("SCAN (Elevator Algorithm)");
    
    int totalRequests = RequestNumber + 2;
    int *allCylinders = new int[totalRequests];
    
    for (int i = 0; i < RequestNumber; i++) {
        allCylinders[i] = Cylinder[i];
    }
    allCylinders[RequestNumber] = MinCylinder;
    allCylinders[RequestNumber + 1] = MaxCylinder;
    
    std::sort(allCylinders, allCylinders + totalRequests);
    
    int currentPos = 0;
    for (int i = 0; i < totalRequests; i++) {
        if (allCylinders[i] >= Current) {
            currentPos = i;
            break;
        }
    }
    
    std::cout << "Seek sequence: " << Current;
    
    if (Direction == 1) {
        for (int i = currentPos; i < totalRequests; i++) {
            if (allCylinders[i] != MinCylinder && allCylinders[i] != MaxCylinder) {
                std::cout << " -> " << allCylinders[i];
                SeekNumber += abs(Current - allCylinders[i]);
                Current = allCylinders[i];
            } else if (allCylinders[i] == MaxCylinder) {
                SeekNumber += abs(Current - MaxCylinder);
                Current = MaxCylinder;
            }
        }
        
        if (currentPos > 0) {
            SeekChange++;
            std::cout << std::endl << "Direction changed at boundary: " << Current;
            
            for (int i = currentPos - 1; i >= 0; i--) {
                if (allCylinders[i] != MinCylinder && allCylinders[i] != MaxCylinder) {
                    std::cout << " -> " << allCylinders[i];
                    SeekNumber += abs(Current - allCylinders[i]);
                    Current = allCylinders[i];
                }
            }
        }
    } else {
        for (int i = currentPos - 1; i >= 0; i--) {
            if (allCylinders[i] != MinCylinder && allCylinders[i] != MaxCylinder) {
                std::cout << " -> " << allCylinders[i];
                SeekNumber += abs(Current - allCylinders[i]);
                Current = allCylinders[i];
            } else if (allCylinders[i] == MinCylinder) {
                SeekNumber += abs(Current - MinCylinder);
                Current = MinCylinder;
            }
        }
        
        if (currentPos < totalRequests) {
            SeekChange++;
            std::cout << std::endl << "Direction changed at boundary: " << Current;
            
            for (int i = currentPos; i < totalRequests; i++) {
                if (allCylinders[i] != MinCylinder && allCylinders[i] != MaxCylinder) {
                    std::cout << " -> " << allCylinders[i];
                    SeekNumber += abs(Current - allCylinders[i]);
                    Current = allCylinders[i];
                }
            }
        }
    }
    
    delete[] allCylinders;
    Report();
}

void DiskArm::CSCAN(){
    int Current = CurrentCylinder;
    int Direction = SeekDirection;
    InitSpace("C-SCAN (Circular SCAN)");
    
    // 创建请求数组的副本用于排序
    int *sortedRequests = new int[RequestNumber];
    for (int i = 0; i < RequestNumber; i++) {
        sortedRequests[i] = Cylinder[i];
    }
    std::sort(sortedRequests, sortedRequests + RequestNumber);
    
    // 找到当前位置在排序数组中的位置
    int currentPos = 0;
    for (int i = 0; i < RequestNumber; i++) {
        if (sortedRequests[i] >= Current) {
            currentPos = i;
            break;
        }
    }
    
    std::cout << "Seek sequence: " << Current;
    
    if (Direction == 1) { // 向大磁道号方向移动
        // 先处理大于等于当前位置的请求
        for (int i = currentPos; i < RequestNumber; i++) {
            std::cout << " -> " << sortedRequests[i];
            SeekNumber += abs(Current - sortedRequests[i]);
            Current = sortedRequests[i];
        }
        
        // 如果还有更小的请求需要处理
        if (currentPos > 0) {
            // 跳转到磁盘边界，然后从最小的请求开始
            SeekNumber += abs(Current - MaxCylinder);
            SeekNumber += abs(MaxCylinder - MinCylinder);
            Current = MinCylinder;
            SeekChange++;
            
            std::cout << std::endl << "Jump to beginning: " << Current;
            
            // 处理小于原始当前位置的请求
            for (int i = 0; i < currentPos; i++) {
                std::cout << " -> " << sortedRequests[i];
                SeekNumber += abs(Current - sortedRequests[i]);
                Current = sortedRequests[i];
            }
        }
    } else { // 向小磁道号方向移动
        // 先处理小于等于当前位置的请求
        for (int i = currentPos - 1; i >= 0; i--) {
            std::cout << " -> " << sortedRequests[i];
            SeekNumber += abs(Current - sortedRequests[i]);
            Current = sortedRequests[i];
        }
        
        // 如果还有更大的请求需要处理
        if (currentPos < RequestNumber) {
            // 跳转到磁盘边界，然后从最大的请求开始
            SeekNumber += abs(Current - MinCylinder);
            SeekNumber += abs(MaxCylinder - MinCylinder);
            Current = MaxCylinder;
            SeekChange++;
            
            std::cout << std::endl << "Jump to end: " << Current;
            
            // 处理大于原始当前位置的请求
            for (int i = RequestNumber - 1; i >= currentPos; i--) {
                std::cout << " -> " << sortedRequests[i];
                SeekNumber += abs(Current - sortedRequests[i]);
                Current = sortedRequests[i];
            }
        }
    }
    
    delete[] sortedRequests;
    Report();
}

void DiskArm::LOOK(){
    int Current = CurrentCylinder;
    int Direction = SeekDirection;
    InitSpace("LOOK Algorithm");
    
    // 创建请求数组的副本用于排序
    int *sortedRequests = new int[RequestNumber];
    for (int i = 0; i < RequestNumber; i++) {
        sortedRequests[i] = Cylinder[i];
    }
    std::sort(sortedRequests, sortedRequests + RequestNumber);
    
    // 找到当前位置在排序数组中的位置
    int currentPos = 0;
    for (int i = 0; i < RequestNumber; i++) {
        if (sortedRequests[i] >= Current) {
            currentPos = i;
            break;
        }
    }
    
    std::cout << "Seek sequence: " << Current;
    
    if (Direction == 1) { // 向大磁道号方向移动
        // 先处理大于等于当前位置的请求
        for (int i = currentPos; i < RequestNumber; i++) {
            std::cout << " -> " << sortedRequests[i];
            SeekNumber += abs(Current - sortedRequests[i]);
            Current = sortedRequests[i];
        }
        
        // 改变方向处理剩余请求
        if (currentPos > 0) {
            SeekChange++;
            std::cout << std::endl << "Direction changed: " << Current;
            
            // 处理小于原始当前位置的请求
            for (int i = currentPos - 1; i >= 0; i--) {
                std::cout << " -> " << sortedRequests[i];
                SeekNumber += abs(Current - sortedRequests[i]);
                Current = sortedRequests[i];
            }
        }
    } else { // 向小磁道号方向移动
        // 先处理小于等于当前位置的请求
        for (int i = currentPos - 1; i >= 0; i--) {
            std::cout << " -> " << sortedRequests[i];
            SeekNumber += abs(Current - sortedRequests[i]);
            Current = sortedRequests[i];
        }
        
        // 改变方向处理剩余请求
        if (currentPos < RequestNumber) {
            SeekChange++;
            std::cout << std::endl << "Direction changed: " << Current;
            
            // 处理大于原始当前位置的请求
            for (int i = currentPos; i < RequestNumber; i++) {
                std::cout << " -> " << sortedRequests[i];
                SeekNumber += abs(Current - sortedRequests[i]);
                Current = sortedRequests[i];
            }
        }
    }
    
    delete[] sortedRequests;
    Report();
}

void DiskArm::CompareAllAlgorithms() {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "PERFORMANCE COMPARISON OF ALL ALGORITHMS" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    struct AlgorithmResult {
        std::string name;
        int seekDistance;
        int directionChanges;
        double averageSeek;
    };
    
    AlgorithmResult results[5];
    
    // FCFS
    FCFS();
    results[0] = {"FCFS", SeekNumber, SeekChange, CalculateAverageSeekTime()};
    
    // SSTF
    SSTF();
    results[1] = {"SSTF", SeekNumber, SeekChange, CalculateAverageSeekTime()};
    
    // SCAN
    SCAN();
    results[2] = {"SCAN", SeekNumber, SeekChange, CalculateAverageSeekTime()};
    
    // C-SCAN
    CSCAN();
    results[3] = {"C-SCAN", SeekNumber, SeekChange, CalculateAverageSeekTime()};
    
    // LOOK
    LOOK();
    results[4] = {"LOOK", SeekNumber, SeekChange, CalculateAverageSeekTime()};
    
    // 打印比较表格
    std::cout << std::left << std::setw(12) << "Algorithm" 
              << std::setw(15) << "Total Seek" 
              << std::setw(18) << "Dir Changes" 
              << std::setw(15) << "Avg Seek" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (int i = 0; i < 5; i++) {
        std::cout << std::left << std::setw(12) << results[i].name
                  << std::setw(15) << results[i].seekDistance
                  << std::setw(18) << results[i].directionChanges
                  << std::fixed << std::setprecision(2) << results[i].averageSeek << std::endl;
    }
    
    // 找出最优算法
    int bestIndex = 0;
    for (int i = 1; i < 5; i++) {
        if (results[i].seekDistance < results[bestIndex].seekDistance) {
            bestIndex = i;
        }
    }
    
    std::cout << std::endl << "Best Performance: " << results[bestIndex].name 
              << " (Total Seek Distance: " << results[bestIndex].seekDistance << ")" << std::endl;
}
// 主函数
int main() {
    DiskArm disk;
    
    int choice;
    do {
        std::cout << "\n=== Disk Scheduling Algorithm Menu ===" << std::endl;
        std::cout << "1. FCFS Algorithm" << std::endl;
        std::cout << "2. SSTF Algorithm" << std::endl;
        std::cout << "3. SCAN Algorithm" << std::endl;
        std::cout << "4. C-SCAN Algorithm" << std::endl;
        std::cout << "5. LOOK Algorithm" << std::endl;
        std::cout << "6. Compare All Algorithms" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        
        switch(choice) {
            case 1:
                disk.FCFS();
                break;
            case 2:
                disk.SSTF();
                break;
            case 3:
                disk.SCAN();
                break;
            case 4:
                disk.CSCAN();
                break;
            case 5:
                disk.LOOK();
                break;
            case 6:
                disk.CompareAllAlgorithms();
                break;
            case 0:
                std::cout << "Goodbye!" << std::endl;
                break;
            default:
                std::cout << "Invalid choice! Please try again." << std::endl;
        }
    } while(choice != 0);
    
    return 0;
}