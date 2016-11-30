#pragma once


// CDeviceInfoDlg �Ի���

class CDeviceInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDeviceInfoDlg)

public:
	CDeviceInfoDlg(int nOptFlag, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDeviceInfoDlg();

// �Ի�������
	enum { IDD = IDD_DEVICE_INFO };

public:
	void SetDeviceInfo (CString strDeviceName, int nDeviceNo, int nDeviceType, int nDriverType, int nChannelCount);
	void GetDeviceInfo (CString &strDeviceName, int &nDeviceNo, int &nDeviceType, int &nDriverType, int &nChannelCount);
	void SetDeviceNoBeingUsed (int nDeviceNo);
	void SetDriverType (int nDriverType, CString strDriverTypeName, DWORD dwDeviceTypeBit);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	typedef struct DriverType
	{
		int nDriverType;			//��������
		CString strDriverTypeName;	//������������
		DWORD dwDeviceTypeBit;		//����������֧�ֵļ����豸���ͣ���λ��ʾ

		DriverType ()
		{
			nDriverType = 0;
			strDriverTypeName.Empty();
			dwDeviceTypeBit = 0;
		}
	}DriverType;
	typedef CTypedPtrList<CPtrList, DriverType*> CDriverTypeList;

private:
	int m_nDeviceType;			//�豸����
	int m_nDriverType;			//��������

	int m_nOptFlag;				//��ǰ�Ի�����1��ӻ���2ɾ��

	int	m_nDeviceTypeIndex;		//��ǰѡ����豸��������
	int	m_nDriverTypeIndex;		//��ǰѡ���������������

	CArray<int, int> m_arrDeviceNoUsed;		//��ռ���豸������

	CDriverTypeList m_listDriverType;		//���ذ�װ����������


private:
	void InitAddDevice ();
	void InitModifyDevice ();
	BOOL GetUnusedDeviceNo (int &nDeviceNo);
	void RefreshDriverTypeList (int nDeviceTypeIndex);
	void RefreshChannelCount (int nDriverTypeIndex);

	void ClearMemory ();

public:
	virtual BOOL OnInitDialog();

	CString m_strDeviceName;
	int m_nDeviceNo;
	int m_nChannelCount;
	CComboBox m_ctrlDeviceType;
	CComboBox m_ctrlDriverType;

	afx_msg void OnSelchangeDeviceType();
	afx_msg void OnSelchangeDriverType();
	afx_msg void OnBnClickedOk();
};
