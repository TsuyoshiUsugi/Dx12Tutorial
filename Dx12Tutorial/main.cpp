#include <Windows.h>
#include <tchar.h>	//これないとエラーが出る？
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
/// コンソール画面にフォーマット付き文字列を表示する
/// デバック時にしか動かない
/// </summary>
/// <param name="format">フォーマット</param>
/// <param name="">可変長引数</param>
void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif // _DEBUG
}

/// <summary>
/// ウィンドウを生成するための関数
/// </summary>
/// <param name="hwnd">ウィンドウハンドル</param>
/// <param name="msg">ウィンドウイベントのメッセージID</param>
/// <param name="wparam">ウィンドウイベントに関連する追加の情報</param>
/// <param name="lparam">同上</param>
/// <returns></returns>
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {	//メッセージIDがウィンドウを破棄するイベントかどうか
		PostQuitMessage(0);	//OSにこのアプリはもう終わると伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

#ifdef _DEBUG
int main() 
{
	WNDCLASSEX w = {};	//ウインドウクラスの生成と登録
	int window_width = 480;
	int window_height = 270;

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	//コールバック関数の指定
	w.lpszClassName = _T("DX12Sample");			//アプリケーションクラス名
	w.hInstance = GetModuleHandle(nullptr);		//ハンドルの取得

	RegisterClassEx(&w);	//ウィンドウクラスの指定をOSに伝える

	RECT wrc = { 0, 0, window_width, window_height };	//ウィンドウサイズを決める

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);		//ウィンドウのサイズを補正する

	HWND hwnd = CreateWindow(w.lpszClassName,	//クラス名指定
		_T("DX12テスト"),		//タイトルバーの指定
		WS_OVERLAPPEDWINDOW,	//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,			//表示座標XはOSに任せる
		CW_USEDEFAULT,			//表示座標YはOSに任せる
		wrc.right - wrc.left,	//ウィンドウ幅
		wrc.bottom - wrc.top,	//ウィンドウ高
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		w.hInstance,			//呼び出しアプリケーションハンドル
		nullptr);				//追加パラメータ

	ShowWindow(hwnd, SW_SHOW);

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

	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgiFactory = nullptr;
	IDXGISwapChain4* _swapchain = nullptr;

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (auto lv : levels) {
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK) {
			featureLevel = lv;
			break;
		}
	}

	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
	std::vector <IDXGIAdapter*> adapters;	//利用可能なアダプターを列挙し格納する変数

	IDXGIAdapter* tmpAdapter = nullptr;		//特定の名前を持つアダプターオブジェクトが入る

	for (int i = 0; _dxgiFactory -> EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(tmpAdapter);
	}

	for ( auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);		//アダプターの説明オブジェクト取得

		std::wstring strDesc = adesc.Description;

		if (strDesc.find(L"NVIDIA0") != std::string::npos) {	//探したいアダプターの名前を取得(ここではNVIDIAで確定されているが実際は起動オプションで選べるといい)
			tmpAdapter = adpt;
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
