#include "nbody_system.h"

NBodySystem::NBodySystem(const int numBodies, const bool isMaster, const bool isWorker, const MPI_Comm workerComm) : 
        numBodies(numBodies), 
        isMaster(isMaster), 
        isWorker(isWorker), 
        workerComm(workerComm)
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
    MPI_Bcast(pos, 3 * numBodies, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(vel, 3 * numBodies, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(mass, numBodies, MPI_FLOAT, 0, MPI_COMM_WORLD);

    loadBalancer = std::make_unique<StaticBalancer>(isWorker, workerComm);
    
    MPI_Datatype stridedComponentVecTypeNonResized;
    MPI_Type_vector(3, 1, numBodies, MPI_FLOAT, &stridedComponentVecTypeNonResized);
    MPI_Type_commit(&stridedComponentVecTypeNonResized);
    MPI_Type_create_resized(stridedComponentVecTypeNonResized, 0, 1 * sizeof(float), &stridedComponentVecType);
    MPI_Type_commit(&stridedComponentVecType);
    MPI_Type_free(&stridedComponentVecTypeNonResized);
    mpiFreed = false;
}

void NBodySystem::update(const float dt)
{
    loadBalancer->newWorkInit(numBodies);

    std::vector<MPI_Request> requests;

    int workerRank;
    MPI_Comm_rank(workerComm, &workerRank);

    while (isWorker && !loadBalancer->isFinished())
    {
        WorkTask workTask = loadBalancer->getWork();
        MPI_Request request;

        bool isReceiveTask = workTask.workSource != workerRank && workTask.workSource >= 0;
        if (isReceiveTask)
        {
            MPI_Ibcast(acc+workTask.workStart, workTask.workLength, stridedComponentVecType, workTask.workSource, workerComm, &request);
        }
        else
        {
            int32_t lo = workTask.workStart;
            int32_t hi = lo + workTask.workLength;

            computeAccelleration(acc, pos, mass, lo, hi, numBodies);

            MPI_Ibcast(acc+workTask.workStart, workTask.workLength, stridedComponentVecType, workerRank, workerComm, &request);
        }
        requests.emplace_back(request);
    }
    
    if (isWorker)
    {
        MPI_Waitall(requests.size(), requests.data(), MPI_STATUSES_IGNORE);
    }

    requests.clear();

    loadBalancer->newWorkInit(numBodies);

    while (isWorker && !loadBalancer->isFinished())
    {
        WorkTask workTask = loadBalancer->getWork();

        bool isReceiveTask = workTask.workSource != workerRank && workTask.workSource >= 0;
        if (isReceiveTask)
        {
            requests.push_back(0);
            MPI_Ibcast(pos+workTask.workStart, workTask.workLength, stridedComponentVecType, workTask.workSource, workerComm, &requests.back());
            requests.push_back(0);
            MPI_Ibcast(vel+workTask.workStart, workTask.workLength, stridedComponentVecType, workTask.workSource, workerComm, &requests.back());
        }
        else
        {
            int32_t lo = workTask.workStart;
            int32_t hi = lo + workTask.workLength;


            for (int k = 0; k < 3; k++)
            {
                auto i0 = k * numBodies;
                for (auto i = lo; i < hi; i++)
                {
                    pos[i0 + i] += vel[i0 + i] * dt;
                    vel[i0 + i] += acc[i0 + i] * dt;
                }
            }

            requests.push_back(0);
            MPI_Ibcast(pos+workTask.workStart, workTask.workLength, stridedComponentVecType, workerRank, workerComm, &requests.back());
            requests.push_back(0);
            MPI_Ibcast(vel+workTask.workStart, workTask.workLength, stridedComponentVecType, workerRank, workerComm, &requests.back());
        }
    }

    MPI_Waitall(requests.size(), requests.data(), MPI_STATUSES_IGNORE);
}

NBodySystem::~NBodySystem()
{
    delete[] pos;
    delete[] vel;
    delete[] acc;
    delete[] mass;
    if (!mpiFreed)
    {
        mpiFree();
    }
    
}

void NBodySystem::mpiFree()
{
    MPI_Type_free(&stridedComponentVecType);
    mpiFreed = true;
}