#include "StdAfx.h"
#include "CollectionChannel.h"


CollectionChannel::CollectionChannel(void)
{
	m_clrNormalColor = NORMAL_COLOR;

	m_bySwitchEnableAlarm = 0;
	m_bySwitchAlarmValue = 0;

	m_byChannelState = csNormal;
	m_fMesureValue = 0.0f;
	m_byAlarmLevel = 0;
	m_byChannelStateOld = csNormal;
	m_byAlarmLevelOld = 0;

	m_bChannelStateChangeNotify = FALSE;
	m_bAlarmLevelChangeNotify = FALSE;
}


CollectionChannel::~CollectionChannel(void)
{
	ClearMemory ();
}

void CollectionChannel::ClearMemory ()
{
	FreeObjPtrArray(&m_arrAlarmLevel);
	FreeObjPtrArray(&m_arrAlarmRange);
}

//�����������𻺳���
void CollectionChannel::AnalyzeAlarmLevelBuf ()
{
	//�ж�ͨ����������
	switch (m_byDataType)
	{
	case datTypeProcess:		//������
		{
			this->RefreshAlarmLevel ();
		}
		break;

	case datTypeSwitch:			//������
		{
			byte *pBuf = NULL;
			m_bySwitchEnableAlarm = 0;
			int nLen;

			if (m_bdAlarmLevelData.GetSize() > 0 
				&& m_bdAlarmLevelData.GetData() != NULL)
			{
				pBuf = m_bdAlarmLevelData.GetData();

				nLen = sizeof(byte);
				m_bySwitchEnableAlarm = *(byte *)pBuf;	//�������Ƿ�������
				pBuf += nLen;

				nLen = sizeof(byte);
				m_bySwitchAlarmValue = *(byte *)pBuf;	//������Ϊ0ʱ��������Ϊ1ʱ����
				pBuf += nLen;
			}
		}
		break;

	default:
		break;
	}
}

//����������Χ������
void CollectionChannel::AnalyzeAlarmRangeBuf ()
{
	//�ж�ͨ����������
	switch (m_byDataType)
	{
	case datTypeProcess:		//������
		this->RefreshAlarmRange ();
		break;

	case datTypeSwitch:			//������
		break;

	default:
		break;
	}
}

//��������
void CollectionChannel::RefreshAlarmLevel ()
{
	FreeObjPtrArray(&m_arrAlarmLevel);

	byte *pBuf = NULL;
	byte byCount;
	CString strText;
	int nLen;

	if (m_bdAlarmLevelData.GetSize() > 0 
		&& m_bdAlarmLevelData.GetData() != NULL)
	{
		pBuf = m_bdAlarmLevelData.GetData();

		AlarmLevel *pLevel = NULL;

		nLen = sizeof(COLORREF);
		m_clrNormalColor = *(COLORREF *)pBuf;	//����״̬���Ǳ�����ͨ������ɫ
		pBuf += nLen;

		nLen = sizeof(byte);
		memcpy (&byCount, pBuf, nLen);			//byte[0] �����������
		pBuf += nLen;

		for (byte i=0; i<byCount; i++)
		{
			pLevel = new AlarmLevel();

			//��������
			nLen = sizeof(byte);
			memcpy (&pLevel->byAlarmLevel, pBuf, nLen);
			pBuf += nLen;

			//��������
			nLen = BUF_LEN_S*sizeof(TCHAR);
			memcpy (pLevel->szAlarmLevelName, pBuf, nLen);
			pBuf += nLen;

			//������ɫ
			nLen = sizeof(COLORREF);
			memcpy (&pLevel->clrAlarmColor, pBuf, nLen);
			pBuf += nLen;

			m_arrAlarmLevel.Add(pLevel);
		}
	}
}

//������Χ
void CollectionChannel::RefreshAlarmRange ()
{
	FreeObjPtrArray(&m_arrAlarmRange);

	byte *pBuf = NULL;
	byte byCount;
	CString strText;
	int nLen;

	//������Χ
	if (m_bdAlarmRangeData.GetSize() > 0 
		&& m_bdAlarmRangeData.GetData() != NULL)
	{
		pBuf = m_bdAlarmRangeData.GetData();

		AlarmRange *pRange = NULL;

		nLen = sizeof(byte);
		memcpy (&byCount, pBuf, nLen);		//byte[0] ������Χ����
		pBuf += nLen;

		for (byte i=0; i<byCount; i++)
		{
			pRange = new AlarmRange();

			//��������
			nLen = sizeof(byte);
			memcpy (&pRange->byAlarmLevel, pBuf, nLen);
			pBuf += nLen;

			//��Χ����
			nLen = BUF_LEN_S*sizeof(TCHAR);
			memcpy (pRange->szAlarmRangeName, pBuf, nLen);
			pBuf += nLen;

			//������Χ��
			nLen = sizeof(float);
			memcpy (&pRange->fAlarmRangeLow, pBuf, nLen);
			pBuf += nLen;

			//������Χ��
			nLen = sizeof(float);
			memcpy (&pRange->fAlarmRangeHigh, pBuf, nLen);
			pBuf += nLen;

			m_arrAlarmRange.Add(pRange);
		}
	}
}

//����ͨ������
void CollectionChannel::SetChannelStateChangeNotify (BOOL bNotify)
{
	m_bChannelStateChangeNotify = bNotify;
}

//����ͨ������
void CollectionChannel::SetAlarmLevelChangeNotify (BOOL bNotify)
{
	m_bAlarmLevelChangeNotify = bNotify;
}

//�����µ�ͨ������
void CollectionChannel::SetChannelData (byte byChannelState, float fMesureValue, byte byAlarmLevel)
{
	//�����ϴ�
	m_byChannelStateOld = m_byChannelState;
	m_byAlarmLevelOld = m_byAlarmLevel;

	m_byChannelState = byChannelState;
	m_fMesureValue = fMesureValue;
	m_byAlarmLevel = byAlarmLevel;

	if (m_bChannelStateChangeNotify)
	{
		if (m_byChannelState != m_byChannelStateOld)	//ͨ��״̬�����仯
		{
			CDataCenter *pDataCenter = CDataCenter::GetInstance();
			if (pDataCenter)
				pDataCenter->OnNotify(ncChannelStateChange, (WPARAM )this, NULL);
		}
	}

	if (m_bAlarmLevelChangeNotify)
	{
		if (m_byChannelState == csNormal && m_byAlarmLevel != m_byAlarmLevelOld)	//����ͨ���ı����������仯
		{
			CDataCenter *pDataCenter = CDataCenter::GetInstance();
			if (pDataCenter)
				pDataCenter->OnNotify(ncAlarmLevelChange, (WPARAM )this, NULL);
		}
	}
}
