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

//从数据库初始化设备的通道信息
BOOL RelayDevice::InitializeChannelInfoFromDB ()
{
	FreeObjPtrList(&m_listChannel);

	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	//通道信息
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

//设备初始化
BOOL RelayDevice::Initialize ()
{
	if (!Device::Initialize())
		return FALSE;

	return TRUE;
}

//设备反初始化
BOOL RelayDevice::UnInitialize ()
{
	if (!Device::UnInitialize())
		return FALSE;

	return TRUE;
}

//设备启动运行
BOOL RelayDevice::Start ()
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
// 	nRet = pDeviceIO->Start(nHandle);
// 	if (nRet != DEVICEIO_ERROR_SUCCESS)
// 		return FALSE;

	m_bStarted = TRUE;

	//请求所有关注的通道数据
	RequestAllFollowingChannelData ();

	return TRUE;
}

//设备停止运行
BOOL RelayDevice::Stop ()
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

// 	//停止
// 	nRet = pDeviceIO->Stop(nHandle);
// 	if (nRet != DEVICEIO_ERROR_SUCCESS)
// 		return FALSE;

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

//定时器
void RelayDevice::OnTimer ()
{
	Device::OnTimer();

	if (m_bStarted)
		Output ();
}

//继电器输出
BOOL RelayDevice::Output ()
{
	if (m_bOutputting)
		return FALSE;

	int nChannelCount = m_listChannel.GetCount();
	if (nChannelCount <= 0)
		return FALSE;

	//判断继电器通道有没有表达式数据更新
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

	if (!bExpressionUpdate)		//没有通道更新表达式数据，不用输出
		return FALSE;


	//循环要出每个继电器通道的值
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


	//多通道整体输出
	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO == NULL)
		return FALSE;

	//获得设备句柄
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

//复位指定继电器通道
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

//复位所有继电器通道
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

//请求所有关注的通道数据
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
