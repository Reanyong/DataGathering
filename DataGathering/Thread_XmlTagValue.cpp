// Thread_XmlTagValue.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataGathering.h"
#include "Thread_XmlTagValue.h"
#include "FormView_TagMapping.h"


// CThread_XmlTagValue

IMPLEMENT_DYNCREATE(CThread_XmlTagValue, CWinThread)

CThread_XmlTagValue::CThread_XmlTagValue()
{
	m_bEndThread = NULL;
	m_stTagValueList = new ST_ALLTAGValue;
	m_nTagInfoListCheck = 0;
	m_pstAllTagInfoList = new ST_ALLTAGLIST;
}

CThread_XmlTagValue::~CThread_XmlTagValue()
{
	if(DB_Connect != NULL)
	{
		if(DB_Connect->GetDB_ConnectionStatus() == 1)
			DB_Connect->DB_Close();

		delete DB_Connect;
		DB_Connect = NULL;
	}
	CoUninitialize();

}

BOOL CThread_XmlTagValue::InitInstance()
{
	// TODO: ���⿡�� �� �����忡 ���� �ʱ�ȭ�� �����մϴ�.
	CoInitialize(NULL);

	return TRUE;
}

int CThread_XmlTagValue::ExitInstance()
{
	// TODO: ���⿡�� �� �����忡 ���� ������ �����մϴ�.
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThread_XmlTagValue, CWinThread)
END_MESSAGE_MAP()


CString CThread_XmlTagValue::Com_Error(const char *szLogName,_com_error *e)
{
	CString strRunlog_E2 = "",strRunlog_E2Log = "",strErrorID = "",strErrorCode;
	_bstr_t bstrSource(e->Source());
	_bstr_t bstrDescription(e->Description());
	strRunlog_E2.Format("Position : [%s],Description : [%s], DB Error Code : [%08lx], Code meaning : [%s], Source : [%s]",
		szLogName,(LPCTSTR)bstrDescription,e->Error(), e->ErrorMessage(), (LPCTSTR)bstrSource);

	if(m_nDBType == DB_ORACLE)
	{
		strErrorID.Format("%s",(LPCTSTR)bstrDescription);
		strErrorCode = strErrorID.Mid(4,5);//Left(nPos);
	}
	else if(m_nDBType == DB_MSSQL)
	{
		strErrorCode.Format("%08lx",e->Error());
	}

#ifdef _DEBUG
	TRACE("ProcessorName : [%s],Position : [%s][%s]\r\n",m_strLogTitle,szLogName,strRunlog_E2);
#endif

	strRunlog_E2Log.Format("[%s] Position : [DB Com Error..], LogName: [%s], %s",m_strLogTitle,szLogName, strRunlog_E2);

	//	SetWriteLogFile(strRunlog_E2Log);
	SysLogOutPut(m_strLogTitle,strRunlog_E2,USER_COLOR_RED);
	Sleep(500);

	return strErrorCode;
}

void CThread_XmlTagValue::SysLogOutPut(CString strLogName,CString strMsg, COLORREF crBody)
{
	_addSystemMsg(LOG_MESSAGE_4, USER_COLOR_BLUE, "Processor-log : [Xml Data ����]", crBody, strMsg);
}


