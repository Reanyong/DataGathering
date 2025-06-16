
#include ".\Control_List/MyResource/MyResource.h"
#include "JtInterlockMsg.h"

#include <math.h>
#include <list>

//////////////////////////////////////////////////////////////////////////
//창크기 고정
//2015-09-08
#define USER_WINDOWS_SIZE_CX		1300
#define USER_WINDOWS_SIZE_CY		850
//////////////////////////////////////////////////////////////////////////
//고정
#define PRO_TITLE_NAME				"DataAgent"
#define PRO_RUN						1
#define PRO_STOP					2
//////////////////////////////////////////////////////////////////////////
//output MSG Color
//2015-09-08
#define USER_COLOR_BLUE				RGB(0,	84,	255) //제목
#define USER_COLOR_RED				RGB(255,0,	0)
#define USER_COLOR_PINK				RGB(255,0,	255)
#define USER_COLOR_BLACK			RGB(0,	0,	0)
#define USER_COLOR_LIME				RGB(0,	255,	0)
//////////////////////////////////////////////////////////////////////////
//LogFileName
#define LOG_FOLDER_NAME_1				"MinuteDataLog"
#define LOG_FOLDER_NAME_4				"TagSearchLog"


//////////////////////////////////////////////////////////////////////////
//Control ID
#define USER_CONTROL_LIST		(WM_USER + 1100)
#define USER_CONTROL_LIST2		(WM_USER + 1200)

//////////////////////////////////////////////////////////////////////////
//Processor 별 처리할 TAG갯수 단위
#define PROCESSOR_NO1_LIMIT				500
#define PROCESSOR_NO2_LIMIT				1000
#define PROCESSOR_NO3_LIMIT				1500
#define PROCESSOR_NO4_LIMIT				2000
#define PROCESSOR_NO5_LIMIT				2500
#define PROCESSOR_NO6_LIMIT				3000
#define PROCESSOR_NO7_LIMIT				3500

#define PROCESSOR_NO1				1
#define PROCESSOR_NO2				2
#define PROCESSOR_NO3				3
#define PROCESSOR_NO4				4
#define PROCESSOR_NO5				5
#define PROCESSOR_NO6				6
#define PROCESSOR_NO7				7

#define  THREAD_CREADTE_MAX			10

#define  THREAD_LEVEL_1				1
#define  THREAD_LEVEL_2				2
#define  THREAD_LEVEL_3				3
#define  THREAD_LEVEL_4				4
#define  THREAD_LEVEL_5				5
#define  THREAD_LEVEL_6				6
#define  THREAD_MAX					7

//limit 20210831 ksw 1200~7200 -> 600~6000
#define  THREAD_LEVEL_1_LIMIT		600
#define  THREAD_LEVEL_2_LIMIT		1200
#define  THREAD_LEVEL_3_LIMIT		2400
#define  THREAD_LEVEL_4_LIMIT		3600
#define  THREAD_LEVEL_5_LIMIT		4800
#define  THREAD_LEVEL_6_LIMIT		6000

//FormView ID 관리
#define FORM_VIEW_ID_SYSTEM				0
#define FORM_VIEW_ID_1					1
#define FORM_VIEW_ID_2					2
#define FORM_VIEW_ID_3					3
#define FORM_VIEW_ID_4					4
#define FORM_VIEW_ID_5					5
#define FORM_VIEW_ID_6					6
#define FORM_VIEW_ID_7					7

#define LOG_MESSAGE_1					0
#define LOG_MESSAGE_2					1
#define LOG_MESSAGE_3					2
#define LOG_MESSAGE_4					3
#define LOG_MESSAGE_5					4
#define LOG_MESSAGE_6					5
#define LOG_MESSAGE_7					6

//////////////////////////////////////////////////////////////////////////
//공통 ID
#define RETURN_SUCCESS				(WM_USER + 1000)
#define RETURN_FAIL					(WM_USER + 1001)

