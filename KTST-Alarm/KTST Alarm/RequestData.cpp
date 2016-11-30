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

//����Ҫ������豸
void RequestData::SetRequestDevice (CollectionDevice *pDevice)
{
	m_pDevice = pDevice;
}

//���Ҫ������豸
CollectionDevice* RequestData::GetRequestDevice ()
{
	return m_pDevice;
}

//���������豸��
int RequestData::GetRequestDeviceNo ()
{
	if (m_pDevice == NULL)
		return -1;

	return m_pDevice->m_nDeviceNo;
}

//��������
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

	//����������ҪServer��Ϣ
	CString strAppServerIP;
	UINT nAppServerPortNo;

	pDataCenter->GetServerInfo(strAppServerIP, nAppServerPortNo);

	//��������
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

	//����������
	int nChannelNo;
	int nOffset = 0;

	byte byChannelState, byAlarmLevel;
	float fMesureValue;
	CollectionChannel *pChannel = NULL;

	for (int i=0; i<nChannelCount; i++)
	{
		nChannelNo = (int )*(byte *)(pChBuf + nOffset);					//ͨ����
		nOffset += sizeof(byte);
		if (nChannelNo < 0 || nChannelNo >= _MAX_ONEDEVICE_CHL)
			return;

		byChannelState = *(byte *)(pChBuf + nOffset);	//ͨ��״̬
		nOffset += sizeof(byte);

		fMesureValue = *(float *)(pChBuf + nOffset);	//����ֵ
		nOffset += sizeof(float);

		byAlarmLevel = *(byte *)(pChBuf + nOffset);		//��������
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

	//ָ��ʱ������������
	odtsTimeSpan.SetDateTimeSpan(0, 0, 0, REQUEST_INTERVAL);
	if (odtCurrentTime >= (m_odtLastRequestTime + odtsTimeSpan))
		this->Request ();

	//�ж��Ƿ�ʱ��û���յ�����
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
