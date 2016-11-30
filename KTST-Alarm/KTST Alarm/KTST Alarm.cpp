
// KTST Alarm.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "KTST AlarmDlg.h"

#include "ServerSetupDlg.h"
#include "XMLProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��һ�� CKTSTAlarmApp ����

CKTSTAlarmApp theApp;

CADOConnection CKTSTAlarmApp::m_ADOConnection;
CDeviceIO CKTSTAlarmApp::m_DeviceIO(SOFTWARE_NAME);	//�豸�������
CCommunicate* CKTSTAlarmApp::m_pCommunicate = NULL;

// CKTSTAlarmApp

BEGIN_MESSAGE_MAP(CKTSTAlarmApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKTSTAlarmApp ����

CKTSTAlarmApp::CKTSTAlarmApp()
{
	::CoInitialize(NULL);

	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	m_pCommunicate = new CCommunicate();
}

CKTSTAlarmApp::~CKTSTAlarmApp()
{
	FreeAndNullPtr(m_pCommunicate);

	::CoUninitialize();
}

// CKTSTAlarmApp ��ʼ��

BOOL CKTSTAlarmApp::InitInstance()
{
	CString strCmdLine = this->m_lpCmdLine;
	strCmdLine.MakeUpper();
	if (strCmdLine == AUTO_RUN_CMD_LINE)
		Sleep (AUTO_RUN_SLEEP_TIME);

	//�ж��Ƿ���ͬһ������������
	if (this->IsAlreadyExistProc (SOFTWARE_NAME))
		return FALSE;

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;



	//�������ݿ�
	if (!this->ConnectDB())
	{
		// ɾ�����洴���� shell ��������
		if (pShellManager != NULL)
		{
			delete pShellManager;
		}

		return FALSE;
	}

	CString strMainPath = this->GetMainPath ();

	//�޸İ����ļ�·��
	CString strHelpName = strMainPath + _T("\\") + HELP_FILE_NAME;
	free((void *)theApp.m_pszHelpFilePath);
	theApp.m_pszHelpFilePath = _tcsdup(strHelpName);
	theApp.m_eHelpType = afxHTMLHelp;



	CKTSTAlarmDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}



//�ж��Ƿ���ָ������lpSemaphore��������
BOOL CKTSTAlarmApp::IsAlreadyExistProc (LPCTSTR lpSemaphore)
{
	// ��Ӧ�ó����������ź���
	HANDLE hSem = CreateSemaphore(NULL, 1, 1, lpSemaphore);

	// �ź����Ѵ���?
	// �ź������ڣ����������һ��ʵ������
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// �ر��ź������
		CloseHandle(hSem);
		// Ѱ����ǰʵ����������
		HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while (::IsWindow(hWndPrevious))
		{
			// ��鴰���Ƿ���Ԥ��ı��?
			// �У���������Ѱ�ҵ�����
			if (::GetProp(hWndPrevious, lpSemaphore))
			{
				// ������δ��ʾ������ʾ������
				if (!::IsWindowVisible(hWndPrevious))
					::ShowWindow(hWndPrevious, SW_SHOWNORMAL);

				// ����������С������ָ����С
				if (::IsIconic(hWndPrevious))
					::ShowWindow(hWndPrevious, SW_RESTORE);

				//�õ����������ϲ�ĶԻ���
				HWND hWnd = ::GetLastActivePopup(hWndPrevious);

				// ����������
				::SetForegroundWindow(hWndPrevious);

				// �������ĶԻ��򼤻�
				::SetForegroundWindow(hWnd);

				// �˳���ʵ��
				return TRUE;
			}
			// ����Ѱ����һ������
			hWndPrevious = ::GetWindow(hWndPrevious, GW_HWNDNEXT);
		}
		// ǰһʵ���Ѵ��ڣ����Ҳ���������
		// ���ܳ�����
		// �˳���ʵ��
		return TRUE;
	}

	return FALSE;
}