//////////////////////////////////////////////////////////////////////////
//ADO MSG ID
#define ADO_CONNECT_ERR				-1000 //ADO 접속 끊겼을때
#define WM_USER_LOG_MESSAGE			(WM_USER + 2000)

#define ADO_CONNECTION				(WM_USER + 2100)
#define ADO_DISCONNECTION			(WM_USER + 2101)
#define ADO_QUERY_SELECT			(WM_USER + 2102)
#define ADO_QUERY_INSERT			(WM_USER + 2103)
#define ADO_QUERY_UPDATE			(WM_USER + 2104)
#define ADO_QUERY_DELETE			(WM_USER + 2105)
#define ID_START_THREAD				(WM_USER + 1100)
#define ID_STOP_THREAD				(WM_USER + 1101)
#define ID_AUTO_STOP_THREAD			(WM_USER + 1102)

//스레드 별 사용자 메세지
#define WM_USER_TAGINFO_MESSAGE			(WM_USER + 2010)
#define WM_USER_DATAGATHERING_MESSAGE	(WM_USER + 2011)
#define WM_USER_BACKUPDATA_MESSAGE		(WM_USER + 2012)
#define WM_USER_DATAREMOVE_MESSAGE		(WM_USER + 2013)

#define WM_USER_MIN_MAIN_MESSAGE	(WM_USER + 2021)
#define WM_USER_HOUR_MESSAGE		(WM_USER + 2022)
#define WM_USER_DAY_MESSAGE			(WM_USER + 2023)
#define WM_USER_WEATHER_MESSAGE		(WM_USER + 2024)
#define WM_USER_ALARM_MESSAGE		(WM_USER + 2025)

#define USER_ID_MIN_SYSLOG			(WM_USER + 2030) //분처리 결과 USER ID
#define USER_ID_HOUR_SYSLOG			(WM_USER + 2031) //시처리 결과 USER ID
#define USER_ID_DAY_SYSLOG			(WM_USER + 2032) //일처리 결과 USER ID
#define USER_ID_WEATHER_SYSLOG		(WM_USER + 2033) //날씨처리 결과 USER ID

#define ERROR_DB_RECONNECTION	 	-100 //재접속 오류
#define ERROR_DB_COM_ERROR			-2 //컴오류(잘못된 쿼리 오류)
#define ERROR_DB_QUERY_FAIL1		-3 //그외 쿼리 오류
#define ERROR_DB_QUERY_FAIL2		-4 //그외 쿼리 오류
#define ERROR_DB_NO_TABLE			-5 //테이블이 없을때
#define THREAD_END					-10 //그외 쿼리 오류

#define  ST_LIST_CLEAR				0
#define  ST_LIST_RELEASE			1
#define  ST_LIST_INITIAL			2
#define  ST_LIST_DELETE				5

//그리드 ID 관리
#define  ID_GRID_PROCESSOR			(WM_USER + 1010)
#define  ID_GRID_TAG_INFO			(WM_USER + 1011)
#define  ID_GRID_BACKUP_TIME		(WM_USER + 1012)
#define  ID_GRID_TAG_LIST			(WM_USER + 1013)
#define  ID_GRID_TAG_DBLIST			(WM_USER + 1014)

//쿼리 리턴 타입
#define  QUERY_TYPE_INSERT			0
#define  QUERY_TYPE_UPDATE			1
#define  QUERY_TYPE_UPDATEORINSERT	2

#define TIMEWAIT_FILELOG			500

//로그 작성시 색상
#define LOG_COLOR_BLUE				0
#define LOG_COLOR_RED				1
#define LOG_COLOR_PINK				2


#define  ID_GRID_RENEW				100

#define VIEW_TAG					1

//////////////////////////////////////////////////////////////////////////
//URL 접속 관련 ID
#define URL_CONNECT_FAIL_1			1
#define URL_CONNECT_FAIL_2			2
#define URL_CONNECT_FAIL_3			3
#define URL_ACCOUNT_FAIL_1			4





