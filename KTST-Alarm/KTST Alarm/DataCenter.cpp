#include "StdAfx.h"
#include "DataCenter.h"
#include "KTST Alarm.h"

#include "SMSDevice.h"
#include "RelayDevice.h"

#include <string>


//静态成员初始化
CDataCenter* CDataCenter::m_pDataCenter = NULL;
CDataCenter::AutoRelease CDataCenter::ar;


CDataCenter::CDataCenter(void)
{
	m_bStart = FALSE;


	m_strAppServerIP.Empty();
	m_nAppServerPortNo = 0;
}

CDataCenter::~CDataCenter(void)
{
	Stop ();
}

CDataCenter* CDataCenter::GetInstance ()
{
	if (m_pDataCenter == NULL)
		m_pDataCenter = new CDataCenter();

	return m_pDataCenter;
}

void CDataCenter::Release ()
{
	FreeAndNullPtr(m_pDataCenter);
}

BOOL CDataCenter::Start ()
{
	//初始化Server信息
	if (!InitialServerInfo ())
		return FALSE;

	CCommunicate *pCommunicate = CKTSTAlarmApp::GetCommunicatePtr();
	if (pCommunicate == NULL)
		return FALSE;
	pCommunicate->Initialize();


	//初始化短信和继电器设备
	if (!InitialAlarmOutputDevice ())
		return FALSE;

	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO == NULL)
		return FALSE;

	pDeviceIO->Initialize(SOFTWARE_NAME);

	pDeviceIO->CustomBuild(dvrTypeSMSDevice);		//SMS设备需要调用此函数启动通讯

	//短信设备初始化时向数据 中心发送请求的设备和通道
	//报警输出设备初始化，运行
	Device *pDevice = NULL;
	POSITION pos = m_listAlarmOutputDevice.GetHeadPosition ();

	while (pos)
	{
		pDevice = m_listAlarmOutputDevice.GetNext (pos);
		if (pDevice)
		{
			pDevice->Initialize();
			pDevice->Start();
		}
	}

	m_bStart = TRUE;

	m_nTimerID = ::SetTimer(NULL, 0, 1000, (TIMERPROC )CDataCenter::TimerProc);

	return TRUE;
}

BOOL CDataCenter::Stop ()
{
	Device *pDevice = NULL;
	POSITION pos = m_listAlarmOutputDevice.GetHeadPosition ();

	while (pos)
	{
		pDevice = m_listAlarmOutputDevice.GetNext (pos);
		if (pDevice)
		{
			pDevice->Stop();
			pDevice->UnInitialize();
		}
	}

	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO)
	{
		pDeviceIO->CloseAllComm(dvrTypeModbusRTU);		//ModbusRTU设备需要调用关闭
		pDeviceIO->CloseAllComm(dvrTypeModbusTCP);		//ModbusTCP设备需要调用关闭
		pDeviceIO->CloseAllComm(dvrTypeSMSDevice);		//SMS设备需要调用关闭
	}


	FreeObjPtrList(&m_listRequestData);
	FreeObjPtrList(&m_listCollectionDevice);
	FreeObjPtrList(&m_listAlarmOutputDevice);

	CCommunicate *pCommunicate = CKTSTAlarmApp::GetCommunicatePtr();
	if (pCommunicate)
		pCommunicate->UnInitialize();

	m_bStart = FALSE;
	KillTimer (NULL, m_nTimerID);

	return TRUE;
}

//数据中心定时器回调函数
VOID CALLBACK CDataCenter::TimerProc (HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	CDataCenter *pDataCenter = CDataCenter::GetInstance();
	if (pDataCenter)
		pDataCenter->OnTimer();
}

//定时器消息
void CDataCenter::OnTimer ()
{
	if (!m_bStart)
		return;

	//调用数据请求类定时器消息
	RequestData *pRD = NULL;
	POSITION pos = m_listRequestData.GetHeadPosition();
	while (pos)
	{
		pRD = m_listRequestData.GetNext(pos);

		pRD->OnTimer();
	}

	Device *pDevice = NULL;
	pos = m_listAlarmOutputDevice.GetHeadPosition();
	while (pos)
	{
		pDevice = m_listAlarmOutputDevice.GetNext(pos);
		if (pDevice)
			pDevice->OnTimer();
	}
}

