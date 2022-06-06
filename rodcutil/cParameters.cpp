#include "StdAfx.h"
#include <cstringt.h>
#include <afxtempl.h>
#include <afxcoll.h>
#include "cDbgHelper.h"
#include "cParameters.h"



cParameters::cParameters(void)
{
	
}


int cParameters::Initialize(int argc, _TCHAR* argv[])

{
	int i ;
	CString strAux;

	g_GLE = ERROR_SUCCESS;
	
	//Init Commands array
	m_CommandsArray= new CStringArray;
	
	// Adding elements of commands list 
	m_CommandsArray->Add(L"NULL or WRONG command");		//0
	m_CommandsArray->Add(L"objsyncchk");				//1
	m_CommandsArray->Add(L"dclist");					//2
	m_CommandsArray->Add(L"help");						//3
	m_CommandsArray->Add(L"experthelp");				//4
	m_CommandsArray->Add(L"dsaguidchk");				//5
	


	//read all the parameters
	m_ParamArray = new CStringArray;
	//for (i=0;i <= MAX_PARAM_COUNT && i < argc-1;i++)
	// objsyncchk  CN=LdapUser,CN=Users,DC=ra03dom,DC=com  dc.ra03dom.com ms.ra03dom.com
	
	//read all the parameters
	for (i=1; i < argc;i++)
	{
		m_ParamArray->Add(argv[i]);	
	}
	
	//getting options parameters
	for (i=0;(i<m_ParamArray->GetCount());i++)
	{
		if (m_ParamArray->GetAt(i).GetAt(0)== '/')
		{
			if (RecordOption(i)!= ERROR_SUCCESS)
			{
				g_GLE = ERR_NO_SUCH_OPTION;
				goto end;
			}
			else
				i--;

		}		
	}

	//Get Command and set Command ID
	if (m_ParamArray->GetCount()>0)
	{
		strAux= m_ParamArray->GetAt(0);
		strAux.MakeLower();
		//compare argv1 (command) with m_CommandsArray and stop when coincidence.
		for (i=0;(i<m_CommandsArray->GetCount());i++)
		{
			if (m_CommandsArray->GetAt(i)== strAux )
				break;		
		}

		//Wrong parameter
		if (i == m_CommandsArray->GetCount())
		{
			m_Command_ID = 0;		
			g_GLE = ERR_WRONG_PARAMETERS;		
		}
		else
			m_Command_ID = i;		
		
	}
	else 
	{
		//No command specfied
		m_Command_ID = 0;
		g_GLE= ERR_NULL_PARAMETERS;
		//goto end;
	}

	//dbg.trace(L"cParameters::Initialize",L"m_Command_ID = %d  [ %wS ]" ,m_Command_ID,m_CommandsArray->GetAt(m_Command_ID));		

end:
	return g_GLE;
}


cParameters::~cParameters(void)
{
	dbg.trace(L"cParameters::~cParameters",L"Deleteing Parameters object");
	delete m_CommandsArray;
	delete m_ParamArray;
}

int cParameters::GetCommand()
{
	//int i=0;

	//if (m_ParamCount > 1)
	if (m_ParamArray->GetCount()> 0)		
	{
		//find string in command list
	
	}
	else 
		m_Command_ID=0;	
	
	return m_Command_ID;	
}

CString cParameters::Command(void)
{
	return m_CommandsArray->GetAt(m_Command_ID);	
}

// display the actual paramters
void cParameters::DisplayParameters(void)
{
	//cDbgHelper dbg; 
	int count = (int) m_ParamArray->GetCount();
	
	//command running
	dbg.trace(L"cParameters::DisplayParameters",    L"m_Command_ID = %d  [ %wS ]" ,m_Command_ID,m_CommandsArray->GetAt(m_Command_ID));		
	
	//parameters
	for (int i = 0; i < count ;i++)
		dbg.trace(L"cParameters::DisplayParameters",L"Parameter %d = %wS",i,m_ParamArray->GetAt(i));

	//flags
	CString a1;
	a1.Empty();
/*
#define FLAG_HELP						0x1
#define FLAG_CSV						0x2
#define FLAG_QUITE_MODE					0x4
#define FLAG_2K3_COMPATIBILITY_MODE		0x8  
#define FLAG_NOLOGO						0x10

*/
	if (g_FLAGS & FLAG_HELP )					a1 += L"FLAG_HELP ";
	if (g_FLAGS & FLAG_CSV )					a1 += L"FLAG_CSV ";
	if (g_FLAGS & FLAG_QUITE_MODE )				a1 += L"FLAG_QUITE_MODE ";
	if (g_FLAGS & FLAG_2K3_COMPATIBILITY_MODE ) a1 += L"FLAG_2K3_COMPATIBILITY_MODE ";
	if (g_FLAGS & FLAG_NOLOGO )					a1 += L"FLAG_NOLOGO ";
	if (g_FLAGS & FLAG_VERBOSE )				a1 += L"FLAG_VERBOSE ";

	
	
	dbg.trace(L"cParameters::DisplayParameters",    L"g_FLAGS      = %d [ %wS]",g_FLAGS, a1);
	
	return ;
}



