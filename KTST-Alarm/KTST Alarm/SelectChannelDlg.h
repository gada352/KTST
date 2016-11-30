#pragma once


// CSelectChannelDlg 对话框

class CSelectChannelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectChannelDlg)

public:
	void SetDeviceListPtr (CDeviceInfoList *pListDeviceInfo);
	void SetChannelListPtr (CChannelInfoList *pListChannelInfo);

	CChannelInfoList* GetSelChannelListPrt ();

public:
	CSelectChannelDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelectChannelDlg();

// 对话框数据
	enum { IDD = IDD_SELECT_CHANNEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


private:
	void RefreshDeviceList ();
	void RefreshChannelList ();

private:
	CDeviceInfoList *m_pListDeviceInfo;		//外部传入设备链表
	CChannelInfoList *m_pListChannelInfo;	//外部传入通道链表

	CChannelInfoList m_listSelChannelInfo;	//选中的通道链表

	int m_nCurSelDevice;					//当前选中设备索引

public:
	CComboBox m_ctrlDeviceList;
	CListCtrl m_ctrlChannelList;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDeviceList();
	afx_msg void OnItemchangedChannelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
};
