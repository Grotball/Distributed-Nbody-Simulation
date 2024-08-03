#include "nbody.h"

void computeAccelleration(float* acc, const float* pos, const float* mass, const int lo, const int hi, const int numParticles)
{
    #ifdef USE_SIMD
    constexpr int stride = MaxPackedFloats;
    const int n = numParticles % stride;
    
    #ifdef __AVX__
    const __m256 softeningFactor2 = _mm256_set1_ps(softeningFactor*softeningFactor);
    const __m256 G_v = _mm256_set1_ps(G);
    #endif

    #else
    const int n = numParticles;
    #endif
    for (auto i = lo; i < hi; i++)
    {
        for (int k = 0; k < 3; k++)
        {
            acc[i + k * numParticles] = 0;
        }
        const float xi = pos[i];
        const float yi = pos[i + numParticles];
        const float zi = pos[i + 2 * numParticles];
        for (auto j = 0; j < n; j++)
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

        #ifdef USE_SIMD
        
        #ifdef __AVX__
        const __m256 xi_v = _mm256_set1_ps(xi);
        const __m256 yi_v = _mm256_set1_ps(yi);
        const __m256 zi_v = _mm256_set1_ps(zi);
        __m256 a1 = _mm256_setzero_ps();
        __m256 a2 = _mm256_setzero_ps();
        __m256 a3 = _mm256_setzero_ps();

        for (auto j = n; j < numParticles; j+=stride)
        {
            const __m256 xj_v = _mm256_loadu_ps(pos + j);
            const __m256 yj_v = _mm256_loadu_ps(pos + numParticles + j);
            const __m256 zj_v = _mm256_loadu_ps(pos + 2*numParticles + j);
            const __m256 mjG = _mm256_mul_ps(G_v, _mm256_loadu_ps(mass + j));
            const __m256 rx = _mm256_sub_ps(xi_v, xj_v);
            const __m256 ry = _mm256_sub_ps(yi_v, yj_v);
            const __m256 rz = _mm256_sub_ps(zi_v, zj_v);
            const __m256 r2 = _mm256_fmadd_ps(
                rx, 
                rx,
                _mm256_add_ps(
                    _mm256_mul_ps(ry, ry), 
                    _mm256_mul_ps(rz, rz)
                )
            );
            __m256 s = _mm256_rsqrt_ps(
                _mm256_add_ps(softeningFactor2, r2)
            );
            s = _mm256_mul_ps(_mm256_mul_ps(s, mjG), _mm256_mul_ps(s, s));
            a1 = _mm256_fnmadd_ps(s, rx, a1);
            a2 = _mm256_fnmadd_ps(s, ry, a2);
            a3 = _mm256_fnmadd_ps(s, rz, a3);
        }
        #endif

        for (int k = 0; k < MaxPackedFloats; k++)
        {
            acc[i] += a1[k];
            acc[i+numParticles] += a2[k];
            acc[i+2*numParticles] += a3[k];
        }
        #endif
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