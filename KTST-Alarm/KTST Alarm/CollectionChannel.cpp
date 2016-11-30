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

//解析报警级别缓冲区
void CollectionChannel::AnalyzeAlarmLevelBuf ()
{
	//判断通道数据类型
	switch (m_byDataType)
	{
	case datTypeProcess:		//过程量
		{
			this->RefreshAlarmLevel ();
		}
		break;

	case datTypeSwitch:			//开关量
		{
			byte *pBuf = NULL;
			m_bySwitchEnableAlarm = 0;
			int nLen;

			if (m_bdAlarmLevelData.GetSize() > 0 
				&& m_bdAlarmLevelData.GetData() != NULL)
			{
				pBuf = m_bdAlarmLevelData.GetData();

				nLen = sizeof(byte);
				m_bySwitchEnableAlarm = *(byte *)pBuf;	//开关量是否允许报警
				pBuf += nLen;

				nLen = sizeof(byte);
				m_bySwitchAlarmValue = *(byte *)pBuf;	//开关量为0时报警还是为1时报警
				pBuf += nLen;
			}
		}
		break;

	default:
		break;
	}
}

//解析报警范围缓冲区
void CollectionChannel::AnalyzeAlarmRangeBuf ()
{
	//判断通道数据类型
	switch (m_byDataType)
	{
	case datTypeProcess:		//过程量
		this->RefreshAlarmRange ();
		break;

	case datTypeSwitch:			//开关量
		break;

	default:
		break;
	}
}

//报警级别
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
		m_clrNormalColor = *(COLORREF *)pBuf;	//正常状态（非报警）通道的颜色
		pBuf += nLen;

		nLen = sizeof(byte);
		memcpy (&byCount, pBuf, nLen);			//byte[0] 报警级别个数
		pBuf += nLen;

		for (byte i=0; i<byCount; i++)
		{
			pLevel = new AlarmLevel();

			//报警级别
			nLen = sizeof(byte);
			memcpy (&pLevel->byAlarmLevel, pBuf, nLen);
			pBuf += nLen;

			//级别名称
			nLen = BUF_LEN_S*sizeof(TCHAR);
			memcpy (pLevel->szAlarmLevelName, pBuf, nLen);
			pBuf += nLen;

			//报警颜色
			nLen = sizeof(COLORREF);
			memcpy (&pLevel->clrAlarmColor, pBuf, nLen);
			pBuf += nLen;

			m_arrAlarmLevel.Add(pLevel);
		}
	}
}

//报警范围
void CollectionChannel::RefreshAlarmRange ()
{
	FreeObjPtrArray(&m_arrAlarmRange);

	byte *pBuf = NULL;
	byte byCount;
	CString strText;
	int nLen;

	//报警范围
	if (m_bdAlarmRangeData.GetSize() > 0 
		&& m_bdAlarmRangeData.GetData() != NULL)
	{
		pBuf = m_bdAlarmRangeData.GetData();

		AlarmRange *pRange = NULL;

		nLen = sizeof(byte);
		memcpy (&byCount, pBuf, nLen);		//byte[0] 报警范围个数
		pBuf += nLen;

		for (byte i=0; i<byCount; i++)
		{
			pRange = new AlarmRange();

			//报警级别
			nLen = sizeof(byte);
			memcpy (&pRange->byAlarmLevel, pBuf, nLen);
			pBuf += nLen;

			//范围名称
			nLen = BUF_LEN_S*sizeof(TCHAR);
			memcpy (pRange->szAlarmRangeName, pBuf, nLen);
			pBuf += nLen;

			//报警范围低
			nLen = sizeof(float);
			memcpy (&pRange->fAlarmRangeLow, pBuf, nLen);
			pBuf += nLen;

			//报警范围高
			nLen = sizeof(float);
			memcpy (&pRange->fAlarmRangeHigh, pBuf, nLen);
			pBuf += nLen;

			m_arrAlarmRange.Add(pRange);
		}
	}
}

//设置通道发生
void CollectionChannel::SetChannelStateChangeNotify (BOOL bNotify)
{
	m_bChannelStateChangeNotify = bNotify;
}

//设置通道发生
void CollectionChannel::SetAlarmLevelChangeNotify (BOOL bNotify)
{
	m_bAlarmLevelChangeNotify = bNotify;
}

//设置新的通道数据
void CollectionChannel::SetChannelData (byte byChannelState, float fMesureValue, byte byAlarmLevel)
{
	//保存上次
	m_byChannelStateOld = m_byChannelState;
	m_byAlarmLevelOld = m_byAlarmLevel;

	m_byChannelState = byChannelState;
	m_fMesureValue = fMesureValue;
	m_byAlarmLevel = byAlarmLevel;

	if (m_bChannelStateChangeNotify)
	{
		if (m_byChannelState != m_byChannelStateOld)	//通道状态发生变化
		{
			CDataCenter *pDataCenter = CDataCenter::GetInstance();
			if (pDataCenter)
				pDataCenter->OnNotify(ncChannelStateChange, (WPARAM )this, NULL);
		}
	}

	if (m_bAlarmLevelChangeNotify)
	{
		if (m_byChannelState == csNormal && m_byAlarmLevel != m_byAlarmLevelOld)	//工作通道的报警级别发生变化
		{
			CDataCenter *pDataCenter = CDataCenter::GetInstance();
			if (pDataCenter)
				pDataCenter->OnNotify(ncAlarmLevelChange, (WPARAM )this, NULL);
		}
	}
}