//////////////////////////////////////////////////////////////////////////
//구조체 정리

//프로젝트 정보
typedef struct _stProjectInfo_ {
	char szProjectName[128];
	char szProjectPath[256];

	char szDTGatheringPath[256];
	char szDTGatheringLogPath[256];
	char szDTGatheringIniPath[256];

	char szProjectIniPath[256];
	char szProjectLogPath[256];
	char szWeatherLivePath[256];
	char szWeatherForecastPath[256];
	char szProjectFMSLogPath[256];
	char szProjectTAGLogPath[256];
}ST_PROJECT_INFO;//connection

//DB 접속 정보
typedef struct _stDBInfo_ {
	char szServer[64];
	char szDB[64];
	char szID[64];
	char szPW[64];
	UINT unDBType;

	int nSubUseCheck;
	UINT unSub_DBType;
	char szSub_Server[64];
	char szSub_DB[64];
	char szSub_ID[64];
	char szSub_PW[64];
}ST_DBINFO;//connection

//2016-11-28 생성
typedef struct _stDataBesaName_ {
	char szHMIDBName[64]; //HMI DB
	char szWTDBName[64];  //Weather DB
	char szCommonDBName[64]; //Common DB
	char szConfigName[64];  //Config DB
	char szEmsName[64];  //Config DB
	char szFmsName[64];  //Config DB
}ST_DATABASENAME;

//Setting Info
typedef struct ST_GatherInfo_{
	int nGathe_Interval;
	int nGathe_IntervalPos;
	int nDemp_Check;  //덤프 생성/미생성 여부
	int nDemp_AutoCheck; //덤프 데이터 자동 생성
	int nAutoRun_Check; // 데이터 자동/수동 수집
	int nProduct_Check; //20210305 ksw 제품 선택 0: BEMS, 1: EMS
	int nDelete_Interval; //데이터 삭제 사용
	int nDelete_IntervalPos; //데이터 삭제 사용
	int nDetailsLogCheck;
	int nWeather_GatherType; //0:직접 설정,1:Web에서 설정,2:사용안함 //2016-11-28 추가
	int nISmart_GatherType; //0:직접 설정,1:Web에서 설정,2:사용안함 //2016-11-28 추가
	int nEngMng_GatherType; //0:직접 설정,1:Web에서 설정,2:사용안함 //2016-11-28 추가
	char szUrlLive[128];
	char szUrlForecast[128];
}ST_GATHERINFO;

//사이트 정보
typedef struct ST_SiteWeatherList_{
	char szSiteId[48];
	char szSiteName[48];
	char szLiveSeviceKey[128];
	float fLat; //위도
	float fLon; //경도
}ST_WEATHER_LIST;
//////////////////////////////////////////////////////////////////////////
//한전연동 사용자 접속 정보
typedef struct _stFileISmartAccess_ {
	char szSiteID[64];
	char szSiteName[64];
	char szID[48];
	char szPW[48];
}ST_FILE_ISMARTACCESS;

//한전연동 사용할 정보
typedef struct ST_SiteInterlockInfo_{
	char szSiteId[48];
	char szSiteName[48];
	char szID[48];
	char szPW[48];
	int nRowIndex;
	int nUSE_YN;
	int nEdit_Check;
	int nConnectFailCount;
}ST_SITE_INTERLOCK;

//한전데이터 파싱데이터 정보
typedef struct _stIConnectData_ {
	char szSiteId[64];
	int nMin[4];
	float fParsingValue[7][4];
	int nDataType[7][4];
}ST_ICONNECT_DATA;

//TAG List
struct ST_TagInfoList
{
	int nRowNum;
	char szTAG_Id[128];
	char szTAG_Name[64];
	char szTAG_DESC[64];
	char szSITE_Id[30];
	char szGroupName[64];
	float fLastValue;
	int  nTagType;
	float fValue;
};

