
#include <stdio.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>


// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

#define PURE_RED   1.0f,0.0f,0.0f,1.0f
#define PURE_GREEN 0.0f,1.0f,0.0f,1.0f
#define PURE_BLUE  0.0f,0.0f,1.0f,1.0f
#define PURE_WHITE 1.0f,1.0f,1.0f,1.0f
#define PURE_BLACK 0.0f,0.0f,0.0f,1.0f


GLuint hVAO, hVBO, hShader, hUniformModel;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;
constexpr float toRadian = glm::pi<float>() / 180.0f;

float curAngle = 0.0f;

// Vertex Shader defined as string
static const char* vShader = 
"                                                           \n\
#version 330                                                \n\
                                                            \n\
layout (location = 0) in vec3 pos;                          \n\
                                                            \n\
uniform mat4 model;                                         \n\
                                                            \n\
void main()                                                 \n\
{                                                           \n\
    float scale = 0.4f;                                                         \n\
    gl_Position = model * vec4(scale*pos.x, scale*pos.y, scale*pos.z, 1.0);     \n\
}                                                                               \n\
";

// Fragment shader
// The output will be interpreted as the color of the fragment
static const char* fShader =
"                                                           \n\
#version 330                                                \n\
out vec4 colour;                                            \n\
void main()                                                 \n\
{                                                           \n\
    colour = vec4(1.0,0.0,0.0,1.0);                         \n\
}                                                           \n\
";


void CreateTriangle()
{
    GLfloat vertices[] = {
        -1.0f,-1.0f, 0.0f,
         1.0f,-1.0f, 0.0f,
         0.0f, 1.0f, 0.0f
    };

    // Vertex array object created on the graphics card
    glGenVertexArrays(1, &hVAO);
    // Bind the VAO
    glBindVertexArray(hVAO);
    {
        // Create 1 vertex buffer object
        glGenBuffers(1, &hVBO);
        glBindBuffer(GL_ARRAY_BUFFER, hVBO);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Static draw means-> data is not changes (alt. GL_DYNAMIC_DARW)

            // Definition of the attribute pointer
            // 1.arg: layout location, 2.arg: size, 5. Sride: e.g. every nth entry in vertices, 6. offset
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            // Enable layout location 0
            glEnableVertexAttribArray(0);
        }
    }
    // Unbind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
    // Create empty shader
    GLuint theShader = glCreateShader(shaderType);

    // Set the shader source code
    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);
    glShaderSource(theShader, 1, theCode, codeLength);

    // Compile the shader
    glCompileShader(theShader);

    // Check compile errors
    GLint result = 0;
    GLchar eLog[1024] = { 0 };
    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        std::cout << "Error compiling the " << shaderType << " shader: " << eLog << std::endl;
        return;
    }

    // Attach shader to the shader program
    glAttachShader(theProgram, theShader);
}


void CompileShaders() 
{
    // Creating the program
    hShader = glCreateProgram();
    if (!hShader)
    {
        std::cout << "Error creating shader program!" << std::endl;
        return;
    }

    // Adding the shaders
    AddShader(hShader, vShader, GL_VERTEX_SHADER);
    AddShader(hShader, fShader, GL_FRAGMENT_SHADER);

    // Validate the shader
    GLint result = 0;
    GLchar eLog[1024] = { 0 };
    // Link program on the graphics card
    glLinkProgram(hShader);
    glGetProgramiv(hShader, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(hShader, sizeof(eLog), NULL, eLog);
        std::cout << "Error linking program: " << eLog << std::endl;
        return;
    }

    glValidateProgram(hShader);
    glGetProgramiv(hShader, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(hShader, sizeof(eLog), NULL, eLog);
        std::cout << "Error validating program: " << eLog << std::endl;
        return;
    }


    // Get a handle on the 
    hUniformModel = glGetUniformLocation(hShader, "model");
}



void DeleteShaders()
{
    glDeleteProgram(hShader);
}


int main()
{
    if (!glfwInit())
    {
        std::cout << "Initializing GLFW failed." << std::endl;
        glfwTerminate();
        return 1;
    }
    // Setup GLFW window properties
    // OpenGL version (use OpenGL 3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Use modern OpenGL: Core Profile = No backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow forward compatiblity
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a window
    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
    if (!mainWindow)
    {
        std::cout << "GLFW window creation failed." << std::endl;
        glfwTerminate();
        return 1;
    }

    // Get Buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use (the main window is the context)
    // All future commands will be applied to this context
    glfwMakeContextCurrent(mainWindow);

    // Allow modern extension features.
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW initialization failed." << std::endl;
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Setup Viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    CreateTriangle();
    CompileShaders();

    // Loop util window closed
    while (!glfwWindowShouldClose(mainWindow))
    {
        // Get and Handle user input events
        glfwPollEvents();

        if (direction)
        {
            triOffset += triIncrement;
        }
        else
        {
            triOffset -= triIncrement;
        }

        if (std::abs(triOffset) >= triMaxOffset)
        {
            direction = !direction;
        }

        curAngle += 0.01f;
        if (curAngle >= 360.0f)
        {
            curAngle -= 360.0f;
        }

        // Clear Window
        glClearColor(PURE_BLACK);
        glClear(GL_COLOR_BUFFER_BIT);

        // Choose the program
        glUseProgram(hShader);
        {
            glm::mat4 model{ 1.0f };
            model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
            model = glm::rotate(model, curAngle * toRadian, glm::vec3(0.0f, 0.0f, 1.0f));

            // Pass value to the uniform variable
            glUniformMatrix4fv(hUniformModel, 1, GL_FALSE, glm::value_ptr(model));

            // Choose the vertex array
            glBindVertexArray(hVAO);
            {
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
            glBindVertexArray(0);
        }
        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    DeleteShaders();

    return 0;
}

