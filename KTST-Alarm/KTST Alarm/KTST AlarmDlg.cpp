
// KTST AlarmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "KTST AlarmDlg.h"
#include "afxdialogex.h"

#include "Registry.h"

#include "DeviceManageDlg.h"
#include "LoginDlg.h"
#include "XMLProfile.h"
#include "SMSSetupDlg.h"
#include "RelayManageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CKTSTAlarmDlg 对话框




CKTSTAlarmDlg::CKTSTAlarmDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CKTSTAlarmDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bVisable = FALSE;
	m_nShowWndFuncCnt = 0;
	m_bStartFlag = FALSE;
	m_nTimerID = 8729;

	m_bOnStartModule = FALSE;

	m_bAutoRun = FALSE;
	m_bAutoConnectDB = FALSE;
}

void CKTSTAlarmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKTSTAlarmDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CKTSTAlarmDlg::OnStart)
	ON_BN_CLICKED(IDC_STOP, &CKTSTAlarmDlg::OnStop)
	ON_BN_CLICKED(IDC_DEVICE_MANAGE, &CKTSTAlarmDlg::OnDeviceManage)
	ON_BN_CLICKED(IDC_HTML_HELP, &CKTSTAlarmDlg::OnHtmlHelp)
	ON_BN_CLICKED(IDC_ABOUT, &CKTSTAlarmDlg::OnAbout)
	ON_WM_CLOSE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_COMMAND(ID_START, &CKTSTAlarmDlg::OnStart)
	ON_COMMAND(ID_STOP, &CKTSTAlarmDlg::OnStop)
	ON_COMMAND(ID_DEVICE_MANAGE, &CKTSTAlarmDlg::OnDeviceManage)
	ON_COMMAND(ID_AUTO_RUN, &CKTSTAlarmDlg::OnAutoRun)
	ON_COMMAND(ID_AUTO_CONNECT_SERVER, &CKTSTAlarmDlg::OnAutoConnectDBServer)
	ON_COMMAND(ID_HTML_HELP, &CKTSTAlarmDlg::OnHtmlHelp)
	ON_COMMAND(ID_ABOUT, &CKTSTAlarmDlg::OnAbout)
	ON_COMMAND(ID_EXIT, &CKTSTAlarmDlg::OnCancel)
	ON_MESSAGE(WM_TRAY_NOTIFY_ICON, &CKTSTAlarmDlg::OnTrayNotifyIcon)//托盘图标消息处理函数映射
	ON_COMMAND(ID_HELP, &CKTSTAlarmDlg::OnHtmlHelp)
	ON_COMMAND(ID_SMS_SETUP, &CKTSTAlarmDlg::OnSmsSetup)
	ON_BN_CLICKED(IDC_SMS_SETUP, &CKTSTAlarmDlg::OnSmsSetup)
	ON_COMMAND(ID_RELAY_SETUP, &CKTSTAlarmDlg::OnRelaySetup)
	ON_BN_CLICKED(IDC_RELAY_SETUP, &CKTSTAlarmDlg::OnRelaySetup)
	ON_MESSAGE(WM_UDP_RECEIVE, &CKTSTAlarmDlg::OnUdpReceiveMessage)//通讯类接收数据消息
END_MESSAGE_MAP()


// CKTSTAlarmDlg 消息处理程序

BOOL CKTSTAlarmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

// 	// 将“关于...”菜单项添加到系统菜单中。
// 
// 	// IDM_ABOUTBOX 必须在系统命令范围内。
// 	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
// 	ASSERT(IDM_ABOUTBOX < 0xF000);
// 
// 	CMenu* pSysMenu = GetSystemMenu(FALSE);
// 	if (pSysMenu != NULL)
// 	{
// 		BOOL bNameValid;
// 		CString strAboutMenu;
// 		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
// 		ASSERT(bNameValid);
// 		if (!strAboutMenu.IsEmpty())
// 		{
// 			pSysMenu->AppendMenu(MF_SEPARATOR);
// 			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
// 		}
// 	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//初始打开为启动状态
	m_bStartFlag = TRUE;
	m_bOnStartModule = TRUE;

	//要根据启动状态，在托盘中添加不同的图标
	HICON hIcon;
	UINT nIconID;
	CString strTip, strLoad;
	if (m_bStartFlag)
	{
		strLoad.LoadString(IDS_STATUS_START);
		nIconID = IDI_START;
		hIcon = AfxGetApp()->LoadIcon(IDI_START);
	}
	else
	{
		strLoad.LoadString(IDS_STATUS_STOP);
		nIconID = IDI_STOP;
		hIcon = AfxGetApp()->LoadIcon(IDI_STOP);
	}
	strTip.Format(_T("%s(%s)"), AfxGetApp()->m_pszAppName, strLoad);
	SetTrayIcon(0, nIconID, strTip);

	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	//启动定时器
	m_nTimerID = this->SetTimer(m_nTimerID, 100, NULL);


	//开机后自动启动
	CRegistry reg;
	CString strRunPath;
	reg.RegReadAutoRunService(m_bAutoRun, strRunPath);


	//写入到xml文件中
	CString strFilePath = theApp.GetCFGFilePath ();
	CXMLProfile xmlFile(strFilePath);

	//自动连接数据库
	xmlFile.GetAutoConnectDB(m_bAutoConnectDB);
	if (m_bAutoRun && !m_bAutoConnectDB)
	{
		m_bAutoConnectDB = TRUE;
		xmlFile.SetAutoConnectDB(m_bAutoConnectDB);
	}

	CCommunicate *pCommunicate = CKTSTAlarmApp::GetCommunicatePtr();
	if (pCommunicate == NULL)
		return FALSE;
	pCommunicate->SetRecWnd(this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CKTSTAlarmDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CKTSTAlarmDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CKTSTAlarmDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//启动
void CKTSTAlarmDlg::OnStart() 
{
	if (m_bStartFlag)
		return;	

	m_bStartFlag = TRUE;
	m_bOnStartModule = TRUE;

	this->EnableControlWindow (m_bStartFlag);

	//要根据启动状态，在托盘中添加不同的图标
	HICON hIcon;
	UINT nIconID;
	CString strTip, strLoad;
	strLoad.LoadString(IDS_STATUS_START);
	nIconID = IDI_START;
	hIcon = AfxGetApp()->LoadIcon(IDI_START);
	strTip.Format(_T("%s(%s)"), AfxGetApp()->m_pszAppName, strLoad);
	SetTrayIcon(1, nIconID, strTip);

	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);
}

