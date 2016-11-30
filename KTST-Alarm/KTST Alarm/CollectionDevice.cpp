#include "StdAfx.h"
#include "CollectionDevice.h"


CollectionDevice::CollectionDevice(void)
{
}

CollectionDevice::CollectionDevice(int nDeviceNo)
{
	m_nDeviceNo = nDeviceNo;
}

CollectionDevice::~CollectionDevice(void)
{
}

//初始化
BOOL CollectionDevice::Initialize ()
{
	if (!InitializeDeviceInfoFromDB())
		return FALSE;

	if (!InitializeChannelInfoFromDB())
		return FALSE;

	if (!Device::Initialize())
		return FALSE;

	return TRUE;
}

//初始化设备的通道信息
BOOL CollectionDevice::InitializeDeviceInfoFromDB ()
{
	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	//设备信息
	int nDeviceType, nDriverType, nChannelCount, nSampleInterval, nDefBufLen;
	TCHAR szDeviceName[BUF_LEN_S], szDeviceGUID[BUF_LEN_M];
	BlobData<byte> bdDefBuf;

	memset (szDeviceName, 0x0, sizeof(szDeviceName));
	memset (szDeviceGUID, 0x0, sizeof(szDeviceGUID));

	int nDBRet = dbAccess.GetOneDeviceInfo (m_nDeviceNo, szDeviceName, nDeviceType, nDriverType, nChannelCount, 
										szDeviceGUID, nSampleInterval);
	if (nDBRet != RET_DB_SUCCESS)
		return FALSE;
	nDBRet = dbAccess.GetCurDefBufData (nDefBufLen, &bdDefBuf);
	if (nDBRet != RET_DB_SUCCESS)
		return FALSE;

	m_strDeviceName = szDeviceName;
	m_nDeviceType = nDeviceType;
	m_nDriverType = nDriverType;
	m_nChannelCount = nChannelCount;
	m_strDeviceGUID = szDeviceGUID;
	m_nSampleInterval = nSampleInterval;
	m_bdDefBuf = bdDefBuf;

	return TRUE;
}

//初始化设备的通道信息
BOOL CollectionDevice::InitializeChannelInfoFromDB ()
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
	CollectionChannel *pChannel = NULL;

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


		pChannel = new CollectionChannel();
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

		//解析报警级别和报警范围缓冲区
		pChannel->AnalyzeAlarmLevelBuf ();
		pChannel->AnalyzeAlarmRangeBuf ();

		dbAccess.MoveNext();
	}

	return TRUE;
}

//设备指定通道是否通知
BOOL CollectionDevice::SetChannelNotify (int nChannelNo, BOOL bNotify)
{
	CollectionChannel *pChannel = (CollectionChannel *)GetChannel (nChannelNo);
	if (pChannel == NULL)
		return FALSE;

	pChannel->SetChannelStateChangeNotify(bNotify);
	pChannel->SetAlarmLevelChangeNotify(bNotify);

	return TRUE;
}
