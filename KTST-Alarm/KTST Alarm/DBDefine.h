#pragma once


//************************************************************************************
//		���ݿ����б��ֶζ���
//************************************************************************************

//ͨ���ֶ�
#define field_DefBufLen									_T("DefBufLen")
#define field_DefBuf									_T("DefBuf")

//ϵͳ��
#define table_DBSystem					_T("DBSystem")
#define DBSystem_RecordID								_T("RecordID")

//�û���Ϣ��
#define table_UserInfo					_T("UserInfo")
#define UserInfo_UserID									_T("UserID")
#define UserInfo_UserName								_T("UserName")
#define UserInfo_Password								_T("Password")
#define UserInfo_UserRight								_T("UserRight")

//ͼ��
#define table_Plot						_T("Plot")
#define Plot_PlotID										_T("PlotID")
#define Plot_PlotName									_T("PlotName")
#define Plot_PlotType									_T("PlotType")
#define Plot_DataLen									_T("DataLen")
#define Plot_Data										_T("Data")

//�豸��
#define table_Device					_T("Device")
#define Device_DeviceNo									_T("DeviceNo")
#define Device_DeviceName								_T("DeviceName")
#define Device_DeviceType								_T("DeviceType")
#define Device_DriverType								_T("DriverType")
#define Device_ChannelCount								_T("ChannelCount")
#define Device_DeviceGUID								_T("DeviceGUID")
#define Device_SampleInterval							_T("SampleInterval")

//ͨ����
#define table_Channel					_T("Channel")
#define Channel_DeviceNo								_T("DeviceNo")
#define Channel_ChannelNo								_T("ChannelNo")
#define Channel_ChannelName								_T("ChannelName")
#define Channel_WorkEnable								_T("WorkEnable")
#define Channel_FullScaleHigh							_T("FullScaleHigh")
#define Channel_FullScaleLow							_T("FullScaleLow")
#define Channel_CalibrationHigh							_T("CalibrationHigh")
#define Channel_CalibrationLow							_T("CalibrationLow")
#define Channel_DataType								_T("DataType")
#define Channel_DecimalDigits							_T("DecimalDigits")
#define Channel_UnitString								_T("UnitString")
#define Channel_AlarmLevelDataLen						_T("AlarmLevelDataLen")
#define Channel_AlarmLevelData							_T("AlarmLevelData")
#define Channel_AlarmRangeDataLen						_T("AlarmRangeDataLen")
#define Channel_AlarmRangeData							_T("AlarmRangeData")

//ͨ�����ݱ�
#define table_ChannelData				_T("ChannelData")
#define ChannelData_DataID								_T("DataID")
#define ChannelData_DeviceNo							_T("DeviceNo")
#define ChannelData_ChannelNo							_T("ChannelNo")
#define ChannelData_SampleTime							_T("SampleTime")
#define ChannelData_MeasureValue						_T("MeasureValue")
#define ChannelData_AlarmLevel							_T("AlarmLevel")
#define ChannelData_DataLen								_T("DataLen")
#define ChannelData_Data								_T("Data")

//�������ݱ�
#define table_AlarmData					_T("AlarmData")
#define AlarmData_AlarmDataID							_T("AlarmDataID")
#define AlarmData_DataID								_T("DataID")
#define AlarmData_DeviceNo								_T("DeviceNo")
#define AlarmData_ChannelNo								_T("ChannelNo")
#define AlarmData_DataType								_T("DataType")
#define AlarmData_SampleTime							_T("SampleTime")
#define AlarmData_MeasureValue							_T("MeasureValue")
#define AlarmData_AlarmLevel							_T("AlarmLevel")

//��������
#define table_Controller				_T("Controller")
#define Controller_DeviceNo								_T("DeviceNo")
#define Controller_ControllerNo							_T("ControllerNo")
#define Controller_ControllerName						_T("ControllerName")
#define Controller_ControllerType						_T("ControllerType")
#define Controller_DataLen								_T("DataLen")
#define Controller_Data									_T("Data")

//�ʲ���
#define table_Asset						_T("Asset")
#define Asset_AssetID									_T("AssetID")
#define Asset_AssetNumber								_T("AssetNumber")
#define Asset_ProjectNumber								_T("ProjectNumber")
#define Asset_AssetName									_T("AssetName")
#define Asset_Brand										_T("Brand")
#define Asset_Specification								_T("Specification")
#define Asset_SerialNumber								_T("SerialNumber")
#define Asset_Usage										_T("Usage")
#define Asset_PurchaseDate								_T("PurchaseDate")
#define Asset_PurchasePrice								_T("PurchasePrice")
#define Asset_Remarks									_T("Remarks")

