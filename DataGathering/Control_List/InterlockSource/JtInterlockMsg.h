

//프로그램 상태 메시지
#define JT_INTERLOCK_MSG_01			"본 프로그램을 종료하고있는 시간동안\n데이터가 생성되지 않습니다.\n\n그래도 종료 하시겠습니까?\n"
#define JT_INTERLOCK_CODE_01		"INTERLOCK-01"

//#define JT_INTERLOCK_MSG_01			"프로그램 정상으로 시작 하였습니다."
//#define JT_INTERLOCK_CODE_01		"DataGathering - Start"



//DataGathering 처음 시작시 ini 파일 정보 생성

// 20200211 JSH : DB_TYPE : or POSTRGRE 추가
#define START_SETTING_LIST_1			"#DataAgentini\r\n"\
									"#[#DataAgent#]\r\n"\
									"#Max_Processor : 처리할 프로세서 최대 10까지 사용가능 \r\n"\
									"#InPutFileType : InPutFileType 1:ini,2:csv 사용 선택\r\n"\
									"#Gathe_interval : 수집 간격 0:1분,1:5분,2:10분,3:15분 \r\n"\
									"#Delete_interval : 데이터 삭제 간격 0:1달,1:2달 \r\n"\
									"#DEMP_USECheck : DB 통신 이상시 발생시간 생성 여부 0:미사용,1:사용\r\n"\
									"#Demp_AutoManual : 이상 발생 시간 자동 수집 여부 0:미사용,1:사용\r\n"\
									"#LOG_USECheck : 세부 로그 생성 여부 0:미사용,1:사용\r\n"\
									"#AutoRun_UseCheck : 자동 수집 여부 0:미사용,1:사용\r\n"\
									"#SUBDB_USECheck : Sub DB 사용여부 0:미사용,1:사용\r\n"\
									"#LogPath : 로그파일 생성 경로\r\n"\
									"#[#DBINFO#] : DataBase 접속 정보\r\n"\
									"#Server : Data Base 접속 주소\r\n"\
									"#DB_TYPE : Data Base Type : MSSQL or ORACLE or MySql or POSTRGRE \r\n"\
									"#DB : Data Base Name\r\n"\
									"#ID : Data Base ID\r\n"\
									"#PW : Data Base Password\r\n\r\n"\
									"#[EngMng] : 에너지관리공단 XML 데이터 연동 관련\r\n"\
									"#XMLPath : XML 생성 경로 지정\r\n"\
									"[DataAgent]\r\n"\
									"Max_Processor=1\r\n"\
									"Gathe_interval=1\r\n"\
									"Delete_interval=1\r\n"\
									"SubDb_UseCheck=0\r\n"\
									"LOG_UseCheck=0\r\n"\
									"AutoRun_UseCheck=0\r\n"\
									"LogPath=\r\n\r\n"\
									"[Weather]\r\n"\
									"UrlLive=\r\n"\
									"UrlForecast=\r\n"\
									"Services=\r\n"\
									"LiveUseYN=\r\n"\
									"LiveLat=0\r\n"\
									"LiveLon=0\r\n\r\n"\
									"ForecastUseYN=\r\n"\
									"ForecastLat=0\r\n"\
									"ForecastLon=0\r\n\r\n"\
									"[DBINFO]\r\n"\
									"DB_TYPE=MSSQL\r\n"\
									"Server=127.0.0.1\r\n"\
									"DB=BEMS\r\n"\
									"ID=sa\r\n"\
									"PW=1\r\n"\
									"ConnectionTest=0\r\n\r\n"\
									"[DBName]\r\n"\
									"HMIDB=EASY_COMMON\r\n"\
									"WTDB=EASY_COMMON\r\n\r\n"\
									"[ISmart]\r\n"\
									"SiteName=\r\n"\
									"AccessID=\r\n"\
									"AccessPW=\r\n\r\n"\
									"[EngMng]\r\n"\
									"XMLPath=\r\n\r\n"\
									"[DataGathering]\r\n"\
									"Gathe_interval=0\r\n"\
									"Delete_interval=0\r\n"\
									"AutoRun_UseCheck=1\r\n"\
									"Demp_UseCheck=0\r\n"\
									"Demp_AutoManual=0\r\n"\
									"Weather_Gather=2\r\n"\
									"ISmart_Gather=2\r\n"\
									"EngMng_Gather=2\r\n"\
									"[GatherType]\r\n"\
									"Type=1\r\n"\
									"Count=0\r\n\r\n"\
									/*
									"[DataGathering]\r\n"\
									"Gathe_interval=0\r\n"\
									"Delete_interval=0\r\n"\
									"AutoRun_UseCheck=1\r\n"\
									"Demp_UseCheck=0\r\n"\
									"Demp_AutoManual=0\r\n"\
									"Weather_Gather=2\r\n"\
									"ISmart_Gather=2\r\n"\
									"EngMng_Gather=2\r\n"\
									"[GatherType]\r\n"\
									"Type=1\r\n"\
									"Count=0\r\n\r\n"\	*/	
