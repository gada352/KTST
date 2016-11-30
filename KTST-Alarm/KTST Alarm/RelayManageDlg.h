#pragma once

#include "RelayDevice.h"
#include "RelayChannel.h"

// CRelayManageDlg 对话框

class CRelayManageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRelayManageDlg)

public:
	void SetRelayDevice (const DeviceInfo &device);
	void SetRelayChannel (const ChannelInfo &channel);

	void SetCollectionDevice (const DeviceInfo &device);
	void SetCollectionChannel (const ChannelInfo &channel);

	CRelayChannelList* GetRelayChannelListPtr ();

private:
	void ClearMemory ();

	void RefreshRelayDeviceList ();
	void RefreshRelayChannelList ();

	void UpdateRelayChannelInfo (int nItem, RelayChannel *pChannel);

	void CheckAllRelayChannelExpression ();
	BOOL IsCollectionChannel (int nDeviceNo, int nChannelNo);

public:
	CRelayManageDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRelayManageDlg();

// 对话框数据
	enum { IDD = IDD_RELAY_MANAGE };

private:
	CRelayDeviceList m_listRelayDevice;			//继电器设备链表
	CRelayChannelList m_listRelayChannel;		//继电器通道链表

	CDeviceInfoList m_listCollectionDevice;		//采集设备链表
	CChannelInfoList m_listCollectionChannel;	//采集设备通道链表

	int m_nCurSelDevice;						//当前选中设备索引

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ctrlDeviceList;
	CListCtrl m_ctrlChannelList;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeRelayDeviceList();
	afx_msg void OnItemchangedRelayChannelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRelayChannelSetup();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDblclkRelayChannelList(NMHDR *pNMHDR, LRESULT *pResult);
};
