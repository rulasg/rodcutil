#include "StdAfx.h"
//#include <ntdsapi.h>
//#include "ntdsapi.h"
#include <windows.h>
#include <rpc.h>
#include "helper.h"
//#include "cParameters.h"
//#include "cdbghelper.h"
#include "cAttribInfo.h"
#include "cSrvInfo.h"

cSrvInfo::cSrvInfo(void)
: m_AttribCount(0)
, m_SrvName   (_T(""))
, m_DsaGUID   (_T(""))
, m_ObjectDN  (_T(""))
,m_pAttribInfoList (NULL)
, m_Status (STATUS_NOT_INITIALIZED)
, m_findCacheID(0)
{
	dbg.trace(L"cSrvInfo::cSrvInfo",L"Creating Sever info Object");
		
}

cSrvInfo::~cSrvInfo(void)
{

	dbg.trace(L"cSrvInfo::~cSrvInfo",L"Destroy obj for %wS",m_SrvName);
	
	if (m_pAttribInfoList)
		delete m_pAttribInfoList;

	//if (m_pReplMetaData) DsReplicaFreeInfo(DS_REPL_INFO_METADATA_FOR_OBJ,m_pReplMetaData);

}

// initialize srvInfo Object loading SrvName information
int cSrvInfo::Initialize(CString SrvName)
{
	m_SrvName = SrvName;
	dbg.trace(L"cSrvInfo::Initialize",L"Init obj for %wS",m_SrvName);
	return 0;
}

// triggers to update replication information with metadata for objStr object
int cSrvInfo::GetObjMetadata(CString objStr)
{
	//cDbgHelper dbg;
	HANDLE hDS = NULL;
	RPC_AUTH_IDENTITY_HANDLE cred;
	//DS_REPL_INFO_METADATA_FOR_OBJ* pReplMetaData;
	DS_REPL_OBJ_META_DATA * pReplMetaData=NULL;
	cAttribInfo * pAttribInfo = NULL;

	m_ObjectDN=objStr;
	
	int retI=0;
	
	//Paramter check
	if (m_SrvName.IsEmpty()|| m_ObjectDN.IsEmpty())
	{
		dbg.print(L"cSrvInfo::GetObjMetadata",L"Wrong function parameters" );
		g_GLE = ERR_WRONG_PARAMETERS;
		goto error;	
	}	

	//Clean information if exists and create Array structure
	if (m_pAttribInfoList)
		delete m_pAttribInfoList;
	m_pAttribInfoList = new CArray<cAttribInfo>;

	//Bind
	dbg.print(L"cSrvInfo::GetObjMetadata",L"Bind and reading metadata from %wS ...",m_SrvName);

	if (g_Cred.Exist())		
	{	
		dbg.trace(L"cSrvInfo::GetObjMetadata",L"Binding with specific credentials [ %wS\\%wS ]",
			g_Cred.GetDomain(),g_Cred.GetUser());
		//DsMakePasswordCredentials(L"ldapuser",L"ra03dom.com",L"ldap",&cred);
		DsMakePasswordCredentials(g_Cred.GetUser(),g_Cred.GetDomain(),g_Cred.GetPassword(),&cred);
		retI= DsBindWithCred(m_SrvName,NULL,cred,&hDS);
	}
	else 
	{
		dbg.trace(L"cSrvInfo::GetObjMetadata",L"Binding with NULL credentials");
		retI=DsBind(m_SrvName,NULL,&hDS);
	}

	//checking binding errors
	if (retI == ERROR_SUCCESS )
		dbg.trace(L"cSrvInfo::GetObjMetadata",L"...Bind SUCCESS");
	else 
	{
		g_GLE = retI;
		dbg.error(L"cSrvInfo::GetObjMetadata",L"Error binding [retI  %d]",retI); 
		SetStatus(STATUS_ERROR);
		goto error;
	}	

	//Retreive information
	dbg.trace(L"cSrvInfo::GetObjMetadata",L"Retreiving ReplInformation ....");
	
	retI = DsReplicaGetInfo(hDS,DS_REPL_INFO_METADATA_FOR_OBJ,m_ObjectDN,NULL,(void**)&pReplMetaData);

	if (retI == ERROR_SUCCESS)
	{		
		dbg.trace(L"cSrvInfo::GetObjMetadata",L"Succesfully retreived [ %d ] entries",pReplMetaData->cNumEntries);
				
		//Save data to object
		m_AttribCount = pReplMetaData->cNumEntries;

		for (int i =0; i < pReplMetaData->cNumEntries; i++)
		{	
/*			
			pAttribInfo = new cAttribInfo;						
			pAttribInfo->setAttribInfo(pReplMetaData->rgMetaData[i]);
			m_pAttribInfoList->Add(*pAttribInfo);
*/
			m_pAttribInfoList->GetAt(AddAttrib()).setAttribInfo(pReplMetaData->rgMetaData[i]);

		//	m_pAttribInfoList->GetAt(m_pAttribInfoList->Add(*(new cAttribInfo))).setAttribInfo(pReplMetaData->rgMetaData[i]);
			
		}
		
		//tag all attributes to tag they are reference info.
		if (GetStatus()==STATUS_REF_INFO)
		{
			for (int i =0; i < pReplMetaData->cNumEntries; i++)
			{
				m_pAttribInfoList->GetAt(i).SetStatus(STATUS_REF_INFO);

			}		
		}
	}
	else
	{
		dbg.trace(L"cSrvInfo::GetObjMetadata",L"Error retreiving object metadata");
		g_GLE=retI;
		SetStatus(STATUS_ERROR);		
		goto error;
	}

	//UnBind
	if (hDS)
	{
		dbg.trace(L"cSrvInfo::GetObjMetadata",L"UnBinding...");
		DsUnBind(&hDS);
	}
	
	SetStatus(STATUS_INITIALIZED);
	return ERR_SUCCESS;
error:
	//set obj status 
	//if (g_GLE==ERROR_DS_OBJ_NOT_FOUND)
	if (g_GLE==8333)
	{
		SetStatus(STATUS_NOT_FOUND);
	}
	else
	{
		SetStatus(STATUS_ERROR);		
	}
	
	//cleanup connection
	if (hDS)
	{
		dbg.trace(L"cSrvInfo::GetObjMetadata",L"UnBinding after error...");
		DsUnBind(&hDS);
	}
	
	//cleanup possible metadata information 
	if (pReplMetaData)
		DsReplicaFreeInfo(DS_REPL_INFO_METADATA_FOR_OBJ,pReplMetaData);

	return g_GLE;
}

