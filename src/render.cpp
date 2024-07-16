#include "render.h"

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

GLFWwindow* glInit(bool& success)
{
    glfwInit();
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFW_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFW_VERSION_MINOR);

    int width = 1200;
    int height = 1200;
    GLFWwindow* window = glfwCreateWindow(width, height, "N Body Simulation", NULL, NULL);
    
    if (window == nullptr)
    {
        glfwTerminate();
        success = false;
        std::cerr << "Failed to create GLFW window." << std::endl;
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        success = false;
        std::cerr << "GLAD load failed." << std::endl;
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    success = true;
    return window;
}

unsigned int createShader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath)
{
    std::string vertSource;
    std::string fragSource;
    std::ifstream vertFile;
    std::ifstream fragFile;

    vertFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fragFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    std::string ver_string = std::to_string(100 * GLFW_VERSION_MAJOR + 10 * GLFW_VERSION_MINOR);
    #ifdef USE_GLES
    std::string prof_string = " es\n"
    std::string prec_line = "#ifdef GL_ES\nprecision highp float;\n#endif\n";
    #else
    std::string prof_string = " core\n";
    #endif

    try
    {
        vertFile.open(vertPath);
        std::stringstream vertSS;
        vertSS << vertFile.rdbuf();
        vertSource = std::string("#version ") + ver_string + prof_string + vertSS.str();
        vertFile.close();
        fragFile.open(fragPath);
        std::stringstream fragSS;
        fragSS << fragFile.rdbuf();
        #ifdef USE_GLES
        fragSource = std::string("#version ") + ver_string + prof_string + prec_line + fragSS.str();
        #else
        fragSource = std::string("#version ") + ver_string + prof_string + fragSS.str();
        #endif
        fragFile.close();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << "\tFailed to read shader files"<< '\n';
        return 0;
    }


    unsigned int vert, frag;
    int success;
    char infoLog[512];

    const char* vertSource_cstr = vertSource.c_str();
    const char* fragSource_cstr = fragSource.c_str();

    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertSource_cstr, nullptr);
    glCompileShader(vert);
    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vert, 512, nullptr, infoLog);
        std::cerr << "failed to compile vertex shader\n" << infoLog << std::endl;
    }

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragSource_cstr, nullptr);
    glCompileShader(frag);
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(frag, 512, nullptr, infoLog);
        std::cerr << "failed to compile fragment shader\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);
    glDeleteShader(vert);
    glDeleteShader(frag);

    return shaderProgram;
}