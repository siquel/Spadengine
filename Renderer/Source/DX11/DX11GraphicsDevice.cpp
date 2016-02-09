#ifdef DIRECTX11

#include <Windows.h>
#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

#include <iostream>
#include "SDL/SDL_syswm.h"

#include "Core/Assert.h"
#include "Renderer/DX11/DX11Buffer.h"
#include "Renderer/DX11/DX11Pipeline.h"
#include "Renderer/DX11/DX11Shader.h"
#include "Renderer/GraphicsDevice.h"
#include "Renderer/VertexLayout.h"
#include "Renderer/Window.h"

namespace sge
{
	struct GraphicsDevice::Impl
	{
		Impl(Window& window) : 
			hdc(nullptr), 
			hwnd(nullptr),
			device(nullptr),
			context(nullptr),
			swapChain(nullptr),
			renderTargetView(nullptr)
		{
			SDL_SysWMinfo info;

			SDL_VERSION(&info.version);

			if (SDL_GetWindowWMInfo(window.getSDLWindow(), &info))
			{
				switch (info.subsystem)
				{
				case SDL_SYSWM_WINDOWS:
				{
					hwnd = info.info.win.window;
					hdc = info.info.win.hdc;
				}
				default: break;
				}
			}

			std::cout << "SDL version " << 
				static_cast<int>(info.version.major) << "." << 
				static_cast<int>(info.version.minor) << "." << 
				static_cast<int>(info.version.patch) << std::endl;

			ZeroMemory(&sd, sizeof(sd));

			sd.BufferCount = 1;
			sd.BufferDesc.Width = window.getWidth();
			sd.BufferDesc.Height = window.getHeight();
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.RefreshRate.Numerator = 60;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow = hwnd;
			sd.SampleDesc.Count = 4;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = true;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;
			UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

#ifdef _DEBUG
			flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

			HRESULT result = D3D11CreateDeviceAndSwapChain(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				D3D11_CREATE_DEVICE_DEBUG,
				&featureLevels,
				1,
				D3D11_SDK_VERSION,
				&sd,
				&swapChain,
				&device,
				&featureLevel,
				&context
				);

			SGE_ASSERT(result == S_OK);

			ID3D11Texture2D* backBuffer = nullptr;
			result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

			SGE_ASSERT(result == S_OK);

			device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
			context->OMSetRenderTargets(1, &renderTargetView, nullptr);
			backBuffer->Release();

			ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

			viewport.Width = static_cast<FLOAT>(window.getWidth());
			viewport.Height = static_cast<FLOAT>(window.getHeight());
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;

			context->RSSetViewports(1, &viewport);
		}

		~Impl()
		{
			swapChain->SetFullscreenState(false, nullptr);

			swapChain->Release();
			device->Release();
			context->Release();
			renderTargetView->Release();
		}

		HDC hdc;
		HWND hwnd;
		ID3D11Device* device;
		ID3D11DeviceContext* context;
		IDXGISwapChain* swapChain;
		ID3D11RenderTargetView* renderTargetView;
		D3D_FEATURE_LEVEL featureLevel;
		DXGI_SWAP_CHAIN_DESC sd;
		D3D11_VIEWPORT viewport;
	};

	GraphicsDevice::GraphicsDevice(Window& window) :
		impl(new Impl(window))
	{
	}

	GraphicsDevice::~GraphicsDevice()
	{
		delete impl;
	}

	void GraphicsDevice::init()
	{

	}

	void GraphicsDevice::deinit()
	{

	}

	void GraphicsDevice::swap()
	{
		impl->swapChain->Present(0, 0);
	}

	void GraphicsDevice::clear(float r, float g, float b, float a)
	{
		float clear[] = { r, g, b, a };
		impl->context->ClearRenderTargetView(impl->renderTargetView, clear);
	}

	Buffer* GraphicsDevice::createBuffer(BufferType type, BufferUsage usage, size_t size)
	{
		DX11Buffer* dx11Buffer = new DX11Buffer();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		switch (usage)
		{
		case BufferUsage::DYNAMIC:
			bd.Usage = D3D11_USAGE_DYNAMIC; 
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
			break;
		case BufferUsage::STATIC:
			// TODO Fix default usage.
			SGE_ASSERT(false);
			break;
		}

		switch (type)
		{
		case BufferType::VERTEX:
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; break;
		case BufferType::INDEX:
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER; break;
		case BufferType::UNIFORM:
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; break;
		}
		

		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.ByteWidth = size;

		impl->device->CreateBuffer(&bd, nullptr, &dx11Buffer->buffer);

		dx11Buffer->header.size = size;

		return &dx11Buffer->header;
	}

