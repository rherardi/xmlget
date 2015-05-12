// XMLProc.h: interface for the CXMLProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLPROC_H__98F73F2A_7E4C_4D14_AF61_06664CED6C51__INCLUDED_)
#define AFX_XMLPROC_H__98F73F2A_7E4C_4D14_AF61_06664CED6C51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

XERCES_CPP_NAMESPACE_USE
XALAN_CPP_NAMESPACE_USE

class CXMLProc  
{
public:
	CXMLProc();
	virtual ~CXMLProc();
	int Parse();
	int PrepareValue();

	// Get/Set m_verbose
	bool const GetVerbose();
	void SetVerbose(const bool verbose);

	// Get/Set m_configFile
	_TCHAR const * GetConfigFile();
	void SetConfigFile(const _TCHAR *configFile);

	// Get/Set m_arrayTag
	_TCHAR const * GetArrayTag();
	void SetArrayTag(const _TCHAR *arrayTag);

	// Get/Set m_multiSep
	_TCHAR const * GetMultiSep();
	void SetMultiSep(const _TCHAR *multiSep);

	// Get/Set m_useKey
	bool const IsUseKey();
	void SetUseKey();

	// Get/Set m_selector
	_TCHAR const * GetSelector();
	void SetSelector(const _TCHAR *selector);

	// Get/Set m_value
	_TCHAR const * GetValue();
	void SetValue(const _TCHAR *value);

	// Get/Set m_versionInfo
	bool const IsVersionInfo();
	void OmitVersionInfo();

	// Get/Set m_valueInMemory
	bool const IsValueInMemory();
	void SetValueInMemory();

	// Get/Set m_valuePtr
	_TCHAR const * GetValuePtr();
	void SetValuePtr(_TCHAR const *value);

private:

protected:
	bool m_verbose;
	bool m_useKey;
	bool m_versionInfo;
	_TCHAR m_configFile[256];
	_TCHAR m_arrayTag[64];
	_TCHAR m_selector[256];
	_TCHAR m_value[2048];
	_TCHAR m_multiSep[32];
	_TCHAR *m_valuePtr;
	bool m_valueInMemory;

	XercesDOMParser *parser;
	SAXErrorHandler *errHandler;
	DOMDocument* theXercesDocument;
	XalanDocument* theDocument;
	XercesDOMSupport*		theDOMSupport;
	XalanDocumentPrefixResolver*		thePrefixResolver;
	XercesParserLiaison* theLiaison;
};

#endif // !defined(AFX_XMLPROC_H__98F73F2A_7E4C_4D14_AF61_06664CED6C51__INCLUDED_)
