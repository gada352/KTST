#include "StdAfx.h"
#include "SMSDevice.h"

#include "CollectionDevice.h"
#include "CollectionChannel.h"

#import <msxml3.dll>


SMSDevice::SMSDevice(void)
{
	m_bDialUp = FALSE;
	m_bStarted = FALSE;
}

SMSDevice::~SMSDevice(void)
{
	ClearMemory ();
}

//设备初始化
BOOL SMSDevice::Initialize ()
{
	if (!Device::Initialize())
		return FALSE;

	//从设备自定义字段中读取短信设备所选择的采集通道形成链表
	AnalyzeSMSDeviceBuffer ();

	return TRUE;
}

//设备反初始化
BOOL SMSDevice::UnInitialize ()
{
	if (!Device::UnInitialize())
		return FALSE;

	m_bDialUp = FALSE;

	ClearMemory ();

	return TRUE;
}

//设备启动运行
BOOL SMSDevice::Start ()
{
	if (!Device::Start())
		return FALSE;

	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO == NULL)
		return FALSE;

	//获得设备句柄
	int nHandle;
	int nRet = pDeviceIO->GetCardHandle(m_nDriverType, m_nDeviceNo, nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS || nHandle == 0)
		return FALSE;

	//打开
	nRet = pDeviceIO->Open(nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return FALSE;

	//启动
	nRet = pDeviceIO->Start(nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return FALSE;

	m_bStarted = TRUE;

	//请求所有关注的通道数据
	RequestAllFollowingChannelData ();

	return TRUE;
}

//设备停止运行
BOOL SMSDevice::Stop ()
{
	if (!m_bStarted)
		return FALSE;

	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO == NULL)
		return FALSE;

	//获得设备句柄
	int nHandle;
	int nRet = pDeviceIO->GetCardHandle(m_nDriverType, m_nDeviceNo, nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS || nHandle == 0)
		return FALSE;

	//停止
	nRet = pDeviceIO->Stop(nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return FALSE;

	//关闭
	nRet = pDeviceIO->Close(nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return FALSE;

	if (!Device::Stop())
		return FALSE;

	m_bStarted = FALSE;

	return TRUE;
}

//接收通知
void SMSDevice::OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam)
{
	Device::OnNotify(code, wParam, lParam);

	switch (code)
	{
// 	case ncChannelStateChange:		//通道状态发生改变
	case ncAlarmLevelChange:		//通道报警级别发生改变
		{
			CollectionChannel *pChannel = (CollectionChannel *)wParam;
			if (pChannel == NULL)
				return;

			//判断此通道是否是本短信设备所关注的通道
			if (!IsFollowingChannel (pChannel->m_nDeviceNo, pChannel->m_nChannelNo))
				return;

			//如果报警级别由正常变为报警则触发发短信
			if (pChannel->m_byAlarmLevelOld == 0 && pChannel->m_byAlarmLevel > 0)
			{
				CDataCenter *pDataCenter = CDataCenter::GetInstance();
				if (pDataCenter == NULL)
					return;

				CollectionDevice *pDevice = pDataCenter->GetCollectionDevice(pChannel->m_nDeviceNo);
				if (pDevice == NULL)
					return;

				CString strDeviceName, strChannelName, strAlarmLevelName;

				//跟据报警级别得到报警级别名称
				AlarmLevel *pAlarmLevel = NULL;
				int nLevelCnt = pChannel->m_arrAlarmLevel.GetCount();
				for (int i=0; i<nLevelCnt; i++)
				{
					pAlarmLevel = pChannel->m_arrAlarmLevel.GetAt(i);
					if (pAlarmLevel && pAlarmLevel->byAlarmLevel == pChannel->m_byAlarmLevel)
					{
						strAlarmLevelName = pAlarmLevel->szAlarmLevelName;
						break;
					}
				}

				strDeviceName = pDevice->m_strDeviceName;
				strChannelName = pChannel->m_strChannelName;

				//拼出短信字符串
				CString strSMS, strText, strSMSTemplate;

				switch (pChannel->m_byDataType)
				{
				case datTypeProcess:	//过程量通道
					{
						CString strValue;
						strValue.Format(L"%0.6f", pChannel->m_fMesureValue);

						byte byDecimalDigits = pChannel->m_byDecimalDigits;
						byDecimalDigits = byDecimalDigits > 6 ? 6 : byDecimalDigits;

						if (byDecimalDigits == 0)
						{
							int len = strValue.GetLength();
							strValue = strValue.Left(len - 7);
						}
						else
						{
							int len = strValue.GetLength();
							strValue = strValue.Left(len - 6 + byDecimalDigits);
						}

						CString strText;
						//设备名称
						strText.LoadString(IDS_DEVICE);
						strSMS += strText + _T(": ") + strDeviceName;
						strSMS += _T("\r\n");
						//通道名称
						strText.LoadString(IDS_CHANNEL);
						strSMS += strText + _T(": ") + strChannelName;
						strSMS += _T("\r\n");
						//测量值
						strText.LoadString(IDS_MEASURE_VALUE);
						strSMS += strText + _T(": ") + strValue + pChannel->m_strUnitString;
						strSMS += _T("\r\n");
						//报警级别
						if (!strAlarmLevelName.IsEmpty())
						{
							strText.LoadString(IDS_ALARM_LEVEL);
							strSMS += strText + _T(": ") + strAlarmLevelName;
						}
					}
					break;

				default:
					{
						strSMSTemplate.LoadString(IDS_SMS_TEMPLATE);
						strText.Format(strSMSTemplate, strDeviceName, strChannelName);
						strSMS += strText;
					}
					break;
				}

				//发送短信
				if (!strSMS.IsEmpty())
					SendSMS (strSMS);
			}
		}
		break;

	default:
		break;
	}
}

//发送短信
BOOL SMSDevice::SendSMS (CString strText)
{
	if (strText.IsEmpty())
		return FALSE;

	if (strText.GetLength() > MAX_SMS_TEXT_LEN)
		strText = strText.Left(MAX_SMS_TEXT_LEN);

	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO == NULL)
		return FALSE;

	//获得设备句柄
	int nHandle;
	int nRet = pDeviceIO->GetCardHandle(m_nDriverType, m_nDeviceNo, nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS || nHandle == 0)
		return FALSE;

	BufferFlag flag = bfSMS;	//仅发送短信
	if (m_bDialUp)
		flag = bfDialUp;	//发送短信后拨号提醒

	//注意：短信猫动态库为多字节编码，传进去的短信文本内容为char类型
	CStringA strA(strText);
	nRet = pDeviceIO->AnalogOutputs(nHandle, flag, strA.GetLength(), (int *)(LPCSTR )strA);	//将字符串地址传给动态库
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return FALSE;


	return TRUE;
}