// returns the paramter in ord position
CString cParameters::GetParam(int ord)
{
	CString aux;
	aux.Empty();
	
	if ((ord >= 0)&& (ord < m_ParamArray->GetCount()))
	{
		aux =m_ParamArray->GetAt(ord);
	}
	
	return aux;
}

// //return the number of paramters available
int cParameters::GetCount(void)
{
//	return m_ParamCount;
	return (int) m_ParamArray->GetCount();
}

// //display tool help text
void cParameters::DisplayHelp(void)
{
	//cDbgHelper dbg;
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"[Syntax]");
	//dbg.dataPrint(L"cParameters::DisplayHelp",L"          rodcutil.exe [options] { DcList | ObjSyncChk | DsaGuidChk | help | experthelp } [ Parameters ] ");
	dbg.dataPrint(L"cParameters::DisplayHelp",L"          rodcutil.exe [options] { DcList | ObjSyncChk | DsaGuidChk | Help } [ Parameters ] ");
	//dbg.dataPrint(L"DisplayHelp",L"");	
//	dbg.dataPrint(L"DisplayHelp",L"For extra help run :> rodcutil.exe help ");	
	//dbg.print(L"DisplayHelp",L"          rodcutil.exe help ");	
	//dbg.print(L"DisplayHelp",L"");	
	/*dbg.print(L"DisplayHelp",L"options:");	
	dbg.print(L"DisplayHelp",L"\t /user <user> /password <password> [/domain <domain>]");	
	dbg.print(L"DisplayHelp",L"\t /verbose");	
	dbg.print(L"DisplayHelp",L"\t /colors");
	*/
	dbg.print(L"cParameters::DisplayHelp",L"");	

	return;
}

// //record option to the param object
int cParameters::RecordOption(int  i)
{
	//dbg.trace(L"cParameters::RecordOption",L"Recording option [%wS]",m_ParamArray->GetAt(i));
	
	//allow updper and lower case in options
	CString strAux;
	strAux=m_ParamArray->GetAt(i);
	strAux.MakeLower();
	
	//go though the options
	if ((m_ParamArray->GetAt(i) == L"/user")|| (m_ParamArray->GetAt(i) == L"/u"))
	{
		if (i+1 < m_ParamArray->GetCount())
		{
			g_Cred.SetUser(m_ParamArray->GetAt(i+1));
			m_ParamArray->RemoveAt(i+1);
			goto success;
		}
	}

	if ((strAux == L"/password")|| (strAux == L"/p"))
	{
		if (i+1 < m_ParamArray->GetCount())
		{
			g_Cred.SetPassword( GetParam(i+1));
			m_ParamArray->RemoveAt(i+1);
			goto success;
		}		
	}
	
	if ((strAux == L"/domain") || (strAux == L"/d"))
	{
		if (i+1 < m_ParamArray->GetCount())
		{
			g_Cred.SetDomain( m_ParamArray->GetAt(i+1));
			m_ParamArray->RemoveAt(i+1);
			goto success;
		}
	}

	if ((strAux == L"/verbose") || (strAux == L"/v"))
	{
		dbg.m_debug_level = DBG_TRACE;
		g_FLAGS = g_FLAGS | FLAG_VERBOSE;
		goto success;
	}
	
	//if ((strAux == L"/colors") || (strAux == L"/c"))
	if ((strAux == L"/colors") || (strAux == L"/c"))
	{
		dbg.m_UseColors=true;
		goto success;
	}

	if ((strAux == L"/2k3") || (strAux == L"/2003"))
	{
		g_FLAGS = g_FLAGS | FLAG_2K3_COMPATIBILITY_MODE;
		
		goto success;
	}

	if ((strAux == L"/nologo") || (strAux == L"/n"))
	{
		g_FLAGS = g_FLAGS | FLAG_NOLOGO;
		
		goto success;
	}

	if ((strAux == L"/?") )
	{
		g_FLAGS = g_FLAGS | FLAG_HELP;
		
		goto success;
	}

	if ((strAux == L"/csv"))
	{
		g_FLAGS = g_FLAGS | FLAG_CSV;
		
		goto success;
	}

	if ((strAux == L"/quite") || (strAux == L"/q"))
	{
		g_FLAGS = g_FLAGS | FLAG_QUITE_MODE;
		
		goto success;
	}

	return ERR_NO_SUCH_OPTION;

success:
	//remove option from the list
	m_ParamArray->RemoveAt(i);	

	return ERROR_SUCCESS;
}

