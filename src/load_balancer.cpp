#include "load_balancer.h"

LoadBalancer::LoadBalancer(bool isWorker, MPI_Comm workerComm) : isWorker(isWorker), workerComm(workerComm)
{
    MPI_Comm_rank(workerComm, &workerRank);
}
int32_t LoadBalancer::getMaxWorkLength() const
{
    return maxWorkLength;
}

LoadBalancer::~LoadBalancer()
{

}

StaticBalancer::StaticBalancer(bool isWorker_, MPI_Comm workerComm_) : LoadBalancer(isWorker_, workerComm_)
{
    finished = !isWorker;
    completedCount = 0;
}

void StaticBalancer::newWorkInit(const int32_t newWorkSize)
{
    completedCount = 0;
    if (!isWorker)
    {
        finished = true;
        return;
    }

    finished = false;
    
    if (newWorkSize == workSize)
    {
        return;
    }

    workSize = newWorkSize;

    int workerCommSize;

    MPI_Comm_size(workerComm, &workerCommSize);

    maxWorkLength = (int32_t)std::ceil(workSize * 1.0 / workerCommSize);
}

bool StaticBalancer::isFinished()
{
    return finished;
}

WorkTask StaticBalancer::getWork()
{
    WorkTask workTask;
    if (!isWorker)
    {
        workTask.workLength = 0;
        workTask.workStart = 0;
        workTask.workSource = -1;
        return workTask;
    }
    
    auto lo = completedCount * maxWorkLength;
    auto hi = std::min(lo + maxWorkLength, workSize);
    
    workTask.workLength = hi - lo;
    workTask.workStart = lo;
    workTask.workSource = completedCount;

    int workerCommSize;
    MPI_Comm_size(workerComm, &workerCommSize);
    
    completedCount++;

    if (completedCount >= workerCommSize)
    {
        finished = true;
    }

    return workTask;
}