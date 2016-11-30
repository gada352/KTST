// SMSSetupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "SMSSetupDlg.h"
#include "afxdialogex.h"
#include "SelectChannelDlg.h"


// CSMSSetupDlg 对话框

IMPLEMENT_DYNAMIC(CSMSSetupDlg, CDialogEx)

CSMSSetupDlg::CSMSSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSMSSetupDlg::IDD, pParent)
{
	m_nCurSelDevice = -1;
}

CSMSSetupDlg::~CSMSSetupDlg()
{
	this->ClearMemory ();
}

void CSMSSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SMS_DEVICE_LIST, m_ctrlSMSDeviceList);
	DDX_Control(pDX, IDC_CHECK_DAIL_UP, m_ctrlDailUp);
	DDX_Control(pDX, IDC_SMS_FOLLOW_CH_LIST, m_ctrlSMSFollowChList);
}


BEGIN_MESSAGE_MAP(CSMSSetupDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_SMS_DEVICE_LIST, &CSMSSetupDlg::OnSelchangeSmsDeviceList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SMS_FOLLOW_CH_LIST, &CSMSSetupDlg::OnItemchangedSmsFollowChList)
	ON_BN_CLICKED(IDC_CHECK_DAIL_UP, &CSMSSetupDlg::OnBnClickedCheckDailUp)
	ON_BN_CLICKED(IDC_ADD_FOLLOW_CH, &CSMSSetupDlg::OnBnClickedAddFollowCh)
	ON_BN_CLICKED(IDC_DEL_FOLLOW_CH, &CSMSSetupDlg::OnBnClickedDelFollowCh)
	ON_BN_CLICKED(IDOK, &CSMSSetupDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSMSSetupDlg 消息处理程序


BOOL CSMSSetupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	m_ctrlSMSFollowChList.SetExtendedStyle (m_ctrlSMSFollowChList.GetExtendedStyle () 
											| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//设置列表列头
	CString strLoad;
	int nIndex = 0;
	strLoad.LoadString(IDS_DEVICE_NAME);
	m_ctrlSMSFollowChList.InsertColumn(nIndex, strLoad, LVCFMT_LEFT, 150);
	nIndex ++;
	strLoad.LoadString(IDS_CHANNEL_NAME);
	m_ctrlSMSFollowChList.InsertColumn(nIndex, strLoad, LVCFMT_LEFT, 150);
	nIndex ++;

	GetDlgItem(IDC_DEL_FOLLOW_CH)->EnableWindow(FALSE);

	this->RefreshSMSDeviceList ();		//刷新设备列表

	if (m_ctrlSMSDeviceList.GetCount() > 0)
	{
		m_nCurSelDevice = 0;
		m_ctrlSMSDeviceList.SetCurSel(m_nCurSelDevice);

		ShowSMSDeviceInfo(m_nCurSelDevice);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//传入短信模块设备
void CSMSSetupDlg::SetSMSDevice (const DeviceInfo &device)
{
	SMSDevice *pDevice = new SMSDevice();
	pDevice->SetDeviceInfo(device);
	m_listSMSDevice.AddTail(pDevice);
}

//传入采集模块设备
void CSMSSetupDlg::SetCollectionDevice (const DeviceInfo &device)
{
	m_listCollectionDevice.AddTail(new DeviceInfo(device));
}

//传入采集模块设备通道
void CSMSSetupDlg::SetCollectionChannel (const ChannelInfo &channel)
{
	m_listCollectionChannel.AddTail(new ChannelInfo(channel));
}

//获得短信设备链表
CSMSDeviceList* CSMSSetupDlg::GetSMSDeviceListPtr ()
{
	return &m_listSMSDevice;
}

void CSMSSetupDlg::ClearMemory ()
{
	FreeObjPtrList(&m_listSMSDevice);

	FreeObjPtrList(&m_listCollectionDevice);
	FreeObjPtrList(&m_listCollectionChannel);
}

void CSMSSetupDlg::RefreshSMSDeviceList ()
{
	m_ctrlSMSDeviceList.ResetContent();
	m_nCurSelDevice = -1;

	int nIndex;
	SMSDevice *pDevice = NULL;
	POSITION pos = m_listSMSDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = m_listSMSDevice.GetNext (pos);
		
		pDevice->AnalyzeSMSDeviceBuffer();	//解析短信设备的自定义缓冲区信息：关注通道、拨号提醒等

		nIndex = m_ctrlSMSDeviceList.AddString(pDevice->m_strDeviceName);
		if (nIndex >= 0)
			m_ctrlSMSDeviceList.SetItemData(nIndex, (DWORD )pDevice);
	}
}

//刷新通道列表
void CSMSSetupDlg::RefreshFollowChannelList ()
{
	m_ctrlSMSFollowChList.DeleteAllItems();

	if (m_nCurSelDevice < 0)
		return;

	SMSDevice *pDevice = (SMSDevice *)m_ctrlSMSDeviceList.GetItemData(m_nCurSelDevice);
	if (pDevice == NULL)
		return;

	int nItem = 0;
	int nSubItem;
	FollowingChannel *pFollowingCh = NULL;
	DeviceInfo *pDeviceInfo = NULL;
	ChannelInfo *pChannelInfo = NULL;
	POSITION pos, posDel;

	pos = pDevice->m_listFollowingChannel.GetHeadPosition ();
	while (pos != NULL)
	{
		posDel = pos;
		pFollowingCh = pDevice->m_listFollowingChannel.GetNext (pos);

		//根据关注的设备号和通道，找到对应的采集设备和通道信息
		pDeviceInfo = GetCollectionDevice(pFollowingCh->nDeviceNo);
		pChannelInfo = GetCollectionChannel (pFollowingCh->nDeviceNo, pFollowingCh->nChannelNo);

		if (pDeviceInfo == NULL || pChannelInfo == NULL)	//已关注的设备通道不存在
		{
			pDevice->m_nOptFlag = OPT_MODIFY;
			continue;
		}

		nSubItem = 0;
		m_ctrlSMSFollowChList.InsertItem(nItem, pDeviceInfo->strDeviceName);
		nSubItem ++;
		m_ctrlSMSFollowChList.SetItemText(nItem, nSubItem, pChannelInfo->strChannelName);
		nSubItem ++;

		m_ctrlSMSFollowChList.SetItemData(nItem, (DWORD )pChannelInfo);

		nItem ++;
	}
}

void CSMSSetupDlg::OnSelchangeSmsDeviceList()
{
	int nIndex = m_ctrlSMSDeviceList.GetCurSel();
	if (nIndex<0 || nIndex==m_nCurSelDevice)
		return;

	//保存之前选中的短信设备信息
	if (m_nCurSelDevice >= 0)
		SaveSMSDeviceInfo (m_nCurSelDevice);

	m_nCurSelDevice = nIndex;	//保存新选中设备索引

	if (m_nCurSelDevice >= 0)
		ShowSMSDeviceInfo (m_nCurSelDevice);

	GetDlgItem(IDC_DEL_FOLLOW_CH)->EnableWindow(FALSE);
}


void CSMSSetupDlg::OnItemchangedSmsFollowChList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//判断项目改变为状态改变
	if ((pNMLV->uChanged & LVIF_STATE) == LVIF_STATE)
	{
		int nCount = m_ctrlSMSFollowChList.GetSelectedCount();
		GetDlgItem(IDC_DEL_FOLLOW_CH)->EnableWindow(nCount > 0);
	}

	*pResult = 0;
}

//设置发送短信后拨号提醒
void CSMSSetupDlg::OnBnClickedCheckDailUp()
{
	if (m_nCurSelDevice < 0)
		return;

	SMSDevice *pDevice = (SMSDevice *)m_ctrlSMSDeviceList.GetItemData(m_nCurSelDevice);
	if (pDevice == NULL)
		return;

	pDevice->m_nOptFlag = OPT_MODIFY;
}

//添加关注
void CSMSSetupDlg::OnBnClickedAddFollowCh()
{
	if (m_nCurSelDevice < 0)
		return;

	CSelectChannelDlg dlg;

	dlg.SetDeviceListPtr(&m_listCollectionDevice);
	dlg.SetChannelListPtr(&m_listCollectionChannel);

	if (dlg.DoModal() == IDOK)
	{
		//取消选中的通道
		int nItem = -1;
		int nCount = m_ctrlSMSFollowChList.GetSelectedCount();
		for (int i=0; i<nCount; i++)
		{
			nItem = m_ctrlSMSFollowChList.GetNextItem(nItem, LVNI_SELECTED);
			m_ctrlSMSFollowChList.SetItemState(nItem, ~LVIS_SELECTED, LVIS_SELECTED);
		}

		//将选中的采集通道循环插入到关注列表中
		CChannelInfoList *pListSelChannel = dlg.GetSelChannelListPrt();
		if (pListSelChannel == NULL || pListSelChannel->GetCount() <= 0)
			return;

		int nLastItem;

		nLastItem = -1;
		ChannelInfo *pSelChannel = NULL;
		POSITION pos = pListSelChannel->GetHeadPosition();
		while (pos)
		{
			pSelChannel = pListSelChannel->GetNext(pos);
			if (pSelChannel)
			{
				nItem = InsertFollowingChannel(pSelChannel);
				if (nItem >= 0)
					nLastItem = nItem;
			}
		}

		if (nLastItem >= 0)
			m_ctrlSMSFollowChList.EnsureVisible(nLastItem, FALSE);
		m_ctrlSMSFollowChList.SetFocus();
	}
}

//插入新选择的通道
int CSMSSetupDlg::InsertFollowingChannel (ChannelInfo *pSelChannel)
{
	if (pSelChannel == NULL)
		return -1;

	SMSDevice *pDevice = (SMSDevice *)m_ctrlSMSDeviceList.GetItemData(m_nCurSelDevice);
	if (pDevice == NULL)
		return -1;

	DeviceInfo *pDeviceInfo = GetCollectionDevice(pSelChannel->nDeviceNo);	//得到选中通道所属的设备
	if (pDeviceInfo == NULL)
		return -1;

	int nItem, nSubItem;

	//循环已关注列表，按设备号和通道号排序，插到指定位置
	int nCount = m_ctrlSMSFollowChList.GetItemCount();
	for (nItem=0; nItem<nCount; nItem++)
	{
		FollowingChannel *pFollowingCh = (FollowingChannel *)m_ctrlSMSFollowChList.GetItemData(nItem);

		//此通道已经选择过
		if (pSelChannel->nDeviceNo == pFollowingCh->nDeviceNo 
			&& pSelChannel->nChannelNo == pFollowingCh->nChannelNo)
			return -1;

		//排序规则：设备号和通道号由小到大
		if (pSelChannel->nDeviceNo < pFollowingCh->nDeviceNo 
			|| (pSelChannel->nDeviceNo == pFollowingCh->nDeviceNo && pSelChannel->nChannelNo < pFollowingCh->nChannelNo))
			break;
	}

	//插入到界面列表中
	nSubItem = 0;
	m_ctrlSMSFollowChList.InsertItem(nItem, pDeviceInfo->strDeviceName);
	nSubItem ++;
	m_ctrlSMSFollowChList.SetItemText(nItem, nSubItem, pSelChannel->strChannelName);
	nSubItem ++;

	m_ctrlSMSFollowChList.SetItemData(nItem, (DWORD_PTR )pSelChannel);
	m_ctrlSMSFollowChList.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);

	pDevice->m_nOptFlag = OPT_MODIFY;

	return nItem;
}

