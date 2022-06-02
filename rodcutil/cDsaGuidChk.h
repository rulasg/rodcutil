#pragma once
#include "cdclist.h"

class cDsaGuidChk
{
public:
	cDsaGuidChk(void);
	~cDsaGuidChk(void);
public:
	// will set all members
	int Initialize(cParameters* param);
	// returns the DC Name that has the Guid as DSAGuid. If return String == Guid error will be returned at g_GLE
	CString Guid2Name(CString guid);
private:
	// guid to look for between the DC's
	CString m_Guid;
	//server to conect to
	CString m_RefDC;
	//domain where the server belongs to
	CString m_Domain;
	// result of the dasguidchk process
	CString m_DcName;
	// stores the object state of finding the guid in Dclist
	int m_isFound;
public:
	// displays DsaGuidChk help
	void DisplayHelp(void);
	// trace parameters that are being used
	void DisplayParameters(void);
	// triggers the work using state parameters coming from Initialize funciton.
	int DsaGuidChk(void);
	// return true if object status if GUID found. Can retreive the Server name wiht GetDcName
	bool isFound(void);

	// return object status of the DcName. return the GUID if the name was not found. Check status with siFound() 
	CString GetDcName(void);
	// return the GUID used in the query
	CString GetGuid(void);
};