// CThread_XmlTagValue �޽��� ó�����Դϴ�.
int CThread_XmlTagValue::Run()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	int nTimeTemp = -1;
	CTime currentTime;
	BOOL bStartCheck = FALSE;
	CString strXmlSavePath = "",strMsg;

	ST_DBINFO stDBInfo = _getInfoDBRead(g_stProjectInfo.szProjectIniPath);
	strXmlSavePath = _getXmlPath(g_stProjectInfo.szProjectIniPath);

	m_strLogTitle= "VPN Data ����";
	strMsg.Format("������ ������ġ : %s",strXmlSavePath);
	SysLogOutPut(m_strLogTitle,strMsg,USER_COLOR_BLACK);

	m_nDBType = stDBInfo.unDBType;
	DB_Connect = new CAdo_Control();
	DB_Connect->DB_SetReturnMsg(WM_USER_LOG_MESSAGE,m_WindHwnd,"TagMapping ����",g_stProjectInfo.szDTGatheringLogPath);	
	DB_Connect->DB_ConnectionInfo(stDBInfo.szServer,stDBInfo.szDB,stDBInfo.szID,stDBInfo.szPW,stDBInfo.unDBType);
	
	do 
	{
		if(DB_Connect->GetDB_ConnectionStatus() != 1)
		{
			BOOL bConnectCheck = DB_Connect->DB_Connection();
			if(bConnectCheck != TRUE)
			{
				Sleep(500);
				continue;
			}
		}

		try
		{
			if(m_bEndThread)
				break;

			currentTime = CTime::GetCurrentTime();
			
			if(m_nTagInfoListCheck == 10)
			{
				strMsg.Format("TAGInfo ��� : [%d]�� ����Ʈ �����(TAGInfovo.xml)",m_nTagListCount);
				((CFormView_TagMapping *)(m_pCtrl))->OutputHistory(currentTime.Format("%Y-%m-%d %H:%M"),strMsg);

				int nRet = SetCreateXMLTagInfo(currentTime,strXmlSavePath);
				m_nTagInfoListCheck = 0;
				if(nRet == THREAD_END)
					break;
			}
			if(m_bEndThread == TRUE)
				break;
			
			if((currentTime.GetMinute() % 15) != 0 || nTimeTemp == currentTime.GetMinute())
			{
				Sleep(200);
				continue;
			}
			nTimeTemp = currentTime.GetMinute();

			int nTagToalCount = GetTagTotalCountCheck();

			if(nTagToalCount == THREAD_END)
				break;
			else if(nTagToalCount == ERROR_DB_RECONNECTION)
			{
				Sleep(500);
				nTimeTemp =-1;
				continue;
			}
			else if(nTagToalCount != m_nTagListCount)
			{
				if(bStartCheck == FALSE)
				{
					strMsg.Format("������ TAG ��� : [%d]��",nTagToalCount);
					((CFormView_TagMapping *)(m_pCtrl))->OutputHistory(currentTime.Format("%Y-%m-%d %H:%M"),strMsg);
					bStartCheck = TRUE;
				}
				else
				{
					strMsg.Format("������ TAG ��� : [%d]��->%d�� ����",m_nTagListCount,nTagToalCount);
					((CFormView_TagMapping *)(m_pCtrl))->OutputHistory(currentTime.Format("%Y-%m-%d %H:%M"),strMsg);
				}

				int nRet = GetRegisterTagList();

				if(nRet == THREAD_END)
					break;
				else if(nRet == ERROR_DB_RECONNECTION)
				{
					Sleep(500);
					nTimeTemp =-1;
					continue;
				}

				nRet = GetRegisterTagInfoList();
				if(nRet == THREAD_END)
					break;
				else if(nRet == ERROR_DB_RECONNECTION)
				{
					Sleep(500);
					nTimeTemp =-1;
					continue;
				}
				m_nTagInfoListCheck = 1;
				m_nTagListCount = nTagToalCount;
			}

			if(nTagToalCount != 0)
			{
				int nRet = SetCreateXMLValue(currentTime,nTagToalCount,strXmlSavePath);
				if(nRet == THREAD_END)
					break;
				strMsg.Format("Tag ��� : [%d],ValueVo ���� �Ϸ�",m_nTagListCount);
				((CFormView_TagMapping *)(m_pCtrl))->OutputHistory(currentTime.Format("%Y-%m-%d %H:%M"),strMsg);
			}
			
			if(m_bEndThread == TRUE)
				break;
		}
		catch (...)
		{
			strMsg.Format("Run ������ ���� ����");
			SysLogOutPut(m_strLogTitle,strMsg,USER_COLOR_RED);
		}

	} while (!m_bEndThread);
	PostThreadMessage(WM_QUIT, 0, 0);
	return CWinThread::Run();
}

