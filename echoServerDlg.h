// echoServerDlg.h : 헤더 파일
#pragma once
#include "afxwin.h"
#include "ServerSocket.h"

// CechoServerDlg 대화 상자
class CechoServerDlg : public CDialogEx
{
// 생성입니다.
public:
	CechoServerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ECHOSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
	CPtrList m_ptrClientSocketList;						// 클라이언트 소켓 관리용

protected:
	LRESULT OnClientMsgRecv(WPARAM wParam, LPARAM lParam);
	LRESULT OnClientClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAcceptSocket(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerRecv(WPARAM wParam, LPARAM lParam);
	void BroadcastMessageToClients(const CString& message);
};
