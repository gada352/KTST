#pragma once

#include "channel.h"

enum ExpressionType		//继电器表达式类型
{
	etOperator = 0, 
	etChannel = 1, 
};

//继电器表达式
typedef struct RelayExpression
{
	ExpressionType eExpressionType;	//表达式类型：0操作符、1关注的通道

// 	byte byOperatorType;	//操作符类型：0 End、1 "("、2 ")"、3 "&"、4 "|"、5 "~"
	CString strOperator;	//操作符

	int nDeviceNo;			//关注通道的设备号
	int nChannelNo;			//关注通道的通道号
	byte byAlarmLevel;		//报警级别

	CString strDisplay;		//在表达式编辑器中显示的字符串
	int nIndexBegin;		//在表达式编辑器中起始索引
	int nIndexEnd;			//在表达式编辑器中结束索引

	RelayExpression ()
	{
		eExpressionType = etOperator;
// 		byOperatorType = 0;
		strOperator.Empty();
		nDeviceNo = 0;
		nChannelNo = 0;
		byAlarmLevel = 0;

		strDisplay.Empty();
		nIndexBegin = 0;
		nIndexEnd = 0;
	}
}RelayExpression;
typedef CTypedPtrList<CPtrList, RelayExpression*> CRelayExpressionList;

class RelayChannel :
	public Channel
{
public:
	RelayChannel(void);
	virtual ~RelayChannel(void);

public:
	virtual BOOL Initialize ();
	virtual BOOL UnInitialize ();
	virtual void OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer ();

public:
	BOOL AnalyzeRelayChannelBuffer ();
	BOOL SaveRelayChannelBuffer ();

public:
	int GetOutputValue ();
	void Reset ();
	void RequestAllFollowingChannelData ();

	void SetExpressionUpdate (BOOL bUpdate);
	BOOL IsExpressionUpdated ();

public:
	CString GetExpressionString ();
	BOOL Cal(CString str,int& nReturnValue);

private:
	void ClearMemory ();

public:
	int m_nOutputDelay;						//输出延时(秒)
	BOOL m_bEnergized;						//充电
	BOOL m_bBypass;							//旁路
	BOOL m_bLatching;						//锁定
	CRelayExpressionList m_listExpression;	//表达式链表

private:
	BOOL m_bDelayStarting;					//输出延时启动
	DWORD m_dwDelayStartTime;				//输出延时已经延时时间

	int m_nLastOutputValue;					//前一次输出值

	BOOL m_bExpressionUpdate;				//标识表达式是否有报警状态更新

public:
	BOOL m_bExpressionValid;				//标识表达式是否有效（无效可能是因为选择的采集通道不存在）


};
typedef CTypedPtrList<CPtrList, RelayChannel*> CRelayChannelList;

