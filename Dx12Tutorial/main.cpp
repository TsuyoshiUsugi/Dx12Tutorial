#include <Windows.h>
#include <tchar.h>	//����Ȃ��ƃG���[���o��H
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;

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
