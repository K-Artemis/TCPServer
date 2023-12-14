// ClientSocket.cpp : ���� �����Դϴ�.
#include "stdafx.h"
#include "echoServer.h"
#include "ClientSocket.h"

// CClientSocket
CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}

// CClientSocket ��� �Լ�
void CClientSocket::SetWnd(HWND hWnd)
{
	m_hWnd = hWnd;
}

void CClientSocket::OnReceive(int nErrorCode)
{
	CString strTmp = _T(""), strIPAddress = _T("");
	UINT uPortNumber = 0;
	TCHAR strBuffer[1024];
	ZeroMemory(strBuffer, sizeof(strBuffer));
	
	GetPeerName(strIPAddress, uPortNumber);
	if(Receive(strBuffer, sizeof(strBuffer)) > 0)
		strTmp.Format(_T("[%s : %d]: %s"), strIPAddress, uPortNumber, strBuffer);			// ���޵� ������(���ڿ�)�� ���� ���

	//Main Window�� Send
	SendMessage(m_hWnd,WM_CLIENT_MSG_RECV,0,(LPARAM)((LPCTSTR)strTmp));
	CSocket::OnReceive(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode)
{
	SendMessage(m_hWnd, WM_CLIENT_CLOSE, 0, (LPARAM)this);
	CSocket::OnClose(nErrorCode);
}