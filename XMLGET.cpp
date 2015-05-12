// XMLGET.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XMLGet.hpp"
#include "XMLProcSuppl.hpp"
#include "XMLProc.hpp"

#include "XGetopt.h"
#include "Resources.h"
#include "resource.h"

int wmain(int argc, _TCHAR* argv[])
{
	int retCode;

	CXMLGet *theApp = new CXMLGet();

	retCode = theApp->ProcessCommandLine(argc, argv);
	if (retCode == PARAMETERS_OK) {
		if (theApp->IsVersionInfo())
			CResources::ShowTitle(IDS_TITLE, IDS_VERSION);
		retCode = theApp->PrepareValue();
		if (retCode == STEP_SUCCESS) {
			theApp->PrintValue();
			retCode = SUCCESS;
		}
	}
//	if (theApp->IsVerbose()){
//		::LoadString(hInst, IDS_EXIT_CODE, usage, 256);
//		::_tprintf(_TEXT("%s %d\n"), usage, retCode);
//	}

	theApp->~CXMLGet();
	if (retCode == SUCCESS)
		exit(0);
	exit(-1);
	return retCode;
}


//////////////////////////////////////////////////////////////////////
// CXMLGet Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLGet::CXMLGet():
m_xmlProc(new CXMLProc())
{

}

CXMLGet::~CXMLGet()
{

}

// Process all command line options
int CXMLGet::ProcessCommandLine(int argc, _TCHAR *argv[])
{
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR prompt[512];
	int c;

	if (argc == 1) {
		ShowUsage();
		return ERR_NO_PARAMETERS;
	}

	while ((c = getopt(argc, argv, _TEXT("bf:ikm:s:"))) != EOF)
	{
		 switch (c)
		 {
//			 case _T('a'):
//				 {
//					m_xmlProc->SetArrayTag(argv[optind-1]);
//				 }
//				 break;
			 case _T('b'):
				 {
					m_xmlProc->SetVerbose(true);
				 }
				 break;
			 case _T('f'):
				 {
					m_xmlProc->SetConfigFile(argv[optind-1]);
				 }
				 break;
			 case _T('i'):
				 {
					m_xmlProc->OmitVersionInfo();
				 }
				 break;
			 case _T('k'):
				 {
					m_xmlProc->SetUseKey();
				 }
				 break;
			 case _T('m'):
				 {
					m_xmlProc->SetMultiSep(argv[optind-1]);
				 }
				 break;
			 case _T('s'):
				 {
					m_xmlProc->SetSelector(argv[optind-1]);
				 }
				 break;
			 case _T('!'):
				::LoadString(hInst, IDS_OPT_ERR_MISSEDARG, prompt, 256);
				::_tprintf(_TEXT("%s %s %d %d\n"), prompt, argv[optind-1], optind, argc);
				ShowUsage();
				return ERR_MISSEDARG;
				break;

			 case _T('?'):
				ShowUsage();
				return ERR_ILLEGAL_OPTION;
				break;

			 default:
				{
				::LoadString(hInst, IDS_OPT_ERR_NOHANDLER, prompt, 256);
				::_tprintf(_TEXT("%s %s\n"), prompt, argv[optind-1]);
				}
				return ERR_NO_OPTION_HANDLER;
				break;
		 }
	}
	//
	// check for non-option args here
	//
	if (optind != argc) {
		::LoadString(hInst, IDS_OPT_ERR_MISSEDARG, prompt, 256);
		::_tprintf(_TEXT("%s %s\n"), prompt, argv[1]);
		ShowUsage();
		return ERR_MISSEDARG;
	}

	if (m_xmlProc->Parse() != STEP_SUCCESS) {
		return ERR_FAILED;
	}

	return PARAMETERS_OK;
}

// Show utility full title and copyright

void CXMLGet::ShowUsage()
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);

// Usage: XMLGET [OPTION]...
// -a array_tag, i.e. <array>
// -b
// -f file
// -k
// -m value
// -s selector

	CResources::ShowTitle(IDS_TITLE, IDS_VERSION);
	// -a
//	::LoadString(hInst, IDS_OPTION_ARRAYTAG, usage, 256);
//	::_tprintf(_TEXT("%s\n"), usage);
	// -b
	::LoadString(hInst, IDS_OPTION_VERBOSE, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -f file
	::LoadString(hInst, IDS_OPTION_FILE, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -i
	::LoadString(hInst, IDS_OPTION_OMITVERSIONINFO, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -k
	::LoadString(hInst, IDS_OPTION_USEKEY, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -m
	::LoadString(hInst, IDS_OPTION_MULTISEP, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
	// -s selector
	::LoadString(hInst, IDS_OPTION_SELECTOR, usage, 256);
	::_tprintf(_TEXT("%s\n"), usage);
}

int CXMLGet::PrepareValue()
{
	int retCode = m_xmlProc->PrepareValue();
	return retCode;
}

void CXMLGet::PrintValue()
{
	if (!m_xmlProc->IsValueInMemory())
		_tprintf(_T("%s"), m_xmlProc->GetValue());
	else {
		int len = _tcslen(m_xmlProc->GetValuePtr());
		if (m_xmlProc->GetVerbose())
			_tprintf(_T("PrintValue len: %d\n"), len);
		char *ptr = (char *)malloc(len + 1);
		*ptr = '\0';
		if (m_xmlProc->GetVerbose())
			_tprintf(_T("Before CharToOemBuff\n"));
		CharToOemBuff(m_xmlProc->GetValuePtr(), ptr, len);
		*(ptr + len) = '\0';
		if (m_xmlProc->GetVerbose())
			_tprintf(_T("After CharToOemBuff\n"));
//		_tprintf(_T("%s"), m_xmlProc->GetValuePtr());
		printf("%s", ptr);
		free(ptr);
	}
}

bool CXMLGet::IsVersionInfo()
{
	return m_xmlProc->IsVersionInfo();
}