//�ʲ�ά����
#define table_AssetMaintenance			_T("AssetMaintenance")
#define AssetMaintenance_MaintenanceID					_T("MaintenanceID")
#define AssetMaintenance_AssetID						_T("AssetID")
#define AssetMaintenance_MaintenanceDate				_T("MaintenanceDate")
#define AssetMaintenance_MaintenanceReason				_T("MaintenanceReason")
#define AssetMaintenance_MaintenanceContent				_T("MaintenanceContent")
#define AssetMaintenance_MaintenancePersonnel			_T("MaintenancePersonnel")
#define AssetMaintenance_Remarks						_T("Remarks")


//************************************************************************************
//		���ݿ����б��ŵĺ궨��
//************************************************************************************
#define INVALID_OBJ_ID					0		//��Ч����
#define TABLE_DB_SYSTEM					1		//ϵͳ��
#define TABLE_USER_INFO					2		//�û���Ϣ��
#define TABLE_PLOT						3		//ͼ��
#define TABLE_DEVICE					4		//�豸��
#define TABLE_CHANNEL					5		//ͨ����
#define TABLE_CHANNEL_DATA				6		//ͨ�����ݱ�
#define TABLE_ALARM_DATA				7		//�������ݱ�
#define TABLE_CONTROLLER				8		//��������
#define TABLE_ASSET						9		//�ʲ���
#define TABLE_ASSET_MAINTENANCE			10		//�ʲ�ά����

//************************************************************************************
//		���ݿ������־
//************************************************************************************
#define DB_OPT_NULL						0		//�޲���
#define DB_OPT_ADD						1		//���
#define DB_OPT_BATCH_ADD				2		//�������
#define DB_OPT_MODIFY					3		//�޸�
#define DB_OPT_BATCH_MODIFY				4		//�����޸�
#define DB_OPT_DELETE					5		//ɾ��
#define DB_OPT_BATCH_DELETE				6		//����ɾ��

//************************************************************************************
//		���ݿ�����෵��ֵ
//************************************************************************************
#define RET_DB_SUCCESS					0		//�ɹ�
#define RET_DB_PARAM_INVALID			1		//������Ч
#define RET_DB_EXE_FAIL					2		//ִ��ʧ��
#define RET_DB_RECORD_NULL				3		//��¼Ϊ��
#define RET_DB_BUF_PTR_NULL				4		//������ָ��Ϊ��
#define RET_DB_RECORD_NOT_NULL			5		//��¼����
#define RET_DB_ADD_RECORD_FAIL			6		//��Ӽ�¼ʧ��
#define RET_DB_RECORD_INVALID			7		//��¼��Ч

//************************************************************************************
//		ϵͳ��ID
//************************************************************************************
#define SYSTEM_ID_DB_VERSION			1		//ϵͳ��ID��ʾ�Զ������ݱ���������ݿ�汾
#define SYSTEM_ID_DB_TYPE				2		//ϵͳ��ID��ʾ�Զ������ݱ���������ݿ����ͣ�0���������ݿ⡢1˽�������ݿ�
#define SYSTEM_ID_SERVER_IP_PORT		3		//ϵͳ��ID��ʾ�Զ������ݱ�����Ƿ���������IP��ַ��UDP�˿ں�
#define SYSTEM_ID_PROJECT				4		//ϵͳ��ID��ʾ�Զ������ݱ��������Ŀ����ͼ�ṹ����

//************************************************************************************
//		���ݿ�汾
//************************************************************************************
#define DB_VERSION_UNKNOWN				0		//δ֪�汾���ݿ�İ汾��
#define DB_VERSION_CURRENT				10001	//DBM��ǰ����Ӧ�����ݿ�汾
#define RES_DB_VERSION_MIN				10001	//���ܼ��ݵ�������ݿ�汾�汾��

//************************************************************************************
//		���ݿ�����
//************************************************************************************
#define DB_TYPE_PUBLIC_CLOUD			0		//���ݿ����͹�����
#define DB_TYPE_PRIVATE_CLOUD			1		//���ݿ�����˽����

//************************************************************************************
//		����ʱ��
//************************************************************************************
#define DT_RANGE_BEGIN					COleDateTime(2000, 1, 1, 1, 1, 1)	//����ʱ�䷶ΧĬ����ʼ
#define DT_RANGE_END					COleDateTime(2100, 1, 1, 1, 1, 1)	//����ʱ�䷶ΧĬ�Ͻ���



