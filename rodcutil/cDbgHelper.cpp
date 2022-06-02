#include "StdAfx.h"
#include "cDbgHelper.h"

cDbgHelper::cDbgHelper(void)
: m_UseColors(false)
, m_lineCount(0)
{	
	
	CONSOLE_SCREEN_BUFFER_INFO Info;
	m_debug_level = DBG_DEFAULT; 

	//setting console color attributes
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole,&Info);
	ActualConsoleStatus=Info.wAttributes;
}

cDbgHelper::~cDbgHelper(void)
{
	ResetColor();
}

int cDbgHelper::output(CString str)
{
	wprintf_s(L"%wS\n",str);
	return 0;
}

// Standard output function that takes 1 parameter
int cDbgHelper::trace(CString function, CString str)
{
	SetColor(COLOR_TRACE);
	ftrace(function,str);	
	return 0;
}


int cDbgHelper::trace(CString function, CString str, CString param1)
{
	CString aux;
	aux.Format(str,param1);
	trace(function,aux);	
	return 0;
}

int cDbgHelper::trace(CString function, CString str, int param1)
{
	CString aux;
	aux.Format(str,param1);
	trace(function,aux);
	return 0;
}

/*
int cDbgHelper::trace(CString function, int param1, int param2)
{
	CString aux;
	aux.Format(str,param1,param2);
	trace(function,aux);
	return 0;
}
*/


int cDbgHelper::trace(CString function,CString str,int param1, CString param2)
{
	CString aux;
	aux.Format(str,param1,param2);
	trace(function,aux);	
	return 0;	
}

               
int cDbgHelper::trace(CString function,CString str,int param1,int param2)
{
	CString aux;
	aux.Format(str,param1,param2);
	trace(function,aux);	
	return 0;	
}


int cDbgHelper::trace(CString function,CString str,CString  param1, CString param2)
{
	CString aux;
	aux.Format(str,param1,param2);
	trace(function,aux);	
	return 0;	
}

// final function to write trace text
int cDbgHelper::ftrace(CString function, CString str)
{
	CString strAux;	

	strAux.Format(L"[%-40wS] %wS",function,str);

	if (m_debug_level && DBG_TRACE)
	{ 		
		output(strAux);
	}
	TRACE(strAux);
	return 0;
}


// prints to screen.

// final function for print text
int cDbgHelper::fprint(CString function,CString str)
{	
	//Will print only if !DBG and !QUITE and will call ftrace always.

	//Will output to std output if debug and quite are off.
	if(!((m_debug_level & DBG_TRACE) || (g_FLAGS & FLAG_QUITE_MODE))) 
			output(str);
	
	//Will always call ftrace to trace though dbg chanell if loaded
	ftrace(function,str);
	return 0;
}


int cDbgHelper::fdataprint(CString function,CString str)
{	
	if (g_FLAGS & FLAG_CSV)
	{
		//CString aux;
		str.Format(L"%d,%wS",m_lineCount++,str);
	}
		
	//Will call output if dbg is off .	
	if(!(m_debug_level & DBG_TRACE) ) 
		output(str);
	
	//Will always call ftrace to trace though dbg chanell if loaded
	ftrace(function,str);
	return 0;
}


int cDbgHelper::print(CString function,CString str)
{	
	
	SetColor(COLOR_PRINT);
	fprint(function,str);
	return 0;
}

int cDbgHelper::print(CString function,CString str,int param1)
{
	CString strAux;
	
	strAux.Format(str,param1);
	
	print(function,strAux);
	
	return 0;
}

int cDbgHelper::print(CString function,CString str,CString param1)
{
	CString strAux;
	
	strAux.Format(str,param1);

	print(function,strAux);
	
	return 0;
}
int cDbgHelper::print(CString function,CString str,CString param1,CString param2)
{
	CString strAux;

	strAux.Format(str,param1,param2);

	print(function,strAux);
	
	return 0;
}

int cDbgHelper::print(CString function,CString str,CString param1,CString param2,CString param3)
{
	CString strAux;

	strAux.Format(str,param1,param2,param3);

	print(function,strAux);
	
	return 0;
}

int cDbgHelper::print(CString function,CString str,int param1,CString param2)
{
	CString strAux;

	strAux.Format(str,param1,param2);
	
	print(function,strAux);
	
	return 0;
}

// //Print the tool logo
int cDbgHelper::printLogo(void)
{   
	if (! (g_FLAGS & FLAG_NOLOGO))
	{
		//fprint(L"printLogo",L"rodcutil v1.0 2007 by roblane & raulg. ");
		
		CString aux;

		SetColor(COLOR_LOGO);

		aux.LoadStringW(IDS_APP_TITLE);
		fprint(L"printLogo",aux);

		//aux.LoadStringW(IDS_LOGO_STRING_1);
		//fprint(L"printLogo",aux);

		aux.LoadStringW(IDS_LOGO_STRING_2);
		fprint(L"printLogo",aux);

		aux.LoadStringW(IDS_LOGO_STRING_3);
		fprint(L"printLogo",aux);
	
		//fprint(L"printLogo",L"");
	}

	return 0;
}

