#pragma once

#include "adowrapper.h"
#include "DBDefine.h"
#include "BlobData.h"


class CDBAccess :
	public CADORecordset
{
public:
	CDBAccess(void);
	CDBAccess(CADOConnection *pADOConnection, CString strOptDBName=_T(""));
	~CDBAccess(void);

public:
	void SetOptDBName (CString strOptDBName);
	CString GetOptDBName ();
	void ResetOptDBName ();

public:
	int GetServerDateTime (COleDateTime &odtSampleTime);

public:
	/////////////////////////////////////////下面的函数都是涉及具体操作数据库表的//////////////////////////////////////////////////////////

	//数据库表记录用户自定义字段的获取和修改
	int GetCurDefBufData (int &nDefBufLen, BlobData<byte> *pbdDefBuf);
	int GetDefBufData (int nTableNo, int nKeyID, int nKeyExID, int &nDefBufLen, BlobData<byte> *pbdDefBuf);
	int ModiDefBufData (int nTableNo, int nKeyID, int nKeyExID, int nDefBufLen, BlobData<byte> *pbdDefBuf);


	//系统表
	int GetDBVersion (int &nDBVersion);
	int ModiDBVersion (int nDBVersion);
	int GetDBType (int &nDBType);
	int ModiDBType (int nDBType);
	int GetServerIPandPort (byte byIPAddress[], int &nPortNo);
	int ModiServerIPandPort (byte byIPAddress[], int nPortNo);
	int GetProjectData (BlobData<byte> *pbdDefBuf);
	int ModiProjectData (BlobData<byte> *pbdDefBuf);


	//用户信息表
	int ExecGetAllUser ();
	int GetCurUser (int &nUserID, TCHAR szUserName[], TCHAR szPassword[], int &nUserRight);
	int GetOneUserInfo (int nUserID, TCHAR szUserName[], TCHAR szPassword[], int &nUserRight);
	int AddUser (int &nUserID, TCHAR szUserName[], TCHAR szPassword[], int nUserRight, int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int ModiUser (int nUserID, TCHAR szUserName[], TCHAR szPassword[], int nUserRight, int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int DelUser (int nUserID);
	int CheckUserExist (TCHAR szUserName[], TCHAR szPassword[], BOOL &bExist, int &nUserID, int &nUserRight);


	//图表
	int ExecGetAllPlot ();
	int GetCurPlot (int &nPlotID, TCHAR szPlotName[], byte &byPlotType);
	int GetCurPlot (int &nPlotID, TCHAR szPlotName[], byte &byPlotType, int &nDataLen, BlobData<byte> *pbdData);
	int GetOnePlotInfo (int nPlotID, TCHAR szPlotName[], byte &byPlotType, int &nDataLen, BlobData<byte> *pbdData);
	int AddPlot (int &nPlotID, TCHAR szPlotName[], byte byPlotType, int nDataLen, BlobData<byte> *pbdData, 
				int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int ModiPlot (int nPlotID, TCHAR szPlotName[], byte byPlotType, int nDataLen, BlobData<byte> *pbdData, 
				int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int ModiPlotName (int nPlotID, TCHAR szPlotName[]);
	int DelPlot (int nPlotID);
	//	批量
	int DelPlotBatch (int *pnPlotIDArr, int nArrLen);


	//设备表
	int ExecGetAllDevice (int nDeviceType=-1);
	int GetCurDevice (int &nDeviceNo, TCHAR szDeviceName[], int &nDeviceType, int &nDriverType, int &nChannelCount, 
						TCHAR szDeviceGUID[], int &nSampleInterval);
	int GetOneDeviceInfo (int nDeviceNo, TCHAR szDeviceName[], int &nDeviceType, int &nDriverType, int &nChannelCount, 
						TCHAR szDeviceGUID[], int &nSampleInterval);
	int AddDevice (int &nDeviceNo, TCHAR szDeviceName[], int nDeviceType, int nDriverType, int nChannelCount, 
					TCHAR szDeviceGUID[], int nSampleInterval, int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int ModiDevice (int nDeviceNo, TCHAR szDeviceName[], int nDeviceType, int nDriverType, int nChannelCount, 
					TCHAR szDeviceGUID[], int nSampleInterval, int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int DelDevice (int nDeviceNo);


	//通道表
	int ExecGetAllChannel (int nDeviceNo=-1);
	int GetCurChannel (int &nDeviceNo, int &nChannelNo, TCHAR szChannelName[], BOOL &bWorkEnable, float &fFullScaleHigh, 
						float &fFullScaleLow, float &fCalibrationHigh, float &fCalibrationLow, byte &byDataType, 
						byte &byDecimalDigits, TCHAR szUnitString[], int &nAlarmLevelDataLen, BlobData<byte> *pbdAlarmLevelData, 
						int &nAlarmRangeDataLen, BlobData<byte> *pbdAlarmRangeData);
	int GetOneChannelInfo (int nDeviceNo, int nChannelNo, TCHAR szChannelName[], BOOL &bWorkEnable, float &fFullScaleHigh, 
						float &fFullScaleLow, float &fCalibrationHigh, float &fCalibrationLow, byte &byDataType, 
						byte &byDecimalDigits, TCHAR szUnitString[], int &nAlarmLevelDataLen, BlobData<byte> *pbdAlarmLevelData, 
						int &nAlarmRangeDataLen, BlobData<byte> *pbdAlarmRangeData);
	int AddChannel (int nDeviceNo, int nChannelNo, TCHAR szChannelName[], BOOL bWorkEnable, float fFullScaleHigh, 
					float fFullScaleLow, float fCalibrationHigh, float fCalibrationLow, byte byDataType, byte byDecimalDigits, 
					TCHAR szUnitString[], int nAlarmLevelDataLen, BlobData<byte> *pbdAlarmLevelData, int nAlarmRangeDataLen, 
					BlobData<byte> *pbdAlarmRangeData, int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int ModiChannel (int nDeviceNo, int nChannelNo, TCHAR szChannelName[], BOOL bWorkEnable, float fFullScaleHigh, 
					float fFullScaleLow, float fCalibrationHigh, float fCalibrationLow, byte byDataType, byte byDecimalDigits, 
					TCHAR szUnitString[], int nAlarmLevelDataLen, BlobData<byte> *pbdAlarmLevelData, int nAlarmRangeDataLen, 
					BlobData<byte> *pbdAlarmRangeData, int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int DelChannel (int nDeviceNo, int nChannelNo);


	//通道数据表
	int ExecGetAllChannelData (int nDeviceNo, int nChannelNo, BOOL bIsGetAll=TRUE, 
							COleDateTime odtBeginTime=DT_RANGE_BEGIN, 
							COleDateTime odtEndTime=DT_RANGE_END);
	int ExecGetAllLastChannelData (int nDeviceNo);
	int GetCurChannelData (int &nDataID, int &nDeviceNo, int &nChannelNo, COleDateTime &odtSampleTime, float &fMeasureValue, 
							byte &byAlarmLevel, int &nDataLen, BlobData<byte> *pbdData);
	int GetOneChannelDataInfo (int nDataID, int nDeviceNo, int nChannelNo, COleDateTime &odtSampleTime, float &fMeasureValue, 
							byte &byAlarmLevel, int &nDataLen, BlobData<byte> *pbdData);
	int GetLastChannelDataInfo (int &nDataID, int nDeviceNo, int nChannelNo, COleDateTime &odtSampleTime, float &fMeasureValue, 
								byte &byAlarmLevel, int &nDataLen, BlobData<byte> *pbdData);
	int AddChannelData (int &nDataID, int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, float fMeasureValue, 
						byte byAlarmLevel, int nDataLen, BlobData<byte> *pbdData);
	int ModiChannelData (int nDataID, int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, float fMeasureValue, 
						byte byAlarmLevel, int nDataLen, BlobData<byte> *pbdData);
	int DelChannelData (int nDataID, int nDeviceNo, int nChannelNo);
	int DelAllChannelData (int nDeviceNo, int nChannelNo, BOOL bIsDelAll=TRUE, 
							COleDateTime odtBeginTime=DT_RANGE_BEGIN, 
							COleDateTime odtEndTime=DT_RANGE_END);
	CString GetChannelDataTableName (int nDeviceNo, int nChannelNo);
	//--批量添加
	int BeginAddChannelDataBatch ();
	int AddChannelDataBatch (int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, float fMeasureValue, byte byAlarmLevel, 
							int nDataLen, BlobData<byte> *pbdData);
	int EndAddChannelDataBatch (int *pnDataIDArr, int &nArrLen);


	//报警数据表
	int ExecGetAllAlarmData (int nDeviceNo, int nChannelNo, BOOL bIsGetAll=TRUE, 
							COleDateTime odtBeginTime=DT_RANGE_BEGIN, 
							COleDateTime odtEndTime=DT_RANGE_END);
	int ExecGetAllAlarmDataBatch (int nDeviceNoArr[], int nChannelNoArr[], int nArrLen, BOOL bIsGetAll=TRUE, 
							COleDateTime odtBeginTime=DT_RANGE_BEGIN, 
							COleDateTime odtEndTime=DT_RANGE_END);
	int GetCurAlarmData (int &nAlarmDataID, int &nDataID, int &nDeviceNo, TCHAR szDeviceName[], int &nChannelNo, 
						TCHAR szChannelName[], COleDateTime &odtSampleTime, float &fMeasureValue, byte &byAlarmLevel);
	int GetOneAlarmDataInfo (int nAlarmDataID, int &nDataID, int &nDeviceNo, int &nChannelNo, COleDateTime &odtSampleTime, 
							float &fMeasureValue, byte &byAlarmLevel);
	int AddAlarmData (int &nAlarmDataID, int nDataID, int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, 
					float fMeasureValue, byte byAlarmLevel, int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int ModiAlarmData (int nAlarmDataID, int nDataID, int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, 
					float fMeasureValue, byte byAlarmLevel, int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int DelAlarmData (int nAlarmDataID);
	int DelAllAlarmData (int nDeviceNo, int nChannelNo, BOOL bIsDelAll=TRUE, 
						COleDateTime odtBeginTime=DT_RANGE_BEGIN, 
						COleDateTime odtEndTime=DT_RANGE_END);


	//控制器表
	int ExecGetAllController (int nDeviceNo=-1);
	int GetCurController (int &nDeviceNo, int &nControllerNo, TCHAR szControllerName[], byte &byControllerType, 
				int &nDataLen, BlobData<byte> *pbdData);
	int GetOneControllerInfo (int nDeviceNo, int nControllerNo, TCHAR szControllerName[], byte &byControllerType, 
				int &nDataLen, BlobData<byte> *pbdData);
	int AddController (int nDeviceNo, int nControllerNo, TCHAR szControllerName[], byte byControllerType, 
				int nDataLen, BlobData<byte> *pbdData, int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int ModiController (int nDeviceNo, int nControllerNo, TCHAR szControllerName[], byte byControllerType, 
				int nDataLen, BlobData<byte> *pbdData, int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int DelController (int nDeviceNo, int nControllerNo);
	int DelAllController (int nDeviceNo);

	//资产表
	int ExecGetAllAsset ();
	int GetCurAsset (int &nAssetID, TCHAR szAssetNumber[], TCHAR szProjectNumber[], TCHAR szAssetName[], 
					TCHAR szBrand[], TCHAR szSpecification[], TCHAR szSerialNumber[], TCHAR szUsage[], 
					COleDateTime &odtPurchaseDate, TCHAR szPurchasePrice[], TCHAR szRemarks[]);
	int GetOneAssetInfo (int nAssetID, TCHAR szAssetNumber[], TCHAR szProjectNumber[], TCHAR szAssetName[], 
						TCHAR szBrand[], TCHAR szSpecification[], TCHAR szSerialNumber[], TCHAR szUsage[], 
						COleDateTime &odtPurchaseDate, TCHAR szPurchasePrice[], TCHAR szRemarks[]);
	int AddAsset (int &nAssetID, TCHAR szAssetNumber[], TCHAR szProjectNumber[], TCHAR szAssetName[], 
					TCHAR szBrand[], TCHAR szSpecification[], TCHAR szSerialNumber[], TCHAR szUsage[], 
					COleDateTime odtPurchaseDate, TCHAR szPurchasePrice[], TCHAR szRemarks[], 
					int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int ModiAsset (int nAssetID, TCHAR szAssetNumber[], TCHAR szProjectNumber[], TCHAR szAssetName[], 
					TCHAR szBrand[], TCHAR szSpecification[], TCHAR szSerialNumber[], TCHAR szUsage[], 
					COleDateTime odtPurchaseDate, TCHAR szPurchasePrice[], TCHAR szRemarks[], 
					int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int DelAsset (int nAssetID);

	//资产维护表
	int ExecGetAllAssetMaintenance (int nAssetID=-1);
	int GetCurAssetMaintenance (int &nMaintenanceID, int &nAssetID, COleDateTime &odtMaintenanceDate, 
				TCHAR szMaintenanceReason[], TCHAR szMaintenanceContent[], TCHAR szMaintenancePersonnel[],
				TCHAR szRemarks[]);
	int GetOneAssetMaintenanceInfo (int nMaintenanceID, int &nAssetID, COleDateTime &odtMaintenanceDate, 
				TCHAR szMaintenanceReason[], TCHAR szMaintenanceContent[], TCHAR szMaintenancePersonnel[], 
				TCHAR szRemarks[]);
	int AddAssetMaintenance (int &nMaintenanceID, int nAssetID, COleDateTime odtMaintenanceDate, 
				TCHAR szMaintenanceReason[], TCHAR szMaintenanceContent[], TCHAR szMaintenancePersonnel[], 
				TCHAR szRemarks[], int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int ModiAssetMaintenance (int nMaintenanceID, int nAssetID, COleDateTime odtMaintenanceDate, 
				TCHAR szMaintenanceReason[], TCHAR szMaintenanceContent[], TCHAR szMaintenancePersonnel[], 
				TCHAR szRemarks[], int nDefBufLen=-1, BlobData<byte> *pbdDefBuf=NULL);
	int DelAssetMaintenance (int nMaintenanceID);


private:
	BOOL GetTableInfo (int nTableNo, CString &strTable, CString &strKeyID, CString &strKeyExID);
	int RecordOperate (int nDBOptFlag, int nTableNo, int nKeyID=-1, int nKeyExID=-1);
	int RecordOperate (int nDBOptFlag, CString strTableName, CString strKeyIDName, CString strKeyExIDName, 
						int nKeyID=-1, int nKeyExID=-1);
	int RecordOperateBatch (int nDBOptFlag, int nTableNo, int *pnKeyIDArr=NULL, int *pnKeyExIDArr=NULL, int nArrLen=0);
	int RecordOperateBatch (int nDBOptFlag, CString strTableName, CString strKeyIDName, CString strKeyExIDName, 
							int *pnKeyIDArr=NULL, int *pnKeyExIDArr=NULL, int nArrLen=0);

private:
	void AddOptDBName (CString &strObjectName);

private:
	//设置不同类型字段的值
	int SetFeildValue (CString strName, int nValue);
	int SetFeildValue (CString strName, float fValue);
	int SetFeildValue (CString strName, double dValue);
	int SetFeildValue (CString strName, byte byValue);
	int SetFeildValue (CString strName, TCHAR szValue[]);
	int SetFeildValue (CString strName, const CString &strValue);
	int SetFeildValue (CString strName, const COleDateTime &odtValue);
	int SetFeildValue (CString strName, BlobData<byte> *pbdValue, int nValueLen);

	//得到不同类型字段的值
	int GetFeildValue (CString strName, int &nValue);
	int GetFeildValue (CString strName, float &fValue);
	int GetFeildValue (CString strName, double &dValue);
	int GetFeildValue (CString strName, byte &byValue);
	int GetFeildValue (CString strName, TCHAR szValue[]);
	int GetFeildValue (CString strName, CString &strValue);
	int GetFeildValue (CString strName, COleDateTime &odtValue);
	int GetFeildValue (CString strName, BlobData<byte> *pbdValue);

private:
	CString m_strOptDBName;
};
