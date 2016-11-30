// DeviceManageDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "DeviceManageDlg.h"
#include "afxdialogex.h"

#include "DeviceInfoDlg.h"

// CDeviceManageDlg �Ի���

IMPLEMENT_DYNAMIC(CDeviceManageDlg, CDialogEx)

CDeviceManageDlg::CDeviceManageDlg(CDeviceIO *pDeviceIO, CWnd* pParent /*=NULL*/)
	: CDialogEx(CDeviceManageDlg::IDD, pParent)
	, m_pDeviceIO (pDeviceIO)
{
	if (m_pDeviceIO)
	{
		TCHAR szGUID[BUF_LEN_M];
		memset (szGUID, 0x0, sizeof(szGUID));
		m_pDeviceIO->GetLcdeGid(szGUID);
		m_strLocalGUID = szGUID;
	}

	m_bShowAllDevice = FALSE;
}

CDeviceManageDlg::~CDeviceManageDlg()
{
	this->ClearMemory ();
}

void CDeviceManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVICE_LIST, m_ctrlDeviceList);
}


BEGIN_MESSAGE_MAP(CDeviceManageDlg, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DEVICE_LIST, &CDeviceManageDlg::OnItemchangedDeviceList)
	ON_BN_CLICKED(IDC_ADD_DEVICE, &CDeviceManageDlg::OnBnClickedAddDevice)
	ON_BN_CLICKED(IDC_MODIFY_DEVICE, &CDeviceManageDlg::OnBnClickedModifyDevice)
	ON_BN_CLICKED(IDC_DELETE_DEVICE, &CDeviceManageDlg::OnBnClickedDeleteDevice)
	ON_BN_CLICKED(IDC_EXTENDED_DEF, &CDeviceManageDlg::OnBnClickedExtendedDef)
	ON_BN_CLICKED(IDOK, &CDeviceManageDlg::OnBnClickedOk)
	ON_NOTIFY(NM_RCLICK, IDC_DEVICE_LIST, &CDeviceManageDlg::OnRclickDeviceList)
	ON_COMMAND(ID_SHOW_ALL_DEVICE, OnShowAllDevice)
	ON_COMMAND(ID_SHOW_LOCAL_DEVICE, OnShowLocalDevice)
	ON_BN_CLICKED(IDCANCEL, &CDeviceManageDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDeviceManageDlg ��Ϣ�������

void CDeviceManageDlg::SetDevice (const DeviceInfo &device)
{
	m_listDevice.AddTail(new DeviceInfo(device));
}

CDeviceInfoList* CDeviceManageDlg::GetDeviceListPtr ()
{
	return &m_listDevice;
}

void CDeviceManageDlg::ClearMemory ()
{
	FreeObjPtrList(&m_listDevice);
}


BOOL CDeviceManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ctrlDeviceList.SetExtendedStyle (m_ctrlDeviceList.GetExtendedStyle () | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//�����豸�б���ͷ
	CString strLoad;
	int nIndex = 0;
	strLoad.LoadString (IDS_NAME);
	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_LEFT, 110);
	nIndex ++;
	strLoad.LoadString (IDS_NO);
	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_CENTER, 40);
	nIndex ++;
	strLoad.LoadString(IDS_DEVICE_TYPE);
	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_CENTER, 120);
	nIndex ++;
	strLoad.LoadString (IDS_DRIVER_TYPE);
	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_CENTER, 120);
	nIndex ++;
	strLoad.LoadString (IDS_CHANNEL_COUNT);
	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_CENTER, 50);
	nIndex ++;