//启动系统
void CKTSTAlarmDlg::StartModule ()
{
	CWaitCursor curWait;

	//启动数据中心
	CDataCenter *pDataCenter = CDataCenter::GetInstance();
	if (pDataCenter)
		pDataCenter->Start ();
}

//停止
void CKTSTAlarmDlg::OnStop() 
{
	if (!m_bStartFlag)
		return;

	//***	输入用户名、密码校验	**************************************
	CLoginDlg dlgLogin;
	if (dlgLogin.DoModal() != IDOK)
		return;
	//********************************************************************


	this->StopModule ();	//停止系统

	m_bStartFlag = FALSE;

	this->EnableControlWindow (m_bStartFlag);

	//要根据启动状态，在托盘中添加不同的图标
	HICON hIcon;
	UINT nIconID;
	CString strTip, strLoad;
	strLoad.LoadString(IDS_STATUS_STOP);
	nIconID = IDI_STOP;
	hIcon = AfxGetApp()->LoadIcon(IDI_STOP);
	strTip.Format(_T("%s(%s)"), AfxGetApp()->m_pszAppName, strLoad);
	SetTrayIcon(1, nIconID, strTip);

	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);
}

//停止系统
void CKTSTAlarmDlg::StopModule ()
{
	CDataCenter *pDataCenter = CDataCenter::GetInstance();
	if (pDataCenter)
		pDataCenter->Stop ();
}

