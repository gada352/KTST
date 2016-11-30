#pragma once


// CSelectChannelDlg �Ի���

class CSelectChannelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectChannelDlg)

public:
	void SetDeviceListPtr (CDeviceInfoList *pListDeviceInfo);
	void SetChannelListPtr (CChannelInfoList *pListChannelInfo);

	CChannelInfoList* GetSelChannelListPrt ();

public:
	CSelectChannelDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelectChannelDlg();

// �Ի�������
	enum { IDD = IDD_SELECT_CHANNEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


private:
	void RefreshDeviceList ();
	void RefreshChannelList ();

private:
	CDeviceInfoList *m_pListDeviceInfo;		//�ⲿ�����豸����
	CChannelInfoList *m_pListChannelInfo;	//�ⲿ����ͨ������

	CChannelInfoList m_listSelChannelInfo;	//ѡ�е�ͨ������

	int m_nCurSelDevice;					//��ǰѡ���豸����

public:
	CComboBox m_ctrlDeviceList;
	CListCtrl m_ctrlChannelList;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDeviceList();
	afx_msg void OnItemchangedChannelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
};
