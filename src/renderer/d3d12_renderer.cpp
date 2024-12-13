#include "d3d12_renderer.h"

#include "directx/d3dx12_barriers.h"

#include <iostream>
#include <string.h>

D3D12Renderer::D3D12Renderer()
    : m_CurrentBackBufferIndex(0)
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

        m_PresentFenceValues[i] = 0u;
        m_Device->CreateFence(0u, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_PresentFences[i]));
    }

    if (m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_CommandList)) != S_OK)
    {
        return false;
    }
    m_CommandList->Close();


    return true;
}
#pragma warning(pop)

void D3D12Renderer::OnWindowChange(void* pPlatformWindow, uint32_t width, uint32_t height)
{
    if (m_WindowResources.m_SwapChain)
    {
        // TODO: Resize swap chain
    }
    else
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = s_BackBufferCount;
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
        fullScreenDesc.Windowed = TRUE;

        ComPtr<IDXGISwapChain1> tempSwapChain;
        HRESULT res = m_Factory->CreateSwapChainForHwnd(
            m_Queue.Get(), (HWND)pPlatformWindow, &swapChainDesc, &fullScreenDesc,
            nullptr, tempSwapChain.GetAddressOf()
        );

        tempSwapChain.CopyTo(m_WindowResources.m_SwapChain.GetAddressOf());
        

        if (res != S_OK)
        {
            return;
        }

        //ThrowIfFailed(m_dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    D3D12_RENDER_TARGET_VIEW_DESC rtViewDesc = {};
    rtViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    for (int i = 0; i < s_BackBufferCount; ++i)
    {
        m_WindowResources.m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_WindowResources.m_BackBuffers[i].GetAddressOf()));

        // Ehmmm. Ok
        D3D12_CPU_DESCRIPTOR_HANDLE destHandle{};
        destHandle.ptr = m_RTVHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * i;

        m_Device->CreateRenderTargetView(m_WindowResources.m_BackBuffers[i].Get(), &rtViewDesc, destHandle);
        m_WindowResources.m_BackBufferView[i] = destHandle;
    }

    m_CurrentBackBufferIndex = m_WindowResources.m_SwapChain->GetCurrentBackBufferIndex();

    // Depth buffer...
}

void D3D12Renderer::NewFrame()
{
    while (m_PresentFences[m_CurrentBackBufferIndex]->GetCompletedValue() < m_PresentFenceValues[m_CurrentBackBufferIndex])
    {
        // Wait...
    }

    m_CommandAllocators[m_CurrentBackBufferIndex]->Reset();
    m_CommandList->Reset(m_CommandAllocators[m_CurrentBackBufferIndex].Get(), nullptr);

    // TODO: Works.. But not nice, need to track states
    if (m_PresentFences[m_CurrentBackBufferIndex] != 0)
    {
        m_CommandList->ResourceBarrier( 1, 
            &CD3DX12_RESOURCE_BARRIER::Transition(
                m_WindowResources.m_BackBuffers[m_CurrentBackBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
        ));
    }

    float clear[4] = { 1,0,1,1 };
    m_CommandList->ClearRenderTargetView(
        m_WindowResources.m_BackBufferView[m_CurrentBackBufferIndex], clear, 0, nullptr
    );
}

void D3D12Renderer::Present()
{
    m_CommandList->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            m_WindowResources.m_BackBuffers[m_CurrentBackBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT
    ));

    m_CommandList->Close();

    ID3D12CommandList* pCommandList[1] = { (ID3D12CommandList*)m_CommandList.Get() };
    m_Queue->ExecuteCommandLists(1, pCommandList);    

    ++m_PresentFenceValues[m_CurrentBackBufferIndex];
    m_Queue->Signal(m_PresentFences[m_CurrentBackBufferIndex].Get(), m_PresentFenceValues[m_CurrentBackBufferIndex]);

    m_WindowResources.m_SwapChain->Present(1, 0);

    m_CurrentBackBufferIndex = m_WindowResources.m_SwapChain->GetCurrentBackBufferIndex();
}