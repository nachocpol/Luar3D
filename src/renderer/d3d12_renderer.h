#ifndef D3D12_RENDERER
#define D3D12_RENDERER

#include "renderer.h"

#include "wrl/client.h"

#include "d3d12.h"
#include "dxgi1_6.h"
#include "dxgidebug.h"

static const uint8_t s_BackBufferCount = 2;

class D3D12Renderer : Renderer
{
private:
	friend Renderer;
	D3D12Renderer();
	~D3D12Renderer();
	D3D12Renderer(const D3D12Renderer& other) = delete;

public:
	bool Init();

private:
	Microsoft::WRL::ComPtr<IDXGIFactory6> m_Factory;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> m_GPU;
	Microsoft::WRL::ComPtr<ID3D12Device8> m_Device;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_Queue;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocators[s_BackBufferCount];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_CommandList;
};

#endif