//////////////////////////////////////////////////////////////////////////
//Gather Type
typedef struct ST_GatherUseList_{
	char szSiteId[48];
	char szSiteName[48];
	char szDeviceId[48];
	char szDeviceName[48];
	short nUse_YN;
}ST_GATHER_USE_LIST;
typedef struct ST_GatherInfoType_{
	short nGatherType; //1:TAG 리스트 정부 수집 2:사이트 디바이스별 수집
	short nCount;
	ST_GATHER_USE_LIST stGatherUseList[12];
}ST_GATHERINFO_TYPE;
//////////////////////////////////////////////////////////////////////////
//알람 - 관제점 알람 리스트 처리시
typedef struct _stDeviceListInfo_{
	char szSite_Id[48];
	char szSite_Name[48];
	char szDevice_Id[48];
	char szDevice_Name[48];
	int  nTagTotalNumber;
}ST_DEVICE_INFO;

typedef struct _stSubThreadInfo_ {
	int nInterval;
	int nProduct;			// 20210308 제품 버전
	int nThreadNumber;
	char szThreadName[28];
	int nThreadDataCount;
	BOOL bDataChangeCheck;
	ST_DEVICE_INFO stDeviceInfo;
	void init()
	{
		nThreadNumber = -1;
		nThreadDataCount = -1;
		memset(&stDeviceInfo,0x00,sizeof(stDeviceInfo));
	}
}ST_SUBTHREAD_INFO;//connection
//////////////////////////////////////////////////////////////////////////
struct ST_TagAlarmList
{
	char szStartTime[20];	//알람 발생 시간
	char szAlarmId[48];		//알람 ID
	int nAlarmKind;			//알람 타입
	char szAlarmLevel[48];	//알람레벨
	char szAlarmTypeTxt[12]; //DI 조건시 사용
	int nAlarmType;
	float fHIHIVal;			//HIHI
	float fHIGHVal;			//High
	float fLOWVal;			//low
	float fLOLOVal;			//lolo
	BOOL bHihi_Result;
	BOOL bHigh_Result;
	BOOL bLow_Result;
	BOOL bLolo_Result;
	char szTagId[128];		//TagID
	char szTagName[64];		//TagName
	int nTagType;			//TagType
	float fCurrentValue;	//현재 값
	float fLastValue;
	float fAckValue;		//발생값
	int nAlarmSt;			//알람
	int nOldAlarmAckType;
};

typedef struct _stAlarmCheck_{
	char szAlarmStartTime[24];
	int nOldAlarmAckType;
	float fAckValue;
}ST_ALARMCHECK;

//////////////////////////////////////////////////////////////////////////
//알람 - 시설물 알람 구조
struct ST_FMSAlarmList{
	char szSite_Id[48];
	char szSite_Name[48];
	char szFCT_ID[48];
	char szFCT_Name[48];
	char szAlarmId[48];
	char szAlarmLevel[48];	//알람레벨
	CTime timeExptChangeDate;
	int nChangeAlarmDate;
};

struct ST_UMSSend_UserList
{
	char szUserName[48];
	char szAlarmleveldesc[48];
	int nSMS_SendCheck;
	int nEmail_SendCheck;
	char szUserTEL[16];
	char szUserEmail[64];
};

//////////////////////////////////////////////////////////////////////////
//에너지관리공단 연동 구조
typedef struct _st_siteinfo_{
	char szSiteName[24];
	char szBEMS_ID[12];
	char szBEL_ID[12];
}ST_SITE_INFO;