//设备管理
void CKTSTAlarmDlg::OnDeviceManage()
{
	CWaitCursor curWait;

	CDeviceManageDlg dlg(CKTSTAlarmApp::GetDeviceIOPtr());

	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	//传入设备信息
	int nDeviceNo, nDeviceType, nDriverType, nChannelCount, nSampleInterval;
	TCHAR szDeviceName[BUF_LEN_S], szDeviceGUID[BUF_LEN_M];
	DeviceInfo device;

	int nDBRet = dbAccess.ExecGetAllDevice ();
	if (nDBRet != RET_DB_SUCCESS)
		return;

	while (!dbAccess.IsReachEnd())
	{
		memset (szDeviceName, 0x0, sizeof(szDeviceName));
		memset (szDeviceGUID, 0x0, sizeof(szDeviceGUID));

		nDBRet = dbAccess.GetCurDevice (nDeviceNo, szDeviceName, nDeviceType, nDriverType, nChannelCount, 
										szDeviceGUID, nSampleInterval);
		if (nDBRet != RET_DB_SUCCESS)
			return;

		device.nDeviceNo = nDeviceNo;
		device.strDeviceName = szDeviceName;
		device.nDeviceType = nDeviceType;
		device.nDriverType = nDriverType;
		device.nChannelCount = nChannelCount;
		device.strDeviceGUID = szDeviceGUID;
		device.nSampleInterval = nSampleInterval;

		dlg.SetDevice(device);

		dbAccess.MoveNext();
	}

	dlg.DoModal();	//设备管理对话框特殊,关闭后必须保存,所以不用判断返回值
//	if (dlg.DoModal() == IDOK)
	{
		CDeviceInfoList *pDeviceList = dlg.GetDeviceListPtr();
		if (pDeviceList == NULL)
			return;

		DeviceInfo *pDevice = NULL;
		POSITION pos = pDeviceList->GetHeadPosition();
		while (pos)
		{
			pDevice = pDeviceList->GetNext(pos);
			if (pDevice)
			{
				switch (pDevice->nOptFlag)
				{
				case OPT_ADD:		//添加
					memset (szDeviceName, 0x0, sizeof(szDeviceName));
					memset (szDeviceGUID, 0x0, sizeof(szDeviceGUID));
					memcpy (szDeviceName, (LPCTSTR )pDevice->strDeviceName, pDevice->strDeviceName.GetLength()*sizeof(TCHAR));
					memcpy (szDeviceGUID, (LPCTSTR )pDevice->strDeviceGUID, pDevice->strDeviceGUID.GetLength()*sizeof(TCHAR));

					nDBRet = dbAccess.AddDevice(pDevice->nDeviceNo, szDeviceName, pDevice->nDeviceType, pDevice->nDriverType, 
												pDevice->nChannelCount, szDeviceGUID, pDevice->nSampleInterval);
					if (nDBRet != RET_DB_SUCCESS)
						return;

					this->AddDeviceTrigger (pDevice->nDeviceNo, pDevice->nChannelCount, pDevice->nDeviceType);

					break;

				case OPT_MODIFY:	//修改
					memset (szDeviceName, 0x0, sizeof(szDeviceName));
					memset (szDeviceGUID, 0x0, sizeof(szDeviceGUID));
					memcpy (szDeviceName, (LPCTSTR )pDevice->strDeviceName, pDevice->strDeviceName.GetLength()*sizeof(TCHAR));
					memcpy (szDeviceGUID, (LPCTSTR )pDevice->strDeviceGUID, pDevice->strDeviceGUID.GetLength()*sizeof(TCHAR));

					nDBRet = dbAccess.ModiDevice(pDevice->nDeviceNo, szDeviceName, pDevice->nDeviceType, pDevice->nDriverType, 
												pDevice->nChannelCount, szDeviceGUID, pDevice->nSampleInterval);
					if (nDBRet != RET_DB_SUCCESS)
						return;

					break;

				case OPT_DELETE:	//删除
					nDBRet = dbAccess.DelDevice(pDevice->nDeviceNo);
					if (nDBRet != RET_DB_SUCCESS)
						return;

					break;

				default:
					break;
				}
			}
		}
	}
}

BOOL CKTSTAlarmDlg::AddDeviceTrigger (int nDeviceNo, int nChannelCount, int nDeviceType)
{
	switch (nDeviceType)
	{
	case devTypeProcessSample:	//过程量采集设备
	case devTypeVideoSample:	//图像采集设备
	case devTypeRelay:			//继电器设备
		{
			int nChannelNo, nAlarmLevelDataLen, nAlarmRangeDataLen;
			TCHAR szChannelName[BUF_LEN_S], szUnitString[BUF_LEN_S];
			BOOL bWorkEnable;
			float fFullScaleHigh, fFullScaleLow, fCalibrationHigh, fCalibrationLow;
			byte byDataType, byDecimalDigits;
			BlobData<byte> bdAlarmLevelData, bdAlarmRangeData;

			//设置通道默认参数
			bWorkEnable = FALSE;
			fFullScaleHigh = 1.0f;
			fFullScaleLow = 0.0f;
			fCalibrationHigh = 1.0f;
			fCalibrationLow = 0.0f;
			switch (nDeviceType)
			{
			case devTypeProcessSample:	//过程量采集设备
				byDataType = datTypeProcess;
				break;
			case devTypeVideoSample:	//图像采集设备
				byDataType = datTypeCamera;
				break;
			default:
				byDataType = datTypeProcess;
				break;
			}
			byDecimalDigits = 1;
			memset (szUnitString, 0x0, sizeof(szUnitString));

			//报警级别的默认值
			nAlarmLevelDataLen = sizeof(COLORREF) + sizeof(byte);	//默认值缓冲区长度
			byte *pbyBuf = bdAlarmLevelData.SetData(NULL, nAlarmLevelDataLen);
			*(COLORREF *)pbyBuf = NORMAL_COLOR;		//非报警状态颜色
			pbyBuf += sizeof(COLORREF);
			*(byte *)pbyBuf = 0;					//报警级别个数
			pbyBuf += sizeof(byte);

			//报警范围的默认值
			nAlarmRangeDataLen = sizeof(byte);		//默认值缓冲区长度
			pbyBuf = bdAlarmRangeData.SetData(NULL, nAlarmRangeDataLen);
			*(byte *)pbyBuf = 0;					//报警范围个数
			pbyBuf += sizeof(byte);

			CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());
			int nDBRet;

			for (int i=0; i<nChannelCount; i++)
			{
				//只有通道号和通道名称不同，其余参数相同
				nChannelNo = i;
				memset (szChannelName, 0x0, sizeof(szChannelName));
				_stprintf_s(szChannelName, BUF_LEN_S, _T("Channel%d"), nChannelNo);

				nDBRet = dbAccess.AddChannel(nDeviceNo, nChannelNo, szChannelName, bWorkEnable, fFullScaleHigh, 
											fFullScaleLow, fCalibrationHigh, fCalibrationLow, byDataType, byDecimalDigits, 
											szUnitString, nAlarmLevelDataLen, &bdAlarmLevelData, nAlarmRangeDataLen, 
											&bdAlarmRangeData);
				if (nDBRet != RET_DB_SUCCESS)
					return FALSE;
			}
		}
		break;

	default:
		break;
	}

	return TRUE;
}