// returns SrvName 
CString cSrvInfo::GetSrvName(void)
{
	 return m_SrvName;
	//return CString();
}

// Display actual metadata for that server
int cSrvInfo::DisplayObjMetaData(void)
{	

	DisplayAttribStatusHeader();

/*	
	//for (unsigned int i = 0; i < m_pReplMetaData->cNumEntries; i++)
	for (unsigned int i = 0; i < GetAttribCount(); i++)
	{
		DisplayAttribMetaData(i);			
	}	
	*/
	DisplayAttribStatus();

	dbg.print(L"cSrvInfo::DisplayObjMetaData",L"",NULL);

	return 0;
}

/* format a UUID into human GUID format */
int cSrvInfo::BuildGUIDString(CString *strGUID, GUID* pGuid)
{
	//RPC_WSTR* pszChar=NULL;	
	PWCHAR pszChar=NULL;	
	if (UuidToString(pGuid,(RPC_WSTR*)&pszChar)==RPC_S_OK)
		strGUID->Format(L"%wS",pszChar);
	else
		//strGUID->Format(L"00000000-0000-0000-0000-000000000000");
		strGUID->Format(NULL_GUID);	

	RpcStringFree((RPC_WSTR*)&pszChar);
	
	
	return 0;
}

/*
// returns pointer to a attribute metadata strucutre
DS_REPL_ATTR_META_DATA * cSrvInfo::GetAttribReplInfo(CString AttribName)
{
	int i =0;
	if (m_pReplMetaData==NULL)
	{
		g_GLE = ERR_STRUCT_NOT_INIT;
		return NULL;
	}
	for ( i = 0 ; 
		((i < (int)m_pReplMetaData->cNumEntries) && (m_pReplMetaData->rgMetaData[i].pszAttributeName!= AttribName));
		i++);
	
	if (i==m_pReplMetaData->cNumEntries )
		return NULL;
	else 
		return &(m_pReplMetaData->rgMetaData[i]);
}
*/

