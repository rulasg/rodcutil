// rodcutil.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "helper.h"
//#include "cDbghelper.h"
//#include "cParameters.h"

#include "cDcList.h"
#include "cSyncChk.h"
#include "cDsaGuidChk.h"
#include "rodcutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

using namespace std;

CWinApp theApp;
cDbgHelper dbg;
int g_GLE = ERR_SUCCESS;
int g_FLAGS = 0;
cCred g_Cred;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	int retI = ERR_SUCCESS;
	cObjSyncChk *pObjSyncchk = NULL;	
	cParameters *param =NULL;
	cDcList *pDcList= NULL;
	cDsaGuidChk * pDsaGuidChk = NULL;

	// initialize MFC and trace and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{	
		param = new cParameters;
		
		//Printing logo
		//dbg.printLogo(); can't print logo here as it will not be colored. 
		//Need to be after initialize param structure.

		//create object check
		if (param)
		{

			//initializae and read parameters
			retI = param->Initialize(argc,argv);
			
			//Print Logo
			dbg.printLogo();

			if (!g_FLAGS & FLAG_CSV)
				dbg.dataPrint(L"Main",L"");

			if (retI){
				
				//ir wrong parameters
				if (g_GLE == ERR_WRONG_PARAMETERS)
				{
					dbg.error(L"Main",L"Error wrong parameters");
					dbg.error(L"Main",L"[ %wS ] is not a valid command",param->GetParam(0));
				}
				
				param->DisplayHelp();
				goto end;
			}

		}
		else
		{			
			g_GLE = ERR_NOT_ENOUGH_MEMORY;
			dbg.error(L"Main",L"Error not enough memory [ %d ]",g_GLE);
			goto end;
		}
		
		param->DisplayParameters();
				
		dbg.trace(L"Main",L"Command : %wS",param->Command());			

		switch (param->GetCommand())
		{

		case 0 :  //No command

			//display help
			param->DisplayHelp();

			break;

		case 1 :  //objsyncchk

			//Create instance of sysnchk object
			pObjSyncchk = new (cObjSyncChk);

			//create object check
			if (!pObjSyncchk) 					
			{
				g_GLE = ERR_NOT_ENOUGH_MEMORY;
				dbg.error(L"Main",L"Error not enough memory [ %d ]",g_GLE);
				break;
			}
			
			//Init objsyncchk with parameters and show if error			
			if (pObjSyncchk->Initialize(param)!=ERR_SUCCESS)
			{
				pObjSyncchk->DisplayHelp();
				break;
			}
			
			//trace parametrs to work with
			pObjSyncchk->DisplayParameters();

			//make the work
			pObjSyncchk->ObjSynChk();

			//clean up
			delete pObjSyncchk;
			
			break;
		
		case 2: //dclist
			
			//cDcList *pDcList= NULL;			
			
			//Create instance of DcList object
			pDcList= new (cDcList);

			//create object check
			if (!pDcList) 					
			{
				g_GLE = ERR_NOT_ENOUGH_MEMORY;
				dbg.error(L"Main",L"Error not enough memory [ %d ]",g_GLE);
				break;
			}

			//check parameters
			if (pDcList->Initialize(param)!=ERR_SUCCESS)
			{
				pDcList->DisplayHelp();
				break;
			}
			
			//trace parametes to work with
			pDcList->DisplayParameters();

			//do the work
			if (!pDcList->GetDcList())
				pDcList->DisplayDCList();
			
			//clean up
			delete pDcList;

			break;
		case 3 : //help
			param->DisplayRootHelp();
			break;

		case 4:  //experthelp
			dbg.print(L"Main",L"Pending to develope a full description of the tool, troubleshooting and errors");

			break;

		case 5: //DsaGuidChk
			//Create instance of DcList object
			pDsaGuidChk= new (cDsaGuidChk);

			//create object check
			if (!pDsaGuidChk) 					
			{
				g_GLE = ERR_NOT_ENOUGH_MEMORY;
				dbg.error(L"Main",L"Error not enough memory [ %d ]",g_GLE);
				break;
			}

			//check parameters
			if (pDsaGuidChk->Initialize(param)!=ERR_SUCCESS)
			{
				pDsaGuidChk->DisplayHelp();
				break;
			}
			
			//trace parametes to work with
			pDsaGuidChk->DisplayParameters();

			//do the work			
			if (pDsaGuidChk->DsaGuidChk() == ERR_SUCCESS)
			{	
				dbg.print (L"Main",L"");
				if (pDsaGuidChk->isFound())
					//dbg.dataPrint (L"Main",L"%wS - %wS",pDsaGuidChk->GetDcName(),pDsaGuidChk->GetGuid());
					dbg.dataPrint (L"Main",L"%wS",pDsaGuidChk->GetDcName());
				else
					dbg.dataPrint (L"Main",L"No Server found for %wS",pDsaGuidChk->GetGuid());
			}
			else
			{
				dbg.print (L"Main",L"");			
				dbg.error(L"Main",L"Error trynig to resolve GUID. Check paramters.");
			}


			//clean up
			delete pDsaGuidChk;

			break;

			
		default:
			dbg.error(L"Main",L"ERROR: Command unknown");
			param->DisplayHelp();
			break;
		}
end:
		//remove parameters object
		if (param)
			delete param;	
		dbg.ResetColor();
	}	
	
	return nRetCode;
}
