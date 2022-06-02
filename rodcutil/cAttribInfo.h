#pragma once


class cAttribInfo
{
public:
	cAttribInfo(void);
	~cAttribInfo(void);
	// defines the status of the attrib.
	int m_Status;
private:

	CString m_OrgGUID;		
	CString m_OrgUSN;
	CString m_DateChange;
	CString m_Version;
	CString m_Name;

public:
	CString OrgGUID(void);
	CString Version(void);
	CString DateChange(void);
	CString OrgUSN(void);
	CString Name(void);
	
	void setOrgGUID(CString s);
	void setVersion(CString s);
	void setDateChange(CString s);
	void setOrgUSN(CString s);
	void setName(CString s);
	
	// saves metadata info to object
	int setAttribInfo(DS_REPL_ATTR_META_DATA data);
	
	int BuildGUIDString(CString *strGUID, GUID* pGuid);
	
	void SetStatus(int status);
	// return status TAG
	CString GetStatusTag(void);
};
