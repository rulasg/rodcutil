#include "StdAfx.h"
#include <string.h>
#include "Helper.h"
//#include "cParameters.h"
//#include "cDbgHelper.h"

#include "cDclist.h"
#include "cSrvInfo.h"
#include "cSyncChk.h"

cObjSyncChk::cObjSyncChk(void)
: m_RefServer(_T(""))
, m_DcToList   (_T(""))
, m_ObjectDN(_T(""))
, m_RefServerPos(0)
{
	m_DCInfoList = NULL;
	//m_Domain = _wgetenv(L"USERDNSDOMAIN");
	m_Domain.GetEnvironmentVariableW(L"USERDNSDOMAIN");
}

cObjSyncChk::~cObjSyncChk(void)
{
	dbg.trace(L"cObjSyncChk::~cObjSyncChk",L"Deleting cObjSyncChk object");
	if (m_DCInfoList)
	{	
		delete m_DCInfoList;	
	}
}

//initialize objsyncchk members
int cObjSyncChk::Initialize(cParameters *param)
{
	//ObjSyncChk <objectDN> <ReferenceDCName> [* | RODCs | <DCName> [<DCName> ...]
	//   1       2              3                       4
	//save paramters and return ERR_WRONG_PARAMETERS if can't fit.
	//int retI;
	dbg.trace(L"cObjSyncChk::Initialize",L"Initialization of parameters");

	if (param->GetCount()<4)
		return ERR_WRONG_PARAMETERS;
	m_ObjectDN=param->GetParam(1);
	m_RefServer=param->GetParam(2);
	m_DcToList =param->GetParam(3);

	if (param->GetCount() < 5)
		//if domain not specified get session user damain name
		//m_Domain = _wgetenv(L"USERDNSDOMAIN");
		m_Domain.GetEnvironmentVariableW(L"USERDNSDOMAIN");
	else 
		m_Domain=param->GetParam(4);
	
	////Create struture with target DC info.
	//// Should use m_DCList to choose targets DC's
	//retI=CreateDcInfoList();

	//if (retI != ERR_SUCCESS)
	//{
	//	g_GLE =retI;
	//	return retI;
	//}

	return ERR_SUCCESS;
}



// //Display the actual paramters for the ObjSyncChk
void cObjSyncChk::DisplayParameters(void)
{
	//cDbgHelper dbg;
	dbg.trace(L"cObjSyncChk::DisplayParameters",L"m_ObjectDN : \t[ %wS ]",m_ObjectDN);
	dbg.trace(L"cObjSyncChk::DisplayParameters",L"m_RefServer : \t[ %wS ]",m_RefServer);
	dbg.trace(L"cObjSyncChk::DisplayParameters",L"m_DCList : \t[ %wS ]",m_DcToList);
	
}

