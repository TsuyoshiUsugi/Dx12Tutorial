#include <Windows.h>
#include <tchar.h>	//����Ȃ��ƃG���[���o��H
#include <d3d12.h>
#include <dxgi1_6.h>
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG
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
	printf(format, valist);
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

ID3D12Device* _dev = nullptr;
IDXGIFactory6* _dxgiFactory = nullptr;
IDXGISwapChain4* _swapchain = nullptr;

//�f�o�C�X�I�u�W�F�N�g�����
HRESULT D3D12CreateDevice(
	IUnknown* pAdapter,
	D3D_FEATURE_LEVEL MinimumFeatureLevel,
	REFIID            riid,
	void** ppDevice
);

#ifdef _DEBUG
int main() 
{
	WNDCLASSEX w = {};	//�E�C���h�E�N���X�̐����Ɠo�^
	int window_width = 480;
	int window_height = 270;

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

	MSG msg = {};

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (auto lv : levels)
	{
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK)
		{
			featureLevel = lv;
			break;
		}
	}



#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	

#endif
	DebugOutputFormatString("Show window test.");
	getchar();
	return 0;
}
