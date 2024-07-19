#pragma once
#include <cstdint>
#include <vector>
#include <mpi.h>
#include <cmath>

struct WorkTask
{
    int32_t workStart;
    int32_t workLength;
    int32_t workSource;
};

// NOTE: This (and all derived) class will likely
// experience major changes.
class LoadBalancer
{
    protected:
    int32_t workSize;
    bool isWorker;
    int32_t workerRank;
    MPI_Comm workerComm;
    int32_t maxWorkLength;
    public:
    LoadBalancer(const bool isWorker, const MPI_Comm workerComm);
    virtual void newWorkInit(const int32_t newWorkSize) = 0;
    virtual bool isFinished() = 0;
    virtual WorkTask getWork() = 0;
    int32_t getMaxWorkLength()const;
    virtual ~LoadBalancer();
};

class StaticBalancer : public LoadBalancer
{
    bool finished;
    int32_t completedCount;
    public:
    StaticBalancer(const bool isWorker, const MPI_Comm workerComm);
    void newWorkInit(const int32_t newWorkSize);
    bool isFinished();
    WorkTask getWork();
};