// //print command help
void cObjSyncChk::DisplayHelp(void)
{

//print the sum of the status.
/*
[ == ] Equal
[ != ] Different	
[ 00 ] Not Present
[ ER ] Error checking server
[ ?? ] Unknown status
[ ERROR ] We shouldn't ever output this error tag.
*/

	//cDbgHelper dbg;
	dbg.dataPrint(L"syncchkHelp",L"");
	dbg.dataPrint(L"syncchkHelp",L"[OBJSYNCCHK]");
	dbg.dataPrint(L"syncchkHelp",L"");
	dbg.dataPrint(L"syncchkHelp",L"rodcutil.exe objsyncchk <objectDN> <ReferenceDCName> [* | RODC | \"<DCName> [<DCName> ...\"]");
	dbg.dataPrint(L"syncchkHelp",L"");
	//dbg.dataPrint(L"syncchkHelp",L"Helps to check object sync status between DC's");
	dbg.dataPrint(L"syncchkHelp",L"Check objects are synchronised between DC's by comparing the replication metadata from the reference with the target(s).");
	dbg.dataPrint(L"syncchkHelp",L"");
	dbg.dataPrint(L"syncchkHelp",L"objectDN           Object to check the Sync status between servers.");
	dbg.dataPrint(L"syncchkHelp",L"ReferenceDCName    DC used as reference to comapre with. This should be the good info server.");
	dbg.dataPrint(L"syncchkHelp",L"*                  Compare against all the DC's of the domain");	
	dbg.dataPrint(L"syncchkHelp",L"RODC               Compare against all Read Only DCs of the domain.");
	dbg.dataPrint(L"syncchkHelp",L"DCName             Name of a DC as part of a list of DC's to compare with.  ");	
	dbg.dataPrint(L"syncchkHelp",L"");
	dbg.dataPrint(L"syncchkHelp",L"Output Server Status: ");
	dbg.dataPrint(L"syncchkHelp",L"\t [ == ] Object is Equal as in the reference server. ");
	dbg.dataPrint(L"syncchkHelp",L"\t [ != ] Object is Different than in reference server. ");
	dbg.dataPrint(L"syncchkHelp",L"\t [ 00 ] Object is missing in the DC ");
	dbg.dataPrint(L"syncchkHelp",L"\t [ ER ] Error checking server. Enable Verbose (/verbose) for troubleshooting");
	dbg.dataPrint(L"syncchkHelp",L"");
	dbg.dataPrint(L"syncchkHelp",L"Output Attribute Status: ");
	dbg.dataPrint(L"syncchkHelp",L"\t [ = ] Attribute is the same in server as in reference server. ");
	dbg.dataPrint(L"syncchkHelp",L"\t [ X ] Attribute is different in server than in reference server.  ");
	dbg.dataPrint(L"syncchkHelp",L"\t [ - ] Attribute is not present in server. ");	
	dbg.dataPrint(L"syncchkHelp",L"\t [ + ] Attribute is not present in reference server. ");	
	dbg.dataPrint(L"syncchkHelp",L"\t [ # ] Attribute in the reference server.");	
	dbg.dataPrint(L"syncchkHelp",L"");
	dbg.dataPrint(L"syncchkHelp",L"Sample: ");
	dbg.dataPrint(L"syncchkHelp",L"\t rodcutil.exe objsyncchk  CN=LdapUser,CN=Users,DC=ra03dom,DC=com dc.ra03dom.com ms.ra03dom.com");
	dbg.dataPrint(L"syncchkHelp",L"\t rodcutil.exe objsyncchk  CN=LdapUser,CN=Users,DC=ra03dom,DC=com dc.ra03dom.com \"ms.ra03dom.com ms2.ra03dom.com\"");
	dbg.dataPrint(L"syncchkHelp",L"\t rodcutil.exe objsyncchk  CN=LdapUser,CN=Users,DC=ra03dom,DC=com dc.ra03dom.com RODC");
	dbg.dataPrint(L"syncchkHelp",L"\t rodcutil.exe objsyncchk  CN=LdapUser,CN=Users,DC=ra03dom,DC=com dc.ra03dom.com *");
	dbg.dataPrint(L"syncchkHelp",L"");

	

	return;
}

