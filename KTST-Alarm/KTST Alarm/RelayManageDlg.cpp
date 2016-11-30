// RelayManageDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "RelayManageDlg.h"
#include "afxdialogex.h"

#include "RelayChannelSetupDlg.h"

// CRelayManageDlg �Ի���

IMPLEMENT_DYNAMIC(CRelayManageDlg, CDialogEx)

CRelayManageDlg::CRelayManageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRelayManageDlg::IDD, pParent)
{
	m_nCurSelDevice = -1;
}

CRelayManageDlg::~CRelayManageDlg()
{
	this->ClearMemory ();
}

void CRelayManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RELAY_DEVICE_LIST, m_ctrlDeviceList);
	DDX_Control(pDX, IDC_RELAY_CHANNEL_LIST, m_ctrlChannelList);
}


BEGIN_MESSAGE_MAP(CRelayManageDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_RELAY_DEVICE_LIST, &CRelayManageDlg::OnSelchangeRelayDeviceList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RELAY_CHANNEL_LIST, &CRelayManageDlg::OnItemchangedRelayChannelList)
	ON_BN_CLICKED(IDC_RELAY_CHANNEL_SETUP, &CRelayManageDlg::OnBnClickedRelayChannelSetup)
	ON_BN_CLICKED(IDOK, &CRelayManageDlg::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_RELAY_CHANNEL_LIST, &CRelayManageDlg::OnDblclkRelayChannelList)
END_MESSAGE_MAP()


// CRelayManageDlg ��Ϣ�������


BOOL CRelayManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//���̵���ͨ���ı��ʽ�Ƿ���Ч
	CheckAllRelayChannelExpression ();

	m_ctrlChannelList.SetExtendedStyle (m_ctrlChannelList.GetExtendedStyle () 
										| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//�����б���ͷ
	CString strLoad;
	int nIndex = 0;
	strLoad.LoadString(IDS_NO);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_LEFT, 40);
	nIndex ++;
	strLoad.LoadString(IDS_NAME);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_LEFT, 140);
	nIndex ++;
	strLoad.LoadString(IDS_OUTPUT_DELAY);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_RIGHT, 100);
	nIndex ++;
	strLoad.LoadString(IDS_ENERGIZED);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_CENTER, 40);
	nIndex ++;
	strLoad.LoadString(IDS_BYPASS);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_CENTER, 40);
	nIndex ++;
	strLoad.LoadString(IDS_EXPRESSION);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_CENTER, 100);
	nIndex ++;

	GetDlgItem(IDC_RELAY_CHANNEL_SETUP)->EnableWindow(FALSE);

	this->RefreshRelayDeviceList ();		//ˢ���豸�б�

	if (m_ctrlDeviceList.GetCount() > 0)
	{
		m_nCurSelDevice = 0;
		m_ctrlDeviceList.SetCurSel(m_nCurSelDevice);

		RefreshRelayChannelList();
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//����̵���ģ���豸
void CRelayManageDlg::SetRelayDevice (const DeviceInfo &device)
{
	RelayDevice *pDevice = new RelayDevice();
	pDevice->SetDeviceInfo(device);
	m_listRelayDevice.AddTail(pDevice);
}

//����̵���ģ���豸
void CRelayManageDlg::SetRelayChannel (const ChannelInfo &channel)
{
	RelayChannel *pChannel = new RelayChannel();
	pChannel->SetChannelInfo(channel);
	pChannel->AnalyzeRelayChannelBuffer();	//�����̵���ͨ��������
	m_listRelayChannel.AddTail(pChannel);
}

//����ɼ�ģ���豸
void CRelayManageDlg::SetCollectionDevice (const DeviceInfo &device)
{
	m_listCollectionDevice.AddTail(new DeviceInfo(device));
}

//����ɼ�ģ���豸ͨ��
void CRelayManageDlg::SetCollectionChannel (const ChannelInfo &channel)
{
	m_listCollectionChannel.AddTail(new ChannelInfo(channel));
}

//��ü̵���ͨ������
CRelayChannelList* CRelayManageDlg::GetRelayChannelListPtr ()
{
	return &m_listRelayChannel;
}

void CRelayManageDlg::ClearMemory ()
{
	FreeObjPtrList(&m_listRelayDevice);
	FreeObjPtrList(&m_listRelayChannel);

	FreeObjPtrList(&m_listCollectionDevice);
	FreeObjPtrList(&m_listCollectionChannel);
}

void CRelayManageDlg::RefreshRelayDeviceList ()
{
	m_ctrlDeviceList.ResetContent();
	m_nCurSelDevice = -1;

	int nIndex;
	RelayDevice *pDevice = NULL;
	POSITION pos = m_listRelayDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = m_listRelayDevice.GetNext (pos);

		nIndex = m_ctrlDeviceList.AddString(pDevice->m_strDeviceName);
		if (nIndex >= 0)
			m_ctrlDeviceList.SetItemData(nIndex, (DWORD )pDevice);
	}
}