// return number of attib in metadata
int cSrvInfo::GetAttribCount(void)
{
	//return m_AttribCount;
	if (m_pAttribInfoList)
		return m_pAttribInfoList->GetCount();

	return 0;
}


// returns the string usedfor attribute comparation
CString cSrvInfo::GetAttribMetadataString(CString attrName)
{
	CString a2;
	int i;
	//build format string. Same as the used in DisplayObjMetaData
	//aux.Format(format,L"OrgGUID",L"OrgUSN",L"Date",L"Ver",L"AttribName");	
	//WCHAR format[]=L" %-36wS  %-7wS  %-19wS  %5wS  %-wS";	

	i=findAttr(attrName);

/*	if (i==ERR_NULL)
	{
		a2.Empty();
		return a2;
	}*/

	//Build string;	

	if (g_FLAGS & FLAG_CSV)
		a2.Format(METADATA_CSV_FORMAT,GetAttribGUID(i),GetAttribUSN(i),GetAttribDate(i),GetAttribVersion(i),attrName);
	else
		a2.Format(METADATA_FORMAT_OUTPUT,GetAttribGUID(i),GetAttribUSN(i),GetAttribDate(i),GetAttribVersion(i),attrName);

	return a2;	
}



// returns the string usedfor attribute comparation
CString cSrvInfo::GetAttribMetadataString(int i)
{
	//dbg.trace(L"testtesttest",L"GetAttribMetadataString for %wS ",m_pReplMetaData->rgMetaData[i].pszAttributeName);
	CString a2;

	//Build string;	

	if (g_FLAGS & FLAG_CSV)
		a2.Format(METADATA_CSV_FORMAT,GetAttribGUID(i),GetAttribUSN(i),GetAttribDate(i),GetAttribVersion(i),GetAttribName(i));
	else
		a2.Format(METADATA_FORMAT_OUTPUT,GetAttribGUID(i),GetAttribUSN(i),GetAttribDate(i),GetAttribVersion(i),GetAttribName(i));
	

	
	return a2;
}

// returns m_isEqual value
bool cSrvInfo::isEqual(void)
{
	if (GetStatus() == STATUS_EQUAL)
		return true;
	else
		return false;
}


// //return the value of m_Status
int cSrvInfo::GetStatus(void)
{
	return m_Status;	
}

// sets object status
void cSrvInfo::SetStatus(int status)
{	
	if (m_Status != STATUS_NOT_FOUND)
	{
		dbg.trace(L"cSrvInfo::SetStatus",L"Setting status %d to server %wS",status,GetSrvName());
		m_Status=status;
	}

}

// return the name of the attrib at that position
CString cSrvInfo::GetAttribName(int i)
{
	CString a3;
	a3= L"NULL";

	if (i !=ERR_NULL &&  i < GetAttribCount())
		//a3=m_pReplMetaData->rgMetaData[i].pszAttributeName;
		a3=m_pAttribInfoList->GetAt(i).Name();
	else
		a3= L"NULL";


	return a3;
}

// return the lanth of the longest attribute name
int cSrvInfo::GetMaxAttrNameLength(void)
{
	int i1=0;
	int i2 =0;
	CString strAux;

	for (int i = 0 ; i < m_AttribCount ; i++)
	{
		//strAux = m_pReplMetaData->rgMetaData[i].pszAttributeName;
		strAux = m_pAttribInfoList->GetAt(i).Name();
		i2=strAux.GetLength();
		if (i2> i1)
			i1 = i2;	
	}
		
	return i1;
}

// set GUID value to the server
void cSrvInfo::SetDsaGUID(CString guid)
{
	m_DsaGUID=guid;	
}

