#ifndef DISK_H
#define DISK_H

#include <iostream>
#include <algorithm>
#include <cmath>
#include <climits>
#include <random>
#include <chrono>
#include <iomanip>

class DiskArm {
public:
    DiskArm();
    ~DiskArm();
    void InitSpace(const char* MethodName);
    void Report();
    void FCFS();
    void SSTF();
    void SCAN();
    void CSCAN();
    void LOOK();
    void CompareAllAlgorithms();
    void GenerateRandomRequests(int count, bool useRandom = false);
    
private:
    int *Request;
    int *Cylinder;
    int RequestNumber;
    int CurrentCylinder;
    int SeekDirection;
    int SeekNumber;
    int SeekChange;
    int MinCylinder;
    int MaxCylinder;
    int OriginalCurrentCylinder;
    int OriginalSeekDirection;
    
    void ResetState();
    double CalculateAverageSeekTime();
};

#endif