//#undef UNICODE
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

union net
{
	DWORD full;
	BYTE oct[4];
};

BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
TCHAR* dec(TCHAR* Buf, DWORD n);
TCHAR* bin(TCHAR* Buf, DWORD n);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
}

BOOL CALLBACK DialogProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	static HWND out;
	static HWND in_mask;
	static HWND in_ip;
	TCHAR ip_buf[16];
	TCHAR out_buf[25500];
	TCHAR out_tmp[64];
	TCHAR tmp1[16];
	TCHAR tmp2[36];
	int net_prefix;
	union net ip;
	union net mask;
	union net net_name;
	union net net_broadcast;

	switch (uMessage)
	{
	case WM_INITDIALOG:
		out = GetDlgItem(hWnd, IDC_STATIC1);
		in_mask = GetDlgItem(hWnd, IDC_COMBO1);
		in_ip = GetDlgItem(hWnd, IDC_IPADDRESS1);

		SetWindowText(in_ip, TEXT("192.168.0.1"));

		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/32 255.255.255.255")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/31 255.255.255.254")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/30 255.255.255.252")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/29 255.255.255.248")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/28 255.255.255.240")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/27 255.255.255.224")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/26 255.255.255.192")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/25 255.255.255.128")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/24 255.255.255.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/23 255.255.254.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/22 255.255.252.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/21 255.255.248.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/20 255.255.240.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/19 255.255.224.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/18 255.255.192.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/17 255.255.128.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/16 255.255.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/15 255.254.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/14 255.252.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/13 255.248.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/12 255.240.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/11 255.224.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/10 255.192.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/9 255.128.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/8 255.0.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/7 254.0.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/6 252.0.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/5 248.0.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/4 240.0.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/3 224.0.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/2 192.0.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/1 128.0.0.0")));
		SendMessage(in_mask, CB_ADDSTRING, 0, LPARAM(TEXT("/0 0.0.0.0")));

		SendMessage(in_mask, CB_SETCURSEL, 8, 0);

		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			GetWindowText(in_ip, ip_buf, sizeof(ip_buf));
			_stscanf(ip_buf, TEXT("%hhu.%hhu.%hhu.%hhu"), &ip.oct[3], &ip.oct[2], &ip.oct[1], &ip.oct[0]);

			net_prefix = SendMessage(in_mask, CB_GETCURSEL, 0, 0);
			mask.full = 0xFFFFFFFF << net_prefix;

			net_name.full = ip.full & mask.full;
			net_broadcast.full = net_name.full + ~mask.full;

			wsprintf(out_buf, TEXT("IP\t\t%s \t%s\n"), dec(tmp1, ip.full), bin(tmp2, ip.full));

			wsprintf(out_tmp, TEXT("Bitmask\t\t%d\n"), 32 - net_prefix);
			lstrcat(out_buf, out_tmp);

			wsprintf(out_tmp, TEXT("Netmask\t%s \t%s\n"), dec(tmp1, mask.full), bin(tmp2, mask.full));
			lstrcat(out_buf, out_tmp);

			wsprintf(out_tmp, TEXT("Network\t%s \t%s\n"), dec(tmp1, net_name.full), bin(tmp2, net_name.full));
			lstrcat(out_buf, out_tmp);

			wsprintf(out_tmp, TEXT("Hostmin\t\t%s \t%s\n"), dec(tmp1, net_name.full + 1), bin(tmp2, net_name.full + 1));
			lstrcat(out_buf, out_tmp);

			wsprintf(out_tmp, TEXT("Hostmax\t%s \t%s\n"), dec(tmp1, net_broadcast.full - 1), bin(tmp2, net_broadcast.full - 1));
			lstrcat(out_buf, out_tmp);

			wsprintf(out_tmp, TEXT("Broadcast\t%s \t%s\n"), dec(tmp1, net_broadcast.full), bin(tmp2, net_broadcast.full));
			lstrcat(out_buf, out_tmp);

			wsprintf(out_tmp, TEXT("Hosts\t\t%lu\n"), ~mask.full - 1);
			lstrcat(out_buf, out_tmp);

			SetWindowText(out, out_buf);
			break;
		}
		return TRUE;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	}
	return FALSE;
}

TCHAR* dec(TCHAR* Buf, DWORD n)
{
	union net tmp;
	tmp.full = n;
	wsprintf(Buf, TEXT("%d.%d.%d.%d"), tmp.oct[3], tmp.oct[2], tmp.oct[1], tmp.oct[0]);
	return Buf;
}

TCHAR* bin(TCHAR* Buf, DWORD n)
{
	int r = 0;
	for (DWORD i = 0x80000000; i > 0; i >>= 1)
	{
		Buf[r++] = (n & i) ? '1' : '0';
		if (i == 0x01000000 | i == 0x10000 | i == 0x100)
			Buf[r++] = '.';
	}
	return Buf;
}