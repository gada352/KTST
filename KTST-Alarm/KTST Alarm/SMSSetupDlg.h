#pragma once
#include "afxwin.h"

#include "SMSDevice.h"

// CSMSSetupDlg 对话框

class CSMSSetupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSMSSetupDlg)

public:
	void SetSMSDevice (const DeviceInfo &device);
	void SetCollectionDevice (const DeviceInfo &device);
	void SetCollectionChannel (const ChannelInfo &channel);

	CSMSDeviceList* GetSMSDeviceListPtr ();

private:
	void ClearMemory ();
	void RefreshSMSDeviceList ();
	void RefreshFollowChannelList ();

	DeviceInfo* GetCollectionDevice (int nDeviceNo);
	ChannelInfo* GetCollectionChannel (int nDeviceNo, int nChannelNo);

	int InsertFollowingChannel (ChannelInfo *pSelChannel);

	BOOL ShowSMSDeviceInfo (int nDeviceIndex);
	BOOL SaveSMSDeviceInfo (int nDeviceIndex);

public:
	CSMSSetupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMSSetupDlg();

// 对话框数据
	enum { IDD = IDD_SMS_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


private:
	CSMSDeviceList m_listSMSDevice;				//短信设备链表

	CDeviceInfoList m_listCollectionDevice;		//采集设备链表
	CChannelInfoList m_listCollectionChannel;	//采集设备通道链表

	int m_nCurSelDevice;						//当前选中设备索引

public:
	CComboBox m_ctrlSMSDeviceList;
	CButton m_ctrlDailUp;
	CListCtrl m_ctrlSMSFollowChList;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSmsDeviceList();
	afx_msg void OnItemchangedSmsFollowChList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckDailUp();
	afx_msg void OnBnClickedAddFollowCh();
	afx_msg void OnBnClickedDelFollowCh();
	afx_msg void OnBnClickedOk();
};
