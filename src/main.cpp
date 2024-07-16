#include <iostream>
#include <mpi.h>



constexpr int masterRank = 0;


int main(int argc, char** argv)
{
    int ierror, rank, clusterSize;

    ierror = MPI_Init(&argc, &argv);

    if (ierror != MPI_SUCCESS)
    {
        std::cerr << "Failed to initialise MPI\n";
        return 1;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &clusterSize);




    const bool isMaster = rank == masterRank;




    MPI_Finalize();

    return 0;
}