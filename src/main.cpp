#include <iostream>
#include <mpi.h>
#include <random>
#include <cstring>


#ifdef ENABLE_OPENGL

#include "render.h"

#endif


#include "nbody_system.h"




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


    NBodySystem nbodySystem(numParticles, isMaster, isWorker, workerComm);



    #ifdef ENABLE_OPENGL
        float fieldOfView = 0.25f * 3.141f;
        Camera camera(0.0f, 0.0f, 25.0f, fieldOfView, 0.1f, 100.0f, 10, 10);

        unsigned int vao, vbo, particleShader;

        if (isRenderer)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glBufferData(GL_ARRAY_BUFFER, 7 * numParticles * sizeof(float), nullptr, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * numParticles * sizeof(float), nbodySystem.pos);
            glBufferSubData(GL_ARRAY_BUFFER, 3 * numParticles * sizeof(float), 3 * numParticles * sizeof(float), nbodySystem.vel);
            glBufferSubData(GL_ARRAY_BUFFER, 6 * numParticles * sizeof(float), 1 * numParticles * sizeof(float), nbodySystem.mass);

            for (int i = 0; i < 7; i++)
            {
                glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(i * numParticles * sizeof(float)));
                glEnableVertexAttribArray(i);
            }
             

            particleShader = createShader(std::filesystem::path("src/shaders/particle.vert"), std::filesystem::path("src/shaders/particle.frag"));

            glUseProgram(particleShader);
            glUniformMatrix4fv(glGetUniformLocation(particleShader, "view"), 1, GL_FALSE, camera.getViewMatrix());
            glUniformMatrix4fv(glGetUniformLocation(particleShader, "projection"), 1, GL_FALSE, camera.getProjectionMatrix());
            glUniform1f(glGetUniformLocation(particleShader, "fovScale"), 1.0f / std::tan(0.5f * fieldOfView));

            #ifdef USE_GLES
                glEnable(GL_VERTEX_PROGRAM_POINTSIZE);
            #else
                glEnable(GL_PROGRAM_POINT_SIZE);
            #endif
            glEnable(GL_DEPTH_TEST);  
        }
    #endif
    
    


    bool shouldRun = true;



    // The timestep is the real-world time elapsed since start of last frame
    // on the master process. But there is a hard limit on the maximum timestep size.
    // If it  takes longer than this to compute frame,
    // the time will instead update at a sub-realtime speed,
    // to prevent the timestep being to large.
    constexpr float maximumTimeStep = 1.0f / 60;

    #ifdef ENABLE_OPENGL
    float time = glfwGetTime();
    char windowTitle[64];
    // keeps track of frames times
    // to average over for an fps calculation.
    constexpr int numFrameTimes = 32;
    float frameTimes[numFrameTimes];
    for (int i = 0; i < numFrameTimes; i++)
    {
        frameTimes[i] = maximumTimeStep;
    }
    #endif


    while (shouldRun)
    {
        // The real world time since last frame. (is not same as simulation timestep).
        float frameTime;
        // the time step used in simulation.
        float dt;
        #ifdef ENABLE_OPENGL
        if (isMaster)
        {
            auto time_old = time;
            time = glfwGetTime();
            frameTime = time - time_old;
            dt = std::min(frameTime, maximumTimeStep);

            float last = frameTime;
            float sum = last;
            for (int i = 0; i < numFrameTimes; i++)
            {
                sum += frameTimes[i];
                auto q = frameTimes[i];
                frameTimes[i] = last;
                last = q;
            }

            float averageFrameTime = sum / (numFrameTimes + 1);
            sprintf(windowTitle, "N Body Simulation  -  fps: %d", static_cast<int>(round(1 / averageFrameTime)));
            glfwSetWindowTitle(window, windowTitle);
        }
        #endif
        MPI_Bcast(&dt, 1, MPI_FLOAT, masterRank, MPI_COMM_WORLD);

        //========== Input ==========//

        #ifdef ENABLE_OPENGL

        if (isRenderer && isMaster)
        {
            shouldRun = glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS;
        }

        #endif

        MPI_Request shouldRunRequest;
        MPI_Ibcast(&shouldRun, 1, MPI_CXX_BOOL, masterRank, MPI_COMM_WORLD, &shouldRunRequest);

        
        //========== Physics ==========//

        nbodySystem.update(dt);


        if constexpr (masterIsWorker)
        {
            // If only the master process can render, there is not need to broadcast to itself.
            if (isRenderer && !onlyMasterRenders)
            {
                MPI_Bcast(nbodySystem.pos, numParticles * 3, MPI_FLOAT, masterRank, rendererComm);
                MPI_Bcast(nbodySystem.vel, numParticles * 3, MPI_FLOAT, masterRank, rendererComm);
            }
        }
        else
        {
            // Currently this else block won't run because master must be renderer at the moment.
            // Will later lift this restriction.
        }




        //========== Rendering ==========//

        #ifdef ENABLE_OPENGL

        if (isRenderer)
        {
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * numParticles * sizeof(float), nbodySystem.pos);
            glBufferSubData(GL_ARRAY_BUFFER, 3 * numParticles * sizeof(float), 3 * numParticles * sizeof(float), nbodySystem.vel);
            
            glClearColor(0.05f, 0.06f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(particleShader);
            
            glUniformMatrix4fv(glGetUniformLocation(particleShader, "view"), 1, GL_FALSE, camera.getViewMatrix());
            glUniformMatrix4fv(glGetUniformLocation(particleShader, "projection"), 1, GL_FALSE, camera.getProjectionMatrix());

            glDrawArrays(GL_POINTS, 0, numParticles);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        
        #endif

        MPI_Wait(&shouldRunRequest, MPI_STATUS_IGNORE);

        #ifdef ENABLE_OPENGL
        if (isRenderer && !shouldRun)
        {
            glfwSetWindowShouldClose(window, true);
        }
        #endif
    }


    nbodySystem.mpiFree();

    #ifdef ENABLE_OPENGL
        if (isRenderer)
        {
            glDeleteProgram(particleShader);
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            
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