// Builds the list of servers to chk against
int cObjSyncChk::CreateDcInfoList(void)
{	
	cSrvInfo *pSrvI;
	cDcList *pDcList = NULL;
	int i,iAux;
	CString strAux;

	//initialitazion of memeber
	m_DCInfoList = new CArray<cSrvInfo>;

	//creating reference server node 
	pSrvI = new cSrvInfo;
	pSrvI->Initialize(m_RefServer);
	pSrvI->SetStatus(STATUS_REF_INFO);
	m_DCInfoList->Add(*pSrvI); 
	

	
	//Get Server list
	//1. Imput comming from a txt file. Should be the first to try. If file is not there will think is a server name.
	//2. "*" compare against all the DC's of the domain
	//3. "RODC" compara against all the RODC of the domain. For tghis we need to querie against a LH Ref server
	//4. Simple or multiple server reference. This is for a 1 to 1 comparation	or a 1 to few comparation
	

	//1. Imput comming from a txt file. Should be the first to try. If file is not there will think is a server name.
	//pending to be coded.
	/* Look for m_DCList as a file.
	if exists m_DCList  as a file
	{
		While not end of file
		{
			Read file line
			pSrvI = new cSrvInfo;
			pSrvI->Initialize(file server name);
			m_DCInfoList->Add(*pSrvI);
		}
	
	}
	*/

	//2. "*" compare against all the DC's of the domain
	if (m_DcToList == L"*")
	{		
		//dbg.trace(L"cObjSyncChk::CreateDcInfoList",L"Pending to develope [ * ] DClist");	

		//GetAll DC from Domain
		pDcList = new cDcList;
		pDcList->GetDcList(m_RefServer,m_Domain);
		iAux = pDcList->GetCount();
		for (i=0;i<iAux;i++)
		{
			pSrvI = new cSrvInfo;
			pSrvI->Initialize(pDcList->GetServerName(i));
			pSrvI->SetDsaGUID(pDcList->GetNtdsDsaObjectGuid(i));
			m_DCInfoList->Add(*pSrvI); 		
		}	
	}
	
	//3. "RODC" compara against all the RODC of the domain. For tghis we need to querie against a LH Ref server
	else if ((m_DcToList == L"RODC") || (m_DcToList == L"rodc"))	
	{
		//dbg.trace(L"cObjSyncChk::CreateDcInfoList",L"Pending to develope [ RODC ] DClist");	
		
		//GetAll DC from Domain
		pDcList = new cDcList;
		pDcList->GetDcList(m_RefServer,m_Domain);
		iAux = pDcList->GetCount();
		for (i=0;i<iAux;i++)
		{
			//add to the list only RODC's
			if (pDcList->isRODC(i)){
				pSrvI = new cSrvInfo;
				pSrvI->Initialize(pDcList->GetServerName(i));
				pSrvI->SetDsaGUID(pDcList->GetNtdsDsaObjectGuid(i));
				m_DCInfoList->Add(*pSrvI); 		
			}
		}

	}
	
	//4. Simple or multiple server reference. This is for a 1 to 1 comparation	or a 1 to few comparation
	else
	{	
		int s = 0; //start
		int t = 0; //tail
		CString strAux;
		m_DcToList.Trim();		
			

		//Extract server names from space separate list
		//We base the space separate list inthe  fact that names can't contain spaces.
		do 
		{
			t = m_DcToList.Find(' ',s+1);
			
			//unique or last element in the list
			if (t==-1)
			{				
				strAux=m_DcToList.Mid(s);
			}
			else
			{	
				strAux=m_DcToList.Mid(s,t-s);
			}

			//update counters
			s=t;

			//clean spaces
			strAux.Trim();

			//Create node
			pSrvI=new cSrvInfo;

			//Initialize with server name
			pSrvI->Initialize(strAux);

			//add to list
			m_DCInfoList->Add(*pSrvI);			
		
		}while (t!=-1);

	}

	
	dbg.print(L"cObjSyncChk::CreateDcInfoList",L"Comparing against [%d] servers ",m_DCInfoList->GetCount()-1);

	//clean up cDclist object
	if (!pDcList) delete pDcList;
	
	if (m_DCInfoList->GetCount()>1 )
		return 0;
	else 
		return 1;
}

// Compare obj within all the DClist against RefSrv
int cObjSyncChk::ObjSynChk(void)
{	
	int retI=ERR_SUCCESS;
	int i=0;
	int srvI=1;
//	int retB;
	CString a3;
	CString refStr, srvStr;

	//Create struture with target DC info.
	// Should use m_DCList to choose targets DC's
	retI=CreateDcInfoList();

	if (retI != ERR_SUCCESS)
	{
		g_GLE =retI;
		return retI;
	}

	if (m_DCInfoList->GetCount()==0)
	{
		dbg.error(L"cObjSyncChk::ObjSynChk",L"Not found any DC to compare against.");
		return 1;
	}

	//I know that reference server is at the frontend of the Array (m_RefServerPos)
	//int m_RefServerPos  = (int) m_DCInfoList->GetCount()-1;
	
	//getmetadata from reference server
	retI = m_DCInfoList->GetAt(m_RefServerPos ).GetObjMetadata(m_ObjectDN);
	if (retI != ERR_SUCCESS)
	{		
		dbg.error(L"cObjSyncChk::ObjSynChk",L"Error [ %d ] reading Object Metadata at %wS",retI,m_DCInfoList->GetAt(m_RefServerPos ).GetSrvName());
		return retI;
	}

	//display metadata for reference
	//ComprareServers(m_RefServerPos,m_RefServerPos);
	m_DCInfoList->GetAt(m_RefServerPos ).DisplayObjMetaData();
	
	//list all metadata
	dbg.print(L"cObjSyncChk::ObjSynChk",L"Retreiving information from [%d] servers.",m_DCInfoList->GetCount()-1);
	//jumping reference server	
	for (srvI=1;srvI<m_DCInfoList->GetCount();srvI++)
	//	for (srvI=1;srvI<m_DCInfoList->GetCount();srvI++)
	{	

		retI = m_DCInfoList->GetAt(srvI).GetObjMetadata(m_ObjectDN);
		if (retI != ERR_SUCCESS)
		{
			if (retI == ERROR_DS_OBJ_NOT_FOUND)
			{
				dbg.trace (L"cObjSyncChk::ObjSynChk",L"Error [ ERROR_DS_OBJ_NOT_FOUND ] reading Object Metadata at %wS",m_DCInfoList->GetAt(srvI).GetSrvName());								
				m_DCInfoList->GetAt(srvI).SetStatus(ERROR_DS_OBJ_NOT_FOUND);
			}
			else
				dbg.trace (L"cObjSyncChk::ObjSynChk",L"Error [ %d ] reading Object Metadata at %wS",retI,m_DCInfoList->GetAt(srvI).GetSrvName());								
						
		} 
		else 
		{		
			ComprareServers(m_RefServerPos,srvI);	
		}

	}

	//Print result
	PrintResult();
	
	return 0;
}