//清空内存
void SMSDevice::ClearMemory ()
{
	ClearFollowingChannelList ();
}

//解析短信设备信息，包括解析设备自定义缓冲区中的信息
BOOL SMSDevice::AnalyzeSMSDeviceBuffer ()
{
	FreeObjPtrList(&m_listFollowingChannel);

	int nLen = m_bdDefBuf.GetCount();
	if (nLen <= 0)
		return TRUE;

	//从缓冲区读出XML
	CString strXML((wchar_t *)m_bdDefBuf.GetData(), nLen/sizeof(wchar_t));


	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr;

	hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (FAILED (hr))
		return FALSE;

	pDoc->loadXML((_bstr_t )strXML);

	MSXML2::IXMLDOMNodeListPtr pNodeList;
	pNodeList = pDoc->GetchildNodes();

	if (pNodeList->Getlength() == 0)
		return FALSE;

	MSXML2::IXMLDOMNodePtr rootNode;
	rootNode = pNodeList->Getitem(0);

	CString rootName = rootNode->GetnodeName();
	if (rootName != _T("Device"))
		return FALSE;

	MSXML2::IXMLDOMNodeListPtr pChildNodeList;
	pChildNodeList = rootNode->GetchildNodes();
	int childNodeCount = pChildNodeList->Getlength();

	for (int i = 0; i < childNodeCount; i++)
	{
		MSXML2::IXMLDOMNodePtr childNode;
		childNode = pChildNodeList->Getitem(i);

		CString childNodeName = childNode->GetnodeName();
		CString childNodeXml = childNode->Getxml();

		if (childNodeName == _T("DialUp"))				//拨号提醒
		{
			CString strText = childNode->Gettext();
			m_bDialUp = strText.Compare(_T("0"));
		}

		if (childNodeName == _T("FollowChList"))		//关注通道列表
		{
			MSXML2::IXMLDOMNodeListPtr pChannelNodeList;
			pChannelNodeList = childNode->GetchildNodes();

			int channelNodeCount = pChannelNodeList->Getlength();

			for (int j = 0; j < channelNodeCount; j++)
			{
				MSXML2::IXMLDOMNodePtr channelNode;
				channelNode = pChannelNodeList->Getitem(j);

				CString channelName = channelNode->GetnodeName();
				if (channelName == _T("FollowCh"))
				{
					MSXML2::IXMLDOMNamedNodeMapPtr pAttrs = channelNode->Getattributes();

					MSXML2::IXMLDOMNodePtr pAttrDeviceNo = pAttrs->getNamedItem(_T("DeviceNo"));
					CString strDeviceNo = pAttrDeviceNo->GetnodeValue().bstrVal;

					MSXML2::IXMLDOMNodePtr pAttrChannelNo = pAttrs->getNamedItem(_T("ChannelNo"));
					CString strChannelNo = pAttrChannelNo->GetnodeValue().bstrVal;

					int nDeviceNo = _ttoi(strDeviceNo);
					int nChannelNo = _ttoi(strChannelNo);

					FollowingChannel *pChannel = new FollowingChannel();
					pChannel->nDeviceNo = nDeviceNo;
					pChannel->nChannelNo = nChannelNo;
					m_listFollowingChannel.AddTail(pChannel);
				}
			}
		}
	}

	return TRUE;
}