//系统启动后自动运行DPU
void CKTSTAlarmDlg::OnAutoRun()
{
	CRegistry reg;
	m_bAutoRun = !m_bAutoRun;
	CString strRunPath;

	if (m_bAutoRun)
	{
		//将当前模块路径写入注册表自动启动
		TCHAR szMainPath[_MAX_PATH];
		memset(szMainPath, 0x0, sizeof(szMainPath));
		::GetModuleFileName(NULL, szMainPath, _MAX_PATH);
		strRunPath = szMainPath;
		strRunPath += _T(" ");
		strRunPath += AUTO_RUN_CMD_LINE;

		//自动启动时必须为自动连接数据库
		if (!m_bAutoConnectDB)
		{
			m_bAutoConnectDB = TRUE;

			//写入到xml文件中
			CString strFilePath = theApp.GetCFGFilePath ();
			CXMLProfile xmlFile(strFilePath);

			//自动连接数据库
			xmlFile.SetAutoConnectDB(m_bAutoConnectDB);
		}
	}

	reg.RegWriteAutoRunService(m_bAutoRun, strRunPath);
}

//DPU启动后自动连接数据库服务器
void CKTSTAlarmDlg::OnAutoConnectDBServer()
{
	m_bAutoConnectDB = !m_bAutoConnectDB;

	//写入到xml文件中
	CString strFilePath = theApp.GetCFGFilePath ();
	CXMLProfile xmlFile(strFilePath);

	//自动连接数据库
	xmlFile.SetAutoConnectDB(m_bAutoConnectDB);
}

//帮助
void CKTSTAlarmDlg::OnHtmlHelp() 
{
	HtmlHelp (NULL, NULL);
}

void CKTSTAlarmDlg::WinHelp(DWORD dwData, UINT nCmd)
{
	HtmlHelp (NULL, NULL);

	// 	CDialogEx::WinHelp(dwData, nCmd);
}

//关于
void CKTSTAlarmDlg::OnAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}

//退出
void CKTSTAlarmDlg::OnCancel() 
{
	CString strLoad;
	strLoad.LoadString(IDS_EXIT_SOFTWARE);
	if (AfxMessageBox (strLoad, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2) != IDYES)
		return;

	CWaitCursor curWait;

	this->StopModule ();	//停止系统

	KillTimer(m_nTimerID);
	DeleteTrayIcon();

	CDialogEx::OnCancel();
}

//关闭到托盘
void CKTSTAlarmDlg::OnClose() 
{
	ShowWindow(SW_HIDE);

	//	CDialogEx::OnClose();
}

////////////////////////////////////////////////////////////////////////
//函数说明：托盘图标消息处理函数（左键单击、左键双击、右键点击、其他）
//参数说明：wParam[in]
//          lParam[in]
///////////////////////////////////////////////////////////////////////
LRESULT CKTSTAlarmDlg::OnTrayNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	//如果托盘图标关联的主窗体拥有其他的子窗口并且此时激活的窗口并不是主窗体，便不对托盘消息进行任何响应
	if (!this->IsWindowEnabled())//bwh-09-9-3;只要主窗口不能收到鼠标和键盘消息，就返回；
	{
		CWnd *pWndActive = this->GetLastActivePopup();

		if (pWndActive!=NULL && pWndActive->m_hWnd!=NULL && ::IsWindow(pWndActive->m_hWnd)
			&& (lParam==WM_LBUTTONDOWN || lParam==WM_RBUTTONDOWN))
		{
			::SetForegroundWindow(pWndActive->m_hWnd);
		}
		return 0L;
	}

	CMenu *pSubMenu, TrayMenu;
	CPoint pt;
	pSubMenu = NULL;
	UINT nState;

	switch(lParam)
	{
	case WM_LBUTTONDBLCLK:
		// 		m_bVisable = TRUE;
		// 		ShowWindow(SW_SHOWNORMAL);
		// 		::SetForegroundWindow(m_hWnd);
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		GetCursorPos(&pt);

		if(!TrayMenu.LoadMenu(IDR_MENU_TRAY))
			return 0L;

		pSubMenu = TrayMenu.GetSubMenu(0);
		::SetMenuDefaultItem(pSubMenu->m_hMenu, 0, TRUE);

		if (m_bStartFlag)
		{
			pSubMenu->EnableMenuItem(ID_START, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_STOP, MF_BYCOMMAND | MF_ENABLED);
			pSubMenu->EnableMenuItem(ID_DEVICE_MANAGE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
// 			pSubMenu->EnableMenuItem(ID_CHANNEL_MANAGE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_SMS_SETUP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_RELAY_SETUP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else
		{
			pSubMenu->EnableMenuItem(ID_START, MF_BYCOMMAND | MF_ENABLED);
			pSubMenu->EnableMenuItem(ID_STOP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_DEVICE_MANAGE, MF_BYCOMMAND | MF_ENABLED);
// 			pSubMenu->EnableMenuItem(ID_CHANNEL_MANAGE, MF_BYCOMMAND | MF_ENABLED);
			pSubMenu->EnableMenuItem(ID_SMS_SETUP, MF_BYCOMMAND | MF_ENABLED);
			pSubMenu->EnableMenuItem(ID_RELAY_SETUP, MF_BYCOMMAND | MF_ENABLED);
		}

		if (m_bAutoRun)
		{
			pSubMenu->EnableMenuItem(ID_AUTO_RUN, MF_BYCOMMAND | MF_ENABLED);
			pSubMenu->CheckMenuItem(ID_AUTO_RUN, MF_BYCOMMAND | MF_CHECKED);
		}
		else
		{
			pSubMenu->EnableMenuItem(ID_AUTO_RUN, MF_BYCOMMAND | MF_ENABLED);
			pSubMenu->CheckMenuItem(ID_AUTO_RUN, MF_BYCOMMAND | MF_UNCHECKED);
		}

		nState = m_bAutoRun ? (MF_DISABLED | MF_GRAYED) : MF_ENABLED;
		if (m_bAutoConnectDB)
		{
			pSubMenu->EnableMenuItem(ID_AUTO_CONNECT_SERVER, MF_BYCOMMAND | nState);
			pSubMenu->CheckMenuItem(ID_AUTO_CONNECT_SERVER, MF_BYCOMMAND | MF_CHECKED);
		}
		else
		{
			pSubMenu->EnableMenuItem(ID_AUTO_CONNECT_SERVER, MF_BYCOMMAND | nState);
			pSubMenu->CheckMenuItem(ID_AUTO_CONNECT_SERVER, MF_BYCOMMAND | MF_UNCHECKED);
		}

		::SetForegroundWindow(m_hWnd);	//使主窗口前端显示，这样当不对托盘右键菜单操作时，菜单可以消失
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this);
		break;

	case WM_RBUTTONUP:
		break;

	default:
		break;
	}

	return 0L;
}

