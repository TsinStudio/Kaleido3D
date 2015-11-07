#include "UnitTestRHICommandContext.h"
#include "RHI/D3D12/Public/D3D12RHI.h"

using namespace rhi;

const char * source = K3D_STRINGIFY(
struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
	PSInput result;

	result.position = position;
	result.color = color;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
}
);

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};

Vertex triangleVertices[] =
{
	{ { 0.0f, 0.25f * 0.6f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { 0.25f, -0.25f * 0.6f, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { -0.25f, -0.25f * 0.6f, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
};

const UINT vertexBufferSize = sizeof(triangleVertices);

bool UnitTestRHICommandContext::OnInit()
{
	if(!App::OnInit())
		return false; 
	if (!InitDevice())
		return false;

	InitCommandContext();
	InitSwapChain();
	InitPipeLineState();
	InitRenderResource();
	return true;
}

void UnitTestRHICommandContext::OnDestroy()
{
	if (m_TestDevice)
	{
		delete m_TestDevice;
		m_TestDevice = nullptr;
	}
	if (m_TestCommandContext) 
	{
		delete m_TestCommandContext;
		m_TestCommandContext = nullptr;
	}
	if (m_TestPipelineState)
	{
		delete m_TestPipelineState;
		m_TestPipelineState = nullptr;
	}
	App::OnDestroy();
}

void UnitTestRHICommandContext::OnProcess(Message & msg)
{

}

void UnitTestRHICommandContext::OnUpdate()
{
	m_SwapChain->Present(1, 0);
}

bool UnitTestRHICommandContext::InitDevice()
{
	IDeviceAdapter ** list = nullptr;
	uint32 adapterNum = 0;
	EnumAllDeviceAdapter(list, &adapterNum);
	if (list == nullptr)
	{
		return false;
	}

	m_TestDevice = new Device;
	IDevice::Result Result = m_TestDevice->Create(list[0], true);
	if (Result != IDevice::DeviceFound)
	{
		return false;
	}
	return true;
}

void UnitTestRHICommandContext::InitCommandContext()
{
	m_TestCommandListManager.Create(static_cast<Device*>(m_TestDevice)->Get());
	// test [Device -> NewCommandContext] interface
	m_TestCommandContext = m_TestDevice->NewCommandContext(rhi::ECommandType::ECMD_Graphics);
}

void UnitTestRHICommandContext::InitSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = frame_count;
	swapChainDesc.BufferDesc.Width = HostWindow()->Width();
	swapChainDesc.BufferDesc.Height = HostWindow()->Height();
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = (HWND)HostWindow()->GetHandle();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = TRUE;

	ComPtr<IDXGISwapChain> swapChain;
	ThrowIfFailed(static_cast<Device*>(m_TestDevice)->GetDXGIFactory()->CreateSwapChain(
		m_TestCommandListManager.GetCommandQueue(),
		&swapChainDesc,
		swapChain.GetAddressOf()
		));

	ThrowIfFailed(swapChain.As(&m_SwapChain));
}

void UnitTestRHICommandContext::InitPipeLineState()
{
	ShaderCompiler vsCompiler;
	IShaderBytes* sbVert = vsCompiler.CompileFromSource(rhi::IShaderCompiler::HLSL_5_0, rhi::ES_Vertex, source, "VSMain");

	ShaderCompiler fsCompiler;
	IShaderBytes* sbPixel = fsCompiler.CompileFromSource(rhi::IShaderCompiler::HLSL_5_0, rhi::ES_Fragment, source, "PSMain");
	// test [Device -> NewpipelineState] interface
	m_TestPipelineState = m_TestDevice->NewPipelineState();

	// test PipelineState's interfaces
	RootSignature rs;
	rs.Finalize(static_cast<Device*>(m_TestDevice)->Get(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	static_cast<PipelineState*>(m_TestPipelineState)->SetRootSignature(rs);

	m_TestPipelineState->SetShader(rhi::ES_Vertex, sbVert);
	m_TestPipelineState->SetShader(rhi::ES_Fragment, sbPixel);
	m_TestPipelineState->SetBlendState(rhi::BlendState());
	m_TestPipelineState->SetDepthStencilState(rhi::DepthStencilState());
	m_TestPipelineState->SetRasterizerState(rhi::RasterizerState());
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	m_TestPipelineState->SetVertexInputLayout(new VertexInputLayout(inputElementDescs, 2));
	m_TestPipelineState->SetPrimitiveTopology(rhi::Triangles);
	m_TestPipelineState->Finalize();
}

void UnitTestRHICommandContext::InitRenderResource()
{
	m_FrameIndex = 0;
	Device * device = static_cast<Device*>(m_TestDevice);
	DescriptorHeapAllocator & dha = device->GetViewDescriptorAllocator<D3D12_RENDER_TARGET_VIEW_DESC>();
	SIZE_T outIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE handle = dha.AllocateHeapSlot(outIndex);
	for (int n = 0; n < frame_count; n++)
	{
		ThrowIfFailed(m_SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_RenderTargets[n])));
		device->Get()->CreateRenderTargetView(m_RenderTargets[n].Get(), nullptr, handle);
		handle = dha.AllocateHeapSlot(outIndex);
	}

	// test [Device -> NewGpuResource] interface
	IGpuResource * GpuRes = m_TestDevice->NewGpuResource(rhi::Buffer);
}