//ˢ��ͨ���б�
void CRelayManageDlg::RefreshRelayChannelList ()
{
	m_ctrlChannelList.DeleteAllItems();

	if (m_nCurSelDevice < 0)
		return;

	RelayDevice *pDevice = (RelayDevice *)m_ctrlDeviceList.GetItemData(m_nCurSelDevice);
	if (pDevice == NULL)
		return;

	int nItem = 0;
	int nSubItem;
	CString strText;

	RelayChannel *pChannel = NULL;
	POSITION pos = m_listRelayChannel.GetHeadPosition ();
	while (pos != NULL)
	{
		pChannel = m_listRelayChannel.GetNext (pos);
		if (pChannel->m_nDeviceNo == pDevice->m_nDeviceNo)
		{
			nSubItem = 0;
			strText.Format(_T("%d"), pChannel->m_nChannelNo);
			m_ctrlChannelList.InsertItem(nItem, strText);
			nSubItem ++;

			UpdateRelayChannelInfo(nItem, pChannel);

			m_ctrlChannelList.SetItemData(nItem, (DWORD )pChannel);

			nItem ++;
		}
	}
}

//����ͨ���б���ָ��ͨ������Ϣ
void CRelayManageDlg::UpdateRelayChannelInfo (int nItem, RelayChannel *pChannel)
{
	int nCount = m_ctrlChannelList.GetItemCount();
	if (nItem < 0 || nItem >= nCount || pChannel == NULL)
		return;

	CString strText, strLoad;
	int nSubItem = 1;

	//ͨ������
	m_ctrlChannelList.SetItemText(nItem, nSubItem, pChannel->m_strChannelName);
	nSubItem ++;

	//�����ʱ
	strText.Format(_T("%d"), pChannel->m_nOutputDelay);
	m_ctrlChannelList.SetItemText(nItem, nSubItem, strText);
	nSubItem ++;

	//���
	strText.Empty();
	if (pChannel->m_bEnergized)
		strText = _T("��");
	m_ctrlChannelList.SetItemText(nItem, nSubItem, strText);
	nSubItem ++;

	//��·
	strText.Empty();
	if (pChannel->m_bBypass)
		strText = _T("��");
	m_ctrlChannelList.SetItemText(nItem, nSubItem, strText);
	nSubItem ++;

	//���ʽ
	strText.Empty();
	if (pChannel->m_listExpression.GetCount() > 0)
	{
		strText.LoadString(IDS_HAVE_SET);
		if (!pChannel->m_bExpressionValid)
		{
			strLoad.LoadString(IDS_INVALID);
			strText += _T(" (");
			strText += strLoad;
			strText += _T(")");
		}
	}
	m_ctrlChannelList.SetItemText(nItem, nSubItem, strText);
	nSubItem ++;
}

//������м̵���ͨ���ı��ʽ�Ƿ���Ч
void CRelayManageDlg::CheckAllRelayChannelExpression ()
{
	RelayExpression *pExpression = NULL;
	POSITION posExpression;

	RelayChannel *pChannel = NULL;
	POSITION posChannel = m_listRelayChannel.GetHeadPosition();
	while (posChannel)
	{
		pChannel = m_listRelayChannel.GetNext(posChannel);
		if (pChannel == NULL)
			continue;

		posExpression = pChannel->m_listExpression.GetHeadPosition();
		while (posExpression)
		{
			pExpression = pChannel->m_listExpression.GetNext(posExpression);
			if (pExpression && pExpression->eExpressionType == etChannel)
			{
				if (!IsCollectionChannel(pExpression->nDeviceNo, pExpression->nChannelNo))
				{
					pChannel->m_bExpressionValid = FALSE;
					break;
				}
			}
		}
	}
}