BOOL CKTSTAlarmApp::ConnectDB ()
{
	CString strDBSvrName, strDBSvrPort, strDBName, strDBSvrUserName, strDBSvrUserPwd;
	BOOL bAutoConnectDB = FALSE;

	//д�뵽xml�ļ���
	CString strFilePath = this->GetCFGFilePath ();
	CXMLProfile xmlFile(strFilePath);
	LoginInfo li;

	if (xmlFile.GetLoginInfo(li))
	{
		strDBSvrName = li.strDBSvrIP;
		strDBSvrPort = li.strDBSvrPort;
		strDBSvrUserName = li.strDBSvrUserName;
		strDBSvrUserPwd = li.strDBSvrUserPwd;
		strDBName = li.strDBName;
		bAutoConnectDB = li.bAutoConnectDB;
	}

	if (strDBSvrName.IsEmpty ())
		strDBSvrName = DEF_SERVER;
	if (strDBSvrPort.IsEmpty ())
		strDBSvrPort = DEF_PORT;
	if (strDBSvrUserName.IsEmpty ())
		strDBSvrUserName = DEF_SQLSVR_USER;
	if (strDBSvrUserPwd.IsEmpty ())
		strDBSvrUserPwd = DEF_SQLSVR_PWD;


	//������������ƻ�IP��ַ
	CServerSetupDlg dlg;

	CString strDBNamePrefix = DB_NAME_PREFIX;
	if (!bAutoConnectDB)
	{
		CString strDBNameTemp = strDBName;
		if (strDBNameTemp.Find(strDBNamePrefix) == 0)
			strDBNameTemp.Delete(0, strDBNamePrefix.GetLength());

		dlg.SetServerInfo(strDBSvrName, strDBSvrPort, strDBNameTemp);
		if (dlg.DoModal() != IDOK)
			return FALSE;

		dlg.GetServerInfo(strDBSvrName, strDBSvrPort, strDBNameTemp);
		strDBName = strDBNamePrefix + strDBNameTemp;
	}

	//�������ݿ�
	while (!m_ADOConnection.Connect (strDBSvrName, strDBSvrPort, strDBName, strDBSvrUserName, strDBSvrUserPwd))
	{
		//�������ݿ�ʧ�ܣ������������������
		CString strDBNameTemp = strDBName;
		if (strDBNameTemp.Find(strDBNamePrefix) == 0)
			strDBNameTemp.Delete(0, strDBNamePrefix.GetLength());

		dlg.SetServerInfo(strDBSvrName, strDBSvrPort, strDBNameTemp);
		if (dlg.DoModal() != IDOK)
			return FALSE;

		dlg.GetServerInfo(strDBSvrName, strDBSvrPort, strDBNameTemp);
		strDBName = strDBNamePrefix + strDBNameTemp;
	}

	li.strDBSvrIP = strDBSvrName;
	li.strDBSvrPort = strDBSvrPort;
	li.strDBSvrUserName = strDBSvrUserName;
	li.strDBSvrUserPwd = strDBSvrUserPwd;
	li.strDBName = strDBName;

	xmlFile.SetLoginInfo(li);

	return TRUE;
}

CDeviceIO* CKTSTAlarmApp::GetDeviceIOPtr ()
{
	return &m_DeviceIO;
}

CADOConnection* CKTSTAlarmApp::GetADOConnectionPtr ()
{
	return &m_ADOConnection;
}

CCommunicate* CKTSTAlarmApp::GetCommunicatePtr ()
{
	return m_pCommunicate;
}

//Ҫ��·��
CString CKTSTAlarmApp::GetMainPath ()
{
	//��ע����ж���·��
	CRegistry reg;
	CString strMainPath;
	reg.RegReadMainPath (strMainPath);

	if (strMainPath.IsEmpty ())
	{
		TCHAR szMainPath[_MAX_PATH];
		memset(szMainPath, 0x0, sizeof(szMainPath));
		::GetModuleFileName(NULL, szMainPath, _MAX_PATH);
		strMainPath = szMainPath;
		int nIndex = strMainPath.ReverseFind('\\');
		strMainPath = strMainPath.Left(nIndex);
	}

	return strMainPath;
}

//Ҫ�����ļ�·��
CString CKTSTAlarmApp::GetCFGFilePath ()
{
	CString strMainPath = GetMainPath ();
	CString strFilePath = strMainPath + _T("\\") + CFG_FILE_NAME;

	return strFilePath;
}
