#include <iostream>

#include <Windows.h>

#include "GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "world/world.h"
#include "world/scene.h"

#include "renderer/renderer.h"

int main()
{
    Renderer::Create(RenderBackend::D3D12).Init();
    Renderer& renderer = Renderer::Get();

    // Init GLFW and create a window (withou an API so we can use it as a generic window)
    if (glfwInit() != GLFW_TRUE)
    {
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "App", nullptr, nullptr);
    if (window == nullptr)
    {
        return 1;
    }

    // Let the renderer know we have a window
    renderer.OnWindowChange(glfwGetWin32Window(window), 1280, 720);

    World& world = World::Get();
    Scene* pMainScene = world.CreateScene();

    while (true)
    {
        glfwPollEvents();
        renderer.NewFrame();
        world.Update();
        renderer.Present();
    }

    glfwTerminate();

    return 0;
}