#include "nbody_system.h"

NBodySystem::NBodySystem(const int numBodies, const bool isMaster, const bool isWorker, const MPI_Comm workerComm) : numBodies(numBodies), isMaster(isMaster), isWorker(isWorker), workerComm(workerComm)
{
    pos = new float[numBodies * 3];
    vel = new float[numBodies * 3];
    acc = new float[numBodies * 3];
    mass = new float[numBodies];

    // This is just a placeholder, will later make constructor that
    // gives some control of initial conditions.

    if (isMaster)
    {
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        for (int i = 0; i < numBodies; i++)
        {
            for (int k = 0; k < 3; k++)
            {
                pos[i+k*numBodies] = 10 * dist(rng);
                vel[i+k*numBodies] = 0;

            }
            mass[i] =  100.0f / (numBodies * G);
        }
    }
}

void NBodySystem::update(const float dt)
{
    //TODO: implement.
    // would like a dedicated load balancing class.
}

NBodySystem::~NBodySystem()
{
    delete[] pos;
    delete[] vel;
    delete[] acc;
    delete[] mass;
}