//保存短信设备信息，包括保存设备自定义缓冲区中的信息
BOOL SMSDevice::SaveSMSDeviceBuffer ()
{
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr;
	CString strText;

	//新XML
	hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (FAILED (hr))
		return FALSE;

	//设备
	MSXML2::IXMLDOMElementPtr rootDevice = pDoc->createElement((_bstr_t )_T("Device"));
	pDoc->appendChild(rootDevice);

	//拨号提醒
	MSXML2::IXMLDOMElementPtr docDialUp = pDoc->createElement((_bstr_t )_T("DialUp"));
	strText = m_bDialUp ? _T("1"):_T("0");
	docDialUp->Puttext((_bstr_t)strText);
	rootDevice->appendChild(docDialUp);

	//关注通道
	MSXML2::IXMLDOMElementPtr docFollowingChannel = pDoc->createElement((_bstr_t )_T("FollowChList"));
	rootDevice->appendChild(docFollowingChannel);

	MSXML2::IXMLDOMElementPtr docChannel;
	FollowingChannel *pChannel = NULL;
	POSITION pos = m_listFollowingChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = m_listFollowingChannel.GetNext(pos);

		docChannel = pDoc->createElement((_bstr_t )_T("FollowCh"));
		strText.Format(_T("%d"), pChannel->nDeviceNo);
		docChannel->setAttribute(_T("DeviceNo"), (_bstr_t )strText);
		strText.Format(_T("%d"), pChannel->nChannelNo);
		docChannel->setAttribute(_T("ChannelNo"), (_bstr_t )strText);
		docFollowingChannel->appendChild(docChannel);
	}


	//保存至设备的自定义缓冲区中
	CStringW strXML = pDoc->Getxml();
	m_bdDefBuf.SetData((byte *)(LPCWSTR )strXML, strXML.GetLength()*sizeof(wchar_t));

	return TRUE;
}

//清空关注的通道链表
void SMSDevice::ClearFollowingChannelList ()
{
	FreeObjPtrList(&m_listFollowingChannel);
}

//添加关注的通道
void SMSDevice::AddFollowingChannel (int nDeviceNo, int nChannelNo)
{
	FollowingChannel *pChannel = new FollowingChannel();
	pChannel->nDeviceNo = nDeviceNo;
	pChannel->nChannelNo = nChannelNo;
	m_listFollowingChannel.AddTail(pChannel);
}

//判断指定通道是否是本短信设备所关注的通道
BOOL SMSDevice::IsFollowingChannel (int nDeviceNo, int nChannelNo)
{
	FollowingChannel *pChannel = NULL;
	POSITION pos = m_listFollowingChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = m_listFollowingChannel.GetNext(pos);
		if (pChannel && pChannel->nDeviceNo == nDeviceNo && pChannel->nChannelNo == nChannelNo)
			return TRUE;
	}

	return FALSE;
}

//请求所有关注的通道数据
void SMSDevice::RequestAllFollowingChannelData ()
{
	CDataCenter *pDataCenter = CDataCenter::GetInstance();
	if (pDataCenter == NULL)
		return;

	FollowingChannel *pChannel = NULL;
	POSITION pos = m_listFollowingChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = m_listFollowingChannel.GetNext(pos);
		if (pChannel)
			pDataCenter->Request(pChannel->nDeviceNo, pChannel->nChannelNo);
	}
}
