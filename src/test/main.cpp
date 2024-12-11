#include <iostream>

#include "GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "world/world.h"
#include "world/scene.h"

#include "renderer/renderer.h"

int main()
{
    Renderer::Create(RenderBackend::D3D12).Init();

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

    HWND rawWindow = glfwGetWin32Window(window);

    World& world = World::Get();
    Scene* pMainScene = world.CreateScene();

    while (true)
    {
        glfwPollEvents();

        world.Update();
    }

    glfwTerminate();

    return 0;
}