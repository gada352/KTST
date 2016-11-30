#include "StdAfx.h"
#include "Channel.h"


Channel::Channel(void)
{
	m_nDeviceNo = -1;
	m_nChannelNo = -1;
	m_strChannelName.Empty();
	m_bWorkEnable = FALSE;
	m_fFullScaleHigh = 0.0f;
	m_fFullScaleLow = 0.0f;
	m_fCalibrationHigh = 0.0f;
	m_fCalibrationLow = 0.0f;
	m_byDataType = 0;
	m_byDecimalDigits = 0;
	m_strUnitString.Empty();
	m_bdAlarmLevelData.Reset();
	m_bdAlarmRangeData.Reset();
	m_bdDefBuf.Reset();

	m_nOptFlag = OPT_NULL;
}


Channel::~Channel(void)
{
}

void Channel::SetChannelInfo (const ChannelInfo &ci)
{
	m_nDeviceNo = ci.nDeviceNo;
	m_nChannelNo = ci.nChannelNo;
	m_strChannelName = ci.strChannelName;
	m_bWorkEnable = ci.bWorkEnable;
	m_fFullScaleHigh = ci.fFullScaleHigh;
	m_fFullScaleLow = ci.fFullScaleLow;
	m_fCalibrationHigh = ci.fCalibrationHigh;
	m_fCalibrationLow = ci.fCalibrationLow;
	m_byDataType = ci.byDataType;
	m_byDecimalDigits = ci.byDecimalDigits;
	m_strUnitString = ci.strUnitString;
	m_bdAlarmLevelData = ci.bdAlarmLevelData;
	m_bdAlarmRangeData = ci.bdAlarmRangeData;
	m_bdDefBuf = ci.bdDefBuf;
}

void Channel::GetChannelInfo (ChannelInfo &ci)
{
	ci.nDeviceNo = m_nDeviceNo;
	ci.nChannelNo = m_nChannelNo;
	ci.strChannelName = m_strChannelName;
	ci.bWorkEnable = m_bWorkEnable;
	ci.fFullScaleHigh = m_fFullScaleHigh;
	ci.fFullScaleLow = m_fFullScaleLow;
	ci.fCalibrationHigh = m_fCalibrationHigh;
	ci.fCalibrationLow = m_fCalibrationLow;
	ci.byDataType = m_byDataType;
	ci.byDecimalDigits = m_byDecimalDigits;
	ci.strUnitString = m_strUnitString;
	ci.bdAlarmLevelData = m_bdAlarmLevelData;
	ci.bdAlarmRangeData = m_bdAlarmRangeData;
	ci.bdDefBuf = m_bdDefBuf;
}