//取消关注
void CSMSSetupDlg::OnBnClickedDelFollowCh()
{
	if (m_nCurSelDevice < 0)
		return;

	SMSDevice *pDevice = (SMSDevice *)m_ctrlSMSDeviceList.GetItemData(m_nCurSelDevice);
	if (pDevice == NULL)
		return;

	ChannelInfo *pChannel = NULL;
	int nCount = m_ctrlSMSFollowChList.GetSelectedCount();

	for (int i=0; i<nCount; i++)
	{
		int nItem = m_ctrlSMSFollowChList.GetNextItem(-1, LVNI_SELECTED);
		m_ctrlSMSFollowChList.DeleteItem(nItem);
	}

	pDevice->m_nOptFlag = OPT_MODIFY;
}

//确定
void CSMSSetupDlg::OnBnClickedOk()
{
	//保存当前选中的短信设备信息
	if (m_nCurSelDevice >= 0)
		SaveSMSDeviceInfo (m_nCurSelDevice);


	//每个短信设备保存缓冲区信息
	SMSDevice *pDevice = NULL;
	POSITION pos = m_listSMSDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = m_listSMSDevice.GetNext (pos);

		pDevice->SaveSMSDeviceBuffer();	//保存短信设备的自定义缓冲区信息：关注通道、拨号提醒等
	}

	CDialogEx::OnOK();
}

