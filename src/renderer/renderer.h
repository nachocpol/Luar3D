#ifndef RENDERER_H
#define RENDERER_H

#include "stdint.h"

enum class RenderBackend : uint8_t
{
	HEADLESS,
	D3D12
};

class Renderer
{
protected:
	Renderer() {}
	~Renderer() {}
	Renderer(const Renderer& other) = delete;

public:
	static Renderer& Create(RenderBackend backend);

	static Renderer& Get();

	virtual bool Init() = 0;
};

#endif