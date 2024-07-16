#pragma once
#include <cmath>
#include <mpi.h>
#include <vector>

constexpr float softeningFactor = 1.0e-3f;
constexpr float G = 6.67430e-11f;

void computeAccelleration(float* acc, const float* pos, const float* mass, const int lo, const int hi, const int numParticles);

void nbodyIntegrate(float* pos, float* vel, float* acc, float* mass, const int numParticles, const float dt, const std::vector<int>& loPartitionIndices, const std::vector<int>& hiPartitionIndices, const std::vector<int>& partitionSizes, const MPI_Comm comm);