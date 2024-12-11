#include "renderer.h"
#include "d3d12_renderer.h"

static Renderer* k_pRendererInstance = nullptr;

Renderer& Renderer::Create(RenderBackend backend)
{
	// Already created. Should error?
	if (k_pRendererInstance)
	{
		return *k_pRendererInstance;
	}

	switch (backend)
	{
	case RenderBackend::HEADLESS:
		k_pRendererInstance = nullptr;
		break;
	case RenderBackend::D3D12:
		k_pRendererInstance = new D3D12Renderer;
		break;
	}

	return *k_pRendererInstance;
}

Renderer& Renderer::Get()
{
	// TODO: Need to handle un-initialized renderer
	return *k_pRendererInstance;
}