// 	strLoad.LoadString (IDS_SAMPLE_INTERVAL);
// 	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_CENTER, 110);
// 	nIndex ++;


	DeviceInfo *pDevice = NULL;
	POSITION pos = m_listDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = (DeviceInfo *)m_listDevice.GetNext(pos);
		if (pDevice && m_strLocalGUID.Compare(pDevice->strDeviceGUID)==0)
		{
			//�������豸��һЩ��Ϣ����д��ע���
			this->ModifyDeviceInfoInRegister (pDevice);
		}
	}

	//ɾ��ע����е���Ч�豸
	this->DeleteInvalidDeviceInRegister ();

	m_pDeviceIO->Initialize(SOFTWARE_NAME);	//DeviceIO���³�ʼ�����ø������͵Ķ�̬��Ҳ���³�ʼ������Ϊע������޸����豸��Ϣ��ɾ����һЩ��Ч�豸

	//��ʼ���豸�б�
	this->RefreshDeviceList ();

	GetDlgItem(IDC_MODIFY_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDeviceManageDlg::RefreshDeviceList ()
{
	m_ctrlDeviceList.DeleteAllItems();

	//��ʼ���豸�б�
	int nIndex = 0;
	DeviceInfo *pDevice = NULL;
	POSITION pos = m_listDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = (DeviceInfo *)m_listDevice.GetNext(pos);

		if (pDevice->nOptFlag == OPT_DELETE)
			continue;

		if (pDevice->nDeviceType != devTypeDriver && pDevice->nDeviceType != devTypeRelay)
			continue;

		if (m_bShowAllDevice		//��ʾȫ���豸
			|| m_strLocalGUID.Compare(pDevice->strDeviceGUID)==0)	//ֻ��ʾ�����豸
		{
			this->InsertItemToList(nIndex, pDevice);	//���豸�б��в���һ����¼
			nIndex ++;
		}
	}
}

