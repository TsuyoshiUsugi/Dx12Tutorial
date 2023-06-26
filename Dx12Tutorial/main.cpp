#include <Windows.h>
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

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
	printf(format, valist);
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
int main() {
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WNDCLASS w = {};	//ウインドウクラスの生成と登録

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	//コールバック関数の指定
	w.lpszClassName = _T("DX12Sample");			//アプリケーションクラス名
	w.hInstance = GetModuleHandle(nullptr);		//ハンドルの取得

	RegisterClassEx(&w);	//ウィンドウクラスの指定をOSに伝える

	RECT wrc = { 0, 0, window_width, window_height };	//ウィンドウサイズを決める

#endif
	DebugOutputFormatString("Show window test.");
	getchar();
	return 0;
}
