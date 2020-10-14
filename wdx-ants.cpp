// filesys.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "time.h"
#include "contentplug.h"
#include <Windows.h>

#define _detectstring ""
#define fieldcount 2
char* fieldnames[fieldcount]={"Test string", "Test int32"};
int fieldtypes[fieldcount]={ft_string,ft_numeric_32};
char* fieldunits_and_multiplechoicestrings[fieldcount]={"", ""};
//int fieldflags[fieldcount]={0, 0};
//int sortorders[fieldcount]={1, 1};

/*char* multiplechoicevalues[3]={
	"file","folder","reparse point"
};*/

BOOL GetValueAborted=false;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if (ul_reason_for_call==DLL_PROCESS_ATTACH) {
		OSVERSIONINFO vs;
		srand( (unsigned)time( NULL ) );
		// Bugfix for creators update: Make sure that Uiautomationcore.dll doesn't get unloaded
		vs.dwOSVersionInfoSize=sizeof(vs);
		GetVersionEx(&vs);
		if (vs.dwMajorVersion>=10) {
			UINT oldmode=SetErrorMode(0x8001);
			LoadLibraryEx(TEXT("Uiautomationcore.dll"),NULL,0x800);// load from System32 only
			SetErrorMode(oldmode);
		}
	}
    return TRUE;
}

char* strlcpy(char* p,const char* p2,int maxlen)
{
    if ((int)strlen(p2)>=maxlen) {
        strncpy(p,p2,maxlen);
        p[maxlen]=0;
    } else
        strcpy(p,p2);
    return p;
}

TCHAR* _tcslcpy(TCHAR* p,const TCHAR* p2,int maxlen)
{
    if ((int)_tcslen(p2)>=maxlen) {
        _tcsncpy(p,p2,maxlen);
        p[maxlen]=0;
    } else
        _tcscpy(p,p2);
    return p;
}

/*int __stdcall ContentGetDetectString(char* DetectString,int maxlen)
{
	strlcpy(DetectString,_detectstring,maxlen);
	return 0;
}*/

// Mandatory (must be implemented)
int __stdcall ContentGetSupportedField(int FieldIndex,char* FieldName,char* Units,int maxlen)
{
	if (FieldIndex==10000) {
		strlcpy(FieldName,"Compare as text",maxlen-1);
		Units[0]=0;
		return ft_comparecontent;
	}
	if (FieldIndex==10001) {
		strlcpy(FieldName,"Compare as text, ignore case",maxlen-1);
		Units[0]=0;
		return ft_comparecontent;
	}
	if (FieldIndex<0 || FieldIndex>=fieldcount)
		return ft_nomorefields;

	strlcpy(FieldName,fieldnames[FieldIndex],maxlen-1);
	strlcpy(Units,fieldunits_and_multiplechoicestrings[FieldIndex],maxlen-1);
	
	return fieldtypes[FieldIndex];
}

// Mandatory (must be implemented)
int __stdcall ContentGetValue(TCHAR* FileName,int FieldIndex,int UnitIndex,void* FieldValue,int maxlen,int flags)
{
	WIN32_FIND_DATA fd;
	FILETIME lt;
	SYSTEMTIME st,st2;
	HANDLE fh;
	DWORD handle;
	DWORD dwSize,dayofyear;
	int i;
	__int64 filesize;
	GetValueAborted=false;

	if (_tcslen(FileName)<=3)
		return ft_fileerror;

	/*if (flags & CONTENT_DELAYIFSLOW) {
		if (FieldIndex==17)
			return ft_delayed;
		if (FieldIndex==18)
			return ft_ondemand;
	}*/

	/*if (flags & CONTENT_PASSTHROUGH) {
		switch (FieldIndex) {
		case 1:  // "size"
			filesize=(__int64)*(double*)FieldValue;
			switch (UnitIndex) {
			case 1:
				filesize/=1024;
				break;
			case 2:
				filesize/=(1024*1024);
				break;
			case 3:
				filesize/=(1024*1024*1024);
				break;
			}
			*(double*)FieldValue=(double)filesize;
			_itot((int)filesize,(TCHAR*)((char*)FieldValue+sizeof(double)),10);
			return ft_numeric_floating;
		default:
			return ft_nosuchfield;
		}
	}*/
	
	/*if (FieldIndex!=20 && FieldIndex!=23 && FieldIndex!=24)  //not needed for these fields!
		fh=FindFirstFile(FileName,&fd);
	else
		fh=0;*/

	if (fh!=INVALID_HANDLE_VALUE) {
		/*if (FieldIndex!=20 && FieldIndex!=23 && FieldIndex!=24)
			FindClose(fh);*/

		switch (FieldIndex) {

		case 0:
		{
			
			// TEST String
			_tcslcpy((TCHAR*)FieldValue, L"Test string", maxlen-1);
			break;

		} case 1: {

			// TEST int
			*(int*)FieldValue = 1234567890;
			break;
		
		} default:

			return ft_nosuchfield;

		}
	} else
		return ft_fileerror;

	int retval=fieldtypes[FieldIndex];  // very important!
#ifdef UNICODE
	if (retval==ft_string) {
		switch (FieldIndex) {
		case 0: // name
		case 9: // attrstr
		case 17:// versionstr
		case 21:// "CutNameStart"
			retval=ft_stringw;
		}
	}
#endif;
	return retval;
}