//�޸�ע������豸��Ϣ
BOOL CDeviceManageDlg::ModifyDeviceInfoInRegister (DeviceInfo *pDevice)
{
	if (pDevice == NULL || m_pDeviceIO == NULL)
		return FALSE;

	int nRet = m_pDeviceIO->MoadDeRg (pDevice->nDriverType, pDevice->nDeviceNo, pDevice->nChannelCount, pDevice->strDeviceName);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

//���豸��Ϣ��ӵ�ָ�����б�������
void CDeviceManageDlg::InsertItemToList (int nItem, DeviceInfo *pDevice, BOOL bReplace/*=FALSE*/)
{
	if (pDevice == NULL)
		return;

	CString strText;
	TCHAR szDeviceTypeName[BUF_LEN_S];
	int nSubItem = 0;

	//�豸����
	if (bReplace)
		m_ctrlDeviceList.SetItemText (nItem, nSubItem, pDevice->strDeviceName);
	else
		m_ctrlDeviceList.InsertItem (nItem, pDevice->strDeviceName);
	nSubItem ++;

	//�豸���
	strText.Format (_T("%d"), pDevice->nDeviceNo);
	m_ctrlDeviceList.SetItemText (nItem, nSubItem, strText);
	nSubItem ++;

	//�豸����
	switch (pDevice->nDeviceType)
	{
	case devTypeProgrammableController:		//�ɱ�̿������豸
		strText.LoadString (IDS_DEV_TYPE_CONTROLLER);
		break;
	case devTypeDriver:						//�����豸
		strText.LoadString(IDS_DEV_TYPE_DRIVER);
		break;
	case devTypeProcessSample:				//�������ɼ��豸
		strText.LoadString(IDS_DEV_TYPE_PROCESS);
		break;
	case devTypeRelay:						//�̵����豸
		strText.LoadString(IDS_DEV_TYPE_RELAY);
		break;
	default:
		strText = _T("--");
		break;
	}
	m_ctrlDeviceList.SetItemText (nItem, nSubItem, strText);
	nSubItem ++;

	//��������
	memset(szDeviceTypeName, 0x0, sizeof(szDeviceTypeName));
	m_pDeviceIO->GetTypeDefaultInfo(pDevice->nDriverType, szDeviceTypeName);
	strText = szDeviceTypeName;
	if (strText.IsEmpty())
		strText.Format(_T("No Item"));
	m_ctrlDeviceList.SetItemText (nItem, nSubItem, strText);
	nSubItem ++;

	//ͨ����
	strText.Format (_T("%d"), pDevice->nChannelCount);
	m_ctrlDeviceList.SetItemText (nItem, nSubItem, strText);
	nSubItem ++;

// 	//�ɼ�ʱ����
// 	strText.Format (_T("%d"), pDevice->nSampleInterval);
// 	m_ctrlDeviceList.SetItemText (nItem, nSubItem, strText);
// 	nSubItem ++;

	m_ctrlDeviceList.SetItemData (nItem, (DWORD )pDevice);	//���б��б����豸��Ӧ���ڴ�����ָ��
}

//ɾ��ע����е���Ч�豸
BOOL CDeviceManageDlg::DeleteInvalidDeviceInRegister ()
{
	int nDeviceTypeArr[_MAX_DEVICE], nDeviceNoArr[_MAX_DEVICE], nDriverTypeCnt, nDeviceCnt, nInvalidDeviceCnt, i, j, nDriverType, nDeviceNo;

	nDriverTypeCnt = 0;
	nDeviceCnt = 0;
	nInvalidDeviceCnt = 0;
	m_pDeviceIO->GetCardTypeNum(nDriverTypeCnt);
	for (i=0; i<nDriverTypeCnt; i++)
	{
		nDeviceCnt = 0;
		m_pDeviceIO->GetCardTypeID(i, nDriverType);
		m_pDeviceIO->GetCardNum(nDriverType, nDeviceCnt);

		for (j=0; j<nDeviceCnt; j++)
		{
			m_pDeviceIO->GetCardID(nDriverType, j, nDeviceNo);

			if (!this->ExistInDeviceList (nDriverType, nDeviceNo, m_strLocalGUID))	//�ж��豸�������Ƿ��д��豸
			{
				nDeviceTypeArr[nInvalidDeviceCnt] = nDriverType;	//��ʱ�����豸�����в����ڵ��豸
				nDeviceNoArr[nInvalidDeviceCnt] = nDeviceNo;
				nInvalidDeviceCnt ++;
			}
		}
	}

	//ɾ���豸�����в����ڵ��豸
	for (i=0; i<nInvalidDeviceCnt; i++)
	{
		if (m_pDeviceIO->DltDeRg (nDeviceTypeArr[i], nDeviceNoArr[i]) != DEVICEIO_ERROR_SUCCESS)
			return FALSE;
	}

	return TRUE;
}

//�ж�ָ���豸���ڴ��������Ƿ����
BOOL CDeviceManageDlg::ExistInDeviceList (int nDriverType, int nDeviceNo, CString strGUID)
{
	DeviceInfo *pDevice = NULL;
	POSITION pos = m_listDevice.GetHeadPosition();
	while (pos != NULL)
	{
		pDevice = (DeviceInfo *)m_listDevice.GetAt(pos);

		if (nDriverType==pDevice->nDriverType && nDeviceNo==pDevice->nDeviceNo)
		{
			if (strGUID.Compare (pDevice->strDeviceGUID) == 0)
				return TRUE;
		}

		m_listDevice.GetNext(pos);
	}

	return FALSE;
}

void CDeviceManageDlg::OnItemchangedDeviceList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//�ж���Ŀ�ı�Ϊ״̬�ı�
	if ((pNMLV->uChanged & LVIF_STATE) == LVIF_STATE)
	{
		if (pNMLV->iItem < 0)
			return;

		DeviceInfo *pDevice = (DeviceInfo *)pNMLV->lParam;	//��Ŀ�б������豸��Ӧ�ڴ�ָ��
		if (pDevice == NULL)
			return;

		if ((pNMLV->uOldState & LVIS_SELECTED) == LVIS_SELECTED)		//ʧȥѡ��״̬
		{
			GetDlgItem(IDC_MODIFY_DEVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_DELETE_DEVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);
		}
		else if ((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)	//�õ�ѡ��״̬
		{
			GetDlgItem(IDC_MODIFY_DEVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_DELETE_DEVICE)->EnableWindow(TRUE);

			if (m_strLocalGUID.Compare (pDevice->strDeviceGUID) == 0)		//�����豸
			{
				BOOL bNeed = FALSE;
				int nHandle;
				int nRet = m_pDeviceIO->GetCardHandle(pDevice->nDriverType, pDevice->nDeviceNo, nHandle);
				if (nRet != DEVICEIO_ERROR_SUCCESS)
					return;

				//�жϴ��豸�ܷ�ͨ����չ����
				nRet = m_pDeviceIO->IsChannelDefineNeeded(nHandle, bNeed);
				if (nRet != DEVICEIO_ERROR_SUCCESS)
					return;

				if (bNeed)
					GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(TRUE);
				else
					GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);
			}
			else	//�Ǳ����豸
			{
				GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);
			}
		}
	}

	*pResult = 0;
}