int CThread_XmlTagValue::GetTagTotalCountCheck()
{
	CString strMSGTitle = "GetTagTotalCountCheck";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;

	CString strQuery = "",strRunlog_E2 ="",strDBName = "";
	int nCountRow = 0;

	strQuery.Format("SELECT COUNT(*) as cnt FROM [ENERGY_MANAGE].[dbo].[TAG_MAPPING_LIST]");

	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				double dbValue;
				DB_Connect->GetFieldValue(pRs, "cnt", dbValue);
				nCountRow = (int)dbValue;
			}
			// ���ڵ尡 ������� DB ���� ���� �߻��Դϴ�.
			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}

			return nCountRow;
		}
	}
	catch (_com_error &e)
	{
		CString strErrorCode = Com_Error(strMSGTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMSGTitle);
			SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
			TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			return ERROR_DB_NO_TABLE;
		}
		else if(strErrorCode == "3113" || strErrorCode == "80004005")
		{
			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("%s - DB ���� ����!",strMSGTitle);
				SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
				TRACE("GetTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			}
			return ERROR_DB_RECONNECTION;
		}
		else
			return ERROR_DB_COM_ERROR;
	}
	catch (...)
	{
		if(pRs != NULL)
		{
			pRs->Close();
			pRs = NULL;
		}

		strRunlog_E2.Format("���� ���� Event Error : %s",strQuery);
		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
		return ERROR_DB_QUERY_FAIL1;
	}

	return 0;
}

int CThread_XmlTagValue::GetRegisterTagList()
{
	CString strMSGTitle = "GetRegisterTagList";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;

	CString strQuery = "",strRunlog_E2 ="";
	CString strhmiTagId,strTagId,strTagName,strTagDESC,strTagType,strDvid;
	int nCountRow = 0;
	
	strQuery.Format("SELECT HMI_TAG_ID "
					",DV_ID "
					",TAG_ID "
					",TAG_NAME "
					",TAG_DESC "
					",TAG_TYPE "
					"FROM [ENERGY_MANAGE].[dbo].[TAG_MAPPING_LIST]");
	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				m_stTagValueList->init(nCountRow);
				for(int nSTListCnt = 0; nSTListCnt < nCountRow ; nSTListCnt++)
				{
					DB_Connect->GetFieldValue(pRs, "HMI_TAG_ID", strhmiTagId);				//TAG id
					DB_Connect->GetFieldValue(pRs, "TAG_ID", strTagId);				//TAG id
					DB_Connect->GetFieldValue(pRs, "DV_ID", strDvid);		//TAG �׷��
					DB_Connect->GetFieldValue(pRs, "TAG_NAME", strTagName);			//TAG ��
					DB_Connect->GetFieldValue(pRs, "TAG_DESC", strTagDESC);			//TAG ����
					DB_Connect->GetFieldValue(pRs, "TAG_TYPE", strTagType);			//TAG Ÿ��

					ST_TAG_VALUE_LIST stTagInfo;
					memset(&stTagInfo,0x00,sizeof(ST_TAG_VALUE_LIST));
					strcpy_s(stTagInfo.szHmiTag_ID,strhmiTagId);
					strcpy_s(stTagInfo.szTAG_ID,strTagId);
					strcpy_s(stTagInfo.szDV_ID,strDvid);
					strcpy_s(stTagInfo.szTAG_NAME,strTagName);
					strcpy_s(stTagInfo.szTAG_DESC,strTagDESC);
					stTagInfo.nTAG_TYPE = atoi(strTagType);
					m_stTagValueList->SetDataAdd(stTagInfo);	
#ifdef _DEBUG
					TRACE("TagCnt = %d, Value = %s,%s,%s,%d\n",nSTListCnt,strTagId,strTagName,strDvid,atoi(strTagType));
#endif
					pRs->MoveNext();
				}
			}
			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}	
			return nCountRow;
		}
	}
	catch (_com_error &e)
	{
		CString strErrorCode = Com_Error(strMSGTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMSGTitle);
			SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
			TRACE("GetRegisterTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			return ERROR_DB_NO_TABLE;
		}
		else if(strErrorCode == "3113" || strErrorCode == "80004005")
		{
			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("%s - DB ���� ����!",strMSGTitle);
				SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
				TRACE("GetRegisterTagList()/catch com error - %s\n",strRunlog_E2);
#endif
			}
			return ERROR_DB_RECONNECTION;
		}
		else
			return ERROR_DB_COM_ERROR;
	}
	catch (...)
	{
		if(pRs != NULL)
		{
			pRs->Close();
			pRs = NULL;
		}
		strRunlog_E2.Format("SELECT ���� Event Error : %s",strMSGTitle);
		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

#ifdef _DEBUG
		TRACE("GetRegisterTagList()/catch - %s\n",strRunlog_E2);
#endif
		return ERROR_DB_QUERY_FAIL1;
	}
	return nCountRow;
}

