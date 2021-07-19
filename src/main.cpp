#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Samples/Application.h"
#include "Samples/SampleSelector.h"

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

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

// CPU Frame Timers
struct FrameTimer {
    // High level timers
    double frameTime = 0.0;
    float  deltaTime = 0.0f;
    // CPU timers
    double frameUpdate = 0.0;
    double frameRender = 0.0;
    double win32Events = 0.0;
    double imguiLogic = 0.0;
    double imguiRender = 0.0;
    double swapBuffer = 0.0;
    // GPU timers
    double appGPU = 0.0;
};

//Application* gApplication = 0;
SampleSelector gSampleSelector;
GLuint gVertexArrayObject = 0;
GLuint gGpuApplicationStart = 0;
GLuint gGpuApplicationStop = 0;
struct nk_context* gNkContext = 0;
float gScaleFactor = 1.0f;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main() {
    int clientWidth = 1280;
    int clientHeight = 720;

    //
    glfwSetErrorCallback(error_callback);
    glfwInit();
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(clientWidth, clientHeight, "Game Animation Programming", nullptr, nullptr);

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

    // Enable Vsync
    glfwSwapInterval(1);
    int vsynch = 1;
    std::cout << "Enabled vsynch" << std::endl;

    // GPU Timers
    bool slowFrame = false;
    bool firstRenderSample = true;
    GLint timerResultAvailable = 0;
    GLuint64 gpuStartTime = 0;
    GLuint64 gpuStopTime = 0;

    glGenQueries(1, &gGpuApplicationStart);
    glGenQueries(1, &gGpuApplicationStop);

    // Setup some OpenGL required state
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    // CPU timings
    LARGE_INTEGER timerFrequency;
    LARGE_INTEGER timerStart;
    LARGE_INTEGER timerStop;
    LARGE_INTEGER frameStart;
    LARGE_INTEGER frameStop;
    LONGLONG timerDiff;

    FrameTimer display;
    FrameTimer accumulator;
    memset(&display, 0, sizeof(FrameTimer));
    memset(&accumulator, 0, sizeof(FrameTimer));
    int frameCounter = 0;

    bool enableFrameTiming = true;
    if (!QueryPerformanceFrequency(&timerFrequency)) {
        std::cout << "WinMain: QueryPerformanceFrequency failed\n";
        enableFrameTiming = false;
    }

    // Samples
    gSampleSelector.Initialize();

#pragma region nuklearInit
    gNkContext = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
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

#pragma endregion nuklearInit

    // Get Display Frequency
    int displayFrequency = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
    double frameBudget = (1000.0 / (double)displayFrequency);
    std::cout << "Display frequency: " << displayFrequency << "\n";
    std::cout << "Frame budget: " << frameBudget << " milliseconds\n";

    // Display helpers
    nk_color defaultColor = gNkContext->style.text.color;
    nk_color red = { 255, 0, 0, 255 };
    nk_color orange = { 255, 165, 0, 255 };
    char printBuffer[512];

    // Rendering Loop
    double lastTick = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        // Win32 events
        QueryPerformanceCounter(&timerStart);

        /* Input */
        glfwPollEvents();
        nk_glfw3_new_frame();

        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.win32Events += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        QueryPerformanceCounter(&frameStart);

        Application* application = gSampleSelector.GetCurrentSample();

        // update
        double thisTick = glfwGetTime();
        float deltaTime = float(thisTick - lastTick);
        lastTick = thisTick;
        accumulator.deltaTime += deltaTime;
        if (gSampleSelector.IsRunning() && application != 0) {
            application->Update(deltaTime);
        }
        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.frameUpdate += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        // Render
        QueryPerformanceCounter(&timerStart);
        glfwGetWindowSize(window, &clientWidth, &clientHeight);
        glViewport(0, 0, clientWidth, clientHeight);
        float aspect = (float)clientWidth / (float)clientHeight;
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glPointSize(5.0f * gScaleFactor);
        glLineWidth(1.5f * gScaleFactor);

        glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        if (gSampleSelector.IsRunning()) {
            glBindVertexArray(gVertexArrayObject);

            if (!firstRenderSample) { // Application GPU Timer
                glGetQueryObjectiv(gGpuApplicationStop, GL_QUERY_RESULT, &timerResultAvailable);
                while (!timerResultAvailable) {
                    std::cout << "Waiting on app GPU timer!\n";
                    glGetQueryObjectiv(gGpuApplicationStop, GL_QUERY_RESULT, &timerResultAvailable);
                }
                glGetQueryObjectui64v(gGpuApplicationStart, GL_QUERY_RESULT, &gpuStartTime);
                glGetQueryObjectui64v(gGpuApplicationStop, GL_QUERY_RESULT, &gpuStopTime);
                accumulator.appGPU += (double)(gpuStopTime - gpuStartTime) / 1000000.0;
            }

            glQueryCounter(gGpuApplicationStart, GL_TIMESTAMP);
            if (application != 0) {
                application->Render(aspect);
            }
            glQueryCounter(gGpuApplicationStop, GL_TIMESTAMP);
        }
        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.frameRender += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        // IMGUI Update
        QueryPerformanceCounter(&timerStart);
        if (gNkContext != 0 && gVertexArrayObject != 0) {
            float imguiXPosition = ((float)clientWidth) - 205.0f;
            if (nk_begin(gNkContext, "Display Stats", nk_rect(imguiXPosition, 5.0f, 200.0f, 65.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
                nk_layout_row_static(gNkContext, 15, 200, 1);

                sprintf(printBuffer, "Display frequency: %d\0", displayFrequency);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);

                if (vsynch != 0) {
                    nk_label(gNkContext, "VSynch: on", NK_TEXT_LEFT);
                }
                else {
                    nk_label(gNkContext, "VSynch: off", NK_TEXT_LEFT);
                }

                sprintf(printBuffer, "Frame budget: %0.2f ms\0", frameBudget);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);
            }
            nk_end(gNkContext);

            if (nk_begin(gNkContext, "High Level Timers", nk_rect(imguiXPosition, 75.0f, 200.0f, 45.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
                nk_layout_row_static(gNkContext, 15, 200, 1);

                if (slowFrame) {
                    gNkContext->style.text.color = red;
                }
                else {
                    gNkContext->style.text.color = defaultColor;
                }

                sprintf(printBuffer, "Frame Time: %0.5f ms\0", display.frameTime);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);

                sprintf(printBuffer, "Delta Time: %0.5f ms\0", display.deltaTime);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);

                if (slowFrame) {
                    gNkContext->style.text.color = defaultColor;
                }
            }
            nk_end(gNkContext);

            if (nk_begin(gNkContext, "GPU Timers", nk_rect(imguiXPosition, 125.0f, 200.0f, 30.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
                nk_layout_row_static(gNkContext, 15, 200, 1);

                sprintf(printBuffer, "Game GPU: %0.5f ms\0", display.appGPU);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);
            }
            nk_end(gNkContext);

            if (nk_begin(gNkContext, "CPU Timers", nk_rect(imguiXPosition, 160.0f, 200.0f, 120.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
                nk_layout_row_static(gNkContext, 15, 200, 1);

                sprintf(printBuffer, "Win32 Events: %0.5f ms\0", display.win32Events);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);

                sprintf(printBuffer, "Game Update: %0.5f ms\0", display.frameUpdate);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);

                sprintf(printBuffer, "Game Render: %0.5f ms\0", display.frameRender);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);

                sprintf(printBuffer, "IMGUI logic: %0.5f ms\0", display.imguiLogic);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);

                sprintf(printBuffer, "IMGUI render: %0.5f ms\0", display.imguiRender);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);

                sprintf(printBuffer, "Swap Buffers: %0.5f ms\0", display.swapBuffer);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);
            }
            nk_end(gNkContext);

            if (gSampleSelector.IsRunning()) {
                if (application != 0) {
                    application->ImGui(gNkContext);
                }
                gSampleSelector.ImGui(gNkContext, imguiXPosition);
            }
        }
        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.imguiLogic += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        // Imgui Render
        QueryPerformanceCounter(&timerStart);
        if (gNkContext != 0 && gVertexArrayObject != 0) {
            /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
             * with blending, scissor, face culling, depth test and viewport and
             * defaults everything back into a default state.
             * Make sure to either a.) save and restore or b.) reset your own state after
             * rendering the UI. */
