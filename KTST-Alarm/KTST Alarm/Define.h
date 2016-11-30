

//************************************************************************************
//		常用宏定义
//************************************************************************************
#define SOFTWARE_TYPE_ID			4							//软件类型KTST为4
#define SOFTWARE_NAME				_T("KTST-Alarm")			//软件名称
#define SOFTWARE_REG_NAME			_T("\\KTST-Alarm.reg")		//注册表备份文件名称
#define LOCAL_SERVER_NAME			_T("localhost")				//本地服务器名称
#define DB_NAME						_T("KTSTDB")				//数据库名称
#define	DB_NAME_PREFIX				_T("KTSTDB_")				//系统数据库名前缀
#define	HELP_FILE_NAME				_T("KTST-Alarm.chm")		//帮助文档的全名
#define	CFG_FILE_NAME				_T("KTST.xml")				//打包安装的数据库服务器信息文件（包括用户名密码等）

#define AUTO_RUN_CMD_LINE			_T("-AUTORUN")				//开机自动运行命令行参数
#define AUTO_RUN_SLEEP_TIME			90000						//开机自动运行延时时间(毫秒)

#define NORMAL_COLOR				RGB(100, 220, 10)			//通道正常状态(非报警)的颜色

//************************************************************************************
//		用户权限和用户名
//************************************************************************************
#define USER_NAME_ADMIN				_T("administrator")			//管理员用户名
#define USERRIGHT_NORMAL			1							//普通用户
#define USERRIGHT_ADMIN				0							//administrator用户

//************************************************************************************
//		缓冲区长度
//************************************************************************************
#define BUF_LEN_S					32		//缓冲区长度:小码
#define BUF_LEN_M					128		//缓冲区长度:中码
#define BUF_LEN_L					1024	//缓冲区长度:大码

//************************************************************************************
//		通道状态
//************************************************************************************
enum ChannelState
{
	csNormal = 0,			//正常
	csNotWorking = 1,		//不工作
	csFault = 2,			//故障
};

//************************************************************************************
//		设备类型(按位划分)
//************************************************************************************
enum DeviceType
{
	devTypeProgrammableController = 0x01,	//可编程控制器设备
	devTypeDriver = 0x02,					//驱动设备
	devTypeProcessSample = 0x04,			//过程量采集设备
	devTypeVideoSample = 0x08,				//图像采集设备
	devTypeRelay = 0x10,					//继电器设备
};

//************************************************************************************
//		驱动类型
//************************************************************************************
enum DriverType
{
	dvrTypeModbusRTU = 0,					//Modbus RTU
	dvrTypeModbusTCP = 1,					//Modbus TCP
	dvrTypeController = 2,					//Controller
	dvrTypeSMSDevice = 3,					//SMS Device
	dvrTypeCameraDevice = 4,				//Camera Device
};

//************************************************************************************
//		通道数据类型
//************************************************************************************
enum ChannelDataType
{
	datTypeProcess = 0,						//过程量
	datTypeSwitch = 1,						//开关量
	datTypeCamera = 2,						//图形图像
};

//************************************************************************************
//		最大数量限制	MaxLimit
//************************************************************************************
#define _MAX_DEVICE					65535		//最大设备数
#define _MAX_ONEDEVICE_CHL			32			//一个设备最大的通道数
#define _MAX_CHANNLE				(_MAX_DEVICE*_MAX_ONEDEVICE_CHL)	//最大的通道数
#define _MAX_DEVICE_IN_LIST			_MAX_DEVICE	//在设备管理对话框中能添加的最大设备数
#define _MAX_DEVICE_TYPE			128			//最大设备类型个数
#define _MAX_RELAY_EXPRESSION_LEN	300			//最大继电器表达式长度
#define DEF_CH_CNT_PROCESS			4			//过程量采集设备默认通道数
#define DEF_CH_CNT_RELAY			8			//继电器设备默认通道数

