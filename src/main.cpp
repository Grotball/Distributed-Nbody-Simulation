#include <iostream>
#include <mpi.h>
#include <random>


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

constexpr int numParticles = 64;


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





    std::vector<int> loPartitionIndices;
    std::vector<int> hiPartitionIndices;
    std::vector<int> partitionSizes;
    
    int partitionSizeLimit = (int)std::ceil(numParticles * 1.0 / workerSize);
    for (int i = 0; i < workerSize; i++)
    {
        loPartitionIndices.push_back(i * partitionSizeLimit);
        hiPartitionIndices.push_back(std::min((i+1) * partitionSizeLimit, numParticles));
        partitionSizes.push_back(hiPartitionIndices.back() - loPartitionIndices.back());
    }


    // Vector quantities are stored in a linearly index array. The x, y, z components are NOT
    // interleaved. All the x-components contiguous, followed contiguously by all the y-components.

    float* pos = new float[3 * numParticles];
    float* vel = new float[3 * numParticles];
    float* acc = new float[3 * numParticles];
    float* mass = new float[numParticles];

    MPI_Datatype vecArrayType, vecArrayTypeNonResized;
    MPI_Type_vector(3, 1, numParticles, MPI_FLOAT, &vecArrayTypeNonResized);
    MPI_Type_commit(&vecArrayTypeNonResized);
    MPI_Type_create_resized(vecArrayTypeNonResized, 0, 1 * sizeof(float), &vecArrayType);
    MPI_Type_commit(&vecArrayType);
    MPI_Type_free(&vecArrayTypeNonResized);

    if (isMaster)
    {
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        for (int i = 0; i < numParticles; i++)
        {
            for (int k = 0; k < 3; k++)
            {
                pos[i+k*numParticles] = 10 * dist(rng);
                vel[i+k*numParticles] = 0;

            }
            mass[i] = 100.0f / numParticles;
        }
    }

    MPI_Bcast(pos, numParticles * 3, MPI_FLOAT, masterRank, MPI_COMM_WORLD);
    MPI_Bcast(vel, numParticles * 3, MPI_FLOAT, masterRank, MPI_COMM_WORLD);
    MPI_Bcast(mass, numParticles, MPI_FLOAT, masterRank, MPI_COMM_WORLD);



    #ifdef ENABLE_OPENGL
        float projectionMatrix[16];
        float viewMatrix[16];
        // Set to identity.
        for (int i = 0, k = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++, k++)
            {
                viewMatrix[k] = i == j ? 1 : 0;
            }
        }
        computeProjectionMatrix(projectionMatrix, 3.141f/2, 1, 0.1f, 100);

        unsigned int vao, vbo, particleShader;

        if (isRenderer)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glBufferData(GL_ARRAY_BUFFER, 7 * numParticles * sizeof(float), nullptr, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * numParticles * sizeof(float), pos);
            glBufferSubData(GL_ARRAY_BUFFER, 3 * numParticles * sizeof(float), 3 * numParticles * sizeof(float), vel);
            glBufferSubData(GL_ARRAY_BUFFER, 6 * numParticles * sizeof(float), 1 * numParticles * sizeof(float), mass);

            for (int i = 0; i < 7; i++)
            {
                glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(i * numParticles * sizeof(float)));
                glEnableVertexAttribArray(i);
            }
             

            particleShader = createShader(std::filesystem::path("src/shaders/particle.vert"), std::filesystem::path("src/shaders/particle.frag"));

            glUseProgram(particleShader);
            glUniformMatrix4fv(glGetUniformLocation(particleShader, "view"), 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(glGetUniformLocation(particleShader, "projection"), 1, GL_FALSE, projectionMatrix);

            #ifdef USE_GLES
                glEnable(GL_VERTEX_PROGRAM_POINTSIZE);
            #else
                glEnable(GL_PROGRAM_POINT_SIZE);
            #endif
            glEnable(GL_DEPTH_TEST);  
        }
    #endif
    
    


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
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * numParticles * sizeof(float), pos);
            glBufferSubData(GL_ARRAY_BUFFER, 3 * numParticles * sizeof(float), 3 * numParticles * sizeof(float), vel);
            
            glClearColor(0.05f, 0.06f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(particleShader);
            glUniformMatrix4fv(glGetUniformLocation(particleShader, "view"), 1, GL_FALSE, viewMatrix);

            glDrawArrays(GL_POINTS, 0, numParticles);

            glfwSwapBuffers(window);
            glfwPollEvents();

            if (!shouldRun)
            {
                glfwSetWindowShouldClose(window, true);
            }
        }
        
        #endif

    }







    delete[] pos;
    delete[] vel;
    delete[] acc;
    delete[] mass;



    #ifdef ENABLE_OPENGL
        if (isRenderer)
        {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    #endif

    MPI_Type_free(&vecArrayType);

    MPI_Comm_free(&workerComm);
    MPI_Comm_free(&rendererComm);
    MPI_Comm_free(&nodeComm);

    MPI_Finalize();

    return 0;
}