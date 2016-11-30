#pragma once

#include "RelayChannel.h"

// CRelayChannelSetupDlg 对话框

class CRelayChannelSetupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRelayChannelSetupDlg)

public:
	void SetRelayChannel (const ChannelInfo &channel);
	void GetRelayChannel (ChannelInfo &channel);

	void SetCollectionDeviceListPtr (CDeviceInfoList *pCollectionDeviceList);
	void SetCollectionChannelListPtr (CChannelInfoList *pCollectionChannelList);

public:
	CRelayChannelSetupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRelayChannelSetupDlg();

// 对话框数据
	enum { IDD = IDD_RELAY_CHANNEL_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	void RefreshExpressionDisplayInfo (BOOL bRefreshString);
	CString GetExpressionDisplayInfo (const RelayExpression &exp);
	CString GetCollectionDeviceName (int nDeviceNo);
	CString GetCollectionChannelName (int nDeviceNo, int nChannelNo);
	BOOL IsCollectionChannel (int nDeviceNo, int nChannelNo);

	void ShowExpression ();
	void ShowExpressionStatusInfo ();

	BOOL IsExpressionValid ();

	void InsertExpression (const RelayExpression &expNew);
	POSITION GetExpressionPosFromIndex (int nCursorIndex);
	void MoveCursorToExpElementEnd ();

	BOOL IsExpressionEditorFull ();

private:
	RelayChannel m_RelayChannel;				//继电器通道

	CDeviceInfoList *m_pCollectionDeviceList;	//采集设备链表
	CChannelInfoList *m_pCollectionChannelList;	//采集设备通道链表

	CFont m_fontEditor;							//表达式编辑框字体

	int m_nCurCursorPos;						//记录当前光标位置
	BOOL m_bExpressionValid;					//标识当前表达式是否有效

public:
	CString m_strChannelName;
	CEdit m_ctrlExpressionEditor;
	int m_nOutputDelay;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedSelectChannel();
	afx_msg void OnBnClickedLeftParentheses();
	afx_msg void OnBnClickedRightParentheses();
	afx_msg void OnBnClickedAnd();
	afx_msg void OnBnClickedOr();
	afx_msg void OnBnClickedNot();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedOk();
};
