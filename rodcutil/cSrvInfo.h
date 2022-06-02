#pragma once

extern cDbgHelper dbg;
extern int g_GLE;
extern cCred g_Cred;

//#define METADATA_FORMAT_OUTPUT     L"       %-36wS  %-11wS  %-19wS  %5wS  %-wS"
//#define METADATA_FORMAT_HEADER     L"       %wS"
#define METADATA_FORMAT_OUTPUT     L"%-36wS  %-11wS  %-19wS  %5wS  %-wS"
#define METADATA_FORMAT_HEADER     L"%wS"
#define METADATA_CSV_FORMAT        L"%wS,%wS,%wS,%wS,%wS"

class cSrvInfo
{
private:
	CString m_SrvName;
	CString m_DsaGUID;
	CString m_ObjectDN;		
	//DS_DOMAIN_CONTROLLER_INFO_2 * m_pDCInfo;
	//DS_REPL_OBJ_META_DATA * m_pReplMetaData;	
	//DS_REPL_OBJ_META_DATA * pReplMetaData;	
	// number of attributes of the metadata list
	int m_AttribCount;
	//defines if this srv is equal or not to the reference.
//	bool m_isEqual;
	int m_Status;	
	CArray<cAttribInfo> * m_pAttribInfoList;
		
public:

	cSrvInfo(void);
	~cSrvInfo(void);

	// initialize srvInfo Object loading SrvName information
	int Initialize(CString SrvName);
	// triggers to update replication information with metadata for objStr object
	int GetObjMetadata(CString objStr);
	// returns SrvName 
	CString GetSrvName(void);
	// Display actual metadata for that server
	int DisplayObjMetaData(void);
	// format a UUID into human GUID format
	int BuildGUIDString(CString *strGUID, GUID* pGuid);
	// returns pointer to a attribute metadata strucutre
	//DS_REPL_ATTR_META_DATA * GetAttribReplInfo(CString AttribName);

	// return number of attib in metadata
	int GetAttribCount(void);
	// returns the string usedfor attribute comparation
	CString GetAttribMetadataString(int i);
	CString GetAttribMetadataString(CString attrName);

	// returns m_isEqual value
	bool isEqual(void);

	//return the value of m_Status
	int GetStatus(void);

	// return the name of the attrib at that position
	CString GetAttribName(int i);
	// return the lanth of the longest attribute name
	int GetMaxAttrNameLength(void);
	// set GUID value to the server
	void SetDsaGUID(CString guid);
	
	//return position if metadata for that attribute exists, ERR_NULL if not present
	int findAttr(CString attrName);
	// return the attrib originator guid
	CString GetAttribGUID(int i);
	// return the attrib version metadata
	CString GetAttribVersion(int i);
	// displays Attrib metadata prefixing preString to the output
	int DisplayAttribMetaData(int i,bool isTrace=false, CString preString =L"");
	int DisplayAttribMetaData(CString AttribName ,bool isTrace=false, CString preString =L"");
	// return object DN string
	CString GetObjectDN(void);
	// print the server status
	int DisplayServerStatus(void);
	// sets object status
	void SetStatus(int status);

private:

	// return attribute date
	CString GetAttribDate(int i);
	// return the attrib metadata USN
	CString GetAttribUSN(int i);
	int DisplayAttribMetaDataString(CString AttribString,bool isTrace=false, CString preString =L"");

public:
	// sets comp attrib status
	int SetAttribStatus(CString AttribName, int Status);
	int m_findCacheID;
	// adds elemento to the attrib list
	int AddAttrib(void);
	int AddAttrib(CString attribName);
private:
	cAttribInfo GetAttrib(int i);
public:		
	// print the attrib status based on outputs switches
	int DisplayAttribStatus(void);

	// return attrib status
	int GetAttribStatus(int i);
	// print header for Attribstats output.
	int DisplayAttribStatusHeader(void);
};