//return position if metadata for that attribute exists, ERR_NULL if not present
int cSrvInfo::findAttr(CString attrName)
{
	CString strAux;
	int auxI;
	
	//check for the attrib list initialization
	if (m_pAttribInfoList == NULL || m_pAttribInfoList->GetCount() == 0)
	{
		dbg.trace(L"cSrvInfo::findAttr",L"Attrib list is empty");
		//add string to trace the not found attributes
		AddAttrib(attrName);
		return ERR_NULL;
	}

	//Could add the cache to ERR_NULL restult searchs

	//first try cache
	if (attrName == m_pAttribInfoList->GetAt(m_findCacheID).Name())
	{
		//dbg.trace(L"cSrvInfo::findAttr",L"Hitting cache with [%wS]",attrName);
		//have to check if is a extra attribute
		if (GetAttribStatus(m_findCacheID) != STATUS_NOT_FOUND)
			return m_findCacheID;
		else
			//found in cached as not found
			return ERR_NULL;
	}

	//dbg.trace(L"cSrvInfo::findAttr",L"Looking on list for [%wS]",attrName);
	//Looking in the list
	for (int i = 0 ; i < this->GetAttribCount(); i++)
	{
		//strAux = m_pReplMetaData->rgMetaData[i].pszAttributeName;
		strAux = m_pAttribInfoList->GetAt(i).Name();
		if (strAux == attrName)
		{					
			//update cache
			m_findCacheID = i;		

			//check if its a added attribute
			if (GetAttribStatus(i) != STATUS_NOT_FOUND)
				return i;
			else
				return ERR_NULL;
		}
	}

	//Not found.
	//Adding elements that where not present in server 
	//dbg.trace(L"cSrvInfo::findAttr",L"Adding extra attribute");
	auxI = AddAttrib(attrName);
	//m_pAttribInfoList->GetAt(auxI).setName(attrName);
	//m_pAttribInfoList->GetAt(auxI).SetStatus(STATUS_NOT_FOUND);

		
	return ERR_NULL;

}


// return attribute date
CString cSrvInfo::GetAttribDate(int i)
{
	CTime tDate;
	CString date;

	//initialize return variable
	//date = L"0000-00-00 00:00:00";
	date = L"(never)";

	if (i !=ERR_NULL &&  i < GetAttribCount())
	{		
//		if (tDate.IsValidFILETIME(m_pReplMetaData->rgMetaData[i].ftimeLastOriginatingChange))
//		{
			/*
			tDate=m_pReplMetaData->rgMetaData[i].ftimeLastOriginatingChange;
			date = tDate.Format(L"%Y-%m-%d %H:%M:%S");
			*/
	
		date=m_pAttribInfoList->GetAt(i).DateChange();
//		}
	}
	
	return date;
}

// return the attrib originator guid
CString cSrvInfo::GetAttribGUID(int i)
{
	
	CString a1;	
	a1= NULL_GUID;
	
	if (i !=ERR_NULL &&  i < GetAttribCount())
		//BuildGUIDString(&a1,& m_pReplMetaData->rgMetaData[i].uuidLastOriginatingDsaInvocationID);
		a1 = m_pAttribInfoList->GetAt(i).OrgGUID();
	
	return a1;
	
}

// return the attrib version metadata
CString cSrvInfo::GetAttribVersion(int i)
{
	CString ver;
	ver = NULL_VERSION;
	
	if (i !=ERR_NULL &&  i < GetAttribCount())
		//ver.Format(L"%d",m_pReplMetaData->rgMetaData[i].dwVersion);
		ver = m_pAttribInfoList->GetAt(i).Version();
	
	return ver;
}

// return the attrib metadata USN
CString cSrvInfo::GetAttribUSN(int i)
{
	CString usn;
	usn = NULL_USN;

	if (i !=ERR_NULL &&  i < GetAttribCount())
		//usn.Format(L"%d",m_pReplMetaData->rgMetaData[i].usnOriginatingChange);
		usn = m_pAttribInfoList->GetAt(i).OrgUSN();
	return usn;
}


// displays Attrib metadata prefixing preString to the output
int cSrvInfo::DisplayAttribMetaData(int i,bool isTrace, CString preString)
{
	return DisplayAttribMetaDataString(GetAttribMetadataString(i),isTrace,preString);
}

int cSrvInfo::DisplayAttribMetaData(CString AttribName,bool isTrace, CString preString)
{
	return DisplayAttribMetaDataString(GetAttribMetadataString(AttribName),isTrace,preString);
}