// compare attrib metadata of two server
bool cObjSyncChk::AttribEqual(DS_REPL_ATTR_META_DATA* pAttrib1, DS_REPL_ATTR_META_DATA* pAttrib2)
{
	bool b= true;
	//cSrvInfo s;
	CString a1;
	CString a2;
	CString a3;

	//s.BuildGUIDString(&a3,&(pAttrib1->uuidLastOriginatingDsaInvocationID));	
	//a3.Format(L"%d",pAttrib1->uuidLastOriginatingDsaInvocationID.Data1);
	BuildGUIDString(&a3,&(pAttrib1->uuidLastOriginatingDsaInvocationID));
	a1.Format(L"%-20wS \t%7d\t ",
		pAttrib1->pszAttributeName,
		pAttrib1->usnOriginatingChange
		);
	a1+=a3 ;

	//s.BuildGUIDString(&a3,&(pAttrib2->uuidLastOriginatingDsaInvocationID));
	//a3.Format(L"%d",pAttrib2->uuidLastOriginatingDsaInvocationID.Data1);
	BuildGUIDString(&a3,&(pAttrib2->uuidLastOriginatingDsaInvocationID));
	a2.Format(L"%-20wS \t%7d\t ",pAttrib2->pszAttributeName,pAttrib2->usnOriginatingChange);
	a2+=a3;
		
	b =	a1 == a2 ;	
	
	//	b =	wcscmp(pAttrib1->pszAttributeName,pAttrib2->pszAttributeName) ;
		//&&pAttrib1->usnOriginatingChange == pAttrib2->usnOriginatingChange &&	pAttrib1->uuidLastOriginatingDsaInvocationID == pAttrib2->uuidLastOriginatingDsaInvocationID;
	a3= b? L"[   ]":L"[ X ]";
	dbg.trace(L"cObjSyncChk::AttribEqual",L"%wS %wS",a3,a1);

	return b;
	
}

// translates bin GUID to String GUID
int cObjSyncChk::BuildGUIDString(CString* strGUID, GUID* pGUID)
{
	//RPC_WSTR* pszChar=NULL;	
	PWCHAR pszChar=NULL;	
	if (UuidToString(pGUID,(RPC_WSTR*)&pszChar)==RPC_S_OK)
		strGUID->Format(L"%wS",pszChar);
	else
		strGUID->Format(NULL_GUID);

	RpcStringFree((RPC_WSTR*)&pszChar);
	
	
	return 0;
}

