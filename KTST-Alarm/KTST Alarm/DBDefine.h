#pragma once


//************************************************************************************
//		数据库所有表字段定义
//************************************************************************************

//通用字段
#define field_DefBufLen									_T("DefBufLen")
#define field_DefBuf									_T("DefBuf")

//系统表
#define table_DBSystem					_T("DBSystem")
#define DBSystem_RecordID								_T("RecordID")

//用户信息表
#define table_UserInfo					_T("UserInfo")
#define UserInfo_UserID									_T("UserID")
#define UserInfo_UserName								_T("UserName")
#define UserInfo_Password								_T("Password")
#define UserInfo_UserRight								_T("UserRight")

//图表
#define table_Plot						_T("Plot")
#define Plot_PlotID										_T("PlotID")
#define Plot_PlotName									_T("PlotName")
#define Plot_PlotType									_T("PlotType")
#define Plot_DataLen									_T("DataLen")
#define Plot_Data										_T("Data")

//设备表
#define table_Device					_T("Device")
#define Device_DeviceNo									_T("DeviceNo")
#define Device_DeviceName								_T("DeviceName")
#define Device_DeviceType								_T("DeviceType")
#define Device_DriverType								_T("DriverType")
#define Device_ChannelCount								_T("ChannelCount")
#define Device_DeviceGUID								_T("DeviceGUID")
#define Device_SampleInterval							_T("SampleInterval")

//通道表
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

//通道数据表
#define table_ChannelData				_T("ChannelData")
#define ChannelData_DataID								_T("DataID")
#define ChannelData_DeviceNo							_T("DeviceNo")
#define ChannelData_ChannelNo							_T("ChannelNo")
#define ChannelData_SampleTime							_T("SampleTime")
#define ChannelData_MeasureValue						_T("MeasureValue")
#define ChannelData_AlarmLevel							_T("AlarmLevel")
#define ChannelData_DataLen								_T("DataLen")
#define ChannelData_Data								_T("Data")

//报警数据表
#define table_AlarmData					_T("AlarmData")
#define AlarmData_AlarmDataID							_T("AlarmDataID")
#define AlarmData_DataID								_T("DataID")
#define AlarmData_DeviceNo								_T("DeviceNo")
#define AlarmData_ChannelNo								_T("ChannelNo")
#define AlarmData_DataType								_T("DataType")
#define AlarmData_SampleTime							_T("SampleTime")
#define AlarmData_MeasureValue							_T("MeasureValue")
#define AlarmData_AlarmLevel							_T("AlarmLevel")

//控制器表
#define table_Controller				_T("Controller")
#define Controller_DeviceNo								_T("DeviceNo")
#define Controller_ControllerNo							_T("ControllerNo")
#define Controller_ControllerName						_T("ControllerName")
#define Controller_ControllerType						_T("ControllerType")
#define Controller_DataLen								_T("DataLen")
#define Controller_Data									_T("Data")

//资产表
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

//资产维护表
#define table_AssetMaintenance			_T("AssetMaintenance")
#define AssetMaintenance_MaintenanceID					_T("MaintenanceID")
#define AssetMaintenance_AssetID						_T("AssetID")
#define AssetMaintenance_MaintenanceDate				_T("MaintenanceDate")
#define AssetMaintenance_MaintenanceReason				_T("MaintenanceReason")
#define AssetMaintenance_MaintenanceContent				_T("MaintenanceContent")
#define AssetMaintenance_MaintenancePersonnel			_T("MaintenancePersonnel")
#define AssetMaintenance_Remarks						_T("Remarks")


//************************************************************************************
//		数据库所有表编号的宏定义
//************************************************************************************
#define INVALID_OBJ_ID					0		//无效表编号
#define TABLE_DB_SYSTEM					1		//系统表
#define TABLE_USER_INFO					2		//用户信息表
#define TABLE_PLOT						3		//图表
#define TABLE_DEVICE					4		//设备表
#define TABLE_CHANNEL					5		//通道表
#define TABLE_CHANNEL_DATA				6		//通道数据表
#define TABLE_ALARM_DATA				7		//报警数据表
#define TABLE_CONTROLLER				8		//控制器表
#define TABLE_ASSET						9		//资产表
#define TABLE_ASSET_MAINTENANCE			10		//资产维护表

//************************************************************************************
//		数据库操作标志
//************************************************************************************
#define DB_OPT_NULL						0		//无操作
#define DB_OPT_ADD						1		//添加
#define DB_OPT_BATCH_ADD				2		//批量添加
#define DB_OPT_MODIFY					3		//修改
#define DB_OPT_BATCH_MODIFY				4		//批量修改
#define DB_OPT_DELETE					5		//删除
#define DB_OPT_BATCH_DELETE				6		//批量删除

//************************************************************************************
//		数据库操作类返回值
//************************************************************************************
#define RET_DB_SUCCESS					0		//成功
#define RET_DB_PARAM_INVALID			1		//参数无效
#define RET_DB_EXE_FAIL					2		//执行失败
#define RET_DB_RECORD_NULL				3		//记录为空
#define RET_DB_BUF_PTR_NULL				4		//缓冲区指针为空
#define RET_DB_RECORD_NOT_NULL			5		//记录不空
#define RET_DB_ADD_RECORD_FAIL			6		//添加记录失败
#define RET_DB_RECORD_INVALID			7		//记录无效

//************************************************************************************
//		系统表ID
//************************************************************************************
#define SYSTEM_ID_DB_VERSION			1		//系统表ID表示自定义数据保存的是数据库版本
#define SYSTEM_ID_DB_TYPE				2		//系统表ID表示自定义数据保存的是数据库类型：0公有云数据库、1私有云数据库
#define SYSTEM_ID_SERVER_IP_PORT		3		//系统表ID表示自定义数据保存的是服务器程序IP地址和UDP端口号
#define SYSTEM_ID_PROJECT				4		//系统表ID表示自定义数据保存的是项目树视图结构数据

//************************************************************************************
//		数据库版本
//************************************************************************************
#define DB_VERSION_UNKNOWN				0		//未知版本数据库的版本号
#define DB_VERSION_CURRENT				10001	//DBM当前所对应的数据库版本
#define RES_DB_VERSION_MIN				10001	//所能兼容的最低数据库版本版本号

//************************************************************************************
//		数据库类型
//************************************************************************************
#define DB_TYPE_PUBLIC_CLOUD			0		//数据库类型公有云
#define DB_TYPE_PRIVATE_CLOUD			1		//数据库类型私有云

//************************************************************************************
//		日期时间
//************************************************************************************
#define DT_RANGE_BEGIN					COleDateTime(2000, 1, 1, 1, 1, 1)	//日期时间范围默认起始
#define DT_RANGE_END					COleDateTime(2100, 1, 1, 1, 1, 1)	//日期时间范围默认结束



