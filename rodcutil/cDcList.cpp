#include "StdAfx.h"
#include <Dsgetdc.h>
#include <lm.h>
#include "helper.h"
//#include "cDcInfo.h"
#include "cDcList.h"


cDcInfo::cDcInfo(void)
: m_DcDnsName (_T(""))
, m_DcGUID (_T(""))
, m_DcFlags (0)
, m_ID(0)
{
	static ULONG id;
	m_ID = id++;
	//dbg.trace(L"cDcInfo::cDcInfo",L"Creating DcInfo object [%d] ",m_ID);
	
	
	
}

cDcInfo::~cDcInfo(void)
{
	//dbg.trace(L"cDcInfo::~cDcInfo",L"Destroying [%d] [%wS]",m_ID,this->m_DcDnsName);

}

///////////////////////////////////////////////////////////////////////////////////////
cDcList::cDcList(void)
: m_DCsToList(_T("*"))
{
	dbg.trace(L"cDcList::cDcList",L"Creating DcList object" );
	m_pDcInfoList = NULL;
	m_pTrustList = NULL;
	m_DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM = g_FLAGS & FLAG_2K3_COMPATIBILITY_MODE? DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM_2K3:DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM_2K8;
	EmptyList();
}

cDcList::~cDcList(void)
{	
	EmptyList();
	
	//empty the domain trust list
	if (m_pTrustList)	
	{
		delete m_pTrustList;
		m_pTrustList=NULL;
	}
	dbg.trace(L"cDcList::cDcList",L"Deleting DcList object");
}

// Loads dclist to object using object memeber info
int cDcList::GetDcList(void)
{
	return GetDcList(m_RefDC,m_Domain,m_DCsToList);
}

// Returns DC's in the domain based on memeber DcToList that by default is "*"
int cDcList::GetDcList(CString RefDC,CString Domain)
{
	return GetDcList(RefDC,Domain,m_DCsToList);
}


// Queries DC List for DC filtered by DcToList
int cDcList::GetDcList(CString RefDC,CString Domain,CString DCsToList)
{
/*
	When calling DsGetDomainControllerInfo with DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM 2 or 3
	the DC doesn't return the DC's in the environment.
*/
	HANDLE hDS=NULL;
	int retI = ERROR_SUCCESS;	
	DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K8 * pDcList = NULL ;
	DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K3 * pDcList2k3;


	//if we want all the DC's in the forest
	if (Domain==L"*")
		return GetDcListAll();

	//cDcInfo* p_DcList;
	//p_DcList = new cDcInfo;
	cDcInfo dcInfo;
	
	//check and reset actual information if exists
	EmptyList();

	//set DC to connect to
	m_RefDC = RefDC;
	m_DCsToList = DCsToList;
	m_Domain = Domain;
	

	//dbg.print(L"cDcList::GetDcList",L"Getting dclist from [ %wS ] for domain [ %wS ] filtered [ %wS ]  ",m_RefDC,m_Domain, m_DCsToList);
	dbg.print(L"cDcList::GetDcList",L"Getting dclist from [ %wS ] for domain [ %wS ] ",m_RefDC,m_Domain);
	
	if (g_FLAGS & FLAG_2K3_COMPATIBILITY_MODE)
	{
		dbg.warning(L"cDcList::GetDcList",L"");
		dbg.warning(L"cDcList::GetDcList",L"You are running in 2k3 compatibility mode. No RODC will be found");
		dbg.warning(L"cDcList::GetDcList",L"");
	}

	//Bind
	retI = Bind(&hDS);
	if (retI)
		goto error;	
	
	//Get Information from DC
	retI = DsGetDomainControllerInfo(
		hDS,		
		m_Domain,
		m_DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM,
		&m_DcCount,
		(void**)&pDcList);	

	if (retI!=ERROR_SUCCESS)
	{	
		dbg.error(L"cDcList::GetDcList",L"Error : DsGetDomainControllerInfo. [GLE : %d] ",retI);
		
		//compatibility error checking.
		//this error will pop if you run DsGetDomaincontrollerInfo with DS_DOMAIN_CONTROLLER_INFO_LEVEL_3
		if (retI == ERROR_INVALID_PARAMETER)
		{
			dbg.error(L"cDcList::GetDcList",L"Probably you are running from or against a 2003 box. Try running with /2k3 switch");
			dbg.error(L"cDcList::GetDcList",L"If not check that the reference DC belongs to the desired Domain.");
		}

		g_GLE=retI;
		goto error;
	}

	dbg.print(L"cDcList::GetDcList",L"Found [ %d ] DC's in the Domain",m_DcCount);

	//unbinding	
	UnBind(&hDS);

	//Add DC's info to DcInfoList
	//cDcInfo* p_DcList;
	//p_DcList = new cDcInfo;
	//cDcInfo dcInfo;
	
	pDcList2k3 = (DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K3 * ) pDcList;
	m_pDcInfoList = new CArray<cDcInfo>;
	for (int i = 0 ; i < m_DcCount ; i++)
	{
		
		if (g_FLAGS & FLAG_2K3_COMPATIBILITY_MODE)
			//p_DcList->SaveDcInfo2List((void*)&pDcList2k3[i]);
			dcInfo.SaveDcInfo2List((void*)&pDcList2k3[i]);

		else 
			//p_DcList->SaveDcInfo2List((void*)&pDcList[i]);
			dcInfo.SaveDcInfo2List((void*)&pDcList[i]);

		//p_DcList->SaveDcInfo2List((void*)&pDcList[i]);
		//m_pDcInfoList->Add(*p_DcList);
		m_pDcInfoList->Add(dcInfo);
	
	}
	//delete p_DcList;

	if (pDcList)DsFreeDomainControllerInfo(DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM,m_DcCount,pDcList);

	return ERROR_SUCCESS;
error:
	//clening up in error
	UnBind(&hDS);
	if (pDcList)DsFreeDomainControllerInfo(DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM,m_DcCount,pDcList);
	return g_GLE;
}



