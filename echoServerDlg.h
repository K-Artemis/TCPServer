// echoServerDlg.h : ��� ����
#pragma once
#include "afxwin.h"
#include "ServerSocket.h"

// CechoServerDlg ��ȭ ����
class CechoServerDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CechoServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ECHOSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CClientSocket *m_pClientSocket;
	CServerSocket *m_pServerSocket;

public:
	CListBox m_list_client;								// IDC_LIST_CLIENT
	CListBox m_list_msg;								// IDC_LIST_MSG
	CPtrList m_ptrClientSocketList;						// Ŭ���̾�Ʈ ���� ������

protected:
	LRESULT OnClientMsgRecv(WPARAM wParam, LPARAM lParam);
	LRESULT OnClientClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAcceptSocket(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerRecv(WPARAM wParam, LPARAM lParam);
	void BroadcastMessageToClients(const CString& message);
};
