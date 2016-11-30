
// KTST AlarmDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CKTSTAlarmDlg �Ի���




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
	ON_MESSAGE(WM_TRAY_NOTIFY_ICON, &CKTSTAlarmDlg::OnTrayNotifyIcon)//����ͼ����Ϣ������ӳ��
	ON_COMMAND(ID_HELP, &CKTSTAlarmDlg::OnHtmlHelp)
	ON_COMMAND(ID_SMS_SETUP, &CKTSTAlarmDlg::OnSmsSetup)
	ON_BN_CLICKED(IDC_SMS_SETUP, &CKTSTAlarmDlg::OnSmsSetup)
	ON_COMMAND(ID_RELAY_SETUP, &CKTSTAlarmDlg::OnRelaySetup)
	ON_BN_CLICKED(IDC_RELAY_SETUP, &CKTSTAlarmDlg::OnRelaySetup)
	ON_MESSAGE(WM_UDP_RECEIVE, &CKTSTAlarmDlg::OnUdpReceiveMessage)//ͨѶ�����������Ϣ
END_MESSAGE_MAP()


// CKTSTAlarmDlg ��Ϣ�������

BOOL CKTSTAlarmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

// 	// ��������...���˵�����ӵ�ϵͳ�˵��С�
// 
// 	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	//��ʼ��Ϊ����״̬
	m_bStartFlag = TRUE;
	m_bOnStartModule = TRUE;

	//Ҫ��������״̬������������Ӳ�ͬ��ͼ��
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

	//������ʱ��
	m_nTimerID = this->SetTimer(m_nTimerID, 100, NULL);


	//�������Զ�����
	CRegistry reg;
	CString strRunPath;
	reg.RegReadAutoRunService(m_bAutoRun, strRunPath);


	//д�뵽xml�ļ���
	CString strFilePath = theApp.GetCFGFilePath ();
	CXMLProfile xmlFile(strFilePath);

	//�Զ��������ݿ�
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CKTSTAlarmDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CKTSTAlarmDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//����
void CKTSTAlarmDlg::OnStart() 
{
	if (m_bStartFlag)
		return;	

	m_bStartFlag = TRUE;
	m_bOnStartModule = TRUE;

	this->EnableControlWindow (m_bStartFlag);

	//Ҫ��������״̬������������Ӳ�ͬ��ͼ��
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

//����ϵͳ
void CKTSTAlarmDlg::StartModule ()
{
	CWaitCursor curWait;

	//������������
	CDataCenter *pDataCenter = CDataCenter::GetInstance();
	if (pDataCenter)
		pDataCenter->Start ();
}

//ֹͣ
void CKTSTAlarmDlg::OnStop() 
{
	if (!m_bStartFlag)
		return;

	//***	�����û���������У��	**************************************
	CLoginDlg dlgLogin;
	if (dlgLogin.DoModal() != IDOK)
		return;
	//********************************************************************


	this->StopModule ();	//ֹͣϵͳ

	m_bStartFlag = FALSE;

	this->EnableControlWindow (m_bStartFlag);

	//Ҫ��������״̬������������Ӳ�ͬ��ͼ��
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

//ֹͣϵͳ
void CKTSTAlarmDlg::StopModule ()
{
	CDataCenter *pDataCenter = CDataCenter::GetInstance();
	if (pDataCenter)
		pDataCenter->Stop ();
}

//�豸����
void CKTSTAlarmDlg::OnDeviceManage()
{
	CWaitCursor curWait;

	CDeviceManageDlg dlg(CKTSTAlarmApp::GetDeviceIOPtr());

	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	//�����豸��Ϣ
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

	dlg.DoModal();	//�豸����Ի�������,�رպ���뱣��,���Բ����жϷ���ֵ
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
				case OPT_ADD:		//���
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

				case OPT_MODIFY:	//�޸�
					memset (szDeviceName, 0x0, sizeof(szDeviceName));
					memset (szDeviceGUID, 0x0, sizeof(szDeviceGUID));
					memcpy (szDeviceName, (LPCTSTR )pDevice->strDeviceName, pDevice->strDeviceName.GetLength()*sizeof(TCHAR));
					memcpy (szDeviceGUID, (LPCTSTR )pDevice->strDeviceGUID, pDevice->strDeviceGUID.GetLength()*sizeof(TCHAR));

					nDBRet = dbAccess.ModiDevice(pDevice->nDeviceNo, szDeviceName, pDevice->nDeviceType, pDevice->nDriverType, 
												pDevice->nChannelCount, szDeviceGUID, pDevice->nSampleInterval);
					if (nDBRet != RET_DB_SUCCESS)
						return;

					break;

				case OPT_DELETE:	//ɾ��
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
	case devTypeProcessSample:	//�������ɼ��豸
	case devTypeVideoSample:	//ͼ��ɼ��豸
	case devTypeRelay:			//�̵����豸
		{
			int nChannelNo, nAlarmLevelDataLen, nAlarmRangeDataLen;
			TCHAR szChannelName[BUF_LEN_S], szUnitString[BUF_LEN_S];
			BOOL bWorkEnable;
			float fFullScaleHigh, fFullScaleLow, fCalibrationHigh, fCalibrationLow;
			byte byDataType, byDecimalDigits;
			BlobData<byte> bdAlarmLevelData, bdAlarmRangeData;

			//����ͨ��Ĭ�ϲ���
			bWorkEnable = FALSE;
			fFullScaleHigh = 1.0f;
			fFullScaleLow = 0.0f;
			fCalibrationHigh = 1.0f;
			fCalibrationLow = 0.0f;
			switch (nDeviceType)
			{
			case devTypeProcessSample:	//�������ɼ��豸
				byDataType = datTypeProcess;
				break;
			case devTypeVideoSample:	//ͼ��ɼ��豸
				byDataType = datTypeCamera;
				break;
			default:
				byDataType = datTypeProcess;
				break;
			}
			byDecimalDigits = 1;
			memset (szUnitString, 0x0, sizeof(szUnitString));

			//���������Ĭ��ֵ
			nAlarmLevelDataLen = sizeof(COLORREF) + sizeof(byte);	//Ĭ��ֵ����������
			byte *pbyBuf = bdAlarmLevelData.SetData(NULL, nAlarmLevelDataLen);
			*(COLORREF *)pbyBuf = NORMAL_COLOR;		//�Ǳ���״̬��ɫ
			pbyBuf += sizeof(COLORREF);
			*(byte *)pbyBuf = 0;					//�����������
			pbyBuf += sizeof(byte);

			//������Χ��Ĭ��ֵ
			nAlarmRangeDataLen = sizeof(byte);		//Ĭ��ֵ����������
			pbyBuf = bdAlarmRangeData.SetData(NULL, nAlarmRangeDataLen);
			*(byte *)pbyBuf = 0;					//������Χ����
			pbyBuf += sizeof(byte);

			CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());
			int nDBRet;

			for (int i=0; i<nChannelCount; i++)
			{
				//ֻ��ͨ���ź�ͨ�����Ʋ�ͬ�����������ͬ
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

//ϵͳ�������Զ�����DPU
void CKTSTAlarmDlg::OnAutoRun()
{
	CRegistry reg;
	m_bAutoRun = !m_bAutoRun;
	CString strRunPath;

	if (m_bAutoRun)
	{
		//����ǰģ��·��д��ע����Զ�����
		TCHAR szMainPath[_MAX_PATH];
		memset(szMainPath, 0x0, sizeof(szMainPath));
		::GetModuleFileName(NULL, szMainPath, _MAX_PATH);
		strRunPath = szMainPath;
		strRunPath += _T(" ");
		strRunPath += AUTO_RUN_CMD_LINE;

		//�Զ�����ʱ����Ϊ�Զ��������ݿ�
		if (!m_bAutoConnectDB)
		{
			m_bAutoConnectDB = TRUE;

			//д�뵽xml�ļ���
			CString strFilePath = theApp.GetCFGFilePath ();
			CXMLProfile xmlFile(strFilePath);

			//�Զ��������ݿ�
			xmlFile.SetAutoConnectDB(m_bAutoConnectDB);
		}
	}

	reg.RegWriteAutoRunService(m_bAutoRun, strRunPath);
}

//DPU�������Զ��������ݿ������
void CKTSTAlarmDlg::OnAutoConnectDBServer()
{
	m_bAutoConnectDB = !m_bAutoConnectDB;

	//д�뵽xml�ļ���
	CString strFilePath = theApp.GetCFGFilePath ();
	CXMLProfile xmlFile(strFilePath);

	//�Զ��������ݿ�
	xmlFile.SetAutoConnectDB(m_bAutoConnectDB);
}

//����
void CKTSTAlarmDlg::OnHtmlHelp() 
{
	HtmlHelp (NULL, NULL);
}

void CKTSTAlarmDlg::WinHelp(DWORD dwData, UINT nCmd)
{
	HtmlHelp (NULL, NULL);

	// 	CDialogEx::WinHelp(dwData, nCmd);
}

//����
void CKTSTAlarmDlg::OnAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}

//�˳�
void CKTSTAlarmDlg::OnCancel() 
{
	CString strLoad;
	strLoad.LoadString(IDS_EXIT_SOFTWARE);
	if (AfxMessageBox (strLoad, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2) != IDYES)
		return;

	CWaitCursor curWait;

	this->StopModule ();	//ֹͣϵͳ

	KillTimer(m_nTimerID);
	DeleteTrayIcon();

	CDialogEx::OnCancel();
}

//�رյ�����
void CKTSTAlarmDlg::OnClose() 
{
	ShowWindow(SW_HIDE);

	//	CDialogEx::OnClose();
}

////////////////////////////////////////////////////////////////////////
//����˵��������ͼ����Ϣ��������������������˫�����Ҽ������������
//����˵����wParam[in]
//          lParam[in]
///////////////////////////////////////////////////////////////////////
LRESULT CKTSTAlarmDlg::OnTrayNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	//�������ͼ�������������ӵ���������Ӵ��ڲ��Ҵ�ʱ����Ĵ��ڲ����������壬�㲻��������Ϣ�����κ���Ӧ
	if (!this->IsWindowEnabled())//bwh-09-9-3;ֻҪ�����ڲ����յ����ͼ�����Ϣ���ͷ��أ�
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

		::SetForegroundWindow(m_hWnd);	//ʹ������ǰ����ʾ�����������������Ҽ��˵�����ʱ���˵�������ʧ
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
//����˵��������ͼ����ʾ
//����˵����int nFlag[in]       0��ʾ�������ͼ�ꣻ1��ʾ�޸�����ͼ��
//			UINT nIDTraIcon[in] ����ͼ����ԴID
//			CString strTip[in]  ����ͼ����ʾ��Ϣ
//����ֵ˵�����޸ĳɹ�����TRUE
//            �޸�ʧ�ܷ���FALSE
///////////////////////////////////////////////////////////////////////
BOOL CKTSTAlarmDlg::SetTrayIcon(int nFlag, UINT nIDTrayIcon, CString strTip)
{
	HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nIDTrayIcon));//����ֹͣͼ��

	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_TRAY_NOTIFY_ICON;//�Զ�����Ϣ
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

	BOOL bRes = Shell_NotifyIcon(dwMessage, &nid);//��ȡ�޸���Ϣ

	if (hIcon)
		DestroyIcon(hIcon);

	return bRes;
}

////////////////////////////////////////////////////////////////////////
//����˵�����ڳ���ֹͣʱ��������֪ͨ�����Զ�ɾ������ͼ��
//����ֵ˵����ɾ���ɹ�����TRUE
//            ɾ��ʧ�ܷ���FALSE
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
//����˵������������ʱ�Զ���С��������ʾ
///////////////////////////////////////////////////////////////////////
void CKTSTAlarmDlg::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CDialogEx::OnWindowPosChanging(lpwndpos);

	if (!m_bVisable && (lpwndpos->flags & SWP_SHOWWINDOW) == SWP_SHOWWINDOW)
	{
		m_nShowWndFuncCnt ++;
		if (m_nShowWndFuncCnt == 2)	//ʵ��ó������̸�����ʱ�������δ���SWP_SHOWWINDOW��־	zhjb
			m_bVisable = TRUE;

		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
}

void CKTSTAlarmDlg::OnTimer(UINT nIDEvent) 
{
	if (!m_bStartFlag)	//ע�⣺ֹͣϵͳ״̬ҲҪ����ִ��OnTimer����Ϊֹͣʱ���ܻ����ͨ��У���Ѱ�Ҽ���
		return;

	if (m_bOnStartModule)
	{
		m_bOnStartModule = FALSE;
		this->StartModule ();
	}

	CDialogEx::OnTimer(nIDEvent);
}

//���ƿؼ����ڱ�ұ���
void CKTSTAlarmDlg::EnableControlWindow(BOOL bStartFlag)
{
	GetDlgItem(IDC_START)->EnableWindow(!bStartFlag);
	GetDlgItem(IDC_STOP)->EnableWindow(bStartFlag);
	GetDlgItem(IDC_DEVICE_MANAGE)->EnableWindow(!bStartFlag);
// 	GetDlgItem(IDC_CHANNEL_MANAGE)->EnableWindow(!bStartFlag);
	GetDlgItem(IDC_SMS_SETUP)->EnableWindow(!bStartFlag);
	GetDlgItem(IDC_RELAY_SETUP)->EnableWindow(!bStartFlag);
}

//����ģ������
void CKTSTAlarmDlg::OnSmsSetup()
{
	CWaitCursor curWait;
	CSMSSetupDlg dlg;

	//�õ�����GUID
	TCHAR szGUID[BUF_LEN_M];
	memset (szGUID, 0x0, sizeof(szGUID));
	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO)
		pDeviceIO->GetLcdeGid(szGUID);
	CString strLocalGUID = szGUID;


	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	//�����豸��Ϣ
	int nDeviceNo, nDeviceType, nDriverType, nChannelCount, nSampleInterval, nDefBufLen;
	TCHAR szDeviceName[BUF_LEN_S], szDeviceGUID[BUF_LEN_M];
	BlobData<byte> bdDefBuf;
	DeviceInfo device;
	CArray<int, int> arrDeviceNo;	//����ɼ��豸��

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

		if (nDeviceType == devTypeDriver && nDriverType == dvrTypeSMSDevice)	//�����豸
		{
			if (_tcscmp(szGUID, szDeviceGUID) == 0)
				dlg.SetSMSDevice(device);
		}

		if (nDeviceType == devTypeProcessSample)	//�ɼ��豸
		{
			arrDeviceNo.Add(nDeviceNo);		//����ɼ��豸��

			dlg.SetCollectionDevice(device);
		}

		dbAccess.MoveNext();
	}

	//����ͨ����Ϣ
	int nChannelNo, nAlarmLevelDataLen, nAlarmRangeDataLen;
	TCHAR szChannelName[BUF_LEN_S], szUnitString[BUF_LEN_S];
	BOOL bWorkEnable;
	float fFullScaleHigh, fFullScaleLow, fCalibrationHigh, fCalibrationLow;
	byte byDataType, byDecimalDigits;
	BlobData<byte> bdAlarmLevelData, bdAlarmRangeData;
	ChannelInfo channel;

	for (int i=0; i<arrDeviceNo.GetCount(); i++)	//ֻ����ɼ��豸��ͨ��
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
			case OPT_ADD:		//���
				break;

			case OPT_MODIFY:	//�޸�
				nDBRet = dbAccess.ModiDefBufData(TABLE_DEVICE, 
												pDevice->m_nDeviceNo, 
												0, 
												pDevice->m_bdDefBuf.GetSize(), 
												&pDevice->m_bdDefBuf);
				if (nDBRet != RET_DB_SUCCESS)
					return;

				break;

			case OPT_DELETE:	//ɾ��
				break;

			default:
				break;
			}
		}
	}
}

