#include "StdAfx.h"
#include "RelayDevice.h"
#include "RelayChannel.h"

#import <msxml3.dll>


RelayDevice::RelayDevice(void)
{
	m_bOutputting = FALSE;
	m_bStarted = FALSE;
}


RelayDevice::~RelayDevice(void)
{
}

//�����ݿ��ʼ���豸��ͨ����Ϣ
BOOL RelayDevice::InitializeChannelInfoFromDB ()
{
	FreeObjPtrList(&m_listChannel);

	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	//ͨ����Ϣ
	int nDeviceNo, nChannelNo, nAlarmLevelDataLen, nAlarmRangeDataLen, nDefBufLen;
	TCHAR szChannelName[BUF_LEN_S], szUnitString[BUF_LEN_S];
	BOOL bWorkEnable;
	float fFullScaleHigh, fFullScaleLow, fCalibrationHigh, fCalibrationLow;
	byte byDataType, byDecimalDigits;
	BlobData<byte> bdAlarmLevelData, bdAlarmRangeData, bdDefBuf;
	RelayChannel *pChannel = NULL;

	int nDBRet = dbAccess.ExecGetAllChannel (m_nDeviceNo);
	if (nDBRet != RET_DB_SUCCESS)
		return FALSE;

	while (!dbAccess.IsReachEnd())
	{
		memset (szChannelName, 0x0, sizeof(szChannelName));
		memset (szUnitString, 0x0, sizeof(szUnitString));

		nDBRet = dbAccess.GetCurChannel (nDeviceNo, nChannelNo, szChannelName, bWorkEnable, fFullScaleHigh, 
										fFullScaleLow, fCalibrationHigh, fCalibrationLow, byDataType, 
										byDecimalDigits, szUnitString, nAlarmLevelDataLen, &bdAlarmLevelData, 
										nAlarmRangeDataLen, &bdAlarmRangeData);
		if (nDBRet != RET_DB_SUCCESS)
			return FALSE;
		nDBRet = dbAccess.GetCurDefBufData (nDefBufLen, &bdDefBuf);
		if (nDBRet != RET_DB_SUCCESS)
			return FALSE;


		pChannel = new RelayChannel();
		m_listChannel.AddTail(pChannel);

		pChannel->m_nDeviceNo = nDeviceNo;
		pChannel->m_nChannelNo = nChannelNo;
		pChannel->m_strChannelName = szChannelName;
		pChannel->m_bWorkEnable = bWorkEnable;
		pChannel->m_fFullScaleHigh = fFullScaleHigh;
		pChannel->m_fFullScaleLow = fFullScaleLow;
		pChannel->m_fCalibrationHigh = fCalibrationHigh;
		pChannel->m_fCalibrationLow = fCalibrationLow;
		pChannel->m_byDataType = byDataType;
		pChannel->m_byDecimalDigits = byDecimalDigits;
		pChannel->m_strUnitString = szUnitString;
		pChannel->m_bdAlarmLevelData = bdAlarmLevelData;
		pChannel->m_bdAlarmRangeData = bdAlarmRangeData;
		pChannel->m_bdDefBuf = bdDefBuf;

		dbAccess.MoveNext();
	}

	return TRUE;
}

//�豸��ʼ��
BOOL RelayDevice::Initialize ()
{
	if (!Device::Initialize())
		return FALSE;

	return TRUE;
}

//�豸����ʼ��
BOOL RelayDevice::UnInitialize ()
{
	if (!Device::UnInitialize())
		return FALSE;

	return TRUE;
}

//�豸��������
BOOL RelayDevice::Start ()
{
	if (!Device::Start())
		return FALSE;

	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO == NULL)
		return FALSE;

	//����豸���
	int nHandle;
	int nRet = pDeviceIO->GetCardHandle(m_nDriverType, m_nDeviceNo, nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS || nHandle == 0)
		return FALSE;

	//��
	nRet = pDeviceIO->Open(nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return FALSE;

	//����
// 	nRet = pDeviceIO->Start(nHandle);
// 	if (nRet != DEVICEIO_ERROR_SUCCESS)
// 		return FALSE;

	m_bStarted = TRUE;

	//�������й�ע��ͨ������
	RequestAllFollowingChannelData ();

	return TRUE;
}

