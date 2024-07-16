#include "nbody.h"

void computeAccelleration(float* acc, const float* pos, const float* mass, const int lo, const int hi, const int numParticles)
{
    for (auto i = lo; i < hi; i++)
    {
        for (int k = 0; k < 3; k++)
        {
            acc[i + k * numParticles] = 0;
        }
        auto xi = pos[i];
        auto yi = pos[i + numParticles];
        auto zi = pos[i + 2 * numParticles];
        for (auto j = 0; j < numParticles; j++)
        {
            auto xj = pos[j];
            auto yj = pos[j + numParticles];
            auto zj = pos[j + 2 * numParticles];
            float rx = xi - xj;
            float ry = yi - yj;
            float rz = zi - zj;
            float r2 = rx*rx + ry*ry + rz*rz;
            float dist = std::sqrt(r2 + (softeningFactor*softeningFactor));
            float s = G * mass[j] / (dist*dist*dist);
            
            acc[i] -= s * rx;
            acc[i + numParticles] -= s * ry;
            acc[i + 2 * numParticles] -= s * rz;
        }
    }
}

void nbodyIntegrate(float* pos, float* vel, float* acc, float* mass, const int numParticles, const float dt, const std::vector<int>& loPartitionIndices, const std::vector<int>& hiPartitionIndices, const std::vector<int>& partitionSizes, const MPI_Comm comm)
{
    int rank;
    MPI_Comm_rank(comm, &rank);

    int lo = loPartitionIndices[rank];
    int hi = hiPartitionIndices[rank];

    computeAccelleration(acc, pos, mass, lo, hi, numParticles);

    for (int k = 0; k < 3; k++)
    {
        auto i0 = k * numParticles;
        for (int i = lo; i < hi; i++)
        {
            pos[i0 + i] += vel[i0 + i] * dt;
            vel[i0 + i] += acc[i0 + i] * dt;
        }
    }
}