
// KTST Alarm.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "KTST AlarmDlg.h"

#include "ServerSetupDlg.h"
#include "XMLProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的一个 CKTSTAlarmApp 对象

CKTSTAlarmApp theApp;

CADOConnection CKTSTAlarmApp::m_ADOConnection;
CDeviceIO CKTSTAlarmApp::m_DeviceIO(SOFTWARE_NAME);	//设备管理对象
CCommunicate* CKTSTAlarmApp::m_pCommunicate = NULL;

// CKTSTAlarmApp

BEGIN_MESSAGE_MAP(CKTSTAlarmApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKTSTAlarmApp 构造

CKTSTAlarmApp::CKTSTAlarmApp()
{
	::CoInitialize(NULL);

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_pCommunicate = new CCommunicate();
}

CKTSTAlarmApp::~CKTSTAlarmApp()
{
	FreeAndNullPtr(m_pCommunicate);

	::CoUninitialize();
}

// CKTSTAlarmApp 初始化

BOOL CKTSTAlarmApp::InitInstance()
{
	CString strCmdLine = this->m_lpCmdLine;
	strCmdLine.MakeUpper();
	if (strCmdLine == AUTO_RUN_CMD_LINE)
		Sleep (AUTO_RUN_SLEEP_TIME);

	//判断是否有同一程序正在运行
	if (this->IsAlreadyExistProc (SOFTWARE_NAME))
		return FALSE;

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;



	//连接数据库
	if (!this->ConnectDB())
	{
		// 删除上面创建的 shell 管理器。
		if (pShellManager != NULL)
		{
			delete pShellManager;
		}

		return FALSE;
	}

	CString strMainPath = this->GetMainPath ();

	//修改帮助文件路径
	CString strHelpName = strMainPath + _T("\\") + HELP_FILE_NAME;
	free((void *)theApp.m_pszHelpFilePath);
	theApp.m_pszHelpFilePath = _tcsdup(strHelpName);
	theApp.m_eHelpType = afxHTMLHelp;



	CKTSTAlarmDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}



//判断是否有指定程序lpSemaphore正在运行
BOOL CKTSTAlarmApp::IsAlreadyExistProc (LPCTSTR lpSemaphore)
{
	// 用应用程序名创建信号量
	HANDLE hSem = CreateSemaphore(NULL, 1, 1, lpSemaphore);

	// 信号量已存在?
	// 信号量存在，则程序已有一个实例运行
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 关闭信号量句柄
		CloseHandle(hSem);
		// 寻找先前实例的主窗口
		HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while (::IsWindow(hWndPrevious))
		{
			// 检查窗口是否有预设的标记?
			// 有，则是我们寻找的主窗
			if (::GetProp(hWndPrevious, lpSemaphore))
			{
				// 主窗口未显示，则显示主窗口
				if (!::IsWindowVisible(hWndPrevious))
					::ShowWindow(hWndPrevious, SW_SHOWNORMAL);

				// 主窗口已最小化，则恢复其大小
				if (::IsIconic(hWndPrevious))
					::ShowWindow(hWndPrevious, SW_RESTORE);

				//得到主窗口最上层的对话框
				HWND hWnd = ::GetLastActivePopup(hWndPrevious);

				// 将主窗激活
				::SetForegroundWindow(hWndPrevious);

				// 将主窗的对话框激活
				::SetForegroundWindow(hWnd);

				// 退出本实例
				return TRUE;
			}
			// 继续寻找下一个窗口
			hWndPrevious = ::GetWindow(hWndPrevious, GW_HWNDNEXT);
		}
		// 前一实例已存在，但找不到其主窗
		// 可能出错了
		// 退出本实例
		return TRUE;
	}

	return FALSE;
}

BOOL CKTSTAlarmApp::ConnectDB ()
{
	CString strDBSvrName, strDBSvrPort, strDBName, strDBSvrUserName, strDBSvrUserPwd;
	BOOL bAutoConnectDB = FALSE;

	//写入到xml文件中
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


	//输入服务器名称或IP地址
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

	//连接数据库
	while (!m_ADOConnection.Connect (strDBSvrName, strDBSvrPort, strDBName, strDBSvrUserName, strDBSvrUserPwd))
	{
		//连接数据库失败，重新输入服务器名称
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

//要主路径
CString CKTSTAlarmApp::GetMainPath ()
{
	//从注册表中读主路径
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

//要配置文件路径
CString CKTSTAlarmApp::GetCFGFilePath ()
{
	CString strMainPath = GetMainPath ();
	CString strFilePath = strMainPath + _T("\\") + CFG_FILE_NAME;

	return strFilePath;
}
