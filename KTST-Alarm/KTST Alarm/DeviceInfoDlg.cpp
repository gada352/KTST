// DeviceInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "DeviceInfoDlg.h"
#include "afxdialogex.h"


// CDeviceInfoDlg 对话框

IMPLEMENT_DYNAMIC(CDeviceInfoDlg, CDialogEx)

CDeviceInfoDlg::CDeviceInfoDlg(int nOptFlag, CWnd* pParent /*=NULL*/)
	: CDialogEx(CDeviceInfoDlg::IDD, pParent)
	, m_nOptFlag(nOptFlag)
{
	m_strDeviceName.Empty();
	m_nDeviceNo = 0;
	m_nChannelCount = 8;
}

CDeviceInfoDlg::~CDeviceInfoDlg()
{
	this->ClearMemory ();
}

void CDeviceInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DEVICE_NO, m_nDeviceNo);
	DDX_Text(pDX, IDC_CHANNEL_COUNT, m_nChannelCount);
// 	DDV_MinMaxInt(pDX, m_nChannelCount, 1, _MAX_ONEDEVICE_CHL);
	DDX_Control(pDX, IDC_DEVICE_TYPE, m_ctrlDeviceType);
	DDX_Control(pDX, IDC_DRIVER_TYPE, m_ctrlDriverType);
	DDX_Text(pDX, IDC_DEVICE_NAME, m_strDeviceName);
	DDV_MaxChars(pDX, m_strDeviceName, BUF_LEN_S-1);
}