// empty actual DCList information
void cDcList::EmptyList(void)
{
	dbg.trace(L"cDcList::EmptyList",L"Flushing Dclist");
	m_RefDC.Empty();
	m_Domain.Empty();
	m_DcCount=0;
	
/*	//delete DcList strucutre
	if (m_pDcList)
		DsFreeDomainControllerInfo(m_DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM,m_DcCount,m_pDcList);
	m_pDcList=NULL;
*/
	
	//empty DcInfoList array.
	if (m_pDcInfoList){
		delete m_pDcInfoList;
		m_pDcInfoList=NULL;
	}
/*
	//empty the domain trust list
	if (m_pTrustList)	
	{
		delete m_pTrustList;
		m_pTrustList=NULL;
	}
*/
}

// bind to server
int cDcList::Bind(HANDLE *phDS)
{
	HANDLE hDS = *phDS;
	RPC_AUTH_IDENTITY_HANDLE cred;
	int retI;

	//Bind
	dbg.print(L"cDcList::Bind",L"Bind for building DcList to %wS ...",m_RefDC);

	if (g_Cred.Exist())		
	{	
		dbg.trace(L"cDcList::Bind",L"Binding with specific credentials [ %wS\\%wS ]",
			g_Cred.GetDomain(),g_Cred.GetUser());		
		DsMakePasswordCredentials(g_Cred.GetUser(),g_Cred.GetDomain(),g_Cred.GetPassword(),&cred);
		retI= DsBindWithCred(m_RefDC,NULL,cred,phDS);
	}
	else 
	{
		dbg.trace(L"cDcList::Bind",L"Binding with NULL credentials");
		retI=DsBind(m_RefDC,NULL,phDS);
	}

	//checking binding errors
	if (retI == ERROR_SUCCESS )
		dbg.trace(L"cDcList::Bind",L"...Bind SUCCESS");
	else 
	{
		g_GLE = retI;
		dbg.error(L"cDcList::Bind",L"Error binding [retI  %d]",retI); 
		goto error;
	}	
	
	//if (hDS) DsUnBind(&hDS);
	return ERROR_SUCCESS;

error:
	if (*phDS) DsUnBind(phDS);	
	return g_GLE;
}

