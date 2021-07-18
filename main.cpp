#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

int mWidth = 1280;
int mHeight = 720;

int main() {
    std::cout << "Hello, World!" << std::endl;

    //
    glfwInit();
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(mWidth, mHeight, "oglBoilerplate", nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    // Load OpenGL Functions
    if (!gladLoadGL() ) {
        std::cout << "Could not initialize GLAD\n";
        return EXIT_FAILURE;
    }
    else {
        std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
    }


    // Viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // shader
#pragma region shader
    GLuint Program;
    {
        std::string vertexCode;
        std::string fragmentCode;
        if (false)
        {
            // 1. Retrieve the vertex/fragment source code from filePath

            std::ifstream vShaderFile;
            std::ifstream fShaderFile;
            // ensures ifstream objects can throw exceptions:
            vShaderFile.exceptions (std::ifstream::badbit);
            fShaderFile.exceptions (std::ifstream::badbit);
            try
            {
                // Open files
                vShaderFile.open("shaders/vert.vert");
                fShaderFile.open("shaders/frag.frag");
                std::stringstream vShaderStream, fShaderStream;
                // Read file's buffer contents into streams
                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();
                // close file handlers
                vShaderFile.close();
                fShaderFile.close();
                // Convert stream into string
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();
            }
            catch (std::ifstream::failure e)
            {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
                return EXIT_FAILURE;
            }
            const GLchar* vShaderCode = vertexCode.c_str();
            const GLchar * fShaderCode = fragmentCode.c_str();
        }


        vertexCode = "#version 330 core\n"
                     "\n"
                     "layout (location = 0) in vec3 position;\n"
                     "layout (location = 1) in vec3 color;\n"
                     "\n"
                     "out vec3 ourColor;\n"
                     "\n"
                     "void main() {\n"
                     "    gl_Position = vec4(position, 1.0f);\n"
                     "    ourColor = color;\n"
                     "}";
        fragmentCode = "#version 330 core\n"
                       "\n"
                       "in vec3 ourColor;\n"
                       "\n"
                       "out vec4 color;\n"
                       "\n"
                       "void main() {\n"
                       "    color = vec4(ourColor, 1.0f);\n"
                       "}";


        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar * fShaderCode = fragmentCode.c_str();
        // 2. Compile shaders
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];
        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
//        std::cout << vShaderCode << std::endl;
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // Print compile errors if any
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            return EXIT_FAILURE;
        }
        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
//        std::cout << fShaderCode << std::endl;
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        // Print compile errors if any
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            return EXIT_FAILURE;
        }
        // Shader Program
        Program = glCreateProgram();
        glAttachShader(Program, vertex);
        glAttachShader(Program, fragment);
        glLinkProgram(Program);
        // Print linking errors if any
        glGetProgramiv(Program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(Program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            return EXIT_FAILURE;
        }
        // Delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
#pragma endregion shader

    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

#pragma region maindraw
        GLfloat vertices[] = {
                // Positions         // Colors
                 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // Bottom Right
                -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // Bottom Left
                 0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // Top
        };

        glUseProgram(Program);

        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
#pragma endregion maindraw

        // Flip Buffers and Draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }   glfwTerminate();
    return EXIT_SUCCESS;
}
