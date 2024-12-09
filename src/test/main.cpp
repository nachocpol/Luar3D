#include <iostream>

#include "wrl/client.h"

#include "d3d12.h"
#include "dxgi1_6.h"
#include "dxgidebug.h"

using namespace Microsoft::WRL;

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

    ComPtr<IDXGIFactory6> factory;
    if (CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)) != S_OK)
    {
        return false;
    }

    uint8_t gpuIndex = 0;
    ComPtr<IDXGIAdapter1> gpu;
    while (!gpu)
    {
        factory->EnumAdapterByGpuPreference(gpuIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&gpu));

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

    

    return true;
}
#pragma warning(pop)

int main()
{
    InitRenderer();

    return 0;
}