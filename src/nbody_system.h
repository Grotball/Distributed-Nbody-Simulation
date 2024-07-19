#pragma once
#include <mpi.h>
#include <random>
#include "nbody.h"

class NBodySystem
{
    const int numBodies;
    const bool isMaster;
    const bool isWorker;
    float* pos;
    float* vel;
    float* acc;
    float* mass;
    MPI_Comm workerComm;
    public:
    NBodySystem(const int numBodies, const bool isMaster, const bool isWorker, const MPI_Comm workerComm);
    void update(const float dt);
    ~NBodySystem();
};