#define MIN_DEV_SAMPLE_INTERVAL		5			//设备采集最小时间间隔(秒)
#define MAX_DEV_SAMPLE_INTERVAL		86400		//设备采集最大时间间隔(秒)
#define DEF_DEV_SAMPLE_INTERVAL		60			//设备采集默认时间间隔(秒)

#define MAX_ALARM_LEVEL_CNT			5			//最大报警级别个数
#define MAX_ALARM_RANGE_CNT			10			//最大报警范围个数
#define MAX_ALARM_RANGE_ONE_LEVEL	2			//一个报警级别可添加的最大报警范围个数

//*************************************************************************************
//		自定义消息		UserMessage
//*************************************************************************************
#define WM_UDP_RECEIVE				WM_USER + 2001  //UDP接收到数据向框架发送的消息；
#define WM_TRAY_NOTIFY_ICON			WM_USER + 2002	//定义托盘图标消息

//************************************************************************************
//		常规的管理操作标志
//************************************************************************************
#define	OPT_NULL					0			//无操作
#define	OPT_ADD						1			//添加
#define	OPT_MODIFY					2			//修改
#define	OPT_DELETE					3			//删除

//************************************************************************************
//		通知代码
//************************************************************************************
enum NotifyCode
{
	ncChannelStateChange = 1,					//通道状态发生改变
	ncAlarmLevelChange = 2,						//通道报警级别发生改变
};

//////////////////////////////////////////////////////////////////////////

#define FreeObjPtrList(objPtrList) {\
	if ((objPtrList))\
	{\
		ASSERT_VALID((objPtrList));\
		while ((objPtrList)->GetCount())\
			delete (objPtrList)->RemoveHead ();\
	}\
}

#define FreeObjPtrArray(objPtrArr) {\
	if ((objPtrArr))\
	{\
		ASSERT_VALID((objPtrArr));\
		int nCnt = (objPtrArr)->GetCount();\
		for (int i=0; i<nCnt; i++)\
			delete (* objPtrArr)[i];\
		(objPtrArr)->RemoveAll();}\
}

#define FreeAndNullPtr(ptr) {\
	if (ptr)\
	{\
		delete ptr;\
		ptr = NULL;\
	}\
}

#define FreeAndNullArray(arrPtr) {\
	if (arrPtr)\
	{\
		delete [] arrPtr;\
		arrPtr = NULL;\
	}\
}

#define SWITCH_VALUE(x) (x<0.5 ? 0 : 1)

//////////////////////////////////////////////////////////////////////////

//报警级别
typedef struct AlarmLevel
{
	byte byAlarmLevel;
	TCHAR szAlarmLevelName[BUF_LEN_S];
	COLORREF clrAlarmColor;

	AlarmLevel ()
	{
		byAlarmLevel = 0;
		memset (szAlarmLevelName, 0x0, sizeof(szAlarmLevelName));
		clrAlarmColor = RGB(255, 0, 0);
	}

	void Copy (const AlarmLevel &al)
	{
		byAlarmLevel = al.byAlarmLevel;
		memcpy (szAlarmLevelName, al.szAlarmLevelName, sizeof(szAlarmLevelName));
		clrAlarmColor = al.clrAlarmColor;
	}
}AlarmLevel;
typedef CTypedPtrList<CPtrList, AlarmLevel*> CAlarmLevelList;
typedef CTypedPtrArray<CPtrArray, AlarmLevel*> CAlarmLevelArray;


//报警范围
typedef struct AlarmRange
{
	byte byAlarmLevel;
	TCHAR szAlarmRangeName[BUF_LEN_S];
	float fAlarmRangeLow;
	float fAlarmRangeHigh;

	AlarmRange ()
	{
		byAlarmLevel = 0;
		memset (szAlarmRangeName, 0x0, sizeof(szAlarmRangeName));
		fAlarmRangeLow = 0.0f;
		fAlarmRangeHigh = 1.0f;
	}

	void Copy (const AlarmRange &ar)
	{
		byAlarmLevel = ar.byAlarmLevel;
		memcpy (szAlarmRangeName, ar.szAlarmRangeName, sizeof(szAlarmRangeName));
		fAlarmRangeLow = ar.fAlarmRangeLow;
		fAlarmRangeHigh = ar.fAlarmRangeHigh;
	}
}AlarmRange;
typedef CTypedPtrList<CPtrList, AlarmRange*> CAlarmRangeList;
typedef CTypedPtrArray<CPtrArray, AlarmRange*> CAlarmRangeArray;