BEGIN_MESSAGE_MAP(CDeviceInfoDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_DEVICE_TYPE, &CDeviceInfoDlg::OnSelchangeDeviceType)
	ON_CBN_SELCHANGE(IDC_DRIVER_TYPE, &CDeviceInfoDlg::OnSelchangeDriverType)
	ON_BN_CLICKED(IDOK, &CDeviceInfoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDeviceInfoDlg 消息处理程序
void CDeviceInfoDlg::SetDeviceInfo (CString strDeviceName, int nDeviceNo, int nDeviceType, int nDriverType, int nChannelCount)
{
	m_strDeviceName = strDeviceName;
	m_nDeviceNo = nDeviceNo;
	m_nDeviceType = nDeviceType;
	m_nDriverType = nDriverType;
	m_nChannelCount = nChannelCount;
}

void CDeviceInfoDlg::GetDeviceInfo (CString &strDeviceName, int &nDeviceNo, int &nDeviceType, int &nDriverType, int &nChannelCount)
{
	strDeviceName = m_strDeviceName;
	nDeviceNo = m_nDeviceNo;
	nDeviceType = m_nDeviceType;
	nDriverType = m_nDriverType;
	nChannelCount = m_nChannelCount;
}

void CDeviceInfoDlg::SetDeviceNoBeingUsed (int nDeviceNo)
{
	m_arrDeviceNoUsed.Add(nDeviceNo);
}

void CDeviceInfoDlg::SetDriverType (int nDriverType, CString strDriverTypeName, DWORD dwDeviceTypeBit)
{
	DriverType *pDriver = new DriverType();
	pDriver->nDriverType = nDriverType;
	pDriver->strDriverTypeName = strDriverTypeName;
	pDriver->dwDeviceTypeBit = dwDeviceTypeBit;
	m_listDriverType.AddTail(pDriver);
}

void CDeviceInfoDlg::ClearMemory ()
{
	FreeObjPtrList(&m_listDriverType);
}

BOOL CDeviceInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	switch (m_nOptFlag)
	{
	case 1:		//添加
		this->InitAddDevice ();
		break;
	case 2:		//修改
		this->InitModifyDevice ();
		break;
	default:
		break;
	}

	UpdateData (FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//初始化添加设备
void CDeviceInfoDlg::InitAddDevice ()
{
	//设备名称
	m_strDeviceName.Empty();

	//设备号
	this->GetUnusedDeviceNo (m_nDeviceNo);
	GetDlgItem(IDC_DEVICE_NO)->EnableWindow(FALSE);

	//设备类型
	m_ctrlDeviceType.ResetContent();
	m_nDeviceTypeIndex = -1;

	//**	先计算本地所有驱动类型都支持哪些设备类型位
	CString strLoad;
	DWORD dwAllDeviceTypeBit = 0;
	DriverType *pDriver = NULL;
	POSITION pos = m_listDriverType.GetHeadPosition();
	while (pos)
	{
		pDriver = m_listDriverType.GetNext(pos);
		if (pDriver)
			dwAllDeviceTypeBit |= pDriver->dwDeviceTypeBit;
	}

	//**	依次判断总的设备类型位中是否支持每种设备类型(ICM模块判断控制器设备和驱动设备的类型)
	int nDeviceTypeTemp = devTypeDriver;		//驱动设备
	if (dwAllDeviceTypeBit & nDeviceTypeTemp)
	{
		strLoad.LoadString (IDS_DEV_TYPE_DRIVER);
		int nIndex = m_ctrlDeviceType.AddString(strLoad);
		m_ctrlDeviceType.SetItemData(nIndex, (DWORD_PTR )nDeviceTypeTemp);
	}
	nDeviceTypeTemp = devTypeRelay;		//继电器设备
	if (dwAllDeviceTypeBit & nDeviceTypeTemp)
	{
		strLoad.LoadString (IDS_DEV_TYPE_RELAY);
		int nIndex = m_ctrlDeviceType.AddString(strLoad);
		m_ctrlDeviceType.SetItemData(nIndex, (DWORD_PTR )nDeviceTypeTemp);
	}
	if (m_ctrlDeviceType.GetCount() > 0)
	{
		m_nDeviceTypeIndex = 0;
		m_ctrlDeviceType.SetCurSel(m_nDeviceTypeIndex);
	}


	//驱动类型
	this->RefreshDriverTypeList(m_nDeviceTypeIndex);	//根据当前设备类型刷新驱动类型列表
	if (m_ctrlDriverType.GetCount() > 0)
		m_nDriverTypeIndex = 0;
	m_ctrlDriverType.SetCurSel(m_nDriverTypeIndex);


	//通道个数
	RefreshChannelCount (m_nDriverTypeIndex);
}

//初始化修改设备
void CDeviceInfoDlg::InitModifyDevice ()
{
	//设备名称	//可以修改

	//设备号
	GetDlgItem(IDC_DEVICE_NO)->EnableWindow(FALSE);

	//设备类型
	m_ctrlDeviceType.ResetContent();
	m_nDeviceTypeIndex = -1;

	//**	先计算本地所有驱动类型都支持哪些设备类型位
	CString strLoad;
	DWORD dwAllDeviceTypeBit = 0;
	DriverType *pDriver = NULL;
	POSITION pos = m_listDriverType.GetHeadPosition();
	while (pos)
	{
		pDriver = m_listDriverType.GetNext(pos);
		if (pDriver)
			dwAllDeviceTypeBit |= pDriver->dwDeviceTypeBit;
	}

	//**	依次判断总的设备类型位中是否支持每种设备类型(ICM模块判断控制器设备和驱动设备的类型)
	int nDeviceTypeTemp = devTypeProgrammableController;		//控制器设备
	if (dwAllDeviceTypeBit & nDeviceTypeTemp)
	{
		strLoad.LoadString (IDS_DEV_TYPE_CONTROLLER);
		int nIndex = m_ctrlDeviceType.AddString(strLoad);
		m_ctrlDeviceType.SetItemData(nIndex, (DWORD_PTR )nDeviceTypeTemp);

		if (m_nDeviceType == nDeviceTypeTemp)
			m_nDeviceTypeIndex = nIndex;
	}
	nDeviceTypeTemp = devTypeDriver;		//驱动设备
	if (dwAllDeviceTypeBit & nDeviceTypeTemp)
	{
		strLoad.LoadString (IDS_DEV_TYPE_DRIVER);
		int nIndex = m_ctrlDeviceType.AddString(strLoad);
		m_ctrlDeviceType.SetItemData(nIndex, (DWORD_PTR )nDeviceTypeTemp);

		if (m_nDeviceType == nDeviceTypeTemp)
			m_nDeviceTypeIndex = nIndex;
	}
	m_ctrlDeviceType.SetCurSel(m_nDeviceTypeIndex);
	m_ctrlDeviceType.EnableWindow(FALSE);


	//驱动类型
	this->RefreshDriverTypeList(m_nDeviceTypeIndex);	//根据当前设备类型刷新驱动类型列表

	//判断驱动类型索引
	int nCount = m_ctrlDriverType.GetCount();
	for (int i=0; i<nCount; i++)
	{
		if (m_nDriverType == (int )m_ctrlDriverType.GetItemData(i))
		{
			m_nDriverTypeIndex = i;
			break;
		}
	}
	m_ctrlDriverType.SetCurSel(m_nDriverTypeIndex);
	m_ctrlDriverType.EnableWindow(FALSE);

	//通道个数
	GetDlgItem(IDC_CHANNEL_COUNT)->EnableWindow(FALSE);
}

//设备类型改变
void CDeviceInfoDlg::OnSelchangeDeviceType()
{
	int nIndex = m_ctrlDeviceType.GetCurSel();
	if (nIndex<0 || m_nDeviceTypeIndex==nIndex)
		return;

	m_nDeviceTypeIndex = nIndex;				//设备类型选项的索引

	this->RefreshDriverTypeList (m_nDeviceTypeIndex);	//刷新驱动类型列表

	if (m_ctrlDriverType.GetCount() > 0)
		m_ctrlDriverType.SetCurSel(0);

	this->OnSelchangeDriverType();
}

//驱动类型改变
void CDeviceInfoDlg::OnSelchangeDriverType()
{
	int nIndex = m_ctrlDriverType.GetCurSel();
	if (nIndex<0 || m_nDriverTypeIndex==nIndex)
		return;

	m_nDriverTypeIndex = nIndex;			//驱动类型选项的索引

	RefreshChannelCount (m_nDriverTypeIndex);
}

void CDeviceInfoDlg::OnBnClickedOk()
{
	if (!UpdateData (TRUE))
		return;

	CString strLoad, strText;
	CEdit *pEdit = NULL;

	//判断设备名称
	m_strDeviceName.Trim();
	if (m_strDeviceName.IsEmpty())
	{
		AfxMessageBox(IDS_DEV_NAME_NULL);

		pEdit = (CEdit *)GetDlgItem(IDC_DEVICE_NAME);
		pEdit->SetWindowText(_T(""));
		pEdit->SetFocus ();
		return;
	}
	if (m_strDeviceName.GetLength() > BUF_LEN_S-1)
		m_strDeviceName = m_strDeviceName.Left(BUF_LEN_S-1);

	//设备号

	//判断设备类型
	m_nDeviceTypeIndex = m_ctrlDeviceType.GetCurSel();
	if (m_nDeviceTypeIndex < 0)
	{
		AfxMessageBox(IDS_SEL_DEV_TYPE);
		m_ctrlDeviceType.SetFocus();
		return;
	}
	m_nDeviceType = (int )m_ctrlDeviceType.GetItemData(m_nDeviceTypeIndex);

	//判断驱动类型
	m_nDriverTypeIndex = m_ctrlDriverType.GetCurSel();
	if (m_nDriverTypeIndex < 0)
	{
		AfxMessageBox(IDS_SEL_DRIVER_TYPE);
		m_ctrlDriverType.SetFocus();
		return;
	}
	m_nDriverType = m_ctrlDriverType.GetItemData(m_nDriverTypeIndex);	//项目中保存设备属性
	
	//判断通道数
	if (m_nDriverType != dvrTypeSMSDevice)
	{
		if (m_nChannelCount < 1 || m_nChannelCount > _MAX_ONEDEVICE_CHL)
		{
			strLoad.LoadString(IDS_CHANNEL_COUNT_INVALID);
			strText.Format(strLoad, 1, _MAX_ONEDEVICE_CHL);
			AfxMessageBox(strText);

			pEdit = (CEdit *)GetDlgItem(IDC_CHANNEL_COUNT);
			pEdit->SetSel(0, -1);
			pEdit->SetFocus ();
			return;
		}
	}

	CDialogEx::OnOK();
}

//得到一个未被占用的设备号
BOOL CDeviceInfoDlg::GetUnusedDeviceNo (int &nDeviceNo)
{
	BOOL bUsed[_MAX_DEVICE_IN_LIST];
	for (int i=0; i<_MAX_DEVICE_IN_LIST; i++)
		bUsed[i] = FALSE;

	int nSize = m_arrDeviceNoUsed.GetSize();
	for (int i=0; i<nSize; i++)
		bUsed[m_arrDeviceNoUsed[i]] = TRUE;

	for (int i=0; i<_MAX_DEVICE_IN_LIST; i++)
	{
		if (!bUsed[i])
		{
			nDeviceNo = i;
			return TRUE;
		}
	}

	return FALSE;
}

//根据选中的设备类型来刷新驱动类型列表
void CDeviceInfoDlg::RefreshDriverTypeList (int nDeviceTypeIndex)
{
	m_ctrlDriverType.ResetContent();
	m_nDriverTypeIndex = -1;

	if (nDeviceTypeIndex < 0)
		return;

	int nDeviceType = (int )m_ctrlDeviceType.GetItemData(nDeviceTypeIndex);

	int nIndex;
	DriverType *pDriver = NULL;
	POSITION pos = m_listDriverType.GetHeadPosition ();
	while (pos)
	{
		pDriver = m_listDriverType.GetNext(pos);
		if (pDriver && (pDriver->dwDeviceTypeBit & nDeviceType))
		{
			nIndex = m_ctrlDriverType.AddString(pDriver->strDriverTypeName);
			m_ctrlDriverType.SetItemData(nIndex, (DWORD_PTR )pDriver->nDriverType);
		}
	}
}

//根据选中的设备类型来刷新采集时间间隔
void CDeviceInfoDlg::RefreshChannelCount (int nDriverTypeIndex)
{
	if (nDriverTypeIndex < 0)
		return;

	int nDriverType = (int )m_ctrlDriverType.GetItemData(nDriverTypeIndex);

	if (nDriverType == dvrTypeSMSDevice)	//短信设备没有通道
	{
		GetDlgItem(IDC_CHANNEL_COUNT)->EnableWindow(FALSE);
		SetDlgItemInt(IDC_CHANNEL_COUNT, 0);
	}
	else
	{
		GetDlgItem(IDC_CHANNEL_COUNT)->EnableWindow(TRUE);
		SetDlgItemInt(IDC_CHANNEL_COUNT, DEF_CH_CNT_RELAY);
	}
}