	void GraphicsDevice::deleteBuffer(Buffer* buffer)
	{
		DX11Buffer* dx11Buffer = reinterpret_cast<DX11Buffer*>(buffer);
		dx11Buffer->buffer->Release();
		delete dx11Buffer;
		buffer = nullptr;
	}

	Pipeline* GraphicsDevice::createPipeline(VertexLayoutDescription* vertexLayoutDescription, Shader* vertexShader, Shader* pixelShader)
	{
		DX11Pipeline* dx11Pipeline = new DX11Pipeline();

		dx11Pipeline->vertexShader = reinterpret_cast<DX11Shader*>(vertexShader);
		dx11Pipeline->pixelShader = reinterpret_cast<DX11Shader*>(pixelShader);

		D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[vertexLayoutDescription->count];

		size_t positionElements = 0;
		size_t normalElements = 0;
		size_t binormalElements = 0;
		size_t colorElements = 0;
		size_t tangentElements = 0;
		size_t texcoordElements = 0;
		size_t slot = 0;

		for (size_t i = 0; i < vertexLayoutDescription->count; i++)
		{
			switch (vertexLayoutDescription->elements[i].semantic)
			{
			case VertexSemantic::POSITION: 
				ied[i] = {
					"POSITION",
					positionElements++,
					DXGI_FORMAT_R32G32B32_FLOAT,
					slot++,
					D3D11_APPEND_ALIGNED_ELEMENT,
					D3D11_INPUT_PER_VERTEX_DATA, 0
				};
				break;
			case VertexSemantic::NORMAL:
				ied[i] = {
					"NORMAL",
					normalElements++,
					DXGI_FORMAT_R32G32B32_FLOAT,
					slot++,
					D3D11_APPEND_ALIGNED_ELEMENT,
					D3D11_INPUT_PER_VERTEX_DATA, 0
				};
				break;
			case VertexSemantic::BINORMAL:
				ied[i] = {
					"BINORMAL",
					binormalElements++,
					DXGI_FORMAT_R32G32B32_FLOAT,
					slot++,
					D3D11_APPEND_ALIGNED_ELEMENT,
					D3D11_INPUT_PER_VERTEX_DATA, 0
				};
				break;
			case VertexSemantic::COLOR:
				ied[i] = {
					"COLOR",
					colorElements++,
					DXGI_FORMAT_R32G32B32A32_FLOAT,
					slot++,
					D3D11_APPEND_ALIGNED_ELEMENT,
					D3D11_INPUT_PER_VERTEX_DATA, 0
				};
				break;
			case VertexSemantic::TANGENT:
				ied[i] = {
					"TANGENT",
					tangentElements++,
					DXGI_FORMAT_R32G32B32_FLOAT,
					slot++,
					D3D11_APPEND_ALIGNED_ELEMENT,
					D3D11_INPUT_PER_VERTEX_DATA, 0
				};
				break;
			case VertexSemantic::TEXCOORD:
				ied[i] = {
					"TEXCOORD",
					texcoordElements++,
					DXGI_FORMAT_R32G32_FLOAT,
					slot++,
					D3D11_APPEND_ALIGNED_ELEMENT,
					D3D11_INPUT_PER_VERTEX_DATA, 0
				};
				break;
			}
		}

		HRESULT result = impl->device->CreateInputLayout(
			ied,
			vertexLayoutDescription->count,
			dx11Pipeline->vertexShader->source,
			dx11Pipeline->vertexShader->size,
			&dx11Pipeline->inputLayout
		);

		SGE_ASSERT(result == S_OK);

		return &dx11Pipeline->header;
	}

	void GraphicsDevice::deletePipeline(Pipeline* pipeline)
	{
		DX11Pipeline* dx11Pipeline = reinterpret_cast<DX11Pipeline*>(pipeline);

		dx11Pipeline->inputLayout->Release();

		delete dx11Pipeline;
		pipeline = nullptr;
	}

