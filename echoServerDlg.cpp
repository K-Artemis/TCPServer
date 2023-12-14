
// echoServerDlg.cpp : ���� ����
#include "stdafx.h"
#include "echoServer.h"
#include "echoServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SERVER_RECV WM_APP + 1		// ����� ���� �޽��� ����
#define PORT 9999						// ������ ��Ʈ ����

// CechoServerDlg ��ȭ ����
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
	ON_MESSAGE(WM_ACCEPT_SOCKET, &CechoServerDlg::OnAcceptSocket)			// ������ ���� ���α׷��ֿ��� ���Ǹ�, ���� ������ ���ο� ������ �޾Ƶ��� �غ� �Ǿ��� �� �߻�. ��, Ŭ���̾�Ʈ�� ������ ���� ��û�� ������, ������ �̸� ������ �غ� �Ǿ��� �� �ش� �޽����� �߻�
	ON_MESSAGE(WM_SERVER_RECV, &CechoServerDlg::OnServerRecv)				// ������ Ŭ���̾�Ʈ�κ��� �����͸� ���� �غ� �Ǿ����� ��Ÿ���ϴ�. Ŭ���̾�Ʈ�κ��� ������ ������ ���۵Ǹ�, ������ �� �޽����� �ް� recv �Լ��� ����Ͽ� �����͸� ����
END_MESSAGE_MAP()

// CechoServerDlg �޽��� ó����
BOOL CechoServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_pServerSocket = new CServerSocket;				//Listen ���� �ʱ�ȭ
	m_pServerSocket->SetWnd(this->m_hWnd);

	m_pServerSocket->Create(PORT);						//���� Listen�ϱ�
	m_pServerSocket->Listen();

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CechoServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
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
			//UNICODE ����ϸ� ���� �޸�ũ�� *2 �ؾ���
			//Client�� Broadcasting�ϱ�
			pClient->Send(lpszStr, lstrlen(lpszStr) * 2);
		}
	}

	//m_list_msg�� �޽��� �߰�
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
		//m_list_client���� �ش� Client ����
		str.Format(_T("Client (%d)"), (int)pClient);
		idx = m_list_client.SelectString(-1, (LPCTSTR)str);
		m_list_client.DeleteString(idx);
		
		//CPtrList������ �ش� Client ����
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
        // ��� Ŭ���̾�Ʈ���� �޽��� ��ε�ĳ��Ʈ
        BroadcastMessageToClients(*pStr);

        m_list_msg.AddString(*pStr); // ������ ����Ʈ �ڽ��� �޽��� �߰�
        delete pStr;
    } else {
        AfxMessageBox(_T("��ȿ���� ���� �޽��� ����"));
    }
    return 0;
}

// �޽��� ��ε�ĳ��Ʈ�� ���� �Լ�
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