//关注的通道
typedef struct FollowingChannel
{
	int nDeviceNo;
	int nChannelNo;

	FollowingChannel()
	{
		nDeviceNo = -1;
		nChannelNo = -1;
	}
}FollowingChannel;
typedef CTypedPtrList<CPtrList, FollowingChannel*> CFollowingChannelList;
typedef CTypedPtrArray<CPtrArray, FollowingChannel*> CFollowingChannelArray;


//////////////////////////////////////////////////////////////////////////

//系统表
typedef struct DBSystem
{
	int nRecordID;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	DBSystem ()
	{
		nRecordID = 0;
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
}DBSystem;


//用户信息表
typedef struct UserInfo
{
	int nUserID;
	CString strUserName;
	CString strPassword;
	int nUserRight;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	UserInfo ()
	{
		nUserID = 0;
		strUserName.Empty();
		strPassword.Empty();
		nUserRight = 1;
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
}UserInfo;


//图表
typedef struct Plot
{
	int nPlotID;
	CString strPlotName;
	byte byPlotType;
	BlobData<byte> bdData;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	Plot ()
	{
		nPlotID = 0;
		strPlotName.Empty();
		byPlotType = 0;
		bdData.Reset();
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
}Plot;


//设备表
typedef struct DeviceInfo
{
	int nDeviceNo;
	CString strDeviceName;
	int nDeviceType;
	int nDriverType;
	int nChannelCount;
	CString strDeviceGUID;
	int nSampleInterval;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	DeviceInfo ()
	{
		nDeviceNo = 0;
		strDeviceName.Empty();
		nDeviceType = 0;
		nDriverType = 0;
		nChannelCount = 0;
		strDeviceGUID.Empty();
		nSampleInterval = 60;
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
	DeviceInfo (const DeviceInfo &device)
	{
		Copy (device);

		nOptFlag = OPT_NULL;
	}
	void Copy (const DeviceInfo &device)
	{
		nDeviceNo = device.nDeviceNo;
		strDeviceName = device.strDeviceName;
		nDeviceType = device.nDeviceType;
		nDriverType = device.nDriverType;
		nChannelCount = device.nChannelCount;
		strDeviceGUID = device.strDeviceGUID;
		nSampleInterval = device.nSampleInterval;
		bdDefBuf = device.bdDefBuf;
	}
}DeviceInfo;
typedef CTypedPtrList<CPtrList, DeviceInfo*> CDeviceInfoList;
typedef CTypedPtrArray<CPtrArray, DeviceInfo*> CDeviceInfoArray;


//通道表
typedef struct ChannelInfo
{
	int nDeviceNo;
	int nChannelNo;
	CString strChannelName;
	BOOL bWorkEnable;
	float fFullScaleHigh;
	float fFullScaleLow;
	float fCalibrationHigh;
	float fCalibrationLow;
	byte byDataType;
	byte byDecimalDigits;
	CString strUnitString;
	BlobData<byte> bdAlarmLevelData;
	BlobData<byte> bdAlarmRangeData;
	BlobData<byte> bdDefBuf;

	//过程量的报警级别和报警范围缓冲区内容
	COLORREF clrNormalColor;				//正常状态（非报警）通道的颜色
	CAlarmLevelArray arrAlarmLevel;
	CAlarmRangeArray arrAlarmRange;

	//开关量的报警级别和报警范围缓冲区内容
	byte bySwitchEnableAlarm;				//开关量是否允许报警
	byte bySwitchAlarmValue;				//开关量为0时报警还是为1时报警

	//通道设置对话框用到
	byte byOldDataType;			//保存原始通道数据类型
	BOOL bMsgBoxDelData;		//标识提示删除数据是否选择“是”

	int nOptFlag;

	ChannelInfo ()
	{
		nDeviceNo = 0;
		nChannelNo = 0;
		strChannelName.Empty();
		bWorkEnable = FALSE;
		fFullScaleHigh = 0.0f;
		fFullScaleLow = 0.0f;
		fCalibrationHigh = 0.0f;
		fCalibrationLow = 0.0f;
		byDataType = 0;
		byDecimalDigits = 0;
		strUnitString.Empty();
		bdAlarmLevelData.Reset();
		bdAlarmRangeData.Reset();
		bdDefBuf.Reset();

		clrNormalColor = NORMAL_COLOR;

		bySwitchEnableAlarm = 0;
		bySwitchAlarmValue = 0;

		byOldDataType = 0;
		bMsgBoxDelData = FALSE;

		nOptFlag = OPT_NULL;
	}

	ChannelInfo (const ChannelInfo &channel)
	{
		Copy (channel);

		nOptFlag = OPT_NULL;
	}

	void Copy (const ChannelInfo &channel)
	{
		nDeviceNo = channel.nDeviceNo;
		nChannelNo = channel.nChannelNo;
		strChannelName = channel.strChannelName;
		bWorkEnable = channel.bWorkEnable;
		fFullScaleHigh = channel.fFullScaleHigh;
		fFullScaleLow = channel.fFullScaleLow;
		fCalibrationHigh = channel.fCalibrationHigh;
		fCalibrationLow = channel.fCalibrationLow;
		byDataType = channel.byDataType;
		byDecimalDigits = channel.byDecimalDigits;
		strUnitString = channel.strUnitString;
		bdAlarmLevelData = channel.bdAlarmLevelData;
		bdAlarmRangeData = channel.bdAlarmRangeData;
		bdDefBuf = channel.bdDefBuf;
	}
}ChannelInfo;
typedef CTypedPtrList<CPtrList, ChannelInfo*> CChannelInfoList;
typedef CTypedPtrArray<CPtrArray, ChannelInfo*> CChannelInfoArray;


//通道数据表
typedef struct ChannelData
{
	int nDataID;
	int nDeviceNo;
	int nChannelNo;
	COleDateTime odtSampleTime;
	float fMeasureValue;
	byte byAlarmLevel;
	BlobData<byte> bdData;

	int nOptFlag;

	ChannelData ()
	{
		nDataID = 0;
		nDeviceNo = 0;
		nChannelNo = 0;
		odtSampleTime = COleDateTime(2000, 1, 1, 1, 1, 1);
		fMeasureValue = 0.0f;
		byAlarmLevel = 0;
		bdData.Reset();

		nOptFlag = OPT_NULL;
	}
}ChannelData;


//报警数据表
typedef struct AlarmData
{
	int nAlarmDataID;
	int nDataID;
	int nDeviceNo;
	int nChannelNo;
	COleDateTime odtSampleTime;
	float fMeasureValue;
	byte byAlarmLevel;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	AlarmData ()
	{
		nAlarmDataID = 0;
		nDataID = 0;
		nDeviceNo = 0;
		nChannelNo = 0;
		odtSampleTime = COleDateTime(2000, 1, 1, 1, 1, 1);
		fMeasureValue = 0.0f;
		byAlarmLevel = 0;
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
}AlarmData;


//控制器表
typedef struct Controller
{
	int nDeviceNo;
	int nControllerNo;
	CString strControllerName;
	byte byControllerType;
	BlobData<byte> bdData;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	Controller ()
	{
		nDeviceNo = 0;
		nControllerNo = 0;
		strControllerName.Empty();
		byControllerType = 0;
		bdData.Reset();
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
}Controller;
