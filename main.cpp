#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL4_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "demo/glfw_opengl4/nuklear_glfw_gl4.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

/* ===============================================================
 *
 *                          EXAMPLE
 *
 * ===============================================================*/
/* This are some code examples to provide a small overview of what can be
 * done with this library. To try out an example uncomment the defines */
#define INCLUDE_ALL
/*#define INCLUDE_STYLE */
/*#define INCLUDE_CALCULATOR */
//#define INCLUDE_OVERVIEW
/*#define INCLUDE_NODE_EDITOR */

#ifdef INCLUDE_ALL
#define INCLUDE_STYLE
#define INCLUDE_CALCULATOR
#define INCLUDE_OVERVIEW
#define INCLUDE_NODE_EDITOR
#endif

#ifdef INCLUDE_STYLE
#include "demo/style.c"
#endif
#ifdef INCLUDE_CALCULATOR
#include "demo/calculator.c"
#endif
#ifdef INCLUDE_OVERVIEW
#include "demo/overview.c"
#endif
#ifdef INCLUDE_NODE_EDITOR
#include "demo/node_editor.c"
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>


int mWidth = 1280;
int mHeight = 720;

int main() {
    std::cout << "Hello, World!" << std::endl;

    struct nk_context *ctx;
    struct nk_colorf bg;
    struct nk_image img;

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
    if (!gladLoadGL(glfwGetProcAddress) ) {
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


        vertexCode = R"(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 ourColor;

void main() {
    gl_Position = vec4(position, 1.0f);
    ourColor = color;
}
)";
        fragmentCode = R"(
#version 330 core

in vec3 ourColor;

out vec4 color;

void main() {
    color = vec4(ourColor, 1.0f);
}
)";


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

#pragma region nuklearInit
    ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&atlas);
        /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
        /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
        /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
        /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
        /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
        /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
        nk_glfw3_font_stash_end();
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        /*nk_style_set_font(ctx, &droid->handle);*/}

#ifdef INCLUDE_STYLE
    /*set_style(ctx, THEME_WHITE);*/
    /*set_style(ctx, THEME_RED);*/
    /*set_style(ctx, THEME_BLUE);*/
    /*set_style(ctx, THEME_DARK);*/
#endif

    /* Create bindless texture.
     * The index returned is not the opengl resource id.
     * IF you need the GL resource id use: nk_glfw3_get_tex_ogl_id() */
    {
        int tex_index = 0;
        enum {tex_width = 256, tex_height = 256};
        char pixels[tex_width * tex_height * 4];
        memset(pixels, 128, sizeof(pixels));
        tex_index = nk_glfw3_create_texture(pixels, tex_width, tex_height);
        img = nk_image_id(tex_index);
    }

    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
#pragma endregion nuklearInit

    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {
        /* Input */
        glfwPollEvents();
        nk_glfw3_new_frame();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

//        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
//        glClear(GL_COLOR_BUFFER_BIT);

#pragma region nuklearIMGUI
        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
                     NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                     NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

        /* Bindless Texture */
        if (nk_begin(ctx, "Texture", nk_rect(250, 150, 230, 250),
                     NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                     NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            struct nk_command_buffer *canvas = nk_window_get_canvas(ctx);
            struct nk_rect total_space = nk_window_get_content_region(ctx);
            nk_draw_image(canvas, total_space, &img, nk_white);
        }
        nk_end(ctx);

        /* -------------- EXAMPLES ---------------- */
#ifdef INCLUDE_CALCULATOR
        calculator(ctx);
#endif
#ifdef INCLUDE_OVERVIEW
        overview(ctx);
#endif
#ifdef INCLUDE_NODE_EDITOR
        node_editor(ctx);
#endif
        /* ----------------------------------------- */

        /* Draw */
        //glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);

#pragma endregion nuklearIMGUI

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

        /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_glfw3_render(NK_ANTI_ALIASING_ON);
        glfwSwapBuffers(window);
    }
    nk_glfw3_shutdown();
    glfwTerminate();
    return EXIT_SUCCESS;
}