typedef struct _ST_TagDetailsList_{
	char szHmiTag_ID[128];
	char szDV_ID[12];
	char szTAG_ID[12];
	char szNEW_TAG_TP_GRP_code[4];	//관제점그룹
	char szNEW_TAG_TP_GRP_NM[24];
	char szNEW_TAG_TP_code[4];
	char szNEW_TAG_TP_NM[24];		//관제점 유형
	char szNEW_MTAL_code[4];
	char szNEW_MTAL_NM[24];			//물질
	char szNEW_UNIT_code[4];
	char szNEW_UNIT_NM[24];			//단위
	char szNEW_LOC_FLR_code[4];
	char szNEW_LOC_FLR_NM[24];		//층
	char szNEW_DV_CLSF_01_code[4];
	char szNEW_DV_CLSF_01_NM[24];	//장비분류-대
	char szNEW_DV_CLSF_02_code[4];
	char szNEW_DV_CLSF_02_NM[24];	//장비분류-중
	char szNEW_DV_CLSF_03_code[4];
	char szNEW_DV_CLSF_03_NM[24];	//장비분류-소
	char szNEW_DV_CLSF_LOC_code[4];
	char szNEW_DV_CLSF_LOC_NM[24];	//세부분류
	char szNEW_VIRT_TAG_code[4];
	char szNEW_VIRT_TAG_NM[24];		//가상관제점여부
	char szNEW_MEAU_CYCLE_code[4];
	char szNEW_MEAU_CYCLE_NM[24];	//측정주기
	char szNEW_MEAU_CYCLE_UNIT_code[4];
	char szNEW_MEAU_CYCLE_UNIT_NM[24];	//주기단위
	char szNEW_BLD_BUND_code[4];
	char szNEW_BLD_BUND_NM[24];
	char szTAG_NAME[64];
	char szTAG_DESC[64];
	float fHIGH_LIMIT_VALUE;
	float fLOW_LIMIT_VALUE;
	int nTAG_TYPE;
}ST_TAG_LIST,*pST_TAG_LIST;

typedef struct _st_AllTagInfo_{
protected:
	pST_TAG_LIST pstTagList;
	int nTotalCount;
	int nItemCount;
public:
	void init(int nCount)
	{
		pstTagList = new ST_TAG_LIST[nCount];
		memset(pstTagList,0x00,sizeof(ST_TAG_LIST)*nCount);
		nTotalCount = nCount;
		nItemCount = 0;
	}

	int GetSize()								{		return nTotalCount;		}
	void SetDataAdd(ST_TAG_LIST stTaginfo)		{		pstTagList[nItemCount++] = stTaginfo;		}
	ST_TAG_LIST GetData(int nPos)				{		return pstTagList[nPos];	}
	void DeleteItem()
	{
		if(pstTagList != NULL)
		{
			delete []pstTagList;
			pstTagList = NULL;
		}
	}
}ST_ALLTAGLIST;

typedef struct _ST_TagValueList_{
	char szHmiTag_ID[128];
	char szDV_ID[12];
	char szTAG_ID[12];
	char szTAG_NAME[64];
	char szTAG_DESC[64];
	float fCurrentValue;
	float fOldValue;
	int nTAG_TYPE;
}ST_TAG_VALUE_LIST,*pST_TAG_VALUE_LIST;

typedef struct _st_TagValue_{
protected:
	pST_TAG_VALUE_LIST pstTagValue;
	int nTotalCount;
	int nItemCount;
public:
	void init(int nCount)
	{
		pstTagValue = new ST_TAG_VALUE_LIST[nCount];
		memset(pstTagValue,0x00,sizeof(ST_TAG_VALUE_LIST)*nCount);
		nTotalCount = nCount;
		nItemCount = 0;
	}

	int GetSize()								{		return nTotalCount;		}
	void SetDataAdd(ST_TAG_VALUE_LIST stTaginfo){		pstTagValue[nItemCount++] = stTaginfo;		}
	ST_TAG_VALUE_LIST GetData(int nPos)			{		return pstTagValue[nPos];	}
	void SetDataValueUpdate(int nPos,ST_TAG_VALUE_LIST stTaginfo)
	{
		pstTagValue[nPos].fCurrentValue = stTaginfo.fCurrentValue;
		pstTagValue[nPos].fOldValue = stTaginfo.fOldValue;
	}
	void DeleteItem()
	{
		if(pstTagValue != NULL)
		{
			delete []pstTagValue;
			pstTagValue = NULL;
		}
	}
}ST_ALLTAGValue;

