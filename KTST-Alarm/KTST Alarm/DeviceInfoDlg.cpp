// DeviceInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "DeviceInfoDlg.h"
#include "afxdialogex.h"


// CDeviceInfoDlg �Ի���

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


// CDeviceInfoDlg ��Ϣ�������
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
	case 1:		//���
		this->InitAddDevice ();
		break;
	case 2:		//�޸�
		this->InitModifyDevice ();
		break;
	default:
		break;
	}

	UpdateData (FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//��ʼ������豸
void CDeviceInfoDlg::InitAddDevice ()
{
	//�豸����
	m_strDeviceName.Empty();

	//�豸��
	this->GetUnusedDeviceNo (m_nDeviceNo);
	GetDlgItem(IDC_DEVICE_NO)->EnableWindow(FALSE);

	//�豸����
	m_ctrlDeviceType.ResetContent();
	m_nDeviceTypeIndex = -1;

	//**	�ȼ��㱾�������������Ͷ�֧����Щ�豸����λ
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

	//**	�����ж��ܵ��豸����λ���Ƿ�֧��ÿ���豸����(ICMģ���жϿ������豸�������豸������)
	int nDeviceTypeTemp = devTypeDriver;		//�����豸
	if (dwAllDeviceTypeBit & nDeviceTypeTemp)
	{
		strLoad.LoadString (IDS_DEV_TYPE_DRIVER);
		int nIndex = m_ctrlDeviceType.AddString(strLoad);
		m_ctrlDeviceType.SetItemData(nIndex, (DWORD_PTR )nDeviceTypeTemp);
	}
	nDeviceTypeTemp = devTypeRelay;		//�̵����豸
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


	//��������
	this->RefreshDriverTypeList(m_nDeviceTypeIndex);	//���ݵ�ǰ�豸����ˢ�����������б�
	if (m_ctrlDriverType.GetCount() > 0)
		m_nDriverTypeIndex = 0;
	m_ctrlDriverType.SetCurSel(m_nDriverTypeIndex);


	//ͨ������
	RefreshChannelCount (m_nDriverTypeIndex);
}

//��ʼ���޸��豸
void CDeviceInfoDlg::InitModifyDevice ()
{
	//�豸����	//�����޸�

	//�豸��
	GetDlgItem(IDC_DEVICE_NO)->EnableWindow(FALSE);

	//�豸����
	m_ctrlDeviceType.ResetContent();
	m_nDeviceTypeIndex = -1;

	//**	�ȼ��㱾�������������Ͷ�֧����Щ�豸����λ
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

	//**	�����ж��ܵ��豸����λ���Ƿ�֧��ÿ���豸����(ICMģ���жϿ������豸�������豸������)
	int nDeviceTypeTemp = devTypeProgrammableController;		//�������豸
	if (dwAllDeviceTypeBit & nDeviceTypeTemp)
	{
		strLoad.LoadString (IDS_DEV_TYPE_CONTROLLER);
		int nIndex = m_ctrlDeviceType.AddString(strLoad);
		m_ctrlDeviceType.SetItemData(nIndex, (DWORD_PTR )nDeviceTypeTemp);

		if (m_nDeviceType == nDeviceTypeTemp)
			m_nDeviceTypeIndex = nIndex;
	}
	nDeviceTypeTemp = devTypeDriver;		//�����豸
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


	//��������
	this->RefreshDriverTypeList(m_nDeviceTypeIndex);	//���ݵ�ǰ�豸����ˢ�����������б�

	//�ж�������������
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

	//ͨ������
	GetDlgItem(IDC_CHANNEL_COUNT)->EnableWindow(FALSE);
}

//�豸���͸ı�
void CDeviceInfoDlg::OnSelchangeDeviceType()
{
	int nIndex = m_ctrlDeviceType.GetCurSel();
	if (nIndex<0 || m_nDeviceTypeIndex==nIndex)
		return;

	m_nDeviceTypeIndex = nIndex;				//�豸����ѡ�������

	this->RefreshDriverTypeList (m_nDeviceTypeIndex);	//ˢ�����������б�

	if (m_ctrlDriverType.GetCount() > 0)
		m_ctrlDriverType.SetCurSel(0);

	this->OnSelchangeDriverType();
}

//�������͸ı�
void CDeviceInfoDlg::OnSelchangeDriverType()
{
	int nIndex = m_ctrlDriverType.GetCurSel();
	if (nIndex<0 || m_nDriverTypeIndex==nIndex)
		return;

	m_nDriverTypeIndex = nIndex;			//��������ѡ�������

	RefreshChannelCount (m_nDriverTypeIndex);
}

void CDeviceInfoDlg::OnBnClickedOk()
{
	if (!UpdateData (TRUE))
		return;

	CString strLoad, strText;
	CEdit *pEdit = NULL;

	//�ж��豸����
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

	//�豸��

	//�ж��豸����
	m_nDeviceTypeIndex = m_ctrlDeviceType.GetCurSel();
	if (m_nDeviceTypeIndex < 0)
	{
		AfxMessageBox(IDS_SEL_DEV_TYPE);
		m_ctrlDeviceType.SetFocus();
		return;
	}
	m_nDeviceType = (int )m_ctrlDeviceType.GetItemData(m_nDeviceTypeIndex);

	//�ж���������
	m_nDriverTypeIndex = m_ctrlDriverType.GetCurSel();
	if (m_nDriverTypeIndex < 0)
	{
		AfxMessageBox(IDS_SEL_DRIVER_TYPE);
		m_ctrlDriverType.SetFocus();
		return;
	}
	m_nDriverType = m_ctrlDriverType.GetItemData(m_nDriverTypeIndex);	//��Ŀ�б����豸����
	
	//�ж�ͨ����
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

//�õ�һ��δ��ռ�õ��豸��
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

//����ѡ�е��豸������ˢ�����������б�
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

//����ѡ�е��豸������ˢ�²ɼ�ʱ����
void CDeviceInfoDlg::RefreshChannelCount (int nDriverTypeIndex)
{
	if (nDriverTypeIndex < 0)
		return;

	int nDriverType = (int )m_ctrlDriverType.GetItemData(nDriverTypeIndex);

	if (nDriverType == dvrTypeSMSDevice)	//�����豸û��ͨ��
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