//*************************************************************************************************
//	功能：	收到UDP消息
//	参数：	WPARAM wParam[in]，缓冲区地址
//			LPARAM lParam[in]，长度
//*************************************************************************************************
LRESULT CDataCenter::OnUdpReceiveMessage (WPARAM wParam, LPARAM lParam)
{
	if (!m_bStart)
		return S_FALSE;

	if (lParam == 0)
	   return S_FALSE;

	tagUdpComm *pUdpCommObj = (tagUdpComm *)wParam;
	if (pUdpCommObj == NULL)
		return S_FALSE;

	BYTE *pBuf = pUdpCommObj->byRecBuff;
	int nOffset = 0;

	BYTE byProtocolNo = pBuf[0];
	nOffset += sizeof(BYTE);

	switch (byProtocolNo)
	{
	case 2:		//收到实时数据
		{
			//采集设备号
			int nDeviceNo = *(int *)(pBuf+nOffset);
			nOffset += sizeof(int);

			//通道个数
			int nChannelCount = (int )*(byte *)(pBuf+nOffset);
			nOffset += sizeof(byte);

			RequestData *pRequest = GetRequestData(nDeviceNo);
			if (pRequest == NULL)
				return S_FALSE;

			//通道数据缓冲区
			byte *byChBuf = pBuf+nOffset;

			pRequest->OnReceive(nChannelCount, byChBuf);
		}
		break;

	default:  //处理不了
		break;
	}

	return S_OK;
}

RequestData* CDataCenter::GetRequestData (int nDeviceNo)
{
	RequestData *pRD = NULL;
	POSITION pos = m_listRequestData.GetHeadPosition();
	while (pos)
	{
		pRD = m_listRequestData.GetNext(pos);
		if (pRD->GetRequestDeviceNo() == nDeviceNo)
			return pRD;
	}

	return NULL;
}

CollectionDevice* CDataCenter::GetCollectionDevice (int nDeviceNo)
{
	CollectionDevice *pDevice = NULL;
	POSITION pos = m_listCollectionDevice.GetHeadPosition();
	while (pos)
	{
		pDevice = m_listCollectionDevice.GetNext(pos);
		if (pDevice && pDevice->m_nDeviceNo == nDeviceNo)
			return pDevice;
	}

	return NULL;
}

//接收通知
void CDataCenter::OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam)
{
	switch (code)
	{
	case ncChannelStateChange:		//通道状态发生改变
	case ncAlarmLevelChange:		//通道报警级别发生改变
		{
			Device *pDevice = NULL;
			POSITION pos = m_listAlarmOutputDevice.GetHeadPosition ();

			while (pos)
			{
				pDevice = m_listAlarmOutputDevice.GetNext (pos);
				if (pDevice)
					pDevice->OnNotify(code, wParam, lParam);		//传递给短信和继电器设备
			}
		}
		break;

	default:
		break;
	}
}

//初始化Server信息
BOOL CDataCenter::InitialServerInfo ()
{
	//从数据库中读Server信息
	byte byAppServerIP[4];
	int nAppServerPortNo;

	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());
	int nDBRet = dbAccess.GetServerIPandPort(byAppServerIP, nAppServerPortNo);
	if (nDBRet != RET_DB_SUCCESS)
		return FALSE;

	m_strAppServerIP.Format(_T("%d.%d.%d.%d"), byAppServerIP[0], byAppServerIP[1], byAppServerIP[2], byAppServerIP[3]);
	m_nAppServerPortNo = nAppServerPortNo;

	return TRUE;
}

//要Server信息
void CDataCenter::GetServerInfo (CString &strAppServerIP, UINT &nAppServerPortNo)
{
	strAppServerIP = m_strAppServerIP;
	nAppServerPortNo = m_nAppServerPortNo;
}