////////////////////////////////////////////////////////////////////////
//函数说明：托盘图标显示
//参数说明：int nFlag[in]       0表示添加托盘图标；1表示修改托盘图标
//			UINT nIDTraIcon[in] 托盘图标资源ID
//			CString strTip[in]  托盘图标提示信息
//返回值说明：修改成功返回TRUE
//            修改失败返回FALSE
///////////////////////////////////////////////////////////////////////
BOOL CKTSTAlarmDlg::SetTrayIcon(int nFlag, UINT nIDTrayIcon, CString strTip)
{
	HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nIDTrayIcon));//加载停止图标

	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_TRAY_NOTIFY_ICON;//自定义消息
	nid.hIcon = hIcon;

	memset (nid.szTip, 0x0, 64*sizeof(TCHAR));
	if (strTip.GetLength() > 63)
		memcpy(nid.szTip, (LPCTSTR)strTip, 63*sizeof(TCHAR));
	else
		memcpy(nid.szTip, (LPCTSTR)strTip, strTip.GetLength()*sizeof(TCHAR));

	DWORD dwMessage;
	switch (nFlag)
	{
	case 0:
		dwMessage = NIM_ADD;
		break;
	case 1:
		dwMessage = NIM_MODIFY;
		break;
	default:
		break;
	}

	BOOL bRes = Shell_NotifyIcon(dwMessage, &nid);//获取修改信息

	if (hIcon)
		DestroyIcon(hIcon);

	return bRes;
}

////////////////////////////////////////////////////////////////////////
//函数说明：在程序停止时在任务栏通知区域自动删除托盘图标
//返回值说明：删除成功返回TRUE
//            删除失败返回FALSE
///////////////////////////////////////////////////////////////////////
BOOL CKTSTAlarmDlg::DeleteTrayIcon()
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = m_hWnd;
	nid.uID = IDR_MAINFRAME;

	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

////////////////////////////////////////////////////////////////////////
//函数说明：程序运行时自动最小化托盘显示
///////////////////////////////////////////////////////////////////////
void CKTSTAlarmDlg::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CDialogEx::OnWindowPosChanging(lpwndpos);

	if (!m_bVisable && (lpwndpos->flags & SWP_SHOWWINDOW) == SWP_SHOWWINDOW)
	{
		m_nShowWndFuncCnt ++;
		if (m_nShowWndFuncCnt == 2)	//实验得出，进程刚启动时会有两次带有SWP_SHOWWINDOW标志	zhjb
			m_bVisable = TRUE;

		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
}

void CKTSTAlarmDlg::OnTimer(UINT nIDEvent) 
{
	if (!m_bStartFlag)	//注意：停止系统状态也要向下执行OnTimer，因为停止时可能会进行通道校验和寻找键相
		return;

	if (m_bOnStartModule)
	{
		m_bOnStartModule = FALSE;
		this->StartModule ();
	}

	CDialogEx::OnTimer(nIDEvent);
}

