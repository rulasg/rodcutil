#pragma once

extern cDbgHelper dbg;
extern int g_GLE;
extern cCred g_Cred;


class cObjSyncChk
{
private:
	// Reference server for the comapation
	CString m_RefServer;
	// Object to compare
	CString m_ObjectDN;
	// DCList to compare against RefServer
	CString m_DcToList;
	CString m_Domain;
	CArray<cSrvInfo> * m_DCInfoList;
	
public:
	cObjSyncChk(void);
	~cObjSyncChk(void);
	
	//initialize objsyncchk members
	int Initialize(cParameters *param);
	//Display the actual paramters for the ObjSyncChk
	void DisplayParameters(void);
	//print command help
	void DisplayHelp(void);
	
	// Builds the lsit of servers to chk against
	int CreateDcInfoList(void);
	// Compare obj within all the DClist against RefSrv
	int ObjSynChk(void);
private:
	// compare attrib metadata of two server
	bool AttribEqual(DS_REPL_ATTR_META_DATA* pAttrib1, DS_REPL_ATTR_META_DATA* pAttrib2);
public:
	// translates bin GUID to String GUID
	int BuildGUIDString(CString* strGUID, GUID* pGUID);
	// will print restult of the objsyncchk. Will be diferent depending in modes we are (/Q /CSV)
	int PrintResult(void);
private:
	int m_RefServerPos;
	// Compares this two servers. Normaly Ref will be the reference server
	int ComprareServers(int refI, int srvI);

};
