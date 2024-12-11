#include "d3d12_renderer.h"

#include <iostream>
#include <string.h>

using namespace Microsoft::WRL;

D3D12Renderer::D3D12Renderer()
{
}

D3D12Renderer::~D3D12Renderer()
{
}

// Warning for wstr to str conv. This needs to be accounted for properly
#pragma warning(push)
#pragma warning(disable : 4244)
bool D3D12Renderer::Init()
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