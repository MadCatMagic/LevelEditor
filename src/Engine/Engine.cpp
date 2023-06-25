#include "Engine.h"
#include "Engine/Input.h"
#include <iostream>

Engine::Engine()
{
}

void Engine::Mainloop(bool debugging)
{
    this->debug = debugging;

    if (window == nullptr)
    {
        std::cout << "[Fatal Error]: Create a window before calling MainLoop()!\n";
        return;
    }

    Renderer::Init(debug, window);

    Initialize();

    while (!glfwWindowShouldClose(window))
    {
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Input::InputUpdate();
        Update();
        Input::scrollDiff = 0.0f;

        Renderer::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    Release();

    Renderer::Release();

    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Engine::CreateWindow(const v2i& windowSize, const std::string& name)
{
    this->winSize = windowSize;
    winProportions = v2((float)winSize.x / (float)winSize.y, 1.0f);

    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(winSize.x, winSize.y, name.c_str(), NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "GLEW ERROR" << std::endl;
        return false;
    }

    return true;
}

void Engine::Initialize()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    Input::EnableInput(window, io);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // actual stuff
    level = new Level(v2i(4, 3));
    editor.Initialize(level, winSize);
}

void Engine::Update()
{
    // actual stuff first
    editor.Update();
    editor.Render(0);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    editor.RenderUI(io);

    // Rendering
    ImGui::Render();
}

void Engine::Release()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // actual stuff
    delete level;
}