//获得指定采集设备
DeviceInfo* CSMSSetupDlg::GetCollectionDevice (int nDeviceNo)
{
	DeviceInfo *pDevice = NULL;
	POSITION pos = m_listCollectionDevice.GetHeadPosition();
	while (pos)
	{
		pDevice = m_listCollectionDevice.GetNext(pos);
		if (pDevice && pDevice->nDeviceNo == nDeviceNo)
			return pDevice;
	}

	return NULL;
}

//获得指定采集通道
ChannelInfo* CSMSSetupDlg::GetCollectionChannel (int nDeviceNo, int nChannelNo)
{
	ChannelInfo *pChannel = NULL;
	POSITION pos = m_listCollectionChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = m_listCollectionChannel.GetNext(pos);
		if (pChannel && pChannel->nDeviceNo == nDeviceNo && pChannel->nChannelNo == nChannelNo)
			return pChannel;
	}

	return NULL;
}

//显示短信设备信息：拨号提醒、关注通道列表等
BOOL CSMSSetupDlg::ShowSMSDeviceInfo (int nDeviceIndex)
{
	if (nDeviceIndex < 0 || nDeviceIndex>=m_ctrlSMSDeviceList.GetCount())
		return FALSE;

	SMSDevice *pDevice = (SMSDevice *)m_ctrlSMSDeviceList.GetItemData(nDeviceIndex);
	if (pDevice == NULL)
		return FALSE;

	//拨号提醒
	m_ctrlDailUp.SetCheck(pDevice->m_bDialUp);

	//关注的采集通道
	this->RefreshFollowChannelList ();

	return TRUE;
}

//保存短信设备信息：拨号提醒、关注通道列表等
BOOL CSMSSetupDlg::SaveSMSDeviceInfo (int nDeviceIndex)
{
	if (nDeviceIndex < 0 || nDeviceIndex>=m_ctrlSMSDeviceList.GetCount())
		return FALSE;

	SMSDevice *pDevice = (SMSDevice *)m_ctrlSMSDeviceList.GetItemData(nDeviceIndex);
	if (pDevice == NULL)
		return FALSE;

	//拨号提醒
	pDevice->m_bDialUp = m_ctrlDailUp.GetCheck();

	//关注的采集通道
	pDevice->ClearFollowingChannelList();

	ChannelInfo *pChannel = NULL;
	int nCount = m_ctrlSMSFollowChList.GetItemCount();
	for (int i=0; i<nCount; i++)
	{
		pChannel = (ChannelInfo *)m_ctrlSMSFollowChList.GetItemData(i);
		if (pChannel)
			pDevice->AddFollowingChannel(pChannel->nDeviceNo, pChannel->nChannelNo);
	}

	return TRUE;
}