// print errors
int cDbgHelper::error(CString function,CString str, int param1)
{
	CString strAux;
	strAux.Format(str,param1);
	error(function,strAux);	
	return 0;
}

int cDbgHelper::error(CString function,CString str, CString param1)
{
	CString strAux;
	strAux.Format(str,param1);
	error(function,strAux);	
	return 0;
}

int cDbgHelper::error(CString function,CString str, int param1,CString param2)
{
	CString strAux;
	strAux.Format(str,param1,param2);
	error(function,strAux);	
	return 0;
}

int cDbgHelper::error(CString function,CString str, CString param1,CString param2)
{
	CString strAux;
	strAux.Format(str,param1,param2);
	error(function,strAux);	
	return 0;
}

int cDbgHelper::error(CString function,CString str)
{
	SetColor(COLOR_ERROR);
	//check if errors should be displayed on QUITE mode or not.
	//fdataprint(function,str); //displayed on quite mode
	fprint(function,str);		//NOT displayed on quite mode
	ResetColor();
	return 0;
}

int cDbgHelper::warning(CString function,CString str)
{
	SetColor(COLOR_WARNING);
	fprint(function,str);
	ResetColor();
	return 0;
}






int cDbgHelper::dataPrint(CString function,CString str)
{
	SetColor(COLOR_DATA_OUTPUT);
	fdataprint(function,str);
	ResetColor();
	return 0;
}

int cDbgHelper::dataPrint(CString function,CString str,CString param1)
{
	CString strAux;

	strAux.Format(str,param1);

	dataPrint(function,strAux);
	
	return 0;
}

int cDbgHelper::dataPrint(CString function,CString str,CString param1,CString param2)
{
	CString strAux;

	strAux.Format(str,param1,param2);

	dataPrint(function,strAux);
	
	return 0;
}


//COLOR FUNCTIONS

// //set font color to a predefined colr defintion . Search for COLOR_
void cDbgHelper::SetColor(int color)
{
	//if Use color is not set ignore change colors calls.
	if (!m_UseColors)
		return;
/*

#define FOREGROUND_BLUE            0x0001 001 0000 0000 0001
#define FOREGROUND_GREEN           0x0002 002 0000 0000 0010
#define FOREGROUND_RED             0x0004 004 0000 0000 0100
#define FOREGROUND_INTENSITY       0x0008 008 0000 0000 1000
#define BACKGROUND_BLUE            0x0010 010 0000 0001 0000
#define BACKGROUND_GREEN           0x0020 020 0000 0010 0000
#define BACKGROUND_RED             0x0040 040 0000 0100 0000
#define BACKGROUND_INTENSITY       0x0080 080 0000 1000 0000
#define COMMON_LVB_LEADING_BYTE    0x0100 100 0001 0000 0000
#define COMMON_LVB_TRAILING_BYTE   0x0200 200 0010 0000 0000
#define COMMON_LVB_GRID_HORIZONTAL 0x0400 400 0100 0000 0000
#define COMMON_LVB_GRID_LVERTICAL  0x0800 800 1000 0000 0000
#define COMMON_LVB_GRID_RVERTICAL  0x1000 
#define COMMON_LVB_REVERSE_VIDEO   0x4000 
#define COMMON_LVB_UNDERSCORE      0x8000 

*/
	//int a =0; //actual
	//int b =0; // background
	//int f =0; // Foreground
	//int m =0; // mask
	int o =0; // Other attributes
	int ev=0; // final attribute value
	
	//int valf=0;
	//int valb=0;
	
	/*b = b + ActualInfo & BACKGROUND_BLUE ;
	b = b + ActualInfo & BACKGROUND_GREEN ;
	b = b + ActualInfo & BACKGROUND_RED ;
	b = b + ActualInfo & BACKGROUND_INTENSITY ;*/
	
	// b = ActualInfo & (BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);
	// f = ActualInfo & (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		
	//f = ActualInfo & 0x000f;
	//b = ActualInfo & 0x00f0;
	o = ActualConsoleStatus & 0xff00;
		
	//calculate val values
	//valf = val & 0x000f;
	//valb = val & 0x00f0;	
	
	//Keep other attributes.		
	ev = o ;	

	//Adding color
	ev+=o+color;

	//change color 
	SetConsoleTextAttribute(hConsole, ev);

	return;

}

// //set color to before configuration
void cDbgHelper::ResetColor(void)
{
	SetConsoleTextAttribute(hConsole, ActualConsoleStatus);
}

