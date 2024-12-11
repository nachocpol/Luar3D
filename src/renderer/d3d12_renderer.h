#ifndef D3D12_RENDERER
#define D3D12_RENDERER

#include "renderer.h"

#include "wrl/client.h"

#include "d3d12.h"
#include "dxgi1_6.h"
#include "dxgidebug.h"

static const uint8_t s_BackBufferCount = 2;

using namespace Microsoft::WRL;

struct WindowResources
{
	ComPtr<IDXGISwapChain3> m_SwapChain;
	ComPtr<ID3D12Resource> m_BackBuffers[s_BackBufferCount];
	D3D12_CPU_DESCRIPTOR_HANDLE m_BackBufferView[s_BackBufferCount];

};

class D3D12Renderer : Renderer
{
private:
	friend Renderer;
	D3D12Renderer();
	~D3D12Renderer();
	D3D12Renderer(const D3D12Renderer& other) = delete;

public:
	bool Init();
	void OnWindowChange(void* pPlatformWindow, uint32_t width, uint32_t height);
	void NewFrame();
	void Present();

private:
	ComPtr<IDXGIFactory6> m_Factory;
	ComPtr<IDXGIAdapter4> m_GPU;
	ComPtr<ID3D12Device8> m_Device;

	ComPtr<ID3D12CommandQueue> m_Queue;

	ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
	ComPtr<ID3D12DescriptorHeap> m_RTVHeap;

	ComPtr<ID3D12CommandAllocator> m_CommandAllocators[s_BackBufferCount];
	ComPtr<ID3D12GraphicsCommandList6> m_CommandList;

	WindowResources m_WindowResources;

	uint8_t m_CurrentBackBufferIndex;
};

#endif