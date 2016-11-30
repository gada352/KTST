#pragma once

#include "RelayChannel.h"

// CRelayChannelSetupDlg �Ի���

class CRelayChannelSetupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRelayChannelSetupDlg)

public:
	void SetRelayChannel (const ChannelInfo &channel);
	void GetRelayChannel (ChannelInfo &channel);

	void SetCollectionDeviceListPtr (CDeviceInfoList *pCollectionDeviceList);
	void SetCollectionChannelListPtr (CChannelInfoList *pCollectionChannelList);

public:
	CRelayChannelSetupDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRelayChannelSetupDlg();

// �Ի�������
	enum { IDD = IDD_RELAY_CHANNEL_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	RelayChannel m_RelayChannel;				//�̵���ͨ��

	CDeviceInfoList *m_pCollectionDeviceList;	//�ɼ��豸����
	CChannelInfoList *m_pCollectionChannelList;	//�ɼ��豸ͨ������

	CFont m_fontEditor;							//���ʽ�༭������

	int m_nCurCursorPos;						//��¼��ǰ���λ��
	BOOL m_bExpressionValid;					//��ʶ��ǰ���ʽ�Ƿ���Ч

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
