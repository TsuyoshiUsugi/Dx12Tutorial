#include <Windows.h>
#include <tchar.h>	//����Ȃ��ƃG���[���o��H
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;
using namespace DirectX;

/// <summary>
/// �R���\�[����ʂɃt�H�[�}�b�g�t���������\������
/// �f�o�b�N���ɂ��������Ȃ�
/// </summary>
/// <param name="format">�t�H�[�}�b�g</param>
/// <param name="">�ϒ�����</param>
void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif // _DEBUG
}

/// <summary>
/// �E�B���h�E�𐶐����邽�߂̊֐�
/// </summary>
/// <param name="hwnd">�E�B���h�E�n���h��</param>
/// <param name="msg">�E�B���h�E�C�x���g�̃��b�Z�[�WID</param>
/// <param name="wparam">�E�B���h�E�C�x���g�Ɋ֘A����ǉ��̏��</param>
/// <param name="lparam">����</param>
/// <returns></returns>
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {	//���b�Z�[�WID���E�B���h�E��j������C�x���g���ǂ���
		PostQuitMessage(0);	//OS�ɂ��̃A�v���͂����I���Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	debugLayer->EnableDebugLayer();		//�f�o�b�O���C���[�L����
	debugLayer->Release();		//�L����������C���^�[�t�F�[�X���
}

#ifdef _DEBUG
int main() 
{
	WNDCLASSEX w = {};	//�E�C���h�E�N���X�̐����Ɠo�^
	int window_width = 1280;
	int window_height = 720;

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	//�R�[���o�b�N�֐��̎w��
	w.lpszClassName = _T("DX12Sample");			//�A�v���P�[�V�����N���X��
	w.hInstance = GetModuleHandle(nullptr);		//�n���h���̎擾

	RegisterClassEx(&w);	//�E�B���h�E�N���X�̎w���OS�ɓ`����

	RECT wrc = { 0, 0, window_width, window_height };	//�E�B���h�E�T�C�Y�����߂�

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);		//�E�B���h�E�̃T�C�Y��␳����

	HWND hwnd = CreateWindow(w.lpszClassName,	//�N���X���w��
		_T("DX12�e�X�g"),		//�^�C�g���o�[�̎w��
		WS_OVERLAPPEDWINDOW,	//�^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,			//�\�����WX��OS�ɔC����
		CW_USEDEFAULT,			//�\�����WY��OS�ɔC����
		wrc.right - wrc.left,	//�E�B���h�E��
		wrc.bottom - wrc.top,	//�E�B���h�E��
		nullptr,				//�e�E�B���h�E�n���h��
		nullptr,				//���j���[�n���h��
		w.hInstance,			//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);				//�ǉ��p�����[�^

	ShowWindow(hwnd, SW_SHOW);

#ifdef _DEBUG
	//�f�o�b�O���C���[���I����
	EnableDebugLayer();
#endif

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgiFactory = nullptr;
	IDXGISwapChain4* _swapchain = nullptr;

	for (auto lv : levels) {
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK) {
			featureLevel = lv;
			break;
		}
	}

	



	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
	std::vector <IDXGIAdapter*> adapters;	//���p�\�ȃA�_�v�^�[��񋓂��i�[����ϐ�

	IDXGIAdapter* tmpAdapter = nullptr;		//����̖��O�����A�_�v�^�[�I�u�W�F�N�g������

	for (int i = 0; _dxgiFactory -> EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(tmpAdapter);
	}

	for ( auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);		//�A�_�v�^�[�̐����I�u�W�F�N�g�擾

		std::wstring strDesc = adesc.Description;

		if (strDesc.find(L"NVIDIA0") != std::string::npos) {	//�T�������A�_�v�^�[�̖��O���擾(�����ł�NVIDIA�Ŋm�肳��Ă��邪���ۂ͋N���I�v�V�����őI�ׂ�Ƃ���)
			tmpAdapter = adpt;
			break;
		}
	}

	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;

	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));

	ID3D12CommandQueue* _cmdQueue = nullptr;

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;		//�^�C���A�E�g�Ȃ��H
	cmdQueueDesc.NodeMask = 0;								//�A�_�v�^�[��������g��Ȃ�����0
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	//�v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;		//�R�}���h���X�g�ƍ��킹��

	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = window_height;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;

	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;	//�o�b�N�o�b�t�@�[�͐L�яk�݉\
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue, hwnd,
		&swapchainDesc, nullptr, nullptr,
		(IDXGISwapChain1**)& _swapchain);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = {};
	heapDesc.NumDescriptors = 2;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ID3D12DescriptorHeap* rtvHeaps = nullptr;

	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

	DXGI_SWAP_CHAIN_DESC swcDesc = {};

	result = _swapchain->GetDesc(&swcDesc);

	std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);
	for (int idx = 0; idx < swcDesc.BufferCount; ++idx) {
		result = _swapchain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));

		D3D12_CPU_DESCRIPTOR_HANDLE handle
			= rtvHeaps->GetCPUDescriptorHandleForHeapStart();

		handle.ptr += idx * _dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		_dev->CreateRenderTargetView(_backBuffers[idx], nullptr, handle);
	}

	result = _cmdAllocator->Reset();
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();

	auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_RESOURCE_BARRIER BarrierDesc = {};

	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;		//�J��
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		//�w��Ȃ�
	BarrierDesc.Transition.pResource = _backBuffers[bbIdx];
	BarrierDesc.Transition.Subresource = 0;

	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		//���O��Present���
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;		//�����烌���_�[�^�[�Q�b�g���

	_cmdList->ResourceBarrier(1, &BarrierDesc);		//�o���A�w����s



	_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

	float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f }; //���F


	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	_cmdList->Close();

	ID3D12CommandList* cmdlists[] = {_cmdList};
	_cmdQueue->ExecuteCommandLists(1, cmdlists);
	_cmdAllocator->Reset();
	_cmdList->Reset(_cmdAllocator, nullptr);

	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	_cmdList->ResourceBarrier(1, &BarrierDesc);

	_swapchain->Present(1, 0);

	ID3D12Fence* _fence = nullptr;
	UINT64 _fenceVal = 0;

	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	_cmdQueue->Signal(_fence, ++_fenceVal);

	if (_fence->GetCompletedValue() != _fenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);

		_fence->SetEventOnCompletion(_fenceVal, event);

		WaitForSingleObject(event, INFINITE);

		CloseHandle(event);
	}

	XMFLOAT3 vertices[] =
	{
		{-1.0f, -1.0f, 0.0f},	//����
		{-1.0f, 1.0f, 0.0f},	//����
		{1.0f, -1.0f, 0.0f},	//�E��
	};

	D3D12_HEAP_PROPERTIES heapprop = {};

	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

	D3D12_RESOURCE_DESC resdesc = {};

	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeof(vertices);
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.SampleDesc.Count = 1;
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* vertBuff = nullptr;

	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	
	XMFLOAT3* vertmap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertmap);
	std::copy(std::begin(vertices), std::end(vertices), vertmap);

#ifdef _DEBUG
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgiFactory));
#else
	CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif // _DEBUG

#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

#endif
	DebugOutputFormatString("Show window test.");
	getchar();

	MSG msg = {};

	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) {
			break;
		}
	}

	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}