// display DCList
void cDcList::DisplayDCList(void)
{
	CString format;
	//CString roles;
	CString line;
	CString header;
	int SiteLen,DcNameLen,PdcOrRodcLen,GcLen,GuidLen;
	int MaxLen;

	//We could show other DC info 
	// GC | RODC | ServerCode | KVNO |Site	

	//DClist display header
	dbg.print(L"cDcList::DisplayDCList",L"");
	dbg.print(L"cDcList::DisplayDCList",L"Printing Dclist %d DCs:",m_DcCount);
	dbg.trace(L"cDcList::DisplayDCList",L"======================");

	//setting colum lengths
	DcNameLen = GetMaxDcNameLength();
	SiteLen   = GetMaxSiteNameLength();
	//RoleLen	  = 13;
	PdcOrRodcLen = 6;
	GcLen = 4;
	GuidLen	  = 36;
	
	//MaxLen= SiteLen+DcNameLen+RoleLen+GuidLen+4;
	MaxLen= SiteLen+DcNameLen+GcLen+PdcOrRodcLen+GuidLen+5;
	
	//building separation line
	line.Empty();
	for (int i=1;i<MaxLen;i++) line+="=";

	//Building the format string
/*	if (g_FLAGS & FLAG_CSV)
		//format.Format(L"%%wS,%%wS,%%wS,%%wS");		
		format.Format(L"%%wS,%%wS,%%wS,%%wS,%%wS");		
	else
		//format.Format(L"%%-%dwS %%-%dwS %%-%dwS %%-%dwS ",SiteLen,DcNameLen,RoleLen,GuidLen);		
		format.Format(L"%%-%dwS %%-%dwS %%-%dwS %%-%dwS %%-%dwS ",SiteLen,DcNameLen,PdcOrRodcLen,GcLen,GuidLen);		
*/
	//list header	
	//header.Format(format,L"Site",L"ServerName",L"Roles",L"",L"DSA GUID");		
	if (g_FLAGS & FLAG_CSV)	
	{
		//header.Format(format,L"Site",L"ServerName",L"Roles",L"DSA GUID");
		format.Format(L"%%wS,%%wS,%%wS,%%wS,%%wS");	
		header.Format(format,L"Site",L"ServerName",L"Pdc-RODC", L"GC",L"DSA GUID");	
		dbg.dataPrint(L"cDcList::DisplayDCList",L"%wS",header);			
	}
	else
	{
		dbg.dataPrint(L"cDcList::DisplayDCList",L"");
		format.Format(L"%%-%dwS %%-%dwS %%-%dwS %%-%dwS %%-%dwS ",SiteLen,DcNameLen,PdcOrRodcLen,GcLen,GuidLen);		
		header.Format(format,L"Site",L"ServerName",L"Roles",L"",L"DSA GUID");		
		dbg.print(L"cDcList::DisplayDCList",L"%wS",header);
		dbg.print(L"cDcList::DisplayDCList",L"%wS",line);
	}
	


		
	//printing servers in order. First Full and then RO.
	//print full DC's
	for (DWORD i = 0 ; i < m_DcCount ; i++)
		if (!isRODC(i) && m_DCsToList != "RODC")		
			DisplayDC(format,i);			

	//print RODC
	for (DWORD i = 0 ; i < m_DcCount ; i++)
		if (isRODC(i))
			DisplayDC(format,i);		

	
	dbg.trace(L"cDcList::DisplayDCList",L"");
	dbg.trace(L"cDcList::DisplayDCList",L"======================");
}

