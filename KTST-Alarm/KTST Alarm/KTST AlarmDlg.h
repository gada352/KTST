
// KTST AlarmDlg.h : ͷ�ļ�
//

#pragma once


// CKTSTAlarmDlg �Ի���
class CKTSTAlarmDlg : public CDialogEx
{
// ����
public:
	CKTSTAlarmDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_KTSTALARM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


private:
	void StartModule ();
	void StopModule ();

	BOOL SetTrayIcon(int nFlag, UINT nIDTrayIcon, CString strTip);
	BOOL DeleteTrayIcon();
	void EnableControlWindow(BOOL bStartFlag);

	BOOL AddDeviceTrigger (int nDeviceNo, int nChannelCount, int nDeviceType);

// ʵ��
protected:
	HICON m_hIcon;
	BOOL m_bVisable;				//��ǰ��������ʾ��־��TRUE��ʾ��FALSE����
	int m_nShowWndFuncCnt;			//������̸�����ʱOnWindowPosChanging���������SWP_SHOWWINDOW�Ĵ���
	BOOL m_bStartFlag;				//��ǰģ��������־
	int  m_nTimerID;				//TimerID

	BOOL m_bOnStartModule;			//��ʶ�Ƿ�����ϵͳ

	BOOL m_bAutoRun;				//��ʶ�������Ƿ��Զ���������
	BOOL m_bAutoConnectDB;			//��ʶ����������Ƿ��Զ��������ݿ������

	// ���ɵ���Ϣӳ�亯��
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
