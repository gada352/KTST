
// KTST AlarmDlg.h : 头文件
//

#pragma once


// CKTSTAlarmDlg 对话框
class CKTSTAlarmDlg : public CDialogEx
{
// 构造
public:
	CKTSTAlarmDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_KTSTALARM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


private:
	void StartModule ();
	void StopModule ();

	BOOL SetTrayIcon(int nFlag, UINT nIDTrayIcon, CString strTip);
	BOOL DeleteTrayIcon();
	void EnableControlWindow(BOOL bStartFlag);

	BOOL AddDeviceTrigger (int nDeviceNo, int nChannelCount, int nDeviceType);

// 实现
protected:
	HICON m_hIcon;
	BOOL m_bVisable;				//当前主窗口显示标志，TRUE显示，FALSE隐藏
	int m_nShowWndFuncCnt;			//保存进程刚启动时OnWindowPosChanging函数里包含SWP_SHOWWINDOW的次数
	BOOL m_bStartFlag;				//当前模块启动标志
	int  m_nTimerID;				//TimerID

	BOOL m_bOnStartModule;			//标识是否启动系统

	BOOL m_bAutoRun;				//标识开机后是否自动启动程序
	BOOL m_bAutoConnectDB;			//标识启动程序后是否自动连接数据库服务器

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnHtmlHelp();
	afx_msg void OnAbout();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnTimer(UINT nIDEvent);
	LRESULT OnTrayNotifyIcon(WPARAM wParam, LPARAM lParam);
	afx_msg void OnAutoRun();
	afx_msg void OnAutoConnectDBServer();
	afx_msg LRESULT OnUdpReceiveMessage (WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDeviceManage();
	afx_msg void OnSmsSetup();
	afx_msg void OnRelaySetup();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
};