void cDcList::UnBind(HANDLE * phDS)
{
	dbg.trace(L"cDcList::UnBind",L"Unbinding...");
	if (*phDS) DsUnBind(phDS);

}
// display DcList command help
void cDcList::DisplayHelp(void)
{
	///c /v dclist radc3 * k155.com /u raulg /d k155.com /p darkMatter!
	
	//dbg.print(L"cDcList::DisplayHelp",L"");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"[DCLIST]");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"rodcutil.exe dclist  ( * | RODC ) [ ( <domain> | * ) [<RefDcName>]] ");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"Displays the list of DC's in the Domain.");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"*          All DC's in the domain or all domains in the forest");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"RODC       Read Only DCs in the domain");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"domain     Specifies the domain to work on. By default uses the domain of the running computer.");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"RefDcName  Dc used to build DC list. If not specified system will resolve to DC in the domain.");			
	dbg.dataPrint(L"cDcList::DisplayHelp",L"");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"Sample: ");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"\t rodcutil.exe dclist * ra03dom.com  dcName ");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"\t rodcutil.exe dclist * ");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"\t rodcutil.exe dclist * * ");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"\t rodcutil.exe dclist rodc * ");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"\t rodcutil.exe dclist * ra03dom.com ");
	dbg.dataPrint(L"cDcList::DisplayHelp",L"\t rodcutil.exe dclist dcName.ra03dom.com  RODC");	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"\t rodcutil.exe dclist RODC ra03dom.com dcName.ra03dom.com ");		
	
	dbg.dataPrint(L"cDcList::DisplayHelp",L"");

	return;
}

// sets DcList object using param information
int cDcList::Initialize(cParameters* param)
{
	//int paramCount = 2;
	////dclist <ReferenceDCName> [ * | RODCs ] [domain]"
	//dclist < * | RODCs >      [domain]  [ReferenceDCName]
	// 0            1               2           3
	dbg.trace(L"cDcList::Initialize",L"Initialization of parameters");
	
	//check mandatory parametes
	
	// 0. Chcek minimum number of parameters
	if (param->GetCount()< 2)
		return ERR_WRONG_PARAMETERS;

	// 1. Read DcToList

	//reading DCsToList param. 

	if (param->GetParam(1).MakeUpper()== L"RODC")
	{
		m_DCsToList=L"RODC";	
	}
	else
		if (param->GetParam(1)== L"*")
			m_DCsToList= L"*";
		else
			return ERR_WRONG_PARAMETERS;

	// 3. Read Domain
	//reading domain
	if (param->GetCount() < 3)
		//m_Domain = _wgetenv(L"USERDNSDOMAIN");
		m_Domain.GetEnvironmentVariableW(L"USERDNSDOMAIN");
	else 
		m_Domain=param->GetParam(2);
	
	// 4. Read selected server
	//reading reference server
	//if domain == * no need to Specify ref DC
	CString userDomain;
	if (param->GetCount()< 4)
	{
		if (m_Domain==L"*")
			m_RefDC=GetDcName();
		else
			m_RefDC=GetDcName(m_Domain);
	}
	else
		m_RefDC=param->GetParam(3);

	return ERROR_SUCCESS;
}

// trace the actual active parameters used by dclist
void cDcList::DisplayParameters(void)
{	
	dbg.trace(L"cDcList::DisplayParameters",L"m_RefDC     : [ %wS ]",m_RefDC);
	dbg.trace(L"cDcList::DisplayParameters",L"m_DCsToList : [ %wS ]",m_DCsToList);
	dbg.trace(L"cDcList::DisplayParameters",L"m_Domain    : [ %wS ]",m_Domain);
}


// return number of DC in the list
int cDcList::GetCount(void)
{	
	//Error if array=0 as there is no referenced data
	//return this->m_pDcInfoList->GetCount();

	return m_DcCount;
}

// returns server name at position of aray i
CString cDcList::GetServerName(int i)
{
	return this->m_pDcInfoList->GetAt(i).m_DcDnsName;
	//return m_pDcList[i].DnsHostName;
}

// returns the site name the DC resides in.
CString cDcList::GetServerSiteName(int i)
{	
	return this->m_pDcInfoList->GetAt(i).m_DcSiteName;
	//return m_pDcList[i].SiteName;;
}

/* //helper to translate GUID to string */
int cDcList::BuildGUIDString(CString* strGUID, GUID* pGuid)
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

