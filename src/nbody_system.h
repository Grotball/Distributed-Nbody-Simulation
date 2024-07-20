#pragma once
#include <mpi.h>
#include <random>
#include <memory>
#include "nbody.h"
#include "load_balancer.h"


// NOTE: This class will likely experience major changes.
class NBodySystem
{
    const int numBodies;
    const bool isMaster;
    const bool isWorker;
    MPI_Comm workerComm;
    MPI_Datatype stridedComponentVecType;
    std::unique_ptr<LoadBalancer> loadBalancer;
    public:
    float* pos;
    float* vel;
    float* acc;
    float* mass;
    NBodySystem(const int numBodies, const bool isMaster, const bool isWorker, const MPI_Comm workerComm, const MPI_Datatype stridedComponentVecType);
    void update(const float dt);
    ~NBodySystem();
};