int CThread_XmlTagValue::SetCreateXMLValue(CTime currentTime,int nTagCount,const char *szXmlSavePthe)
{
	CString strBuffer,strCurrentTime;
	strCurrentTime.Format("%s00",currentTime.Format("%Y%m%d%H%M"));
	
	CString strFileName;
	strFileName.Format("%s\\TagValueInfoVo_%s.xml",szXmlSavePthe,strCurrentTime);

	TiXmlDocument doc;

	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "yes" );  
	doc.LinkEndChild( decl );

	TiXmlElement * root = new TiXmlElement( "TagValueInfoVo" );  
	doc.LinkEndChild( root );

	TiXmlElement* pHeader = new TiXmlElement( "MsgInfoVo" );  
	root->LinkEndChild( pHeader ); 

	TiXmlElement* pItem = new TiXmlElement( "BEMS_ID" );  
	pHeader->LinkEndChild( pItem ); 
	pItem ->LinkEndChild( new TiXmlText("BEMS0017")); // Text�� �����ؼ� ��ũ�մϴ�.

	TiXmlElement* pItem2 = new TiXmlElement( "BLD_ID" );  
	pHeader->LinkEndChild( pItem2 ); 
	pItem2 ->LinkEndChild( new TiXmlText("B000000001")); // Text�� �����ؼ� ��ũ�մϴ�.

	TiXmlElement* pItem3 = new TiXmlElement( "SEND_DTM" );  
	pHeader->LinkEndChild( pItem3 );
	pItem3 ->LinkEndChild( new TiXmlText(strCurrentTime)); // Text�� �����ؼ� ��ũ�մϴ�.

	if(m_bEndThread == TRUE)
		return  THREAD_END;

	CString strValue;
	for(int nI = 0;nI < nTagCount; nI++)
	{
		if(m_bEndThread == TRUE)
			return  THREAD_END;
		ST_TAG_VALUE_LIST stTag = m_stTagValueList->GetData(nI);
		strValue = GetTagValue(stTag.szTAG_NAME,0);

		if(atof(strValue) != stTag.fCurrentValue)
		{
			stTag.fOldValue = stTag.fCurrentValue;
			stTag.fCurrentValue = (float)atof(strValue);
			m_stTagValueList->SetDataValueUpdate(nI,stTag);
		}		

		TiXmlElement* pValueRoot = new TiXmlElement( "ValueInfoVo" );  
		pHeader->LinkEndChild( pValueRoot ); 

		TiXmlElement* pDvIdItem = new TiXmlElement( "DV_ID" );  
		pValueRoot->LinkEndChild( pDvIdItem );
		pDvIdItem ->LinkEndChild( new TiXmlText(stTag.szDV_ID)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pTagIdItem = new TiXmlElement( "TAG_ID" );  
		pValueRoot->LinkEndChild( pTagIdItem );
		pTagIdItem ->LinkEndChild( new TiXmlText(stTag.szTAG_ID)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pValueItem = new TiXmlElement( "GATH_VALUE" );  
		pValueRoot->LinkEndChild( pValueItem );

		pValueItem ->LinkEndChild( new TiXmlText(strValue)); // Text�� �����ؼ� ��ũ�մϴ�.
	}
	
	doc.SaveFile(strFileName);
	doc.Clear(); 
	return 0;
}

CString CThread_XmlTagValue::GetTagValue(CString strTagName,int nTagTyp)
{
	CString strValue ="";
	int nRet = -1;
	ST_EV_TAG_INFO *pTagInfo = new ST_EV_TAG_INFO;

	nRet = EV_GetTagInfo(strTagName, pTagInfo);

	BYTE byValue; //DI
	double dValue; //AI

	if(TYPE_DI == pTagInfo->nTagType)
	{
		EV_GetDiData(pTagInfo->nStnPos , pTagInfo->nTagPos, &byValue); //�±� �׷� ��ġ, DI�±� ��ġ�� Data�� ����
		strValue.Format("%d",byValue);
	}
	else if(TYPE_AI == pTagInfo->nTagType)
	{
		EV_GetAiData(pTagInfo->nStnPos , pTagInfo->nTagPos, &dValue); //�±� �׷� ��ġ, AI�±� ��ġ�� Data�� ����
		strValue.Format("%0.4f",dValue);	
	}
	else
		strValue = "0";

	if(pTagInfo)
	{
		delete pTagInfo;
		pTagInfo=NULL;
	}
	if (m_bEndThread == TRUE)
		return "";

	return strValue;		
}


int CThread_XmlTagValue::GetRegisterTagInfoList()
{
	CString strMSGTitle = "GetRegisterTagInfoList";
	_RecordsetPtr pRs = NULL;
	_variant_t vTemp;
	

	CString strQuery = "",strRunlog_E2 ="";
	CString strHmiTagId,strDvId,strTagId,strNEW_TAG_TP_GRP_Code,strNEW_TAG_TP_Code;
	CString strNEW_MTAL_Code,strNEW_UNIT_Code,strNEW_LOC_FLR_Code,strNEW_DV_CLSF_01_Code;
	CString strNEW_DV_CLSF_02_Code,strNEW_DV_CLSF_03_Code,strNEW_DV_CLSF_LOC_Code;
	CString strNEW_VIRT_TAG_Code,strNEW_MEAU_CYCLE_Code,strNEW_MEAU_CYCLE_UNIT_Code,strBld_Bund;
	CString strTagName,strTagDESC,strTagType;
	int nCountRow = 0;

	strQuery.Format("SELECT HMI_TAG_ID "
					",DV_ID"
					",TAG_ID"
					",NEW_TAG_TP_GRP"
					",NEW_TAG_TP"
					",NEW_MTAL"
					",NEW_UNIT"
					",NEW_LOC_FLR"
					",NEW_DV_CLSF_01"
					",NEW_DV_CLSF_02"
					",NEW_DV_CLSF_03"
					",NEW_DV_CLSF_LOC"
					",NEW_VIRT_TAG"
					",NEW_MEAU_CYCLE"
					",NEW_MEAU_CYCLE_UNIT"
					",NEW_BLD_BUND"
					",TAG_NAME"
					",TAG_DESC"
					",HIGH_LIMIT_VALUE"
					",LOW_LIMIT_VALUE"
					",TAG_TYPE"
					" FROM [ENERGY_MANAGE].[dbo].[TAG_MAPPING_LIST]");
	try
	{
		pRs = DB_Connect->pADO_Connect->Execute((_bstr_t)strQuery, NULL, adOptionUnspecified);
		nCountRow  = pRs->RecordCount;

		if(pRs != NULL)
		{
			if(!pRs->GetEndOfFile())
			{
				m_pstAllTagInfoList->init(nCountRow);
				for(int nSTListCnt = 0; nSTListCnt < nCountRow ; nSTListCnt++)
				{
					DB_Connect->GetFieldValue(pRs, "HMI_TAG_ID", strHmiTagId);				//TAG id
					DB_Connect->GetFieldValue(pRs, "DV_ID", strDvId);			//TAG ��
					DB_Connect->GetFieldValue(pRs, "TAG_ID", strTagId);		//TAG �׷��
					DB_Connect->GetFieldValue(pRs, "NEW_TAG_TP_GRP", strNEW_TAG_TP_GRP_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_TAG_TP", strNEW_TAG_TP_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_MTAL", strNEW_MTAL_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_UNIT", strNEW_UNIT_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_LOC_FLR", strNEW_LOC_FLR_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_01", strNEW_DV_CLSF_01_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_02", strNEW_DV_CLSF_02_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_03", strNEW_DV_CLSF_03_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_DV_CLSF_LOC", strNEW_DV_CLSF_LOC_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_VIRT_TAG", strNEW_VIRT_TAG_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_MEAU_CYCLE", strNEW_MEAU_CYCLE_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_MEAU_CYCLE_UNIT", strNEW_MEAU_CYCLE_UNIT_Code);
					DB_Connect->GetFieldValue(pRs, "NEW_BLD_BUND", strBld_Bund);
					DB_Connect->GetFieldValue(pRs, "TAG_NAME", strTagName);			//TAG ��
					DB_Connect->GetFieldValue(pRs, "TAG_DESC", strTagDESC);			//TAG ����
					DB_Connect->GetFieldValue(pRs, "TAG_TYPE", strTagType);			//TAG Ÿ��

					ST_TAG_LIST stTagInfo;
					memset(&stTagInfo,0x00,sizeof(ST_TAG_LIST));
					strcpy_s(stTagInfo.szHmiTag_ID,strHmiTagId);
					strcpy_s(stTagInfo.szDV_ID,strDvId);
					strcpy_s(stTagInfo.szTAG_ID,strTagId);
					strcpy_s(stTagInfo.szNEW_TAG_TP_GRP_code,strNEW_TAG_TP_GRP_Code);
					strcpy_s(stTagInfo.szNEW_TAG_TP_code,strNEW_TAG_TP_Code);
					strcpy_s(stTagInfo.szNEW_MTAL_code,strNEW_MTAL_Code);
					strcpy_s(stTagInfo.szNEW_UNIT_code,strNEW_UNIT_Code);
					strcpy_s(stTagInfo.szNEW_LOC_FLR_code,strNEW_LOC_FLR_Code);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_01_code,strNEW_DV_CLSF_01_Code);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_02_code,strNEW_DV_CLSF_02_Code);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_03_code,strNEW_DV_CLSF_03_Code);
					strcpy_s(stTagInfo.szNEW_DV_CLSF_LOC_code,strNEW_DV_CLSF_LOC_Code);
					strcpy_s(stTagInfo.szNEW_VIRT_TAG_code,strNEW_VIRT_TAG_Code);
					strcpy_s(stTagInfo.szNEW_MEAU_CYCLE_code,strNEW_MEAU_CYCLE_Code);
					strcpy_s(stTagInfo.szNEW_MEAU_CYCLE_UNIT_code,strNEW_MEAU_CYCLE_UNIT_Code);
					strcpy_s(stTagInfo.szNEW_BLD_BUND_NM,strBld_Bund);
					strcpy_s(stTagInfo.szTAG_NAME,strTagName);
					strcpy_s(stTagInfo.szTAG_DESC,strTagDESC);					
					stTagInfo.nTAG_TYPE = atoi(strTagType);

					m_pstAllTagInfoList->SetDataAdd(stTagInfo);
#ifdef _DEBUG
					TRACE("TagCnt = %d, Value = %s,%s,%s,%d\n",nSTListCnt,strTagId,strTagName,strDvId,atoi(strTagType));
#endif
					pRs->MoveNext();
				}
			}
			if(pRs != NULL)
			{
				pRs->Close();
				pRs = NULL;
			}	
			return nCountRow;
		}
	}
	catch (_com_error &e)
	{
		CString strErrorCode = Com_Error(strMSGTitle,&e);

		if(strErrorCode == "942")
		{
			strRunlog_E2.Format("%s",strMSGTitle);
			SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
			TRACE("GetRegisterTagInfoList()/catch com error - %s\n",strRunlog_E2);
#endif
			return ERROR_DB_NO_TABLE;
		}
		else if(strErrorCode == "3113" || strErrorCode == "80004005")
		{
			int nResult = DB_Connect->DB_ReConnection();
			if(nResult == 0)
			{
				strRunlog_E2.Format("%s - DB ���� ����!",strMSGTitle);
				SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);
#ifdef _DEBUG
				TRACE("GetRegisterTagInfoList()/catch com error - %s\n",strRunlog_E2);
#endif
			}
			return ERROR_DB_RECONNECTION;
		}
		else
			return ERROR_DB_COM_ERROR;
	}
	catch (...)
	{
		if(pRs != NULL)
		{
			pRs->Close();
			pRs = NULL;
		}
		strRunlog_E2.Format("SELECT ���� Event Error : %s",strMSGTitle);
		SysLogOutPut(m_strLogTitle,strRunlog_E2,LOG_COLOR_RED);

#ifdef _DEBUG
		TRACE("GetRegisterTagInfoList()/catch - %s\n",strRunlog_E2);
#endif
		return ERROR_DB_QUERY_FAIL1;
	}
	return nCountRow;
}

