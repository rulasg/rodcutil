
#pragma once

#include <ntdsapi.h>
//#include "E:\LH\NT\sdpublic\sdk\inc\ntdsapi.h"
///#include "ntdsapi.h"
#include "resource.h"
#include "cDbgHelper.h"
#include "cParameters.h"
#include "cAttribInfo.h"
#include "cSrvInfo.h"


//FLAGS
/*Best Effort to avoid error due to 2k3 compat. 
  Nevertheless changes are very deep in the code and probably very little use will be taken out from it
*/
#define FLAG_HELP						0x1
#define FLAG_CSV						0x2
#define FLAG_QUITE_MODE					0x4
#define FLAG_2K3_COMPATIBILITY_MODE		0x8  
#define FLAG_NOLOGO						0x10
#define FLAG_VERBOSE					0x20

//****** Remember to update cParameters::DisplayParameters for Flags tracing aoutput.



//ERRORS

#define ERR_NULL			   -1 // error not set yet (this value has to be < 0)
#define ERR_SUCCESS			    0 // execution with success
#define ERR_FAIL			    1 // failed with error 

#define ERR_NULL_PARAMETERS		2
#define ERR_WRONG_PARAMETERS	3 
#define ERR_STRUCT_NOT_INIT		4
#define ERR_NO_SUCH_OPTION		5
#define ERR_NOT_ENOUGH_MEMORY	6
#define ERR_READING_OBJECT_INFO 7
#define ERR_OBJ_NOT_PRESENT	    8  //failed with error 
#define ERR_ATTRIB_NOT_FOUND	9

//Status
#define STATUS_NULL					0
#define STATUS_FOUND				1
#define STATUS_NOT_FOUND			2
#define STATUS_NOT_SEARCHED			3
#define STATUS_NOT_INITIALIZED		4
#define STATUS_ERROR				5
#define STATUS_NOT_FOUND_IN_REF_DC	6
#define STATUS_EQUAL				7
#define STATUS_NOT_EQUAL			8
#define STATUS_INITIALIZED			9
#define STATUS_REF_INFO				10


// build in strings

#define NULL_NAME		L"null"
#define NULL_GUID		L"00000000-0000-0000-0000-000000000000"
#define NULL_DATE		L"(never)"  //"0000-00-00 00:00:00"
#define NULL_VERSION	L"-"
#define NULL_USN		L"-"
#define SEPARATOR 		L"," //separator used on CSV mode