//            nk_glfw3_render(NK_ANTI_ALIASING_ON);
            nk_glfw3_render(NK_ANTI_ALIASING_OFF);
        }
        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.imguiRender += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        // Wait for GPU
        QueryPerformanceCounter(&timerStart);
        if (gVertexArrayObject != 0) {
            glfwSwapBuffers(window);
            if (vsynch != 0) {
                glFinish();
            }
        }
        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.swapBuffer += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        QueryPerformanceCounter(&frameStop);
        timerDiff = frameStop.QuadPart - frameStart.QuadPart;
        double frameTime = (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;
        accumulator.frameTime += frameTime;

        // Profiling house keeping
        firstRenderSample = false;
        if (++frameCounter >= 60) {
            frameCounter = 0;

            display.win32Events = accumulator.win32Events / 60.0;
            display.frameUpdate = accumulator.frameUpdate / 60.0;
            display.frameRender = accumulator.frameRender / 60.0;
            display.imguiLogic = accumulator.imguiLogic / 60.0;
            display.imguiRender = accumulator.imguiRender / 60.0;
            display.swapBuffer = accumulator.swapBuffer / 60.0;
            display.frameTime = accumulator.frameTime / 60.0;
            display.deltaTime = accumulator.deltaTime / 60.0f;
            display.appGPU = accumulator.appGPU / 60.0;

            memset(&accumulator, 0, sizeof(FrameTimer));
            slowFrame = display.frameTime >= frameBudget;
        }
    }

    // Shutdown
    if (gSampleSelector.IsRunning()) {
        gSampleSelector.Shutdown();
    }
    else {
        std::cout << "Trying to close window after application has been shut down!\n";
    }
    if (gVertexArrayObject != 0) {
        glDeleteQueries(1, &gGpuApplicationStart);
        glDeleteQueries(1, &gGpuApplicationStop);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &gVertexArrayObject);
        gVertexArrayObject = 0;

        nk_glfw3_shutdown();

        glfwTerminate();
    }
    else {
        std::cout << "Got multiple destroy messages\n";
    }

    return EXIT_SUCCESS;
}