//����豸
void CDeviceManageDlg::OnBnClickedAddDevice()
{
	int nDeviceCnt = 0;
	DeviceInfo *pDevice = NULL;
	POSITION pos = m_listDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = m_listDevice.GetNext (pos);
		if (pDevice->nOptFlag != OPT_DELETE)
			nDeviceCnt ++;
	}

	if (nDeviceCnt >= _MAX_DEVICE_IN_LIST)
	{
		CString strLoad, strText;
		strLoad.LoadString (IDS_DEV_LIST_FULL);
		strText.Format(strLoad, _MAX_DEVICE_IN_LIST);
		AfxMessageBox(strText);
		return;
	}

	CDeviceInfoDlg dlg(1);

	//��Ի�������ռ�õ��豸���
	pos = m_listDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = m_listDevice.GetNext (pos);
		if (pDevice->nOptFlag != OPT_DELETE)
			dlg.SetDeviceNoBeingUsed (pDevice->nDeviceNo);
	}

	int nDriverType, nRet;
	CString strDriverTypeName;
	DWORD dwDeviceTypeBit;
	TCHAR szDeviceTypeName[BUF_LEN_S];

	int nDeviceTypeCnt = 0;
	m_pDeviceIO->GetCardTypeNum(nDeviceTypeCnt);
	for (int i=0; i<nDeviceTypeCnt; i++)
	{
		nDriverType = 0;
		nRet = m_pDeviceIO->GetCardTypeID(i, nDriverType);	//�豸����
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;

		memset (szDeviceTypeName, 0, sizeof(szDeviceTypeName));
		strDriverTypeName.Empty();
		nRet = m_pDeviceIO->GetTypeDefaultInfo(nDriverType, szDeviceTypeName);	//�豸��������
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;
		strDriverTypeName = szDeviceTypeName;
		if (strDriverTypeName.IsEmpty())
			strDriverTypeName.Format(_T("%d"), nDriverType);

		dwDeviceTypeBit = 0;
		nRet = m_pDeviceIO->GetDevicePropInfo(nDriverType, dwDeviceTypeBit);	//�豸����λ
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;

		if (dwDeviceTypeBit != 0)
			dlg.SetDriverType (nDriverType, strDriverTypeName, dwDeviceTypeBit);
	}

	if (dlg.DoModal () == IDOK)
	{
		pDevice = new DeviceInfo();

		dlg.GetDeviceInfo (pDevice->strDeviceName, pDevice->nDeviceNo, pDevice->nDeviceType, pDevice->nDriverType, pDevice->nChannelCount);

		pDevice->strDeviceGUID = m_strLocalGUID;

		pDevice->nOptFlag = OPT_ADD;					//��ʾ���豸���¼��豸
		m_listDevice.AddTail(pDevice);					//�����豸��Ϣ�ӵ�������

		int nIndex = m_ctrlDeviceList.GetItemCount();	//�ü�¼������Ϊ�¼Ӽ�¼������
		this->InsertItemToList (nIndex, pDevice);		//���б��мӼ�¼

		this->ModifyDeviceInfoInRegister (pDevice);		//�������豸��һЩ��Ϣ����д��ע���

		m_pDeviceIO->Initialize (SOFTWARE_NAME);		//��������̬�����³�ʼ��

		//ѡ������ӵ��豸���������DeviceIO��Initialize�󣬷������豸�б��OnItemChange�����ﲻ���жϳ��¼ӵ��豸
		m_ctrlDeviceList.SetItemState(nIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_ctrlDeviceList.EnsureVisible(nIndex, FALSE);
	}
}