typedef struct _st_code_{
	char szCode[4];
	char szCodeName[24];
}ST_CODE,*pST_CODE;

typedef struct _st_codeItems_{
protected:
	pST_CODE pstCode;
	int nTotalCount;
	int nItemCount;
public:
	void init(int nCount)
	{
		pstCode = new ST_CODE[nCount];
		memset(pstCode,0x00,sizeof(ST_CODE)*nCount);
		nTotalCount = nCount;
		nItemCount = 0;
	}
	int GetSize()							{		return nTotalCount;						}
	void SetDataAdd_1(ST_CODE stCodeItem)		{		pstCode[nItemCount++] = stCodeItem;		}
	void SetDataAdd(ST_CODE *stCodeItem)	{		memcpy(pstCode,stCodeItem,sizeof(ST_CODE)*nTotalCount);	}
	ST_CODE GetData(int nPos)				{		return pstCode[nPos];					}
	void DeleteItem()
	{
		if(pstCode != NULL)
		{
			delete []pstCode;
			pstCode = NULL;
		}
	}
}ST_CODEITEMs;


//////////////////////////////////////////////////////////////////////////
//위도,경도 / X, Y 변경시 정보
#define  RE (6371.00877) // 지구 반경(km)
#define  GRID (5.0) // 격자 간격(km)
#define  SLAT1 (30.0) // 투영 위도1(degree)
#define  SLAT2 (60.0) // 투영 위도2(degree)
#define  OLON (126.0) // 기준점 경도(degree)
#define  OLAT  (38.0) // 기준점 위도(degree)
#define  XO (43) // 기준점 X좌표(GRID)
#define  YO (136) // 기1준점 Y좌표(GRID)

//날씨 API URL 정보
typedef struct _stWeatherConnectInfo_ {
	char szURL[128];
	char szServiceKey[128];
	int nX; //좌표
	int nY; //좌표
}ST_WEATHERCONNECT_INFO;//connection

typedef struct _stWeatherInfoData_ {
	char szSite_ID[48];
	char szSite_Name[32];
	char szLiveUSE_YN[2];
	char szServiceKey[128];
	char szLiveLat[24]; //위도
	char szLiveLon[24]; //경도
	char szForecastUSE_YN[2];
	char szForecastLat[24]; //위도
	char szForecastLon[24]; //경도
}ST_WEATHER_INFO;//connection

struct ST_AlarmOccurs
{
	char szOccursTime[20];
	float fOccursValue;
	int nAlarmSt;
};

//날씨 정보 컬럼 타입 사용
struct ST_DB_WeatherColumn {
	char szColumnName[64]; //컬럼 명칭
	char szCategory[24];
	char szValue[24];  //컬럼에 해당하는 값
};


//날씨 사용
#define FRC_SPACEDATA	0
#define FRC_GRID		1
#define FRC_TIMEDATA	2

