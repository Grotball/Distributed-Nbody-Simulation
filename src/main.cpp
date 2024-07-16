#include <iostream>
#include <mpi.h>


#ifdef ENABLE_OPENGL

#include "render.h"

#endif


#include "nbody.h"




constexpr int masterRank = 0;


// Later will lift the restriction on values of the constants below.
constexpr bool onlyMasterRenders = true;
constexpr bool renderersDoWork = true;
constexpr bool masterCanDoWork = true;
constexpr bool masterIsWorker = masterCanDoWork && renderersDoWork;


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



    MPI_Comm nodeComm;

    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, rank, MPI_INFO_NULL, &nodeComm);

    int nodeRank, nodeSize;

    MPI_Comm_rank(nodeComm, &nodeRank);
    MPI_Comm_size(nodeComm, &nodeSize);


    

    bool isRenderer = false;


    #ifdef ENABLE_OPENGL

    GLFWwindow* window = nullptr;

    // Limits rendering to one process per node.
    // master currently must also render.
    if (isMaster || (nodeRank == 0 && !onlyMasterRenders))
    {
        window = glInit(isRenderer);
    }
    
    #endif


    bool isWorker = (!isMaster && (isRenderer || renderersDoWork)) || (isMaster && masterIsWorker);

    MPI_Comm workerComm, rendererComm;

    MPI_Comm_split(MPI_COMM_WORLD, (int)isWorker, rank, &workerComm);
    MPI_Comm_split(MPI_COMM_WORLD, (int)isRenderer, rank, &rendererComm);


    int workerRank, workerSize;
    MPI_Comm_rank(workerComm, &workerRank);
    MPI_Comm_size(workerComm, &workerSize);

    int rendererRank, rendererSize;
    MPI_Comm_rank(rendererComm, &rendererRank);
    MPI_Comm_size(rendererComm, &rendererSize);





    bool shouldRun = true;

    while (shouldRun)
    {
        //========== Input ==========//

        #ifdef ENABLE_OPENGL

        if (isRenderer && isMaster)
        {
            shouldRun = glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS;
        }

        #endif

        MPI_Bcast(&shouldRun, 1, MPI_CXX_BOOL, masterRank, MPI_COMM_WORLD);

        
        //========== Physics ==========//

        // empty...


        //========== Rendering ==========//

        #ifdef ENABLE_OPENGL

        if (isRenderer)
        {
            glClearColor(0.05f, 0.06f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwSwapBuffers(window);
            glfwPollEvents();

            if (!shouldRun)
            {
                glfwSetWindowShouldClose(window, true);
            }
        }
        
        #endif

    }



    #ifdef ENABLE_OPENGL
        if (isRenderer)
        {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    #endif


    MPI_Comm_free(&workerComm);
    MPI_Comm_free(&rendererComm);
    MPI_Comm_free(&nodeComm);

    MPI_Finalize();

    return 0;
}