cCred::cCred(void)
{
	dbg.trace(L"cCred::cCred",L"Creatig Cred object");
	//init object memebers
	m_Password.Empty();
	m_User.Empty();
	m_Domain.Empty();
	m_Exist=false;
}

cCred::~cCred(void)
{
	dbg.trace(L"cCred::~cCred",L"Deleting Cred object");
}

// sets user 
void cCred::SetUser(CString user)
{	
	m_User=user;

	if (m_Domain.IsEmpty())
	{
		m_Domain= _wgetenv(L"USERDNSDOMAIN");
		//m_Domain= L"kk";
	}

	m_Exist = !m_Password.IsEmpty();
}

// SetPassword
void cCred::SetPassword(CString password)
{
	m_Password=password;
	m_Exist = !m_User.IsEmpty();
}

// returns true if cred information where previosly saved
bool cCred::Exist(void)
{
	return m_Exist;
}

// return user value
CString cCred::GetUser(void)
{
	return m_User;
}

// return password value
CString cCred::GetPassword(void)
{
	return m_Password;
}

// //set domain value
void cCred::SetDomain(CString domain)
{
	dbg.trace(L"cCred::SetDomain",L"Setting domain to [ %wS ]",domain);
	m_Domain=domain;
}

// return domain value
CString cCred::GetDomain(void)
{	
	return m_Domain;
}

// Extended root help. Focus on the options
void cParameters::DisplayRootHelp(void)
{
	//cDbgHelper dbg;
	//dbg.print(L"DisplayHelp",L"");
	dbg.dataPrint(L"cParameters::DisplayHelp",L"rodcutil.exe [options] { command }");
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"This tool is Intended to help support environment with RODC's.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"It will perform different commands to improve and help on the support of possible problems related with RODC.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"If you feel that we are missing important scenarios let us now.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"Feedback mailto:rodcutil@microsoft.com | mailto:roblane@microsoft.com;raulg@microsoft.com");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"[options]");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      [ /user <user> /password <password> [ /domain <domain> ]]");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      [ /verbose | /quite ] ");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      [ /colors ]");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      [ /nologo ]");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      [ /csv ]");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      [ /2k3 ]");
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      Options that will affect all commands.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      user        users with which all binds to DC's will me made.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      password    password with wich all binds to DC's will be made.");
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      domain      domain of the user with which all binds to DC's will be made.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      verbose     displays a much extended output. For tool troubleshooting purposes.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      quite       displays only command results and errors. No logging or processing info.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      colors      different colors will be used on the output.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      nologo      avoids the logo to be displayed .");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      csv         outputs data in csv format.");		
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      2k3         enable Windows Server 2003 Compatibility mode.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"[command]");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");
	//dbg.dataPrint(L"cParameters::DisplayHelp",L"      [ DcList | ObjSyncChk | DsaGuidChk | Help | ExpertHelp ]");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      { DcList | ObjSyncChk | DsaGuidChk | Help }");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      Available commands.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      DcList      Displays the list of DC's in the Domain.");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      ObjSyncChk  Helps to check object sync status between DC's of a domain. ");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      DsaGuidChk  Helps to trasnlates a known DC DSA GUID to DC Name. ");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"      Help        Displays this output.");	
	//dbg.dataPrint(L"cParameters::DisplayHelp",L"      ExpertHelp  Displays a more detailed help for deep use of the tool");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"");	
	dbg.dataPrint(L"cParameters::DisplayHelp",L"Run \"rodcutil.exe [command]\" for a specific command help.");	

	return;
}