//�޸��豸
void CDeviceManageDlg::OnBnClickedModifyDevice()
{
	int nIndex = m_ctrlDeviceList.GetNextItem (-1, LVNI_SELECTED);
	if (nIndex < 0)
		return;

	CDeviceInfoDlg dlg(2);

	DeviceInfo *pDevice = (DeviceInfo *)m_ctrlDeviceList.GetItemData(nIndex);
	if (pDevice == NULL)
		return;
	dlg.SetDeviceInfo (pDevice->strDeviceName, pDevice->nDeviceNo, pDevice->nDeviceType, pDevice->nDriverType, pDevice->nChannelCount);

	//��Ի�������ռ�õ��豸���
	POSITION pos = m_listDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = m_listDevice.GetNext (pos);
		if (pDevice->nOptFlag != OPT_DELETE)
			dlg.SetDeviceNoBeingUsed (pDevice->nDeviceNo);
	}

	int nDriverType, nRet;
	CString strDriverTypeName;
	DWORD dwDeviceTypeBit;
	TCHAR szDeviceTypeName[BUF_LEN_S];

	int nDeviceTypeCnt = 0;
	m_pDeviceIO->GetCardTypeNum(nDeviceTypeCnt);
	for (int i=0; i<nDeviceTypeCnt; i++)
	{
		nDriverType = 0;
		nRet = m_pDeviceIO->GetCardTypeID(i, nDriverType);	//�豸����
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;

		memset (szDeviceTypeName, 0, sizeof(szDeviceTypeName));
		strDriverTypeName.Empty();
		nRet = m_pDeviceIO->GetTypeDefaultInfo(nDriverType, szDeviceTypeName);	//�豸��������
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;
		strDriverTypeName = szDeviceTypeName;
		if (strDriverTypeName.IsEmpty())
			strDriverTypeName.Format(_T("%d"), nDriverType);

		dwDeviceTypeBit = 0;
		nRet = m_pDeviceIO->GetDevicePropInfo(nDriverType, dwDeviceTypeBit);	//�豸����λ
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;

		if (dwDeviceTypeBit != 0)
			dlg.SetDriverType (nDriverType, strDriverTypeName, dwDeviceTypeBit);
	}

	if (dlg.DoModal () == IDOK)
	{
		pDevice = (DeviceInfo *)m_ctrlDeviceList.GetItemData(nIndex);

		dlg.GetDeviceInfo (pDevice->strDeviceName, pDevice->nDeviceNo, pDevice->nDeviceType, pDevice->nDriverType, pDevice->nChannelCount);

		if (pDevice->nOptFlag != OPT_ADD)			//���豸Ϊԭ�����ݿ��������豸(���¼��豸)
			pDevice->nOptFlag = OPT_MODIFY;			//��ʾ�豸��Ϣ���޸�

		this->InsertItemToList (nIndex, pDevice, TRUE);		//�޸��豸�б��м�¼

		if (m_strLocalGUID.Compare(pDevice->strDeviceGUID) == 0)			//�޸ĵ��Ǳ�����ӵ��豸
		{
			this->ModifyDeviceInfoInRegister (pDevice);		//�������豸��һЩ��Ϣ����д��ע���
			m_pDeviceIO->Initialize (SOFTWARE_NAME);		//��������̬�����³�ʼ��
		}
	}
}