/* //helper to translate GUID to string */
int cDcInfo::BuildGUIDString(CString* strGUID, GUID* pGuid)
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
// //return DC GUID string
CString cDcList::GetNtdsDsaObjectGuid(int i)
{
	//CString a1;
	//BuildGUIDString(&a1,&(m_pDcList[i].NtdsDsaObjectGuid));
	return this->m_pDcInfoList->GetAt(i).m_DcGUID;

	//return a1;
}

// return true for the different roles 
bool cDcList::isRODC(int i)
{	
	return this->m_pDcInfoList->GetAt(i).m_fIsRodc;	
	//return m_pDcList[i].fIsRodc;

}

// return true if the DC is the PDC
bool cDcList::isPDC(int i)
{
	//return m_pDcList[i].fIsPdc;
	return this->m_pDcInfoList->GetAt(i).m_fIsPdc;
}

// return true if the DC is GC
bool cDcList::isGC(int i)
{
	
	return this->m_pDcInfoList->GetAt(i).m_fIsGc;
	//return m_pDcList[i].fIsGc;
}


// return the size of the biggest name of all the DcList
int cDcList::GetMaxDcNameLength(void)
{
	CString aux;
	int i;
	int max,actual;
	max=0;
	for (i=0; i < m_DcCount;i++)
	{
		//aux=m_pDcList[i].DnsHostName;
		actual = this->m_pDcInfoList->GetAt(i).m_DcDnsName.GetLength();
		//actual=aux.GetLength();
		if (actual>max) max=actual;
	}
		
	return max;
}

// return the length of the longest site name
int cDcList::GetMaxSiteNameLength(void)
{
	CString aux;
	int i;
	int max,actual;
	max=0;
	for (i=0; i < m_DcCount;i++)
	{
		//aux=m_pDcList[i].SiteName;
		actual = this->m_pDcInfoList->GetAt(i).m_DcSiteName.GetLength();
		//actual=aux.GetLength();
		if (actual>max) max=actual;
	}
		
	return max;
}

// displays a single DC info
//void cDcList::DisplayDC(CString format, CString param1,CString param2,CString param3,CString param4)
void cDcList::DisplayDC(CString format,int i)
{
	//DisplayDC(format,GetServerSiteName(i),GetServerName(i),roles,GetNtdsDsaObjectGuid(i));

	CString roles1; // jholds RODC or PDC role. They can't be both at the same time
	CString roles2; //holds GC role
	roles1.Empty();
	roles2.Empty();

	if (g_FLAGS & FLAG_CSV)
	{
		roles1+= isPDC(i) ? L"PDC"  : L"";
		roles1+= isRODC(i)? L"RODC" : L"";		
		roles2= isGC(i)  ? L"GC"   : L"";	
	}
	else
	{
		roles1+= isPDC(i) ? L"[PDC]"  : L"";
		roles1+= isRODC(i)? L"[RODC]" : L"";		
		roles2= isGC(i)  ? L"[GC]"   : L"";	

//		roles+= isRODC(i)? L"[RODC] " : isPDC(i) ? L"[PDC]  ": L"       " ;
//		roles+= isGC(i)  ? L"[GC]"   : L"     " ;
	}		
		
	CString aux;
	//aux.Format(format,param1,param2,param3,param4);
	//dbg.trace(L"cDcList::DisplayDC",L"%d",aux.GetLength());	
	aux.Format(format,GetServerSiteName(i),GetServerName(i),roles1,roles2,GetNtdsDsaObjectGuid(i));
	dbg.dataPrint(L"cDcList::DisplayDC",L"%wS",aux);	
}

// return server name from guid. If guid not present servername = the imput guid
CString cDcList::Guid2Name(CString guid)
{

	for (int i = 0 ; i < m_DcCount ; i++)
	{		
		if (m_pDcInfoList->GetAt(i).m_DcGUID == guid)
		{
			return m_pDcInfoList->GetAt(i).m_DcDnsName;					
		}
	}
	return guid;
}

