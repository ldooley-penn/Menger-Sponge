#include "Application.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include "Utils/ShaderLoader.h"

#include <glm/gtc/type_ptr.hpp>

Application::Application(glm::ivec2 windowSize):
    m_window(nullptr),
    m_mengerSpongeProgram(0),
    m_vao(0),
    m_vbo(0),
    m_mousePosition(glm::dvec2(0, 0)),
    m_windowSize(windowSize),
    m_camera(std::make_unique<Camera>(glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f), m_windowSize.x, m_windowSize.y)),
    m_mengerSponge(nullptr)
{

}

Application::~Application()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Application::Run()
{
    if (!Initialize())
    {
        return;
    }

    double previousTime = glfwGetTime();
    while (!glfwWindowShouldClose(m_window))
    {
        const double currentTime = glfwGetTime();
        Tick(currentTime - previousTime);
        previousTime = currentTime;
    }
}

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->UpdateKeyState(key, action);
}

void Application::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->UpdateMouseButtonState(button, action);
}

void Application::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->UpdateCursorPosition(xpos, ypos);
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->UpdateFramebufferSize(width, height);
}

void Application::UpdateKeyState(const int key, const int action)
{
    if (action == GLFW_PRESS)
    {
        m_keysPressed.insert(key);
    }
    else if (action == GLFW_RELEASE)
    {
        m_keysPressed.erase(key);
    }
}

void Application::UpdateMouseButtonState(int button, int action)
{
    if (action == GLFW_PRESS)
    {
        m_mouseButtonsPressed.insert(button);
    }
    else if (action == GLFW_RELEASE)
    {
        m_mouseButtonsPressed.erase(button);
    }
}

void Application::UpdateCursorPosition(const double xpos, const double ypos)
{
    if (m_mouseButtonsPressed.count(GLFW_MOUSE_BUTTON_RIGHT) > 0)
    {
        const glm::dvec2 mouseDelta = glm::dvec2(xpos, ypos) - m_mousePosition;
        m_camera->Rotate(mouseDelta.x, mouseDelta.y);
    }
    m_mousePosition = glm::dvec2(xpos, ypos);
}

void Application::UpdateFramebufferSize(const int width, const int height)
{
    m_windowSize = glm::ivec2(width, height);

    m_camera->Resize(m_windowSize.x, m_windowSize.y);

    glViewport(0, 0, m_windowSize.x, m_windowSize.y);
}

bool Application::Initialize()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!\n";
        glfwTerminate();
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    m_window = glfwCreateWindow(m_windowSize.x, m_windowSize.y, "Menger-Sponge", nullptr, nullptr);
    if (!m_window)
    {
        std::cerr << "Failed to open GLFW window!\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    glfwSetWindowUserPointer(m_window, this);

    glfwSetKeyCallback(m_window, key_callback);

    glfwSetMouseButtonCallback(m_window, mouse_button_callback);

    glfwSetCursorPosCallback(m_window, cursor_position_callback);

    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    gladLoadGL();

    // Set up imgui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    glfwSwapInterval(1);

    glClearColor(1.0f, 0.5f, 0.25f, 1.0f);

    glViewport(0, 0, m_windowSize.x, m_windowSize.y);

    m_mengerSpongeProgram = ShaderLoader::createShaderProgram("src/Shaders/MengerSponge.vert", "src/Shaders/MengerSponge.frag");

    glEnable(GL_DEPTH_TEST);

    // Generate menger sponge
    m_mengerSponge = std::make_unique<MengerSponge>(1.f, m_layerCount);

    return true;
}

void Application::Tick(double deltaTime)
{
    m_mengerSponge->TryRegenerate();

    m_camera->Move(m_keysPressed, deltaTime);

    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_mengerSpongeProgram);

    GLint viewMatrixUniformLocation = glGetUniformLocation(m_mengerSpongeProgram, "ViewMatrix");
    GLint projectionMatrixUniformLocation = glGetUniformLocation(m_mengerSpongeProgram, "ProjectionMatrix");

    glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(m_camera->GetViewMatrix()));
    glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjectionMatrix()));

    m_mengerSponge->Draw();

    // Draw imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Settings");
    ImGui::SliderInt("Layers: ", &m_layerCount, 0, 5);
    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}