int cSrvInfo::DisplayAttribMetaDataString(CString AttribString,bool isTrace, CString preString )
{	
	CString a1;
	CString a2;
	
	//check if the CSV flag to create a CSV info output string
	if (g_FLAGS & FLAG_CSV)
	{
		//build the rest of the string
		if (preString.IsEmpty())
			a2.Format(L"%wS,\"%wS\",%wS",GetSrvName(),GetObjectDN(),AttribString);		
		else
			a2.Format(L"[ %wS ],\"%wS\",%wS,%wS",preString,GetSrvName(),GetObjectDN(),AttribString);		
		
		//print in quite mode also
		dbg.dataPrint(L"cSrvInfo::DisplayAttribMetaDataString",a2);
	}
	else
	{
		if (!preString.IsEmpty())			
			a2.Format(L"%wS %wS",preString,a1);
		
		a2+= AttribString;

		if (isTrace)
			dbg.trace(L"cSrvInfo::DisplayAttribMetaData",a2);
		else
			dbg.print (L"cSrvInfo::DisplayAttribMetaData",a2);
	}

	return 0;

	
}
// return object DN string
CString cSrvInfo::GetObjectDN(void)
{
	return m_ObjectDN;
}

// print the server status
int cSrvInfo::DisplayServerStatus(void)
{
	CString a3;

	int objstatus = GetStatus();
	dbg.trace (L"cSrvInfo::DisplayServerStatus",L"Servers status %d  on %wS",objstatus,GetSrvName());

	switch (objstatus)
	{
	case STATUS_EQUAL :
		a3= L"==";
		break;
	case STATUS_NOT_EQUAL :
		a3= L"!=";
		break;
	case STATUS_NULL :	
		a3=L"??";
		break;
	case STATUS_ERROR : 
		a3=L"ER";
		break;
	case STATUS_NOT_FOUND :
		a3=L"00";
		break;
	default :			
		a3=L"ERROR"; // We shouldn't ever get to this point
	}		

	if (g_FLAGS & FLAG_CSV)
	{
		//no output on csv. only attrib status output.
		//dbg.dataPrint(L"cSrvInfo::DisplayServerStatus",L"%wS,%wS",a3,GetSrvName());
	}
	else 
		dbg.dataPrint(L"cSrvInfo::DisplayServerStatus",L"[ %wS ] %wS",a3,GetSrvName());

	
	return 0;
}

// sets comp attrib status
int cSrvInfo::SetAttribStatus(CString AttribName, int Status)
{

	if (Status == STATUS_NOT_FOUND)
	// this means that the attribute was not found in the list.
	//should create a attrib entry with this status or a reference list
	{
		//Add element to the attrib list with data on Null.
			
	}	
	else
		m_pAttribInfoList->GetAt(findAttr(AttribName)).SetStatus(Status);
	
	
	return 0;
}

// adds elemento to the attrib list
int cSrvInfo::AddAttrib(void)
{
	//dbg.trace(L"cSrvInfo::AddAttrib",L"Addding attrib to the list for server [%wS]",GetSrvName());
	cAttribInfo * pAttribInfo = new cAttribInfo;
	return m_pAttribInfoList->Add(*pAttribInfo);
}

int cSrvInfo::AddAttrib(CString attribName)
{
	dbg.trace(L"cSrvInfo::AddAttrib",L"Adding attrib [%wS] to server [%wS] ",attribName,GetSrvName());
	cAttribInfo * pAttribInfo = new cAttribInfo;
	pAttribInfo->setName(attribName);
	pAttribInfo->SetStatus(STATUS_NOT_FOUND);
	return m_pAttribInfoList->Add(*pAttribInfo);
}

cAttribInfo cSrvInfo::GetAttrib(int i)
{
	return m_pAttribInfoList->GetAt(i);

}