// Save the DcList info to DcInfoList array
void cDcInfo::SaveDcInfo2List(void* pDclist)
{
	//DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM_2K3

	if (g_FLAGS & FLAG_2K3_COMPATIBILITY_MODE)
		SaveDcInfo2List_2k3((DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K3*)pDclist);
	else
		SaveDcInfo2List_2k8((DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K8*)pDclist);

	DisplayDcInfo();

	return;
}

void cDcInfo::SaveDcInfo2List_2k3(DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K3* p_DcInfo)
{
	//exception
	m_fIsRodc=false;

	//flags
	m_fIsPdc=p_DcInfo->fIsPdc;
	m_fIsGc=p_DcInfo->fIsGc;
	m_fDsEnabled=p_DcInfo->fDsEnabled;
		
	//Computer name
	m_DcDnsName=p_DcInfo->DnsHostName;
	m_DcSiteName=p_DcInfo->SiteName;	
		
	BuildGUIDString(&m_DcGUID,&(p_DcInfo->NtdsDsaObjectGuid));
	
	
	
}
void cDcInfo::SaveDcInfo2List_2k8(DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K8* p_DcInfo)
{
	//exception
	m_fIsRodc=p_DcInfo->fIsRodc;
	
	//flags
	m_fIsPdc=p_DcInfo->fIsPdc;
	m_fIsGc=p_DcInfo->fIsGc;
	m_fDsEnabled=p_DcInfo->fDsEnabled;
			
	//Computer name
	m_DcDnsName =p_DcInfo->DnsHostName;	
	m_DcSiteName=p_DcInfo->SiteName;
		
	BuildGUIDString(&m_DcGUID,&(p_DcInfo->NtdsDsaObjectGuid));	

}


// print  the dclist info 
void cDcInfo::DisplayDcInfo(void)
{
	CString strAux;

	strAux.Format(L"DC = %wS\\%wS [%wS]",m_DcSiteName,m_DcDnsName,m_DcGUID);
	dbg.trace(L"cDcInfo::DisplayDcInfo",L"%wS",strAux);


}

// return the DC Dsa Guid
CString cDcInfo::GetDsaDcGuid(void)
{
	return m_DcGUID;
}

// return a DC name of the domain and corresponding to flags
CString cDcList::GetDcName()
{
	CString aux;
	aux.GetEnvironmentVariableW(L"USERDNSDOMAIN");
	return GetDcName(aux);

}
CString cDcList::GetDcName(CString Domain, int Flags)
{
	int retI;
	PDOMAIN_CONTROLLER_INFO DcInfo =NULL;
	CString DcName;
	int flags =Flags;	

	dbg.trace(L"cDcList::GetDcName",L"Getting DC for Domain [%wS] from [%wS]",Domain,m_RefDC);		

	retI=DsGetDcName(m_RefDC,Domain,NULL,NULL,flags,&DcInfo);

	if(retI != ERROR_SUCCESS)
	{
		dbg.error(L"cDcList::GetDcName",L"Error calling DsGetDcName with error [%d]",retI);	
		DcName.Empty();
	}
	else
	{
		DcName=DcInfo->DomainControllerName;
		dbg.trace(L"cDcList::GetDcName",L"Found [%wS]for Domain [%wS]",DcName,Domain);
	
	}
	
	if (DcInfo) 
		NetApiBufferFree(DcInfo);

	return DcName;
}