	Shader* GraphicsDevice::createShader(ShaderType type, const char* source, size_t size)
	{
		DX11Shader* dx11Shader = new DX11Shader();
		HRESULT result = S_OK;

		switch (type)
		{
		case ShaderType::VERTEX: 
			result = impl->device->CreateVertexShader(source, size, nullptr, &dx11Shader->vertexShader); break;
		case ShaderType::PIXEL: 
			result = impl->device->CreatePixelShader(source, size, nullptr, &dx11Shader->pixelShader); break;
		}

		SGE_ASSERT(result == S_OK);

		dx11Shader->header.type = type;
		dx11Shader->source = source;
		dx11Shader->size = size;

		return &dx11Shader->header;
	}

	void GraphicsDevice::deleteShader(Shader* shader)
	{
		DX11Shader* dx11Shader = reinterpret_cast<DX11Shader*>(shader);

		switch (dx11Shader->header.type)
		{
		case ShaderType::VERTEX: 
			dx11Shader->vertexShader->Release(); break;
		case ShaderType::PIXEL:
			dx11Shader->pixelShader->Release(); break;
		}

		shader = nullptr;
	}

	Texture* GraphicsDevice::createTexture(size_t width, size_t height, unsigned char* source)
	{
		return nullptr;
	}

	void GraphicsDevice::deleteTexture(Texture* texture)
	{

	}

	void GraphicsDevice::bindPipeline(Pipeline* pipeline)
	{
		DX11Pipeline* dx11Pipeline = reinterpret_cast<DX11Pipeline*>(pipeline);
		impl->context->VSSetShader(dx11Pipeline->vertexShader->vertexShader, 0, 0);
		impl->context->PSSetShader(dx11Pipeline->pixelShader->pixelShader, 0, 0);
		impl->context->IASetInputLayout(dx11Pipeline->inputLayout);
		impl->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void GraphicsDevice::debindPipeline(Pipeline* pipeline)
	{

	}

	void GraphicsDevice::bindVertexBuffer(Buffer* buffer)
	{
		DX11Buffer* dx11Buffer = reinterpret_cast<DX11Buffer*>(buffer);

		// TODO super dirty hax fix
		UINT stride = sizeof(float) * 7;
		UINT offset = 0;

		impl->context->IASetVertexBuffers(
			0,
			1,
			&dx11Buffer->buffer,
			&stride,
			&offset
		);
	}

	void GraphicsDevice::bindIndexBuffer(Buffer* buffer)
	{
		impl->context->IASetIndexBuffer(reinterpret_cast<DX11Buffer*>(buffer)->buffer, DXGI_FORMAT_R32_UINT, 0);
	}

	void GraphicsDevice::bindVertexUniformBuffer(Buffer* buffer, size_t slot)
	{
		impl->context->VSSetConstantBuffers(slot, 1, &reinterpret_cast<DX11Buffer*>(buffer)->buffer);
	}

	void GraphicsDevice::bindPixelUniformBuffer(Buffer* buffer, size_t slot)
	{
		impl->context->PSSetConstantBuffers(slot, 1, &reinterpret_cast<DX11Buffer*>(buffer)->buffer);
	}

	void GraphicsDevice::bindViewport(Viewport* viewport)
	{

	}

	void GraphicsDevice::bindTexture(Texture* texture, size_t slot)
	{

	}

	void GraphicsDevice::debindTexture(Texture* texture)
	{

	}

	void GraphicsDevice::copyData(Buffer* buffer, size_t size, const void* data)
	{
		DX11Buffer* dx11Buffer = reinterpret_cast<DX11Buffer*>(buffer);

		D3D11_MAPPED_SUBRESOURCE ms;
		impl->context->Map(dx11Buffer->buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, data, size);
		impl->context->Unmap(dx11Buffer->buffer, NULL);
	}

	void GraphicsDevice::copySubData(Buffer* buffer, size_t offset, size_t size, const void* data)
	{

	}

	void GraphicsDevice::draw(size_t count)
	{
		impl->context->Draw(count, 0);
	}

	void GraphicsDevice::drawIndexed(size_t count)
	{
		impl->context->DrawIndexed(count, 0, 0);
	}

	void GraphicsDevice::drawInstanced(size_t count, size_t instanceCount)
	{

	}

	void GraphicsDevice::drawInstancedIndexed(size_t count, size_t instanceCount)
	{

	}
}
#endif