//�ж�ָ��ͨ���Ƿ��вɼ�ͨ��
BOOL CRelayManageDlg::IsCollectionChannel (int nDeviceNo, int nChannelNo)
{
	ChannelInfo *pChannel = NULL;
	POSITION pos = m_listCollectionChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = m_listCollectionChannel.GetNext(pos);
		if (pChannel && pChannel->nDeviceNo == nDeviceNo && pChannel->nChannelNo == nChannelNo)
			return TRUE;
	}

	return FALSE;
}

void CRelayManageDlg::OnSelchangeRelayDeviceList()
{
	int nIndex = m_ctrlDeviceList.GetCurSel();
	if (nIndex<0 || nIndex==m_nCurSelDevice)
		return;

	m_nCurSelDevice = nIndex;	//������ѡ���豸����

	if (m_nCurSelDevice >= 0)
		RefreshRelayChannelList ();

	GetDlgItem(IDC_RELAY_CHANNEL_SETUP)->EnableWindow(FALSE);
}


void CRelayManageDlg::OnItemchangedRelayChannelList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//�ж���Ŀ�ı�Ϊ״̬�ı�
	if ((pNMLV->uChanged & LVIF_STATE) == LVIF_STATE)
	{
		if ((pNMLV->uOldState & LVIS_SELECTED) == LVIS_SELECTED)			//ʧȥѡ��״̬
		{
			GetDlgItem(IDC_RELAY_CHANNEL_SETUP)->EnableWindow(FALSE);		//���Ʋ������ð�ť
		}
		else if ((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)		//�õ�ѡ��״̬
		{
			GetDlgItem(IDC_RELAY_CHANNEL_SETUP)->EnableWindow(TRUE);		//���Ʋ������ð�ť
		}
	}

	*pResult = 0;
}


void CRelayManageDlg::OnDblclkRelayChannelList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if (m_ctrlChannelList.GetNextItem(-1, LVNI_SELECTED) >= 0)
		OnBnClickedRelayChannelSetup();

	*pResult = 0;
}

//��������
void CRelayManageDlg::OnBnClickedRelayChannelSetup()
{
	int nIndex = m_ctrlChannelList.GetNextItem (-1, LVNI_SELECTED);
	if (nIndex < 0)
		return;

	RelayChannel *pChannel = (RelayChannel *)m_ctrlChannelList.GetItemData (nIndex);
	if (pChannel == NULL)
		return;


	CRelayChannelSetupDlg dlg;

	ChannelInfo ci;
	pChannel->GetChannelInfo(ci);
	dlg.SetRelayChannel(ci);

	dlg.SetCollectionDeviceListPtr (&m_listCollectionDevice);
	dlg.SetCollectionChannelListPtr (&m_listCollectionChannel);

	if (dlg.DoModal() == IDOK)
	{
		dlg.GetRelayChannel(ci);

		pChannel->SetChannelInfo(ci);
		pChannel->AnalyzeRelayChannelBuffer();
		pChannel->m_bExpressionValid = TRUE;	//��Ϊͨ������OK��ı��ʽ��Ч
		pChannel->m_nOptFlag = OPT_MODIFY;

		UpdateRelayChannelInfo (nIndex, pChannel);
	}
}


void CRelayManageDlg::OnBnClickedOk()
{
	//����ÿ���̵���ͨ����������Ϣ
	RelayChannel *pChannel = NULL;
	POSITION pos = m_listRelayChannel.GetHeadPosition ();
	while (pos != NULL)
	{
		pChannel = m_listRelayChannel.GetNext (pos);
		if (pChannel)
			pChannel->SaveRelayChannelBuffer();	//����̵���ͨ����Ϣ�������ʱ�����ʽ��
	}

	CDialogEx::OnOK();
}