// created the  trust list
int cDcList::GetTrustList(void)
{
	PDS_DOMAIN_TRUSTS trusts = NULL;
	ULONG count = 0;
	int retI = 0;


	dbg.trace(L"cDcList::GetTrustList",L"Creating trust list");
	
	dbg.trace(L"cDcList::GetTrustList",L"Calling DsEnumerateDomainTrusts ");
	
	//obtain the trust list
	// ask for domains in the forest
	retI = DsEnumerateDomainTrusts(NULL,DS_DOMAIN_IN_FOREST,&trusts,& count);
	if (retI != ERROR_SUCCESS)
	{
		dbg.error(L"cDcList::GetTrustList",L"Error calling DsEnumerateDomainTrusts [%d] ",retI);
		return retI;
	}
	dbg.trace(L"cDcList::GetTrustList",L"Found [%d] trustes domain",count);

	//Create  trust list
	m_pTrustList = new CStringArray;
	
	//limit the domain list to two for developing porpouses
	//for (int i = 0 ; i < count; i++)
	dbg.error(L"cDcList::GetTrustList",L"Limited the domain list to 2 for developing porpouses");
	for (int i = 0 ; i < count && i < 2; i++)
	{
		m_pTrustList->Add(trusts[i].DnsDomainName);
		dbg.trace(L"cDcList::GetTrustList",L"  %i. %wS",i,m_pTrustList->GetAt(i));
	}

	if (trusts)
		NetApiBufferFree(trusts);

	return 0;
}

// Saves all the Dc's of the forest
int cDcList::GetDcListAll(void)
{
	CArray<cDcInfo> * pDcInfoList = NULL;
	int retI;
	int count;
	CString Domain;
	CString DC;
	
	//Get the list of domains in the forest
	retI = GetTrustList();
	if (!retI==ERROR_SUCCESS)
	{
		dbg.error(L"cDcList::GetDcListAll",L"Error building Forest domain list [%d]",retI);
		return retI;
	}

	//Create structure to hold all the Dc info.
	pDcInfoList = new CArray<cDcInfo>;

	//iterate trough the list to get all DC's
	count = m_pTrustList->GetCount();
	for (int i = 0 ; i< count ;i++)
	{
		Domain=m_pTrustList->GetAt(i);
		dbg.trace(L"cDcList::GetDcListAll",L"Building DcList for Domain [%wS]",Domain);

		//Get DC for that Domain.
		DC = GetDcName(Domain);
		dbg.trace(L"cDcList::GetDcListAll",L"Retreiving Dc List for [%wS] from [%wS]",Domain,DC);
		retI = GetDcList(DC,Domain);
		if (!retI==ERROR_SUCCESS)
		{
			dbg.error(L"cDcList::GetDcListAll",L"Error retreiving Dc List for [%wS] from [%wS]",Domain,DC);
			return retI;
		}
		
		for (int j = 0; j < m_DcCount ;  j++)
		{
			dbg.trace(L"cDcList::GetDcListAll",L"Adding [%wS]",m_pDcInfoList->GetAt(j).m_DcDnsName);
			pDcInfoList->Add(m_pDcInfoList->GetAt(j));	
		}		
		//dbg.error(L"cDcList::GetDcListAll",L"Count up to there [%d]",pDcInfoList->GetCount());
	}

	//replace m_pDcInfoList with the full copy

//	dbg.error(L"cDcList::GetDcListAll",L"Delete m_pDcInfoList content [%d]",m_pDcInfoList->GetCount());
	delete m_pDcInfoList;
//	dbg.error(L"cDcList::GetDcListAll",L"Assigning 	<m_pDcInfoList = pDcInfoList> [%d]",pDcInfoList->GetCount());
	m_pDcInfoList = pDcInfoList;
	m_DcCount = pDcInfoList->GetCount();
//	dbg.error(L"cDcList::GetDcListAll",L"m_pDcInfoList content [%d]",m_pDcInfoList->GetCount());
	pDcInfoList=NULL;

	return ERROR_SUCCESS;
}


// copies information to DcInfo from a DcInfo
int cDcInfo::operator=(cDcInfo dcInfo)
{
	m_DcDnsName		= dcInfo.m_DcDnsName;
	m_DcFlags		= dcInfo.m_DcFlags;
	m_DcGUID		= dcInfo.m_DcGUID;
	m_DcNtdsName	= dcInfo.m_DcNtdsName;
	m_DcSiteName	= dcInfo.m_DcSiteName;
	m_fDsEnabled	= dcInfo.m_fDsEnabled;
	m_fIsGc			= dcInfo.m_fIsGc;
	m_fIsPdc		= dcInfo.m_fIsPdc;
	m_fIsRodc		= dcInfo.m_fIsRodc;

	return 0;
}