//初始化设备信息
BOOL CDataCenter::InitialAlarmOutputDevice ()
{
	FreeObjPtrList(&m_listAlarmOutputDevice);

	//得到本地GUID
	TCHAR szGUID[BUF_LEN_M];
	memset (szGUID, 0x0, sizeof(szGUID));
	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO)
		pDeviceIO->GetLcdeGid(szGUID);
	CString strLocalGUID = szGUID;

	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	//设备信息
	int nDeviceNo, nDeviceType, nDriverType, nChannelCount, nSampleInterval, nDefBufLen;
	TCHAR szDeviceName[BUF_LEN_S], szDeviceGUID[BUF_LEN_M];
	BlobData<byte> bdDefBuf;
	Device *pDevice = NULL;
	COleDateTimeSpan odtsTimeSpan;

	COleDateTime odtCurrentTime = COleDateTime::GetCurrentTime();

	int nDBRet = dbAccess.ExecGetAllDevice ();
	if (nDBRet != RET_DB_SUCCESS)
		return FALSE;

	while (!dbAccess.IsReachEnd())
	{
		memset (szDeviceName, 0x0, sizeof(szDeviceName));
		memset (szDeviceGUID, 0x0, sizeof(szDeviceGUID));

		nDBRet = dbAccess.GetCurDevice (nDeviceNo, szDeviceName, nDeviceType, nDriverType, nChannelCount, 
										szDeviceGUID, nSampleInterval);
		if (nDBRet != RET_DB_SUCCESS)
			return FALSE;
		nDBRet = dbAccess.GetCurDefBufData (nDefBufLen, &bdDefBuf);
		if (nDBRet != RET_DB_SUCCESS)
			return FALSE;

		if (_tcscmp(szGUID, szDeviceGUID) != 0)		//只要本地设备
		{
			dbAccess.MoveNext();
			continue;
		}

		pDevice = NULL;
		if (nDeviceType == devTypeDriver && nDriverType == dvrTypeSMSDevice)
		{
			pDevice = new SMSDevice();
			m_listAlarmOutputDevice.AddTail(pDevice);
		}
		if (nDeviceType == devTypeRelay)
		{
			pDevice = new RelayDevice();
			m_listAlarmOutputDevice.AddTail(pDevice);
		}

		if (pDevice)
		{
			pDevice->m_nDeviceNo = nDeviceNo;
			pDevice->m_strDeviceName = szDeviceName;
			pDevice->m_nDeviceType = nDeviceType;
			pDevice->m_nDriverType = nDriverType;
			pDevice->m_nChannelCount = nChannelCount;
			pDevice->m_strDeviceGUID = szDeviceGUID;
			pDevice->m_nSampleInterval = nSampleInterval;
			pDevice->m_bdDefBuf = bdDefBuf;
		}

		dbAccess.MoveNext();
	}

	//初始化设备的通道信息
	POSITION pos = m_listAlarmOutputDevice.GetHeadPosition ();
	while (pos)
	{
		pDevice = m_listAlarmOutputDevice.GetNext (pos);
		if (pDevice)
			pDevice->InitializeChannelInfoFromDB();
	}

	return TRUE;
}

//请求指定通道的数据
BOOL CDataCenter::Request (int nDeviceNo, int nChannelNo)
{
	CollectionDevice *pDevice = GetCollectionDevice(nDeviceNo);
	if (pDevice == NULL)
	{
		pDevice = new CollectionDevice(nDeviceNo);

		//判断设备是否存在，并且是采集设备
		if (pDevice->Initialize() && IsCollectionDevice(pDevice->m_nDeviceType))
		{
			m_listCollectionDevice.AddTail(pDevice);

			RequestData *pRD = new RequestData();
			m_listRequestData.AddTail(pRD);
			pRD->SetRequestDevice(pDevice);
			pRD->Request();
		}
		else
		{
			delete pDevice;
			pDevice = NULL;

			return FALSE;
		}
	}

	pDevice->SetChannelNotify (nChannelNo, TRUE);

	return TRUE;
}

BOOL CDataCenter::IsCollectionDevice (int nDeviceType)
{
	if (nDeviceType == devTypeProcessSample)
		return TRUE;

	return FALSE;
}
