// LoginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "LoginDlg.h"
#include "afxdialogex.h"


// CLoginDlg �Ի���

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
{

	m_strUserName = _T("");
	m_strPassword = _T("");
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USER_NAME, m_strUserName);
	DDV_MaxChars(pDX, m_strUserName, BUF_LEN_S-1);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, BUF_LEN_S-1);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLoginDlg ��Ϣ�������


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

// 	//��Ĭ���û�����
// 	CRegistry reg;
// 	reg.RegReadUserName(m_strUserName);
// 	m_strUserName.Trim();
// 	if (!m_strUserName.IsEmpty())
// 		SetDlgItemText (IDC_USER_NAME, m_strUserName);

	SetDlgItemText (IDC_USER_NAME, USER_NAME_ADMIN);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CLoginDlg::OnBnClickedOk()
{
	if (!UpdateData(TRUE))
		return;

	CEdit* pEdit = NULL;

	//ע���û�������Ϊ�գ����������Ϊ��
	m_strUserName.Trim();
	if (m_strUserName.IsEmpty())
	{
		AfxMessageBox(IDS_ENTER_USERNAME);
		pEdit = (CEdit*)GetDlgItem(IDC_USER_NAME);
		pEdit->SetSel(0, -1);
		pEdit->SetFocus();
		return;
	}
	m_strPassword.Trim();


	//�ж��û����������¼
	TCHAR szUserName[BUF_LEN_S];
	TCHAR szUserCode[BUF_LEN_S];
	int nUserID = -1;
	int nUserRight = -1;

	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	memset (szUserName, 0x0, sizeof(TCHAR) * BUF_LEN_S);
	memset (szUserCode, 0x0, sizeof(TCHAR) * BUF_LEN_S);
	memcpy (szUserName, (LPCTSTR )m_strUserName, m_strUserName.GetLength()*sizeof(TCHAR));
	memcpy (szUserCode, (LPCTSTR )m_strPassword, m_strPassword.GetLength()*sizeof(TCHAR));

	BOOL bExist = FALSE;
	int nDBRet = dbAccess.CheckUserExist(szUserName, szUserCode, bExist, nUserID, nUserRight);
	if (nDBRet != RET_DB_SUCCESS)
	{
		AfxMessageBox(IDS_CONNECT_DB_FAILED);
		return;
	}

	if (bExist)
	{
		if (nUserRight == USERRIGHT_NORMAL)	//�û�Ȩ�޲���
		{
			AfxMessageBox(IDS_USER_RIGHT_LOW);
			pEdit = (CEdit*)GetDlgItem(IDC_USER_NAME);
			pEdit->SetSel(0, -1);
			pEdit->SetFocus();

			pEdit = (CEdit*)GetDlgItem(IDC_PASSWORD);
			pEdit->SetWindowText (_T(""));
			return;
		}
	}
	else
	{
		AfxMessageBox(IDS_FAIL_LOGON);

		pEdit = (CEdit*)GetDlgItem(IDC_USER_NAME);
		pEdit->SetSel(0, -1);
		pEdit->SetFocus();

		pEdit = (CEdit*)GetDlgItem(IDC_PASSWORD);
		pEdit->SetWindowText (_T(""));
		return;
	}

	//��¼�ɹ����û�����дע���
	CRegistry reg;
	reg.RegWriteUserName(m_strUserName);


	CDialogEx::OnOK();
}
