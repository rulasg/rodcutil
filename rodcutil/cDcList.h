#pragma once

#define DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K3			DS_DOMAIN_CONTROLLER_INFO_2
#define DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM_2K3		2

#define DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K8			DS_DOMAIN_CONTROLLER_INFO_3
#define DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM_2K8		3

#define DS_DOMAIN_CONTROLLER_INFO_LEVEL				DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K8
#define DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM			DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM_2K8


class cDcInfo
{
public:
	cDcInfo(void);
	~cDcInfo(void);

	CString m_DcDnsName;
	CString m_DcGUID;
	CString m_DcSiteName;
	CString m_DcNtdsName;
	bool m_fIsPdc;
	bool m_fIsGc;
	bool m_fDsEnabled;
	bool m_fIsRodc;
	int m_DcFlags;
private:

	
public:
	// Save the DcList info to DcInfoList array
	void SaveDcInfo2List(void * pDcList);

private:
	// Save DcInfo with DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM_2K3 strucutre
	void SaveDcInfo2List_2k3(DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K3 * p_DcInfo);
	// Save DcInfo with DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM_2K8 strucutre
	void SaveDcInfo2List_2k8(DS_DOMAIN_CONTROLLER_INFO_LEVEL_2K8 * p_DcInfo);
	int BuildGUIDString(CString* strGUID, GUID* pGuid);
	
public:
	// print  the dclist info 
	void DisplayDcInfo(void);
	// return the DC Dsa Guid
	CString GetDsaDcGuid(void);
};
class cDcList
{
private:
	CString m_RefDC;
	CString m_Domain;
	DWORD m_DcCount;
	// string that describes the dc's to list. Should be [* | RODC ]
	CString m_DCsToList;
	//DS_DOMAIN_CONTROLLER_INFO_LEVEL * m_pDcList;
	CArray<cDcInfo> * m_pDcInfoList;
	int m_DS_DOMAIN_CONTROLLER_INFO_LEVEL_NUM;
	void EmptyList(void);

public:
	cDcList(void);
	~cDcList(void);
	// Loads dclist to object using object memeber info
	int GetDcList(void);
	// Loads dclist to object using object memeber info
	int GetDcList(CString RefDC,CString Domain);
	// Queries DC List for DC 
	int GetDcList(CString RefDC,CString Domain, CString DCsToList);
	// bind to server
	int Bind(HANDLE * phDS);
	// unbind to server and clean strucutres
	void UnBind(HANDLE * phDS);
	// display DCList
	void DisplayDCList(void);
	// display DcList command help
	void DisplayHelp(void);
	// sets DcList object using param information
	int Initialize(cParameters* param);

	// trace the actual active parameters used by dclist
	void DisplayParameters(void);

	// return number of DC in the list
	int GetCount(void);
	// returns server name at position of aray i
	CString GetServerName(int i);
	// trasnlate bin GUId to string GUID
	//int BuildGUIDString(CString * strGUID, GUID* pGUID);
private:
	// //helper to trasnlate GUID to string
	int BuildGUIDString(CString* strGUID, GUID* pGuid);
public:
	// //return DC GUID string
	CString GetNtdsDsaObjectGuid(int i);
	// return true if the dc is a RODC
	bool isRODC(int i);
	// return true if the DC is the PDC
	bool isPDC(int i);
	// return true if the DC is GC
	bool isGC(int i);
	// returns the site name the DC resides in.
	CString GetServerSiteName(int i);
	// return the size of the biggest name of all the DcList
	int GetMaxDcNameLength(void);
	// return the length of the longest site name
	int GetMaxSiteNameLength(void);
private:
	// displays a single DC info
	//void DisplayDC(CString format, CString param1,CString param2,CString param3,CString param4);
	void DisplayDC(CString format,int i);
public:
	// return server name from guid. If guid not present servername = the imput guid
	CString Guid2Name(CString guid);
};