//�豸ֹͣ����
BOOL RelayDevice::Stop ()
{
	if (!m_bStarted)
		return FALSE;

	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO == NULL)
		return FALSE;

	//����豸���
	int nHandle;
	int nRet = pDeviceIO->GetCardHandle(m_nDriverType, m_nDeviceNo, nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS || nHandle == 0)
		return FALSE;

// 	//ֹͣ
// 	nRet = pDeviceIO->Stop(nHandle);
// 	if (nRet != DEVICEIO_ERROR_SUCCESS)
// 		return FALSE;

	//�ر�
	nRet = pDeviceIO->Close(nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return FALSE;

	if (!Device::Stop())
		return FALSE;

	m_bStarted = FALSE;

	return TRUE;
}

//����֪ͨ
void RelayDevice::OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam)
{
	Device::OnNotify(code, wParam, lParam);

	RelayChannel *pChannel = NULL;
	POSITION pos = m_listChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = (RelayChannel *)m_listChannel.GetNext(pos);
		if (pChannel)
			pChannel->OnNotify(code, wParam, lParam);
	}
}

//��ʱ��
void RelayDevice::OnTimer ()
{
	Device::OnTimer();

	if (m_bStarted)
		Output ();
}

//�̵������
BOOL RelayDevice::Output ()
{
	if (m_bOutputting)
		return FALSE;

	int nChannelCount = m_listChannel.GetCount();
	if (nChannelCount <= 0)
		return FALSE;

	//�жϼ̵���ͨ����û�б��ʽ���ݸ���
	BOOL bExpressionUpdate = FALSE;
	RelayChannel *pChannel = NULL;
	POSITION pos = m_listChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = (RelayChannel *)m_listChannel.GetNext(pos);
		if (pChannel && pChannel->IsExpressionUpdated())
		{
			bExpressionUpdate = TRUE;
			break;
		}
	}

	if (!bExpressionUpdate)		//û��ͨ�����±��ʽ���ݣ��������
		return FALSE;


	//ѭ��Ҫ��ÿ���̵���ͨ����ֵ
	int nOutputValueArr[_MAX_ONEDEVICE_CHL];
	memset (nOutputValueArr, 0x0, sizeof(nOutputValueArr));

	pos = m_listChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = (RelayChannel *)m_listChannel.GetNext(pos);
		if (pChannel && pChannel->m_nChannelNo >= 0 && pChannel->m_nChannelNo < _MAX_ONEDEVICE_CHL)
		{
			nOutputValueArr[pChannel->m_nChannelNo] = pChannel->GetOutputValue();
			pChannel->SetExpressionUpdate(FALSE);
		}
	}


	//��ͨ���������
	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO == NULL)
		return FALSE;

	//����豸���
	int nHandle;
	int nRet = pDeviceIO->GetCardHandle(m_nDriverType, m_nDeviceNo, nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS || nHandle == 0)
		return FALSE;

	m_bOutputting = TRUE;
	nRet = pDeviceIO->DigitalOutputs(nHandle, 0, nChannelCount, nOutputValueArr);
	m_bOutputting = FALSE;
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

//��λָ���̵���ͨ��
void RelayDevice::ResetChannel (int nChannelNo)
{
	RelayChannel *pChannel = NULL;
	POSITION pos = m_listChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = (RelayChannel *)m_listChannel.GetNext(pos);
		if (pChannel && pChannel->m_nChannelNo == nChannelNo)
		{
			pChannel->Reset();
			break;
		}
	}
}

//��λ���м̵���ͨ��
void RelayDevice::ResetAllChannel ()
{
	RelayChannel *pChannel = NULL;
	POSITION pos = m_listChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = (RelayChannel *)m_listChannel.GetNext(pos);
		if (pChannel)
			pChannel->Reset();
	}
}

//�������й�ע��ͨ������
void RelayDevice::RequestAllFollowingChannelData ()
{
	RelayChannel *pChannel = NULL;
	POSITION pos = m_listChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = (RelayChannel *)m_listChannel.GetNext(pos);
		if (pChannel)
			pChannel->RequestAllFollowingChannelData();
	}
}