typedef struct _stWeatherInfo_ {
	char szAddress[128];
	char szServiceKey[128];

	//	nPTY_TYPE = 0 : 강수량
	//	nPTY_TYPE = 1 : 적설량
	int nPTY_TYPE;

	void init()
	{
		nPTY_TYPE = 0;
	}

	CString GetUrl(const char *szAddress, const char *szKey, int nTyp, CTime time,int nX, int nY)
	{
		CString strUrl = "",strTyp = "";

		switch(nTyp)
		{
		case FRC_SPACEDATA:
			strTyp = "ForecastSpaceData";
			break;
		case  FRC_GRID:
			strTyp = "ForecastGrib";  //실황
			//strUrl.Format("%s/service/SecndSrtpdFrcstInfoService/%s?ServiceKey=%s&base_date=%04d%02d%02d&base_time=%02d00&nx=%d&ny=%d",szAddress,strTyp,szKey,
			strUrl.Format("%s/service/SecndSrtpdFrcstInfoService2/%s?ServiceKey=%s&base_date=%04d%02d%02d&base_time=%02d00&nx=%d&ny=%d",szAddress,strTyp,szKey,
				time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(), nX, nY);
			break;
		case  FRC_TIMEDATA:
			//strTyp = "ForecastTimeData";
			strUrl.Format("%s/wid/queryDFS.jsp?gridx=%d&gridy=%d",szAddress,nX,nY);
			break;
		}
		/*strUrl.Format("%s/service/SecndSrtpdFrcstInfoService/%s?ServiceKey=%s&base_date=%04d%02d%02d&base_time=%02d00&nx=%d&ny=%d",szAddress,strTyp,szKey,
			time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(), nX, nY);*/

		return strUrl;
	}

	/*T1H:기온 	RN1:1시간 강수량	SKY:하늘상태	UUU:동서바람성분	VVV:남북바람성분	REH:습도	PTY:강수형태	LGT:낙뢰	VEC:풍향	WSD:풍속*/
	CString GetForecastGrib_TYP(CString strName,CString strValue)
	{
		CString strRetBuff ="";
		if (strName == "T1H")
			return strValue;
		else if (strName == "RN1")
		{
			if(nPTY_TYPE == 0) //기본 강수량
			{
				switch(atoi(strValue))
				{
				case 0: strRetBuff = "0 mm";		break;
				case 1: strRetBuff = "1 mm 미만";	break;
				case 5: strRetBuff = "1 ~ 4 mm";	break;
				case 10: strRetBuff = "5 ~ 9 mm";	break;
				case 20: strRetBuff = "10 ~ 19 mm"; break;
				case 40: strRetBuff = "20 ~ 39 mm"; break;
				case 70: strRetBuff = "40 ~ 69 mm";	break;
				case 100: strRetBuff = "70 mm 이상";	break;
				default: strRetBuff = "0";	break;
				}
			}
			else //적설량
			{
				switch(atoi(strValue))
				{
				case 0: strRetBuff = "0 Cm";		break;
				case 1: strRetBuff = "1 Cm 미만";	break;
				case 5: strRetBuff = "1 ~ 4 Cm";	break;
				case 10: strRetBuff = "5 ~ 9 Cm";	break;
				case 20: strRetBuff = "10 ~ 19 Cm"; break;
				case 100: strRetBuff = "20 Cm 이상";	break;
				default: strRetBuff = "0";	break;
				}
			}

			return strRetBuff;

		}
		else if (strName == "SKY")
			return strValue;
		else if (strName == "UUU")
		{
			float fBuffer = 0;
			fBuffer = (float)atof(strValue);
			if(fBuffer == 0)
				strRetBuff.Format("0");
			else if(fBuffer > 0)
				strRetBuff.Format("동 - %0.2f m/s",fBuffer);
			else
			{
				fBuffer = 0 - (fBuffer);
				strRetBuff.Format("서 - %0.2f m/s",fBuffer);
			}
			return strRetBuff;
		}
		else if (strName == "VVV")
		{
			float fBuffer = 0;
			fBuffer = (float)atof(strValue);
			if(fBuffer == 0)
				strRetBuff.Format("0");
			else if(fBuffer > 0)
				strRetBuff.Format("북 - %0.2f m/s",fBuffer);
			else
			{
				fBuffer = 0 - (fBuffer);
				strRetBuff.Format("남 - %0.2f m/s",fBuffer);
			}
			return strRetBuff;
		}
		else if (strName == "REH")
			return strValue;
		else if (strName == "PTY")
		{
			//강수 형태 - 0:없음,1:비,2:비/눈,3:눈(눈일때만 nPTY_TYP 활성화 1)
			if(strValue == "3")
				nPTY_TYPE = 1;
			else
				nPTY_TYPE = 0;

			return strValue;
		}
		else if (strName == "LGT")
		{
			if(strValue.IsEmpty() != TRUE)
				return strValue;
			else
				return "0";
		}
		else if (strName == "VEC")
			return strValue;
		else if (strName == "WSD")
			return strValue;

		return "Error";
	}

	//위도/경도를 X/Y로 변환
	void XY_Conv(double dLat, double dLon, double *drsX,double *drsY)
	{
		static double PI,DEGRAD,RADDEG;
		static double re, olon, olat, sn, sf, ro;
		double slat1, slat2, ra, theta;

		PI = asin(1.0)*2.0;
		DEGRAD  = PI / 180.0;

		re = RE / GRID;
		slat1 = SLAT1 * DEGRAD;
		slat2 = SLAT2 * DEGRAD;
		olon = OLON * DEGRAD;
		olat = OLAT * DEGRAD;

		sn = tan(PI * 0.25 + slat2 * 0.5) / tan(PI * 0.25 + slat1 * 0.5);
		sn = log(cos(slat1) / cos(slat2)) / log(sn);
		sf = tan(PI * 0.25 + slat1 * 0.5);
		sf = pow(sf, sn) * cos(slat1) / sn;
		ro = tan(PI * 0.25 + olat * 0.5);
		ro = re * sf / pow(ro, sn);

		//////////////////////////////////////////////////////////////////////////
		//위/경도 -> X Y 좌표로 변환
		ra = tan(PI * 0.25 + (dLat) * DEGRAD * 0.5);
		ra = re * sf / pow(ra, sn);
		theta = dLon * DEGRAD - olon;

		if(theta > PI)
			theta -= 2.0 * PI;
		if(theta < - PI)
			theta += 2.0 * PI;

		theta *= sn;

		drsX[0] = floor(ra * sin(theta) + XO + 0.5);
		drsY[0] = floor(ro - ra * cos(theta) + YO + 0.5);
	}
}ST_WEATHERINFO;


