#pragma once


// CDeviceInfoDlg 对话框

class CDeviceInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDeviceInfoDlg)

public:
	CDeviceInfoDlg(int nOptFlag, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDeviceInfoDlg();

// 对话框数据
	enum { IDD = IDD_DEVICE_INFO };

public:
	void SetDeviceInfo (CString strDeviceName, int nDeviceNo, int nDeviceType, int nDriverType, int nChannelCount);
	void GetDeviceInfo (CString &strDeviceName, int &nDeviceNo, int &nDeviceType, int &nDriverType, int &nChannelCount);
	void SetDeviceNoBeingUsed (int nDeviceNo);
	void SetDriverType (int nDriverType, CString strDriverTypeName, DWORD dwDeviceTypeBit);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	typedef struct DriverType
	{
		int nDriverType;			//驱动类型
		CString strDriverTypeName;	//驱动类型名称
		DWORD dwDeviceTypeBit;		//此驱动类型支持的几种设备类型，按位表示

		DriverType ()
		{
			nDriverType = 0;
			strDriverTypeName.Empty();
			dwDeviceTypeBit = 0;
		}
	}DriverType;
	typedef CTypedPtrList<CPtrList, DriverType*> CDriverTypeList;

private:
	int m_nDeviceType;			//设备类型
	int m_nDriverType;			//驱动类型

	int m_nOptFlag;				//当前对话框是1添加还是2删除

	int	m_nDeviceTypeIndex;		//当前选择的设备类型索引
	int	m_nDriverTypeIndex;		//当前选择的驱动类型索引

	CArray<int, int> m_arrDeviceNoUsed;		//已占用设备号数组

	CDriverTypeList m_listDriverType;		//本地安装的驱动类型


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
