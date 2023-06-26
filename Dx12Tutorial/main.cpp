#include <Windows.h>
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

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

#ifdef _DEBUG
int main() {
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WNDCLASS w = {};	//�E�C���h�E�N���X�̐����Ɠo�^

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	//�R�[���o�b�N�֐��̎w��
	w.lpszClassName = _T("DX12Sample");			//�A�v���P�[�V�����N���X��
	w.hInstance = GetModuleHandle(nullptr);		//�n���h���̎擾

	RegisterClassEx(&w);	//�E�B���h�E�N���X�̎w���OS�ɓ`����

	RECT wrc = { 0, 0, window_width, window_height };	//�E�B���h�E�T�C�Y�����߂�

#endif
	DebugOutputFormatString("Show window test.");
	getchar();
	return 0;
}