typedef struct _stWeatherForecast_{
	int nHour; //예보 시간
	int nDay; //(오늘/내일/모레 중 언제)
	float fTemp; //예보 시간의 온도
	float fTempMx; //최고온도
	float fTempMn; //최저온도
	int nSky;//하늘 상태(1:맑음,2:구름조금,3:구름많음,4:흐림
	int nPty;//강수상태(0:없음,1:비,2:비/눈,3:눈/비,4:눈)
	char szWfkor[12]; //typ:맑음,구름 조금,구름 많음, 흐림, 비, 눈/비, 눈
	int nPop;//강수 확률
	int nWs;//풍속(m/s)
	int nWd;//풍향 0~7 (북, 북동, 동, 남동, 남, 남서, 서, 북서)
	char szWdkor[12];  //풍향 동,북,북동,북서,남,남동,남서,서
	int nReh;//습도(%)
}ST_WT_FROECAST,*pST_WT_FROECAST;

typedef struct __stWeatherForecast__{
	pST_WT_FROECAST pStWTData;
	char szDateTime[16];
	int nTotalCnt;
	CTime HeadTime;

	void init(CString strTM)
	{
		memset(szDateTime,0x00,sizeof(szDateTime));
		pStWTData = new ST_WT_FROECAST[nTotalCnt];

		int nY = 0,nM = 0,nD = 0,nH = 0;
		nY = atoi(strTM.Left(4));
		strTM.Delete(0,4);
		nM = atoi(strTM.Left(2));
		strTM.Delete(0,2);
		nD = atoi(strTM.Left(2));
		strTM.Delete(0,2);
		nH = atoi(strTM.Left(2));

		CTime CheckTime(nY,nM,nD,0,0,0);
		 HeadTime = CheckTime;
	}

	void DeleteItme()
	{
		if(pStWTData != NULL)
		{
			//delete pStWTData;
			pStWTData = NULL;
		}
	}
}ST_WEATHER_FROECAST;




