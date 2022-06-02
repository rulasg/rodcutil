#pragma once

#define  MAX_PARAM_COUNT 4

extern cDbgHelper dbg;
extern int g_GLE;
extern int g_FLAGS;
//extern cCred g_Cred;

class cCred
{

private:
	CString m_User;
	CString m_Password;
	CString m_Domain;
	bool m_Exist;

public:
	cCred(void);
	~cCred(void);
	// sets user 
	void SetUser(CString user);
	// SetPassword
	void SetPassword(CString password);
	// returns true if cred information where previosly saved
	bool Exist(void);
	// return user value
	CString GetUser(void);
	// return password value
	CString GetPassword(void);
	// //set domain value
	void SetDomain(CString domain);
	// return domain value
	CString GetDomain(void);
};

class cParameters
{
private:	
	//param content list
	//CString m_ParamArray [MAX_PARAM_COUNT];
	CStringArray *m_ParamArray;
	//param count
	//int m_ParamCount;	
	CStringArray *m_CommandsArray;
	//CString m_CommandsArray [MAX_PARAM_COUNT];
	
public:
	//cParameters(int argc, _TCHAR* argv[]);
	cParameters(void);
	~cParameters(void);
	int GetCommand(void);
	CString Command(void);
	int m_Command_ID;

	//cCred m_Cred;
	
	// display the actual paramters
	void DisplayParameters(void);
	// returns the paramter in ord position
	CString GetParam(int ord);
	// //return the number of paramters available
	int GetCount(void);
	//initializes the parame object reading parameters from OS
	int cParameters::Initialize(int argc, _TCHAR* argv[]);
	
	// //display tool help text
	void DisplayHelp(void);
private:
	// //record option to the param object
	int RecordOption(int i);
public:
	// Extended root help. Focus on the options
	void DisplayRootHelp(void);
};


