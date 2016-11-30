#pragma once


// CDeviceManageDlg �Ի���

class CDeviceManageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDeviceManageDlg)

public:
	void SetDevice (const DeviceInfo &device);
	
	CDeviceInfoList* GetDeviceListPtr ();


public:
	CDeviceManageDlg(CDeviceIO *pDeviceIO, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDeviceManageDlg();

// �Ի�������
	enum { IDD = IDD_DEVICE_MANAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


private:
	void ClearMemory ();
	void RefreshDeviceList ();
	void InsertItemToList (int nItem, DeviceInfo *pDevice, BOOL bReplace=FALSE);
	BOOL DeleteInvalidDeviceInRegister ();
	BOOL ModifyDeviceInfoInRegister (DeviceInfo *pDevice);
	BOOL ExistInDeviceList (int nDeviceType, int nDeviceNo, CString strGUID);

private:
	CDeviceInfoList m_listDevice;

	CDeviceIO *m_pDeviceIO;			//
	CString m_strLocalGUID;			//���ص�GUID
	BOOL m_bShowAllDevice;			//��ʾȫ���豸(TRUE)����ֻ��ʾ�����豸(FALSE)


public:
	CListCtrl m_ctrlDeviceList;
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedDeviceList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAddDevice();
	afx_msg void OnBnClickedModifyDevice();
	afx_msg void OnBnClickedDeleteDevice();
	afx_msg void OnBnClickedExtendedDef();
	afx_msg void OnBnClickedOk();
	afx_msg void OnRclickDeviceList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnShowAllDevice();
	afx_msg void OnShowLocalDevice();
	afx_msg void OnBnClickedCancel();
};
