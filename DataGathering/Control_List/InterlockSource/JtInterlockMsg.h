

//���α׷� ���� �޽���
#define JT_INTERLOCK_MSG_01			"�� ���α׷��� �����ϰ��ִ� �ð�����\n�����Ͱ� �������� �ʽ��ϴ�.\n\n�׷��� ���� �Ͻðڽ��ϱ�?\n"
#define JT_INTERLOCK_CODE_01		"INTERLOCK-01"

//#define JT_INTERLOCK_MSG_01			"���α׷� �������� ���� �Ͽ����ϴ�."
//#define JT_INTERLOCK_CODE_01		"DataGathering - Start"



//DataGathering ó�� ���۽� ini ���� ���� ����

// 20200211 JSH : DB_TYPE : or POSTRGRE �߰�
#define START_SETTING_LIST_1			"#DataAgentini\r\n"\
									"#[#DataAgent#]\r\n"\
									"#Max_Processor : ó���� ���μ��� �ִ� 10���� ��밡�� \r\n"\
									"#InPutFileType : InPutFileType 1:ini,2:csv ��� ����\r\n"\
									"#Gathe_interval : ���� ���� 0:1��,1:5��,2:10��,3:15�� \r\n"\
									"#Delete_interval : ������ ���� ���� 0:1��,1:2�� \r\n"\
									"#DEMP_USECheck : DB ��� �̻�� �߻��ð� ���� ���� 0:�̻��,1:���\r\n"\
									"#Demp_AutoManual : �̻� �߻� �ð� �ڵ� ���� ���� 0:�̻��,1:���\r\n"\
									"#LOG_USECheck : ���� �α� ���� ���� 0:�̻��,1:���\r\n"\
									"#AutoRun_UseCheck : �ڵ� ���� ���� 0:�̻��,1:���\r\n"\
									"#SUBDB_USECheck : Sub DB ��뿩�� 0:�̻��,1:���\r\n"\
									"#LogPath : �α����� ���� ���\r\n"\
									"#[#DBINFO#] : DataBase ���� ����\r\n"\
									"#Server : Data Base ���� �ּ�\r\n"\
									"#DB_TYPE : Data Base Type : MSSQL or ORACLE or MySql or POSTRGRE \r\n"\
									"#DB : Data Base Name\r\n"\
									"#ID : Data Base ID\r\n"\
									"#PW : Data Base Password\r\n\r\n"\
									"#[EngMng] : �������������� XML ������ ���� ����\r\n"\
									"#XMLPath : XML ���� ��� ����\r\n"\
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