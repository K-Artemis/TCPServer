
// echoServerDlg.cpp : 구현 파일
#include "stdafx.h"
#include "echoServer.h"
#include "echoServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SERVER_RECV WM_APP + 1		// 사용자 정의 메시지 정의
#define PORT 9999						// 임의의 포트 설정

// CechoServerDlg 대화 상자
CechoServerDlg::CechoServerDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CechoServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CechoServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT, m_list_client);
	DDX_Control(pDX, IDC_LIST_MES, m_list_msg);
}

BEGIN_MESSAGE_MAP(CechoServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_ACCEPT_SOCKET, &CechoServerDlg::OnAcceptSocket)			// 윈도우 소켓 프로그래밍에서 사용되며, 서버 소켓이 새로운 연결을 받아들일 준비가 되었을 때 발생. 즉, 클라이언트가 서버에 연결 요청을 보내고, 서버가 이를 수락할 준비가 되었을 때 해당 메시지가 발생
	ON_MESSAGE(WM_SERVER_RECV, &CechoServerDlg::OnServerRecv)				// 서버가 클라이언트로부터 데이터를 받을 준비가 되었음을 나타냅니다. 클라이언트로부터 데이터 전송이 시작되면, 서버는 이 메시지를 받고 recv 함수를 사용하여 데이터를 수신
END_MESSAGE_MAP()

// CechoServerDlg 메시지 처리기
BOOL CechoServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_pServerSocket = new CServerSocket;				//Listen 소켓 초기화
	m_pServerSocket->SetWnd(this->m_hWnd);

	m_pServerSocket->Create(PORT);						//소켓 Listen하기
	m_pServerSocket->Listen();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CechoServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CechoServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CechoServerDlg::OnClientMsgRecv(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpszStr = (LPCTSTR)lParam;

	//BroadCasting
	POSITION pos = m_ptrClientSocketList.GetHeadPosition();
	
	while(pos != NULL)
	{
		CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
		if(pClient!=NULL)
		{
			//UNICODE 사용하면 기존 메모리크기 *2 해야함
			//Client에 Broadcasting하기
			pClient->Send(lpszStr, lstrlen(lpszStr) * 2);
		}
	}

	//m_list_msg에 메시지 추가
	m_list_msg.InsertString(-1,lpszStr);
	m_list_msg.SetCurSel(m_list_msg.GetCount()-1);
	return 0;
}

LRESULT CechoServerDlg::OnClientClose(WPARAM wParam, LPARAM lParam)
{
	CClientSocket *pClient = (CClientSocket *)lParam;
	CString str;
	UINT idx=0;
	POSITION pos = m_ptrClientSocketList.Find(pClient);

	if(pos!=NULL)
	{
		//m_list_client에서 해당 Client 삭제
		str.Format(_T("Client (%d)"), (int)pClient);
		idx = m_list_client.SelectString(-1, (LPCTSTR)str);
		m_list_client.DeleteString(idx);
		
		//CPtrList에서도 해당 Client 삭제
		m_ptrClientSocketList.RemoveAt(pos);
	}

	return 0;
}

afx_msg LRESULT CechoServerDlg::OnAcceptSocket(WPARAM wParam, LPARAM lParam)
{
	CString str;
	m_pClientSocket = (CClientSocket*) lParam;
	m_ptrClientSocketList.AddTail(m_pClientSocket);

	str.Format(_T("Client (%d)"), (int)(m_pClientSocket));
	m_list_client.InsertString(-1,str);

	m_pClientSocket = NULL;
	delete m_pClientSocket;

	return 0;
}

afx_msg LRESULT CechoServerDlg::OnServerRecv(WPARAM wParam, LPARAM lParam)
{
    CString* pStr = reinterpret_cast<CString*>(lParam);
    if (pStr != nullptr) {
        // 모든 클라이언트에게 메시지 브로드캐스트
        BroadcastMessageToClients(*pStr);

        m_list_msg.AddString(*pStr); // 서버의 리스트 박스에 메시지 추가
        delete pStr;
    } else {
        AfxMessageBox(_T("유효하지 않은 메시지 수신"));
    }
    return 0;
}

// 메시지 브로드캐스트를 위한 함수
void CechoServerDlg::BroadcastMessageToClients(const CString& message)
{
    POSITION pos = m_ptrClientSocketList.GetHeadPosition();
    while (pos != NULL) {
        CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
        if (pClient != NULL) {
            pClient->Send((LPCTSTR)message, message.GetLength() * sizeof(TCHAR));
        }
    }
}