//控制控件窗口变灰变亮
void CKTSTAlarmDlg::EnableControlWindow(BOOL bStartFlag)
{
	GetDlgItem(IDC_START)->EnableWindow(!bStartFlag);
	GetDlgItem(IDC_STOP)->EnableWindow(bStartFlag);
	GetDlgItem(IDC_DEVICE_MANAGE)->EnableWindow(!bStartFlag);
// 	GetDlgItem(IDC_CHANNEL_MANAGE)->EnableWindow(!bStartFlag);
	GetDlgItem(IDC_SMS_SETUP)->EnableWindow(!bStartFlag);
	GetDlgItem(IDC_RELAY_SETUP)->EnableWindow(!bStartFlag);
}

//短信模块设置
void CKTSTAlarmDlg::OnSmsSetup()
{
	CWaitCursor curWait;
	CSMSSetupDlg dlg;

	//得到本地GUID
	TCHAR szGUID[BUF_LEN_M];
	memset (szGUID, 0x0, sizeof(szGUID));
	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO)
		pDeviceIO->GetLcdeGid(szGUID);
	CString strLocalGUID = szGUID;


	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	//传入设备信息
	int nDeviceNo, nDeviceType, nDriverType, nChannelCount, nSampleInterval, nDefBufLen;
	TCHAR szDeviceName[BUF_LEN_S], szDeviceGUID[BUF_LEN_M];
	BlobData<byte> bdDefBuf;
	DeviceInfo device;
	CArray<int, int> arrDeviceNo;	//保存采集设备号

	int nDBRet = dbAccess.ExecGetAllDevice ();
	if (nDBRet != RET_DB_SUCCESS)
		return;

	while (!dbAccess.IsReachEnd())
	{
		memset (szDeviceName, 0x0, sizeof(szDeviceName));
		memset (szDeviceGUID, 0x0, sizeof(szDeviceGUID));

		nDBRet = dbAccess.GetCurDevice (nDeviceNo, szDeviceName, nDeviceType, nDriverType, nChannelCount, 
										szDeviceGUID, nSampleInterval);
		if (nDBRet != RET_DB_SUCCESS)
			return;
		nDBRet = dbAccess.GetCurDefBufData (nDefBufLen, &bdDefBuf);
		if (nDBRet != RET_DB_SUCCESS)
			return;

		device.nDeviceNo = nDeviceNo;
		device.strDeviceName = szDeviceName;
		device.nDeviceType = nDeviceType;
		device.nDriverType = nDriverType;
		device.nChannelCount = nChannelCount;
		device.strDeviceGUID = szDeviceGUID;
		device.nSampleInterval = nSampleInterval;
		device.bdDefBuf = bdDefBuf;

		if (nDeviceType == devTypeDriver && nDriverType == dvrTypeSMSDevice)	//短信设备
		{
			if (_tcscmp(szGUID, szDeviceGUID) == 0)
				dlg.SetSMSDevice(device);
		}

		if (nDeviceType == devTypeProcessSample)	//采集设备
		{
			arrDeviceNo.Add(nDeviceNo);		//保存采集设备号

			dlg.SetCollectionDevice(device);
		}

		dbAccess.MoveNext();
	}

	//传入通道信息
	int nChannelNo, nAlarmLevelDataLen, nAlarmRangeDataLen;
	TCHAR szChannelName[BUF_LEN_S], szUnitString[BUF_LEN_S];
	BOOL bWorkEnable;
	float fFullScaleHigh, fFullScaleLow, fCalibrationHigh, fCalibrationLow;
	byte byDataType, byDecimalDigits;
	BlobData<byte> bdAlarmLevelData, bdAlarmRangeData;
	ChannelInfo channel;

	for (int i=0; i<arrDeviceNo.GetCount(); i++)	//只传入采集设备的通道
	{
		nDBRet = dbAccess.ExecGetAllChannel (arrDeviceNo.GetAt(i));
		if (nDBRet != RET_DB_SUCCESS)
			return;

		while (!dbAccess.IsReachEnd())
		{
			memset (szChannelName, 0x0, sizeof(szChannelName));
			memset (szUnitString, 0x0, sizeof(szUnitString));

			nDBRet = dbAccess.GetCurChannel (nDeviceNo, nChannelNo, szChannelName, bWorkEnable, fFullScaleHigh, 
											fFullScaleLow, fCalibrationHigh, fCalibrationLow, byDataType, 
											byDecimalDigits, szUnitString, nAlarmLevelDataLen, &bdAlarmLevelData, 
											nAlarmRangeDataLen, &bdAlarmRangeData);
			if (nDBRet != RET_DB_SUCCESS)
				return;

			nDBRet = dbAccess.GetCurDefBufData (nDefBufLen, &bdDefBuf);
			if (nDBRet != RET_DB_SUCCESS)
				return;

			channel.nDeviceNo = nDeviceNo;
			channel.nChannelNo = nChannelNo;
			channel.strChannelName = szChannelName;
			channel.bWorkEnable = bWorkEnable;
			channel.fFullScaleHigh = fFullScaleHigh;
			channel.fFullScaleLow = fFullScaleLow;
			channel.fCalibrationHigh = fCalibrationHigh;
			channel.fCalibrationLow = fCalibrationLow;
			channel.byDataType = byDataType;
			channel.byDecimalDigits = byDecimalDigits;
			channel.strUnitString = szUnitString;
			channel.bdAlarmLevelData = bdAlarmLevelData;
			channel.bdAlarmRangeData = bdAlarmRangeData;
			channel.bdDefBuf = bdDefBuf;

			dlg.SetCollectionChannel(channel);

			dbAccess.MoveNext();
		}
	}


	if (dlg.DoModal() == IDOK)
	{
		CSMSDeviceList *pSMSDeviceList = dlg.GetSMSDeviceListPtr ();
		if (pSMSDeviceList == NULL)
			return;

		SMSDevice *pDevice = NULL;
		POSITION pos = pSMSDeviceList->GetHeadPosition();
		while (pos)
		{
			pDevice = pSMSDeviceList->GetNext(pos);
			if (pDevice == NULL)
				continue;

			switch (pDevice->m_nOptFlag)
			{
			case OPT_ADD:		//添加
				break;

			case OPT_MODIFY:	//修改
				nDBRet = dbAccess.ModiDefBufData(TABLE_DEVICE, 
												pDevice->m_nDeviceNo, 
												0, 
												pDevice->m_bdDefBuf.GetSize(), 
												&pDevice->m_bdDefBuf);
				if (nDBRet != RET_DB_SUCCESS)
					return;

				break;

			case OPT_DELETE:	//删除
				break;

			default:
				break;
			}
		}
	}
}

