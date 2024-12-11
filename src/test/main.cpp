#include <iostream>

#include "wrl/client.h"

#include "d3d12.h"
#include "dxgi1_6.h"
#include "dxgidebug.h"

#include "GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "world/world.h"
#include "world/scene.h"

using namespace Microsoft::WRL;

static const uint8_t s_BackBufferCount = 2;

ComPtr<IDXGIFactory6> m_Factory;
ComPtr<IDXGIAdapter4> m_GPU;
ComPtr<ID3D12Device8> m_Device;

ComPtr<ID3D12CommandQueue> m_Queue;

ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
ComPtr<ID3D12DescriptorHeap> m_RTVHeap;

ComPtr<ID3D12CommandAllocator> m_CommandAllocators[s_BackBufferCount];
ComPtr<ID3D12GraphicsCommandList6> m_CommandList;

// Warning for wstr to str conv. This needs to be accounted for properly
#pragma warning(push)
#pragma warning(disable : 4244)
bool InitRenderer()
{
    uint8_t factoryFlags = 0;

#if defined(DEBUG)
    ComPtr<ID3D12Debug> debugController;
    if (D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)) != S_OK)
    {
        return false;
    }
    debugController->EnableDebugLayer();

    ComPtr<IDXGIInfoQueue> infoQueue;
    if (DXGIGetDebugInterface1(0, IID_PPV_ARGS(&infoQueue)) != S_OK)
    {
        return false;
    }
    infoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
    infoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);

    factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    if (CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_Factory)) != S_OK)
    {
        return false;
    }

    uint8_t gpuIndex = 0;
    ComPtr<IDXGIAdapter4> gpu;
    while (!gpu)
    {
        m_Factory->EnumAdapterByGpuPreference(gpuIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&gpu));

        DXGI_ADAPTER_DESC1 desc = {};
        gpu->GetDesc1(&desc);

        // Ignore software emulated GPU
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            gpu = nullptr;
        }
        else
        {
            std::wstring wideDesc(desc.Description);
            std::string descStr(wideDesc.begin(), wideDesc.end());
            printf_s("GPU: %s \n", descStr.c_str());
            break;
        }
        ++gpuIndex;
    }

    if (!gpu)
    {
        return false;
    }

    // We have an adecuate GPU adapter, let's create the device
    m_GPU.Attach(gpu.Detach());
    if (D3D12CreateDevice(m_GPU.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device)) != S_OK)
    {
        return false;
    }
    
    // TODO: debug device?

    // Now, create core objects needed for rendering, queue, cmd allocators, cmd lists, heaps, swap chain and render targets
    
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    if (m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_Queue)) != S_OK)
    {
        return false;
    }

    D3D12_DESCRIPTOR_HEAP_DESC rtHeapDesc = {};
    rtHeapDesc.NumDescriptors = 64;
    rtHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    if (m_Device->CreateDescriptorHeap(&rtHeapDesc, IID_PPV_ARGS(&m_RTVHeap)) != S_OK)
    {
        return false;
    }

    D3D12_DESCRIPTOR_HEAP_DESC dsHeapDesc = {};
    dsHeapDesc.NumDescriptors = 64;
    dsHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    if (m_Device->CreateDescriptorHeap(&dsHeapDesc, IID_PPV_ARGS(&m_DSVHeap)) != S_OK)
    {
        return false;
    }

    for (uint8_t i = 0; i < s_BackBufferCount; ++i)
    {
        if (m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocators[i])) != S_OK)
        {
            return false;
        }
    }

    if (m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_CommandList)) != S_OK)
    {
        return false;
    }
    m_CommandList->Close();

    
    // TODO: Fence    

    return true;
}
#pragma warning(pop)

// Method used to assign a window that we want to render to. We should call this anytime we change window size.
// Resources tied to window size will be recreated.
bool AttatchWindow(const HWND& window, uint32_t width, uint32_t height)
{
    return true;
}

int main()
{
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


    InitRenderer();

    glfwTerminate();

    return 0;
}