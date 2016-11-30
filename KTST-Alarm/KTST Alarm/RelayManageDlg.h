#pragma once

#include "RelayDevice.h"
#include "RelayChannel.h"

// CRelayManageDlg �Ի���

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
	CRelayManageDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRelayManageDlg();

// �Ի�������
	enum { IDD = IDD_RELAY_MANAGE };

private:
	CRelayDeviceList m_listRelayDevice;			//�̵����豸����
	CRelayChannelList m_listRelayChannel;		//�̵���ͨ������

	CDeviceInfoList m_listCollectionDevice;		//�ɼ��豸����
	CChannelInfoList m_listCollectionChannel;	//�ɼ��豸ͨ������

	int m_nCurSelDevice;						//��ǰѡ���豸����

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
