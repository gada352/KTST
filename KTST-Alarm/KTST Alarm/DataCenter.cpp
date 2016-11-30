#include "StdAfx.h"
#include "DataCenter.h"
#include "KTST Alarm.h"

#include "SMSDevice.h"
#include "RelayDevice.h"

#include <string>


//��̬��Ա��ʼ��
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
	//��ʼ��Server��Ϣ
	if (!InitialServerInfo ())
		return FALSE;

	CCommunicate *pCommunicate = CKTSTAlarmApp::GetCommunicatePtr();
	if (pCommunicate == NULL)
		return FALSE;
	pCommunicate->Initialize();


	//��ʼ�����źͼ̵����豸
	if (!InitialAlarmOutputDevice ())
		return FALSE;

	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO == NULL)
		return FALSE;

	pDeviceIO->Initialize(SOFTWARE_NAME);

	pDeviceIO->CustomBuild(dvrTypeSMSDevice);		//SMS�豸��Ҫ���ô˺�������ͨѶ

	//�����豸��ʼ��ʱ������ ���ķ���������豸��ͨ��
	//��������豸��ʼ��������
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
		pDeviceIO->CloseAllComm(dvrTypeModbusRTU);		//ModbusRTU�豸��Ҫ���ùر�
		pDeviceIO->CloseAllComm(dvrTypeModbusTCP);		//ModbusTCP�豸��Ҫ���ùر�
		pDeviceIO->CloseAllComm(dvrTypeSMSDevice);		//SMS�豸��Ҫ���ùر�
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

//�������Ķ�ʱ���ص�����
VOID CALLBACK CDataCenter::TimerProc (HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	CDataCenter *pDataCenter = CDataCenter::GetInstance();
	if (pDataCenter)
		pDataCenter->OnTimer();
}

//��ʱ����Ϣ
void CDataCenter::OnTimer ()
{
	if (!m_bStart)
		return;

	//�������������ඨʱ����Ϣ
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
//	���ܣ�	�յ�UDP��Ϣ
//	������	WPARAM wParam[in]����������ַ
//			LPARAM lParam[in]������
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
	case 2:		//�յ�ʵʱ����
		{
			//�ɼ��豸��
			int nDeviceNo = *(int *)(pBuf+nOffset);
			nOffset += sizeof(int);

			//ͨ������
			int nChannelCount = (int )*(byte *)(pBuf+nOffset);
			nOffset += sizeof(byte);

			RequestData *pRequest = GetRequestData(nDeviceNo);
			if (pRequest == NULL)
				return S_FALSE;

			//ͨ�����ݻ�����
			byte *byChBuf = pBuf+nOffset;

			pRequest->OnReceive(nChannelCount, byChBuf);
		}
		break;

	default:  //������
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

//����֪ͨ
void CDataCenter::OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam)
{
	switch (code)
	{
	case ncChannelStateChange:		//ͨ��״̬�����ı�
	case ncAlarmLevelChange:		//ͨ�������������ı�
		{
			Device *pDevice = NULL;
			POSITION pos = m_listAlarmOutputDevice.GetHeadPosition ();

			while (pos)
			{
				pDevice = m_listAlarmOutputDevice.GetNext (pos);
				if (pDevice)
					pDevice->OnNotify(code, wParam, lParam);		//���ݸ����źͼ̵����豸
			}
		}
		break;

	default:
		break;
	}
}

//��ʼ��Server��Ϣ
BOOL CDataCenter::InitialServerInfo ()
{
	//�����ݿ��ж�Server��Ϣ
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

//ҪServer��Ϣ
void CDataCenter::GetServerInfo (CString &strAppServerIP, UINT &nAppServerPortNo)
{
	strAppServerIP = m_strAppServerIP;
	nAppServerPortNo = m_nAppServerPortNo;
}

//��ʼ���豸��Ϣ
BOOL CDataCenter::InitialAlarmOutputDevice ()
{
	FreeObjPtrList(&m_listAlarmOutputDevice);

	//�õ�����GUID
	TCHAR szGUID[BUF_LEN_M];
	memset (szGUID, 0x0, sizeof(szGUID));
	CDeviceIO *pDeviceIO = CKTSTAlarmApp::GetDeviceIOPtr();
	if (pDeviceIO)
		pDeviceIO->GetLcdeGid(szGUID);
	CString strLocalGUID = szGUID;

	CDBAccess dbAccess(CKTSTAlarmApp::GetADOConnectionPtr());

	//�豸��Ϣ
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

		if (_tcscmp(szGUID, szDeviceGUID) != 0)		//ֻҪ�����豸
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

	//��ʼ���豸��ͨ����Ϣ
	POSITION pos = m_listAlarmOutputDevice.GetHeadPosition ();
	while (pos)
	{
		pDevice = m_listAlarmOutputDevice.GetNext (pos);
		if (pDevice)
			pDevice->InitializeChannelInfoFromDB();
	}

	return TRUE;
}

//����ָ��ͨ��������
BOOL CDataCenter::Request (int nDeviceNo, int nChannelNo)
{
	CollectionDevice *pDevice = GetCollectionDevice(nDeviceNo);
	if (pDevice == NULL)
	{
		pDevice = new CollectionDevice(nDeviceNo);

		//�ж��豸�Ƿ���ڣ������ǲɼ��豸
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
