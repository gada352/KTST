// ServerSetupDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "ServerSetupDlg.h"
#include "afxdialogex.h"


// CServerSetupDlg �Ի���

IMPLEMENT_DYNAMIC(CServerSetupDlg, CDialogEx)

CServerSetupDlg::CServerSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerSetupDlg::IDD, pParent)
{
	m_strServerName = _T("");
	m_strServerPort = _T("");
	m_strDBName = _T("");

}

CServerSetupDlg::~CServerSetupDlg()
{
}

void CServerSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CServerSetupDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CServerSetupDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CServerSetupDlg ��Ϣ�������
void CServerSetupDlg::SetServerInfo (CString strServerName, CString strServerPort, CString strDBName)
{
	m_strServerName = strServerName;
	m_strServerPort = strServerPort;
	m_strDBName = strDBName;
}

void CServerSetupDlg::GetServerInfo (CString &strServerName, CString &strServerPort, CString &strDBName)
{
	strServerName = m_strServerName;
	strServerPort = m_strServerPort;
	strDBName = m_strDBName;
}

BOOL CServerSetupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetDlgItemText (IDC_SERVER_NAME, m_strServerName);
	SetDlgItemText (IDC_SERVER_PORT, m_strServerPort);
	SetDlgItemText (IDC_DB_NAME, m_strDBName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CServerSetupDlg::OnBnClickedOk()
{
	GetDlgItemText (IDC_SERVER_NAME, m_strServerName);
	m_strServerName.Trim();
	if (m_strServerName.IsEmpty())
	{
		AfxMessageBox(IDS_SERVER_NAME_NULL);
		return;
	}

	GetDlgItemText (IDC_SERVER_PORT, m_strServerPort);
	m_strServerPort.Trim();
	if (m_strServerPort.IsEmpty())
	{
		AfxMessageBox(IDS_SERVER_PORT_NULL);
		return;
	}

	GetDlgItemText (IDC_DB_NAME, m_strDBName);
	m_strDBName.Trim();
	if (m_strDBName.IsEmpty())
	{
		AfxMessageBox(IDS_DB_NAME_NULL);
		return;
	}


	CDialogEx::OnOK();
}
