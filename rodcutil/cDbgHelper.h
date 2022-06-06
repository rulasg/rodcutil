#pragma once

#define DBG_NULL	0
#define DBG_TRACE	1

#define DBG_DEFAULT DBG_NULL

//COLOR constants
#define COLOR_RED			0xC
#define COLOR_BRIGHT_WHITE	0xF
#define COLOR_WHITE			0x7
#define COLOR_GRAY			0x8
#define COLOR_LIGHT_YELLOW	0xE
#define COLOR_LIGHT_BLUE	0x9

#define COLOR_ERROR			COLOR_RED
#define COLOR_WARNING		COLOR_LIGHT_YELLOW
#define COLOR_PRINT			COLOR_WHITE
#define COLOR_DATA_OUTPUT   COLOR_BRIGHT_WHITE
#define COLOR_TRACE			COLOR_GRAY
#define COLOR_TRACE			COLOR_GRAY
#define COLOR_LOGO			COLOR_LIGHT_BLUE

class cDbgHelper
{
private:
	HANDLE hConsole;
	int ActualConsoleStatus;
	

public:
	int m_debug_level;
	cDbgHelper(void);
	~cDbgHelper(void);
	
	// Traces functions
	int trace(CString function, CString str);
	int trace(CString function, CString str, int param1);	
	int trace(CString function, CString str, CString param1);	
	int trace(CString function, CString str, int param1, CString param2);
	int trace(CString function, CString str, int param1, int param2);
	int trace(CString function, CString str, CString param1, CString param2);
	
	// prints to screen.
	int print(CString function,CString str);
	int print(CString function,CString str,int param1);
	int print(CString function,CString str,int param1, CString param2);
	//int print(CString function,CString str,int param1, int param2);
	int print(CString function,CString str,CString param1);
	int print(CString function,CString str,CString param1,CString param2);
	int print(CString function,CString str,CString param1,CString param2,CString param3);

	// print errors
	int error(CString function,CString str);
	int error(CString function,CString str, int param1);
	int error(CString function,CString str, CString param1);
	int error(CString function,CString str, int param1,CString param2);

	//print warnings
	int warning (CString function,CString str);

	//print DataOutput
	int dataPrint(CString function,CString str);
	int dataPrint(CString function,CString str,CString param1);
	int dataPrint(CString function,CString str,CString param1,CString param2);
		
	//Print the tool logo
	int printLogo(void);

	// set color to before configuration
	void ResetColor(void);
	
	
private:
	// unique point of printing to screen.
	int output(CString str);
	
	// set font color to a predefined colr defintion . Search for COLOR_
	void SetColor(int color);
	
	// final function for print text
	int fprint(CString function,CString str);
	// final function for print text
	int fdataprint(CString function,CString str);
	// final function to write trace text
	int ftrace(CString function, CString str);
public:
	// store dbg option to use or not colors
	bool m_UseColors;
private:
	// counts the number of lines to output
	int m_lineCount;
};
