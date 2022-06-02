#include "StdAfx.h"
#include "cAttribInfo.h"

cAttribInfo::cAttribInfo(void)
: m_Status	(STATUS_NOT_INITIALIZED)
, m_OrgGUID	(NULL_GUID)
, m_OrgUSN	(NULL_USN)
, m_DateChange(NULL_DATE)
, m_Version	(NULL_VERSION)
, m_Name(NULL_NAME)

{
		//dbg.trace(L"cAttribInfo::cAttribInfo",L"Creating %d ");
}

int cAttribInfo::BuildGUIDString(CString *strGUID, GUID* pGuid)
{
	//RPC_WSTR* pszChar=NULL;	
	PWCHAR pszChar=NULL;	
	if (UuidToString(pGuid,(RPC_WSTR*)&pszChar)==RPC_S_OK)
		strGUID->Format(L"%wS",pszChar);
	else
		strGUID->Format(NULL_GUID);

	RpcStringFree((RPC_WSTR*)&pszChar);
	
	
	return 0;
}

cAttribInfo::~cAttribInfo(void)
{
	//dbg.trace(L"cAttribInfo::~cAttribInfo",L"Destroying %wS",Name());
}



CString cAttribInfo::OrgGUID(void)
{
	return m_OrgGUID;
}

CString cAttribInfo::OrgUSN(void)
{
	return m_OrgUSN;
}

CString cAttribInfo::DateChange(void)
{
	return m_DateChange;
}

CString cAttribInfo::Version(void)
{
	return m_Version;
}

CString cAttribInfo::Name(void)
{
	return m_Name;
}

void cAttribInfo::setOrgGUID(CString s)
{
	m_OrgGUID =s;
}

void cAttribInfo::setVersion(CString s)
{
	m_Version = s;
}

void cAttribInfo::setDateChange(CString s)
{
	m_DateChange = s;
}

void cAttribInfo::setOrgUSN(CString s)
{
	m_OrgUSN = s;
}

void cAttribInfo::setName(CString s)
{
	m_Name = s;
}

// saves metadata info to object
int cAttribInfo::setAttribInfo(DS_REPL_ATTR_META_DATA  data)
{
	CTime tDate;
	//GUID
	BuildGUIDString(&m_OrgGUID,&(data.uuidLastOriginatingDsaInvocationID));
	//Version
	m_Version.Format(L"%d",data.dwVersion);
	//USN
	m_OrgUSN.Format(L"%d",data.usnOriginatingChange);
	//AttrName
	m_Name.Format(L"%wS",data.pszAttributeName);
	
	//Org Date & Time
	if (tDate.IsValidFILETIME(data.ftimeLastOriginatingChange))
	{
		tDate=data.ftimeLastOriginatingChange;
		m_DateChange = tDate.Format(L"%Y-%m-%d %H:%M:%S");		
	}
	else
		m_DateChange = NULL_DATE;

		
	return 0;
}


void cAttribInfo::SetStatus(int status)
{
	m_Status = status;
}

// return status TAG
CString cAttribInfo::GetStatusTag(void)
{

//	if (g_FLAGS & FLAG_VERBOSE || g_FLAGS & FLAG_CSV)
	if (g_FLAGS & FLAG_CSV)
	{
		switch (m_Status)
		{
		case STATUS_EQUAL :		
			return L"STATUS_EQUAL";
			break;
		case STATUS_NOT_FOUND:
			return L"STATUS_NOT_FOUND";
			break;
		case STATUS_NOT_FOUND_IN_REF_DC:
			//probably will never be set
			return L"STATUS_NOT_FOUND_IN_REF_DC";
			break;
		case STATUS_NOT_EQUAL:
			return L"STATUS_NOT_EQUAL";
			break;
		case STATUS_NOT_INITIALIZED:
			//if a attrib status han't been initialized meen that ref server doesn't has this attribute.
			return L"STATUS_NOT_INITIALIZED";
			break;		
		case STATUS_REF_INFO:
			//this information is reference information
			return L"STATUS_REF_INFO";
			break;	

		default:
			return L"ERROR"; //we shouldn't never reach this condition
			break;
		}
	}
	else{
		switch (m_Status)
		{
		case STATUS_EQUAL :		
			return L"=";
			break;
		case STATUS_NOT_FOUND:
			return L"-";
			break;
		case STATUS_NOT_FOUND_IN_REF_DC:
			//probably will never be set
			return L">";
			break;
		case STATUS_NOT_EQUAL:
			return L"X";
			break;
		case STATUS_NOT_INITIALIZED:
			//if a attrib status han't been initialized meen that ref server doesn't has this attribute.
			return L"+";
			break;
		case STATUS_REF_INFO:
			//this information is reference information
			return L"#";
			break;

		default:
			return L"ERROR"; //we shouldn't never reach this condition
			break;
		}
	}

	//return CString();
}
