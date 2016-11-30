#include "StdAfx.h"
#include "RequestData.h"


RequestData::RequestData(void)
{
	m_pDevice = NULL;

	m_bReceiveData = FALSE;
	m_odtLastRequestTime = COleDateTime::GetCurrentTime();
}

RequestData::~RequestData(void)
{
}

//设置要请求的设备
void RequestData::SetRequestDevice (CollectionDevice *pDevice)
{
	m_pDevice = pDevice;
}

//获得要请求的设备
CollectionDevice* RequestData::GetRequestDevice ()
{
	return m_pDevice;
}

//获得请求的设备号
int RequestData::GetRequestDeviceNo ()
{
	if (m_pDevice == NULL)
		return -1;

	return m_pDevice->m_nDeviceNo;
}

//请求数据
BOOL RequestData::Request ()
{
	if (m_pDevice == NULL)
		return FALSE;

	CCommunicate *pCommunicate = CKTSTAlarmApp::GetCommunicatePtr();
	if (pCommunicate == NULL)
		return FALSE;

	CDataCenter *pDataCenter = CDataCenter::GetInstance();
	if (pDataCenter == NULL)
		return FALSE;

	//从数据中心要Server信息
	CString strAppServerIP;
	UINT nAppServerPortNo;

	pDataCenter->GetServerInfo(strAppServerIP, nAppServerPortNo);

	//发送数据
	int nRet = pCommunicate->AppProtocol_1(strAppServerIP, nAppServerPortNo, m_pDevice->m_nDeviceNo);
	if (nRet == COMMUNICATE_ERROR_FAILURE)
		return FALSE;

	m_odtLastRequestTime = COleDateTime::GetCurrentTime();

	return TRUE;
}

void RequestData::OnReceive (int nChannelCount, byte *pChBuf)
{
	if (pChBuf == NULL)
		return;

	if (m_pDevice == NULL)
		return;

	//解析缓冲区
	int nChannelNo;
	int nOffset = 0;

	byte byChannelState, byAlarmLevel;
	float fMesureValue;
	CollectionChannel *pChannel = NULL;

	for (int i=0; i<nChannelCount; i++)
	{
		nChannelNo = (int )*(byte *)(pChBuf + nOffset);					//通道号
		nOffset += sizeof(byte);
		if (nChannelNo < 0 || nChannelNo >= _MAX_ONEDEVICE_CHL)
			return;

		byChannelState = *(byte *)(pChBuf + nOffset);	//通道状态
		nOffset += sizeof(byte);

		fMesureValue = *(float *)(pChBuf + nOffset);	//测量值
		nOffset += sizeof(float);

		byAlarmLevel = *(byte *)(pChBuf + nOffset);		//报警级别
		nOffset += sizeof(byte);

		pChannel = (CollectionChannel *)m_pDevice->GetChannel(nChannelNo);
		pChannel->SetChannelData(byChannelState, fMesureValue, byAlarmLevel);
	}

	m_bReceiveData = TRUE;
	m_odtLastReceiveTime = COleDateTime::GetCurrentTime();
}

void RequestData::OnTimer ()
{
	COleDateTime odtCurrentTime = COleDateTime::GetCurrentTime();
	COleDateTimeSpan odtsTimeSpan;

	//指定时间间隔请求数据
	odtsTimeSpan.SetDateTimeSpan(0, 0, 0, REQUEST_INTERVAL);
	if (odtCurrentTime >= (m_odtLastRequestTime + odtsTimeSpan))
		this->Request ();

	//判断是否长时间没接收到数据
	odtsTimeSpan.SetDateTimeSpan(0, 0, 0, RECEIVE_TIME_OUT);
	if (odtCurrentTime >= (m_odtLastReceiveTime + odtsTimeSpan))
	{
		for (int i=0; i<_MAX_ONEDEVICE_CHL; i++)
		{
			if (m_pDevice)
			{
				CollectionChannel *pChannel = (CollectionChannel *)m_pDevice->GetChannel(i);
				if (pChannel)
					pChannel->SetChannelData(csFault, 0.0f, 0);
			}
		}
	}
}