int CThread_XmlTagValue::SetCreateXMLTagInfo(CTime currentTime,const char *szXmlSavePthe)
{
	CString strBuffer,strCurrentTime;
	strCurrentTime.Format("%s00",currentTime.Format("%Y%m%d%H%M"));
	CString strFileName;
	strFileName.Format("%s\\TagInfoVo.xml",szXmlSavePthe);
	
	int nTotalCount = m_pstAllTagInfoList->GetSize();
	
	TiXmlDocument doc;

	//TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "euc-kr", "" );  
	TiXmlDeclaration* decl = new TiXmlDeclaration(  "1.0", "utf-8", "yes" );  
	doc.LinkEndChild( decl );

	TiXmlElement * root = new TiXmlElement( "TagInfoVo" );  
	doc.LinkEndChild( root );

	TiXmlElement* pHeader = new TiXmlElement( "MsgInfoVo" );  
	root->LinkEndChild( pHeader ); 

	TiXmlElement* pItem = new TiXmlElement( "BEMS_ID" );  
	pHeader->LinkEndChild( pItem ); 
	pItem ->LinkEndChild( new TiXmlText("BEMS0017")); // Text�� �����ؼ� ��ũ�մϴ�.

	TiXmlElement* pItem2 = new TiXmlElement( "BLD_ID" );  
	pHeader->LinkEndChild( pItem2 ); 
	pItem2 ->LinkEndChild( new TiXmlText("B000000001")); // Text�� �����ؼ� ��ũ�մϴ�.

	TiXmlElement* pItem3 = new TiXmlElement( "SEND_DTM" );  
	pHeader->LinkEndChild( pItem3 );
	pItem3 ->LinkEndChild( new TiXmlText(strCurrentTime)); // Text�� �����ؼ� ��ũ�մϴ�.

	if(m_bEndThread == TRUE)
		return  THREAD_END;

	for(int nRow = 0; nRow < nTotalCount ; nRow++)
	//for(int nRow = 0; nRow < 1 ; nRow++)
	{
		if(m_bEndThread == TRUE)
			return  THREAD_END;

		ST_TAG_LIST stTagInfo = m_pstAllTagInfoList->GetData(nRow);
		
		TiXmlElement* pTagInfoRoot = new TiXmlElement( "AttributeInfoVo" );  
		pHeader->LinkEndChild( pTagInfoRoot ); 

		TiXmlElement* pDvIdItem = new TiXmlElement( "DV_ID" );  
		pTagInfoRoot->LinkEndChild( pDvIdItem );
		pDvIdItem ->LinkEndChild( new TiXmlText(stTagInfo.szDV_ID)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pTagIdItem = new TiXmlElement( "TAG_ID" );  
		pTagInfoRoot->LinkEndChild( pTagIdItem );
		pTagIdItem ->LinkEndChild( new TiXmlText(stTagInfo.szTAG_ID)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pTPGRPItem = new TiXmlElement( "NEW_TAG_TP_GRP" );  
		pTagInfoRoot->LinkEndChild( pTPGRPItem );
		pTPGRPItem ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_TAG_TP_GRP_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pTPItem = new TiXmlElement( "NEW_TAG_TP" );  
		pTagInfoRoot->LinkEndChild( pTPItem );
		pTPItem ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_TAG_TP_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pMTALItem = new TiXmlElement( "NEW_MTAL" );  
		pTagInfoRoot->LinkEndChild( pMTALItem );
		pMTALItem ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_MTAL_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pUNITItem = new TiXmlElement( "NEW_UNIT" );  
		pTagInfoRoot->LinkEndChild( pUNITItem );
		pUNITItem ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_UNIT_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pFLRItem = new TiXmlElement( "NEW_LOC_FLR" );  
		pTagInfoRoot->LinkEndChild( pFLRItem );
		pFLRItem ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_LOC_FLR_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pCLSF_01Item = new TiXmlElement( "NEW_DV_CLSF_01" );  
		pTagInfoRoot->LinkEndChild( pCLSF_01Item );
		pCLSF_01Item ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_DV_CLSF_02_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pCLSF_02Item = new TiXmlElement( "NEW_DV_CLSF_02" );  
		pTagInfoRoot->LinkEndChild( pCLSF_02Item );
		pCLSF_02Item ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_DV_CLSF_02_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pCLSF_03Item = new TiXmlElement( "NEW_DV_CLSF_03" );  
		pTagInfoRoot->LinkEndChild( pCLSF_03Item );
		pCLSF_03Item ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_DV_CLSF_03_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pCLSF_LOCItem = new TiXmlElement( "NEW_DV_CLSF_LOC" );  
		pTagInfoRoot->LinkEndChild( pCLSF_LOCItem );
		pCLSF_LOCItem ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_DV_CLSF_LOC_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pVITRItem = new TiXmlElement( "NEW_VIRT_TAG" );  
		pTagInfoRoot->LinkEndChild( pVITRItem );
		pVITRItem ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_VIRT_TAG_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pCYCLEItem = new TiXmlElement( "NEW_MEAU_CYCLE" );  
		pTagInfoRoot->LinkEndChild( pCYCLEItem );
		pCYCLEItem ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_MEAU_CYCLE_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pCYCLE_UNITItem = new TiXmlElement( "NEW_MEAU_CYCLE_UNIT" );  
		pTagInfoRoot->LinkEndChild( pCYCLE_UNITItem );
		pCYCLE_UNITItem ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_MEAU_CYCLE_UNIT_code)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pBUNDTItem = new TiXmlElement( "NEW_BLD_BUND" );  
		pTagInfoRoot->LinkEndChild( pBUNDTItem );
		pBUNDTItem ->LinkEndChild( new TiXmlText(stTagInfo.szNEW_BLD_BUND_NM)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pTAG_NMItem = new TiXmlElement( "TAG_NM" );  
		pTagInfoRoot->LinkEndChild( pTAG_NMItem );
		UTF8_CONVERSION;
		LPSTR utf8_1 = T2UTF8(stTagInfo.szTAG_NAME);

		pTAG_NMItem ->LinkEndChild( new TiXmlText(utf8_1)); // Text�� �����ؼ� ��ũ�մϴ�.
		

		TiXmlElement* pTAG_DESCItem = new TiXmlElement( "TAG_DESC" );  
		pTagInfoRoot->LinkEndChild( pTAG_DESCItem );

		LPSTR utf8_2 = T2UTF8(stTagInfo.szTAG_DESC);
		pTAG_DESCItem ->LinkEndChild( new TiXmlText(utf8_2)); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pH_LIMIT_VALUEItem = new TiXmlElement( "HIGH_LIMIT_VALUE" );  
		pTagInfoRoot->LinkEndChild( pH_LIMIT_VALUEItem );
		pH_LIMIT_VALUEItem ->LinkEndChild( new TiXmlText("")); // Text�� �����ؼ� ��ũ�մϴ�.

		TiXmlElement* pL_LIMIT_VALUEItem = new TiXmlElement( "LOW_LIMIT_VALUE" );  
		pTagInfoRoot->LinkEndChild( pL_LIMIT_VALUEItem );
		pL_LIMIT_VALUEItem ->LinkEndChild( new TiXmlText("")); // Text�� �����ؼ� ��ũ�մϴ�.
	}

	doc.SaveFile(strFileName);
	doc.Clear(); 

	return 0;
}
