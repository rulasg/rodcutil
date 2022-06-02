#include "StdAfx.h"
#include "cDcList.h"
#include "cDsaGuidChk.h"

cDsaGuidChk::cDsaGuidChk(void)
: m_Guid(_T(""))
, m_DcName(_T(""))
, m_Domain(_T(""))
, m_RefDC(_T(""))
, m_isFound(STATUS_NOT_INITIALIZED)
{
}

cDsaGuidChk::~cDsaGuidChk(void)
{

}

// will set all members
int cDsaGuidChk::Initialize(cParameters* param)
{
	//int paramCount = 2;	
	//dsaguidchk <guid> <ReferenceDCName>  [<domain>]
	// 0            1               2           3
	dbg.trace(L"cDsaGuidChk::Initialize",L"Initialization of parameters");
	
	//check mandatory parametes
	//reading reference server
	if (param->GetCount()< 3)
		return ERR_WRONG_PARAMETERS;
	else
	{
		m_Guid= param->GetParam(1);
		m_RefDC=param->GetParam(2);

		//initialize DcName with the default value of the GUID
		m_DcName=m_Guid; 
	}
	
	//reading domain
	if (param->GetCount() < 4)		
		m_Domain.GetEnvironmentVariableW(L"USERDNSDOMAIN");
	else 
		m_Domain=param->GetParam(3);

	m_isFound =STATUS_NOT_SEARCHED;
	return ERR_SUCCESS;	
}

// returns the DC Name that has the Guid as DSAGuid. If return String == Guid error will be returned at g_GLE
CString cDsaGuidChk::Guid2Name(CString guid)
{		
	int dcCount;	
	//GetAll DC from Domain
	cDcList DcList ;
	
	if (DcList.GetDcList(m_RefDC,m_Domain)!=ERR_SUCCESS)
	{
		m_isFound= STATUS_ERROR;
		return m_Guid;
	}

	dcCount = DcList.GetCount();
	m_Guid=guid;

	for (int i=0; i<dcCount ;i++)
	{
		if (DcList.GetNtdsDsaObjectGuid(i)== m_Guid)
		{
			m_DcName = DcList.GetServerName(i);	
			m_isFound = STATUS_FOUND;
			return m_DcName;
		}
	}

	m_isFound=STATUS_NOT_FOUND;
	
	return m_Guid;
}

// displays DsaGuidChk help
void cDsaGuidChk::DisplayHelp(void)
{
	//dbg.dataPrint(L"cDcList::DisplayHelp",L"");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"[DSAGUIDCHK]");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"rodcutil.exe dsaguidchk <guid> <ReferenceDCName> [<domain>]");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"Translate GUID to DC Name.");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"guid             Guid string we want to find the related DC for");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"ReferenceDCName  Dc used to translate the guid to name");		
	dbg.dataPrint(L"cDcList::DisplayHelp",L"domain           Specifies the domain to work on. By default uses the domain of the running computer.");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"Sample: ");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"\t rodcutil.exe dsaguidchk 495e263b-2f83-4b1b-b952-b9dfc97531b0 dcName");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"\t rodcutil.exe dsaguidchk 495e263b-2f83-4b1b-b952-b9dfc97531b0 dcName ra03dom.com ");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"");

}

// trace parameters that are being used
void cDsaGuidChk::DisplayParameters(void)
{
	dbg.trace(L"cDsaGuidChk::DisplayParameters",L"m_RefDC  : [ %wS ]",m_RefDC);
	dbg.trace(L"cDsaGuidChk::DisplayParameters",L"m_Guid   : [ %wS ]",m_Guid);
	dbg.trace(L"cDsaGuidChk::DisplayParameters",L"m_Domain : [ %wS ]",m_Domain); 

}

// triggers the work using state parameters coming from Initialize funciton.
int cDsaGuidChk::DsaGuidChk(void)
{
	if (m_isFound== STATUS_NOT_INITIALIZED)
	{
		dbg.trace(L"cDsaGuidChk::DsaGuidChk",L"Returning %d due to status %d",ERR_WRONG_PARAMETERS,STATUS_NOT_INITIALIZED);
		return ERR_WRONG_PARAMETERS;
	}

	//at this point the initialization was done so membes have the info to perform search.
	//No need of return as 
	Guid2Name(m_Guid);

	return m_isFound==STATUS_ERROR ? ERR_FAIL:ERR_SUCCESS;
}

// return true if object status if GUID found. Can retreive the Server name wiht GetDcName
bool cDsaGuidChk::isFound(void)
{
	return m_isFound == STATUS_FOUND? true: false;
}

// return object status of the DcName. return the GUID if the name was not found. Check status with siFound() 
CString cDsaGuidChk::GetDcName(void)
{
	return m_DcName;
}

// return the GUID used in the query
CString cDsaGuidChk::GetGuid(void)
{
	return m_Guid;
}