// print header for Attribstats output.
int cSrvInfo::DisplayAttribStatusHeader(void)
{
	CString aux;
	CString a1;
	

	if (g_FLAGS & FLAG_CSV)
	{
		aux.Format(METADATA_CSV_FORMAT,L"OrgGUID",L"OrgUSN",L"Date",L"Ver",L"AttribName");		
		a1.Format(L"%wS,%wS,%wS,%wS",
			L"ServerName",
			L"ObjectDN",
			L"AttribCompStatus",
			aux);
		dbg.dataPrint(L"cSrvInfo::DisplayAttribStatus",L"%wS",a1);			 
	}
	else
	{
		/*
			a1 = m_pAttribInfoList->GetAt(i).GetStatusTag();
			//if we change this condition make sure that STATUS_REF_INFO gets printed and not traced.
			if (GetAttribStatus(i) != STATUS_EQUAL )
				dbg.print(L"cSrvInfo::DisplayAttribStatus",	L"[ %wS ] %wS",	a1,	
				GetAttribMetadataString(GetAttribName(i)));
		
		*/
		aux.Format(METADATA_FORMAT_OUTPUT,L"OrgGUID",L"OrgUSN",L"Date",L"Ver",L"AttribName");		

		int MaxLengthName = GetMaxAttrNameLength();
		int MaxRegLengh = 5 + 7 + 19 + 36 + MaxLengthName + 12;
		CString bar;
		bar.Empty();

		dbg.print(L"cSrvInfo::DisplayAttribStatusHeader",L"",NULL);
		dbg.print(L"cSrvInfo::DisplayAttribStatusHeader",L"Object : [%wS]",m_ObjectDN);
		dbg.print(L"cSrvInfo::DisplayAttribStatusHeader",L"",NULL);
		
		//Print header
		dbg.print(L"cSrvInfo::DisplayAttribStatusHeader",L"Stat  %wS",aux);	
		//Create the menu bar
		for (int i = 0 ; i < MaxRegLengh ; i++) bar+=L"=";
		dbg.print(L"cSrvInfo::DisplayAttribStatusHeader",METADATA_FORMAT_HEADER,bar);
	}
	return 0;
}

// print the attrib status based on outputs switches
//if you cahnge this function output format sync DisplayAttribStatusHeader to match format
int cSrvInfo::DisplayAttribStatus(void)
{
	//if CSV print all.
	//if Q don't print
	//if Q and CSV print.
	//Create comp status string	
	
	CString a1;
	a1.Empty();

	/*
	switch (objstatus)
	{
	case STATUS_EQUAL :
		a3= L"==";
		break;
	case STATUS_NOT_EQUAL :
		a3= L"!=";
		break;
	case STATUS_NULL :	
		a3=L"??";
		break;
	case STATUS_ERROR : 
		a3=L"ER";
		break;
	case STATUS_NOT_FOUND :
		a3=L"00";
		break;
	default :			
		a3=L"ERROR"; // We shouldn't ever get to this point
	*/
	//check srv status
	if (GetStatus() == STATUS_ERROR || GetStatus() == STATUS_NOT_FOUND)
	{
		//on CSV we have to log the error on attrib output format
		dbg.trace(L"cSrvInfo::DisplayAttribStatus",L"Adding _SERVER_IN_ERROR_STATUS Attribute");
		GetAttribMetadataString(L"_SERVER_IN_ERROR_STATUS");
	}
	dbg.trace(L"cSrvInfo::DisplayAttribStatus",L"Listing [ %d ] attributes",m_pAttribInfoList->GetCount());

	for (int i = 0 ; i <  m_pAttribInfoList->GetCount(); i++)
	{
		//a1 = m_pAttribInfoList->GetAt(i).GetStatusTag();
		//dbg.trace(L"cSrvInfo::DisplayAttribStatus",	L"[ %wS ] %wS",	a1,	GetAttribMetadataString(GetAttribName(i)));
	 
		if (g_FLAGS & FLAG_CSV)
		{
			a1.Format(L"%wS,\"%wS\",%wS,%wS",
				GetSrvName(),
				GetObjectDN(),
				m_pAttribInfoList->GetAt(i).GetStatusTag(),
				GetAttribMetadataString(GetAttribName(i)));

			dbg.dataPrint(L"cSrvInfo::DisplayAttribStatus",L"%wS",a1);			 
		}
		else
		{	
			a1 = m_pAttribInfoList->GetAt(i).GetStatusTag();
			//if we change this condition make sure that STATUS_REF_INFO gets printed and not traced.
			if (GetAttribStatus(i) != STATUS_EQUAL )
				dbg.print(L"cSrvInfo::DisplayAttribStatus",	L"[ %wS ] %wS",	a1,	
				GetAttribMetadataString(GetAttribName(i)));
			else
				dbg.trace(L"cSrvInfo::DisplayAttribStatus",	L"[ %wS ] %wS",	a1,	
				GetAttribMetadataString(GetAttribName(i)));
		}
	}

	return 0;
}

// return attrib status
int cSrvInfo::GetAttribStatus(int i)
{
	return m_pAttribInfoList->GetAt(i).m_Status;
}