//ɾ���豸
void CDeviceManageDlg::OnBnClickedDeleteDevice()
{
	int nIndex = m_ctrlDeviceList.GetNextItem (-1, LVNI_SELECTED);
	if (nIndex < 0)
		return;

	DeviceInfo *pDevice = (DeviceInfo *)m_ctrlDeviceList.GetItemData(nIndex);
	if (pDevice == NULL)
		return;

	//��ʾ�Ƿ�ɾ��
	if (AfxMessageBox(IDS_CONFIRM_DEL_DEV, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2) != IDYES)
		return;

	if (m_strLocalGUID.Compare (pDevice->strDeviceGUID) == 0)	//����Ǳ��ص��豸����ע����е���ϢҲɾ��
	{
		m_pDeviceIO->DltDeRg (pDevice->nDriverType, pDevice->nDeviceNo);
		m_pDeviceIO->Initialize(SOFTWARE_NAME);		//��������̬�����³�ʼ��
	}

	//ɾ��ʱ��ԭ���豸ֻ�ǽ����豸��Ϣ�е�nOptFlag�ó�OPT_DELETE���У����¼��豸�����豸��Ϣ��������ɾ��
	if (pDevice->nOptFlag == OPT_ADD)
	{
		//ɾ���豸�����е��豸
		POSITION pos = m_listDevice.Find (pDevice);
		m_listDevice.RemoveAt (pos);
		FreeAndNullPtr(pDevice);
	}
	else
		pDevice->nOptFlag = OPT_DELETE;

	m_ctrlDeviceList.DeleteItem (nIndex);
}

//��չ����
void CDeviceManageDlg::OnBnClickedExtendedDef()
{
	int nIndex = m_ctrlDeviceList.GetNextItem (-1, LVNI_SELECTED);
	if (nIndex < 0)
		return;

	DeviceInfo *pDevice = (DeviceInfo *)m_ctrlDeviceList.GetItemData(nIndex);
	if (pDevice == NULL)
		return;

	int nHandle;
	int nRet = m_pDeviceIO->GetCardHandle(pDevice->nDriverType, pDevice->nDeviceNo, nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return;

	nRet = m_pDeviceIO->ChannelDefine(nHandle, this);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return;

	m_pDeviceIO->Initialize(SOFTWARE_NAME);		//��������̬�����³�ʼ��
}

//�˳�
void CDeviceManageDlg::OnBnClickedOk()
{

	CDialogEx::OnOK();
}

void CDeviceManageDlg::OnBnClickedCancel()
{

	CDialogEx::OnCancel();
}

//�Ҽ�����豸�б�
void CDeviceManageDlg::OnRclickDeviceList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CPoint ptMenu = pNMItemActivate->ptAction;
	m_ctrlDeviceList.ClientToScreen(&ptMenu);

	CMenu menu, *pSubMenu;
	menu.LoadMenu(IDR_RIGHT_DEV_MANAGE);
	pSubMenu = menu.GetSubMenu(0);

	if (m_bShowAllDevice)	//��ʾȫ���豸
		pSubMenu->CheckMenuRadioItem(ID_SHOW_ALL_DEVICE, ID_SHOW_ALL_DEVICE, ID_SHOW_ALL_DEVICE, MF_BYCOMMAND);
	else	//ֻ��ʾ�����豸
		pSubMenu->CheckMenuRadioItem(ID_SHOW_LOCAL_DEVICE, ID_SHOW_LOCAL_DEVICE, ID_SHOW_LOCAL_DEVICE, MF_BYCOMMAND);

	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, ptMenu.x, ptMenu.y, this);

	*pResult = 0;
}

void CDeviceManageDlg::OnShowAllDevice() 
{
	if (m_bShowAllDevice)
		return;
	m_bShowAllDevice = TRUE;

	this->RefreshDeviceList();

	GetDlgItem(IDC_MODIFY_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);
}

void CDeviceManageDlg::OnShowLocalDevice() 
{
	if (!m_bShowAllDevice)
		return;
	m_bShowAllDevice = FALSE;

	this->RefreshDeviceList();

	GetDlgItem(IDC_MODIFY_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);
}
