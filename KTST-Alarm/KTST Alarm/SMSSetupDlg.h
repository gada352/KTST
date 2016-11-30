#pragma once
#include "afxwin.h"

#include "SMSDevice.h"

// CSMSSetupDlg �Ի���

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
	CSMSSetupDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMSSetupDlg();

// �Ի�������
	enum { IDD = IDD_SMS_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


private:
	CSMSDeviceList m_listSMSDevice;				//�����豸����

	CDeviceInfoList m_listCollectionDevice;		//�ɼ��豸����
	CChannelInfoList m_listCollectionChannel;	//�ɼ��豸ͨ������

	int m_nCurSelDevice;						//��ǰѡ���豸����

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