//继电器模块设置
void CKTSTAlarmDlg::OnRelaySetup()
{
	CWaitCursor curWait;
	CRelayManageDlg dlg;

	//得到本地GUID
	TCHAR szGUID[BUF_LEN_M];
	memset (szGUID, 0x0, sizeof(szGUID));
	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO)
		pDeviceIO->GetLcdeGid(szGUID);
	CString strLocalGUID = szGUID;


	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	//传入设备信息
	int nDeviceNo, nDeviceType, nDriverType, nChannelCount, nSampleInterval, nDefBufLen;
	TCHAR szDeviceName[BUF_LEN_S], szDeviceGUID[BUF_LEN_M];
	BlobData<byte> bdDefBuf;
	DeviceInfo device;
	CArray<int, int> arrDeviceNo, arrDeviceType;	//保存采集设备号

	int nDBRet = dbAccess.ExecGetAllDevice ();
	if (nDBRet != RET_DB_SUCCESS)
		return;

	while (!dbAccess.IsReachEnd())
	{
		memset (szDeviceName, 0x0, sizeof(szDeviceName));
		memset (szDeviceGUID, 0x0, sizeof(szDeviceGUID));

		nDBRet = dbAccess.GetCurDevice (nDeviceNo, szDeviceName, nDeviceType, nDriverType, nChannelCount, 
										szDeviceGUID, nSampleInterval);
		if (nDBRet != RET_DB_SUCCESS)
			return;
		nDBRet = dbAccess.GetCurDefBufData (nDefBufLen, &bdDefBuf);
		if (nDBRet != RET_DB_SUCCESS)
			return;

		device.nDeviceNo = nDeviceNo;
		device.strDeviceName = szDeviceName;
		device.nDeviceType = nDeviceType;
		device.nDriverType = nDriverType;
		device.nChannelCount = nChannelCount;
		device.strDeviceGUID = szDeviceGUID;
		device.nSampleInterval = nSampleInterval;
		device.bdDefBuf = bdDefBuf;

		if (nDeviceType == devTypeRelay)	//继电器设备
		{
			if (_tcscmp(szGUID, szDeviceGUID) == 0)
			{
				arrDeviceNo.Add(nDeviceNo);		//保存设备号
				arrDeviceType.Add(nDeviceType);	//保存设备类型

				dlg.SetRelayDevice(device);
			}
		}

		if (nDeviceType == devTypeProcessSample)	//采集设备
		{
			arrDeviceNo.Add(nDeviceNo);		//保存设备号
			arrDeviceType.Add(nDeviceType);

			dlg.SetCollectionDevice(device);
		}

		dbAccess.MoveNext();
	}

	//传入通道信息
	int nChannelNo, nAlarmLevelDataLen, nAlarmRangeDataLen;
	TCHAR szChannelName[BUF_LEN_S], szUnitString[BUF_LEN_S];
	BOOL bWorkEnable;
	float fFullScaleHigh, fFullScaleLow, fCalibrationHigh, fCalibrationLow;
	byte byDataType, byDecimalDigits;
	BlobData<byte> bdAlarmLevelData, bdAlarmRangeData;
	ChannelInfo channel;

	for (int i=0; i<arrDeviceNo.GetCount(); i++)	//只传入采集设备的通道
	{
		nDBRet = dbAccess.ExecGetAllChannel (arrDeviceNo.GetAt(i));
		if (nDBRet != RET_DB_SUCCESS)
			return;

		while (!dbAccess.IsReachEnd())
		{
			memset (szChannelName, 0x0, sizeof(szChannelName));
			memset (szUnitString, 0x0, sizeof(szUnitString));

			nDBRet = dbAccess.GetCurChannel (nDeviceNo, nChannelNo, szChannelName, bWorkEnable, fFullScaleHigh, 
											fFullScaleLow, fCalibrationHigh, fCalibrationLow, byDataType, 
											byDecimalDigits, szUnitString, nAlarmLevelDataLen, &bdAlarmLevelData, 
											nAlarmRangeDataLen, &bdAlarmRangeData);
			if (nDBRet != RET_DB_SUCCESS)
				return;

			nDBRet = dbAccess.GetCurDefBufData (nDefBufLen, &bdDefBuf);
			if (nDBRet != RET_DB_SUCCESS)
				return;

			channel.nDeviceNo = nDeviceNo;
			channel.nChannelNo = nChannelNo;
			channel.strChannelName = szChannelName;
			channel.bWorkEnable = bWorkEnable;
			channel.fFullScaleHigh = fFullScaleHigh;
			channel.fFullScaleLow = fFullScaleLow;
			channel.fCalibrationHigh = fCalibrationHigh;
			channel.fCalibrationLow = fCalibrationLow;
			channel.byDataType = byDataType;
			channel.byDecimalDigits = byDecimalDigits;
			channel.strUnitString = szUnitString;
			channel.bdAlarmLevelData = bdAlarmLevelData;
			channel.bdAlarmRangeData = bdAlarmRangeData;
			channel.bdDefBuf = bdDefBuf;

			if (arrDeviceType.GetAt(i) == devTypeRelay)	//继电器设备
				dlg.SetRelayChannel(channel);

			if (arrDeviceType.GetAt(i) == devTypeProcessSample)	//采集设备
				dlg.SetCollectionChannel(channel);

			dbAccess.MoveNext();
		}
	}


	if (dlg.DoModal() == IDOK)
	{
		CRelayChannelList *pRelayChannelList = dlg.GetRelayChannelListPtr ();
		if (pRelayChannelList == NULL)
			return;

		RelayChannel *pChannel = NULL;
		POSITION pos = pRelayChannelList->GetHeadPosition();
		while (pos)
		{
			pChannel = pRelayChannelList->GetNext(pos);
			if (pChannel == NULL)
				continue;

			switch (pChannel->m_nOptFlag)
			{
			case OPT_ADD:		//添加
				break;

			case OPT_MODIFY:	//修改
				memset (szChannelName, 0x0, sizeof(szChannelName));
				memset (szUnitString, 0x0, sizeof(szUnitString));
				memcpy (szChannelName, (LPCTSTR )pChannel->m_strChannelName, pChannel->m_strChannelName.GetLength()*sizeof(TCHAR));
				memcpy (szUnitString, (LPCTSTR )pChannel->m_strUnitString, pChannel->m_strUnitString.GetLength()*sizeof(TCHAR));

				nDBRet = dbAccess.ModiChannel(pChannel->m_nDeviceNo, 
											pChannel->m_nChannelNo, 
											szChannelName, 
											pChannel->m_bWorkEnable, 
											pChannel->m_fFullScaleHigh, 
											pChannel->m_fFullScaleLow, 
											pChannel->m_fCalibrationHigh, 
											pChannel->m_fCalibrationLow, 
											pChannel->m_byDataType, 
											pChannel->m_byDecimalDigits, 
											szUnitString, 
											pChannel->m_bdAlarmLevelData.GetSize(), 
											&pChannel->m_bdAlarmLevelData, 
											pChannel->m_bdAlarmRangeData.GetSize(), 
											&pChannel->m_bdAlarmRangeData);
				if (nDBRet != RET_DB_SUCCESS)
					return;

				nDBRet = dbAccess.ModiDefBufData(TABLE_CHANNEL, 
												pChannel->m_nDeviceNo, 
												pChannel->m_nChannelNo, 
												pChannel->m_bdDefBuf.GetSize(), 
												&pChannel->m_bdDefBuf);
				if (nDBRet != RET_DB_SUCCESS)
					return;

				break;

			case OPT_DELETE:	//删除
				break;

			default:
				break;
			}
		}
	}
}

//*************************************************************************************************
//	功能：	收到UDP消息
//	参数：	WPARAM wParam[in]，缓冲区地址
//			LPARAM lParam[in]，长度
//*************************************************************************************************
LRESULT CKTSTAlarmDlg::OnUdpReceiveMessage (WPARAM wParam, LPARAM lParam)
{
	//由数据中心处理
	CDataCenter *pDataCenter = CDataCenter::GetInstance();
	if (pDataCenter)
		return pDataCenter->OnUdpReceiveMessage (wParam, lParam);

	return S_OK;
}