// will print restult of the objsyncchk. Will be diferent depending in modes we are (/Q /CSV)
int cObjSyncChk::PrintResult(void)
{
	//print the sum of the status.
	/*
	[ == ] Equal
	[ != ] Different	
	[ 00 ] Not Present
	[ ER ] Error checking server
	[ ?? ] Unknown status
	[ ERROR ] We shouldn't ever output this error tag.

	*/
	CString a3;
	

	dbg.print(L"cObjSyncChk::PrintResult",L"");
	dbg.print(L"cObjSyncChk::PrintResult",L"Server status:");
	dbg.trace(L"cObjSyncChk::PrintResult",L"======================");
	for (int srvI=1;srvI<m_DCInfoList->GetCount();srvI++)
	{			
		m_DCInfoList->GetAt(srvI).DisplayServerStatus();
		//if (!(m_DCInfoList->GetAt(srvI).GetStatus()== STATUS_ERROR))
		m_DCInfoList->GetAt(srvI).DisplayAttribStatus();		

	}
	dbg.trace(L"cObjSyncChk::PrintResult",L"======================");
	dbg.print(L"cObjSyncChk::PrintResult",L"");
	dbg.print(L"cObjSyncChk::PrintResult",L"Run \"rodcutil ObjSyncChk\" to check Legend");

	return 0;
}

// Compares this two servers. Normaly Ref will be the reference server
int cObjSyncChk::ComprareServers(int refI, int srvI)
{
	//int retI=ERR_SUCCESS;		
	//CString a3;
	CString refStr, srvStr;
	int retI=0;
	int srvStatusI=STATUS_EQUAL;

	dbg.trace(L"cObjSyncChk::ComprareServers",L"Comparing [%wS] [%wS]",m_DCInfoList->GetAt(refI).GetSrvName(),m_DCInfoList->GetAt(srvI).GetSrvName() );

	//display server metadata
	//m_DCInfoList->GetAt(srvI).DisplayObjMetaData();
	
	//getobjet metadata count
	//int acount = m_DCInfoList->GetAt(refI).GetAttribCount();
	int attribI = ERR_NULL;
	int acount = m_DCInfoList->GetAt(refI).GetAttribCount();
	
	
	//go throught the attribute list		
	for (int i=0 ; i < acount ; i++)
	{
		//Build reference string				
		refStr= m_DCInfoList->GetAt(m_RefServerPos ).GetAttribMetadataString(i);

		//build server String

		//find attribute in server
		attribI = m_DCInfoList->GetAt(srvI).findAttr(
			m_DCInfoList->GetAt(m_RefServerPos).GetAttribName(i));

		if (attribI == ERR_NULL)
		{
			//Status Set to Not found
			//m_DCInfoList->GetAt(srvI).SetStatus(STATUS_NOT_EQUAL);
			srvStatusI=STATUS_NOT_EQUAL;
			//CAn't update statusof an object that is not found
			//m_DCInfoList->GetAt(srvI).SetAttribStatus(m_DCInfoList->GetAt(m_RefServerPos ).GetAttribName(i),STATUS_NOT_FOUND);
		}	
		else
		{
			srvStr = m_DCInfoList->GetAt(srvI).GetAttribMetadataString(m_DCInfoList->GetAt(m_RefServerPos ).GetAttribName(i));
			
			//Compare strings
			//retB= refStr.Compare(srvStr)?false:true;		
			bool retB = ! refStr.Compare(srvStr);		
			
			//ignore cn,dc,ou attributes as they will be diferent between by servers by design.
			if (!retB && 
				(m_DCInfoList->GetAt(m_RefServerPos).GetAttribName(i)!= "cn") &&  
				(m_DCInfoList->GetAt(m_RefServerPos).GetAttribName(i)!= "dc")&&
				(m_DCInfoList->GetAt(m_RefServerPos).GetAttribName(i)!= "ou"))
			{
				//Set Server status not equal.
				//m_DCInfoList->GetAt(srvI).SetStatus(STATUS_NOT_EQUAL);
				srvStatusI=STATUS_NOT_EQUAL;
				m_DCInfoList->GetAt(srvI).SetAttribStatus(m_DCInfoList->GetAt(m_RefServerPos ).GetAttribName(i),STATUS_NOT_EQUAL);

			}
			else
			{	
				//m_DCInfoList->GetAt(srvI).SetStatus(STATUS_EQUAL);
				 m_DCInfoList->GetAt(srvI).SetAttribStatus(m_DCInfoList->GetAt(m_RefServerPos ).GetAttribName(i),STATUS_EQUAL);
				 
			}
		}
	}
		
	//update server status
	m_DCInfoList->GetAt(srvI).SetStatus(srvStatusI);
	return 0;
}

