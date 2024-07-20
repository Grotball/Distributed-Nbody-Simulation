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
    bool mpiFreed;
    public:
    // Vector quantities are stored in a linearly index array. The x, y, z components are NOT
    // interleaved. All the x-components contiguous, followed contiguously by all the y-components.
    float* pos;
    float* vel;
    float* acc;
    float* mass;
    NBodySystem(const int numBodies, const bool isMaster, const bool isWorker, const MPI_Comm workerComm);
    void update(const float dt);
    // Manually free all mpi reasources.
    // Provides way to manually free mpi resources early
    // when not known if the destructor will run before
    // MPI_Finalize.
    // NBodySystem instance should not be used again
    // after this function is called.
    // TODO: warn user if trying to use after free.
    void mpiFree();
    ~NBodySystem();
};