//�̵���ģ������
void CKTSTAlarmDlg::OnRelaySetup()
{
	CWaitCursor curWait;
	CRelayManageDlg dlg;

	//�õ�����GUID
	TCHAR szGUID[BUF_LEN_M];
	memset (szGUID, 0x0, sizeof(szGUID));
	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO)
		pDeviceIO->GetLcdeGid(szGUID);
	CString strLocalGUID = szGUID;


	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	//�����豸��Ϣ
	int nDeviceNo, nDeviceType, nDriverType, nChannelCount, nSampleInterval, nDefBufLen;
	TCHAR szDeviceName[BUF_LEN_S], szDeviceGUID[BUF_LEN_M];
	BlobData<byte> bdDefBuf;
	DeviceInfo device;
	CArray<int, int> arrDeviceNo, arrDeviceType;	//����ɼ��豸��

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

		if (nDeviceType == devTypeRelay)	//�̵����豸
		{
			if (_tcscmp(szGUID, szDeviceGUID) == 0)
			{
				arrDeviceNo.Add(nDeviceNo);		//�����豸��
				arrDeviceType.Add(nDeviceType);	//�����豸����

				dlg.SetRelayDevice(device);
			}
		}

		if (nDeviceType == devTypeProcessSample)	//�ɼ��豸
		{
			arrDeviceNo.Add(nDeviceNo);		//�����豸��
			arrDeviceType.Add(nDeviceType);

			dlg.SetCollectionDevice(device);
		}

		dbAccess.MoveNext();
	}

	//����ͨ����Ϣ
	int nChannelNo, nAlarmLevelDataLen, nAlarmRangeDataLen;
	TCHAR szChannelName[BUF_LEN_S], szUnitString[BUF_LEN_S];
	BOOL bWorkEnable;
	float fFullScaleHigh, fFullScaleLow, fCalibrationHigh, fCalibrationLow;
	byte byDataType, byDecimalDigits;
	BlobData<byte> bdAlarmLevelData, bdAlarmRangeData;
	ChannelInfo channel;

	for (int i=0; i<arrDeviceNo.GetCount(); i++)	//ֻ����ɼ��豸��ͨ��
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

			if (arrDeviceType.GetAt(i) == devTypeRelay)	//�̵����豸
				dlg.SetRelayChannel(channel);

			if (arrDeviceType.GetAt(i) == devTypeProcessSample)	//�ɼ��豸
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
			case OPT_ADD:		//���
				break;

			case OPT_MODIFY:	//�޸�
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

			case OPT_DELETE:	//ɾ��
				break;

			default:
				break;
			}
		}
	}
}

//*************************************************************************************************
//	���ܣ�	�յ�UDP��Ϣ
//	������	WPARAM wParam[in]����������ַ
//			LPARAM lParam[in]������
//*************************************************************************************************
LRESULT CKTSTAlarmDlg::OnUdpReceiveMessage (WPARAM wParam, LPARAM lParam)
{
	//���������Ĵ���
	CDataCenter *pDataCenter = CDataCenter::GetInstance();
	if (pDataCenter)
		return pDataCenter->OnUdpReceiveMessage (wParam, lParam);

	return S_OK;
}
