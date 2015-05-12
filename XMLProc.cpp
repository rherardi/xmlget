// XMLProc.cpp: implementation of the CXMLProc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <eh.h>

#include <xercesc/util/XercesDefs.hpp>

#include <xalanc/Include/PlatformDefinitions.hpp>

#include <xercesc/util/PlatformUtils.hpp>
//#include <xercesc/framework/LocalFileInputSource.hpp>
//#include <xercesc/framework/MemBufInputSource.hpp>

#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XalanDOM/XalanElement.hpp>
#include <xalanc/XalanDOM/XalanNode.hpp>
#include <xalanc/XalanDOM/XalanNamedNodeMap.hpp>
#include <xalanc/XalanDOM/XalanNodeList.hpp>
#include <xalanc/XalanDOM/XalanDOMException.hpp>
#include <xalanc/XPath/XalanXPathException.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>

#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#include <xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMException.hpp>

#include <xalanc/XPath/NodeRefList.hpp>
#include <xalanc/XPath/MutableNodeRefList.hpp>

#include <xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp>

#include "SAXErrorHandler.hpp"

#include "XMLProc.hpp"
//#include "XMLNameValue.hpp"
#include "resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLProc::CXMLProc()
{
	m_configFile[0] = _T('\0');
	m_selector[0] = _T('\0');
	m_value[0] = _T('\0');
	m_verbose = false;
	m_useKey = false;
	m_versionInfo = true;
	m_valueInMemory = false;
	m_valuePtr = NULL;
	m_multiSep[0] = _T('\0');

	// Initialize the XML4C2 system
	try
	{

		XMLPlatformUtils::Initialize();

		XPathEvaluator::initialize();


		//
		//  Create our parser, then attach an error handler to the parser.
		//  The parser will call back to methods of the ErrorHandler if it
		//  discovers errors during the course of parsing the XML document.
		//
		parser = new XercesDOMParser();

		parser->setDoNamespaces(true);
		parser->setDoSchema(false);

		errHandler = new SAXErrorHandler();
		parser->setErrorHandler(errHandler);

	}
	catch(const XMLException &toCatch)
	{
		_tprintf(_T("XMLException: %s\n"), toCatch.getMessage());
	}
}

CXMLProc::~CXMLProc()
{
	if (m_valuePtr != NULL)
		free(m_valuePtr);

	XPathEvaluator::terminate();

	// And call the termination method
	XMLPlatformUtils::Terminate();
}

int CXMLProc::Parse()
{
	int retCode = STEP_SUCCESS;
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CXMLProc::Parse()");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}

	//
	//  Parse the XML file, catching any XML exceptions that might propogate
	//  out of it.
	//
	bool errorsOccured = false;
	try
	{
		parser->parse(m_configFile);
	}
	catch (const OutOfMemoryException& e)
	{
		_tprintf(_T("error: %s!\n"), e.getMessage());
		errorsOccured = true;
	}
	catch (const XMLException& e)
	{
		_tprintf(_T("error: %s\n"), e.getMessage());
		errorsOccured = true;
	}
	catch (const DOMException& e)
	{
		_tprintf(_T("error: %s\n"), e.getMessage());
		errorsOccured = true;
	}
	catch (...)
	{
		_tprintf(_T("Uncatched exception!\n"));
		errorsOccured = true;
	}

	// If the parse was successful, output the document data from the DOM tree
	if (!errorsOccured && !errHandler->getSawErrors()) {


		XALAN_USING_XALAN(XalanDocument)
		XALAN_USING_XALAN(XalanDocumentPrefixResolver)
		XALAN_USING_XALAN(XalanDOMString)
		XALAN_USING_XALAN(XalanNode)
		XALAN_USING_XALAN(XObjectPtr)

		XALAN_USING_XALAN(XercesParserLiaison)
		XALAN_USING_XALAN(XercesDOMSupport)

		theDOMSupport = new XercesDOMSupport();

		try {
		theXercesDocument = parser->getDocument();
		assert(theXercesDocument != NULL);
		theLiaison = new XercesParserLiaison(*theDOMSupport);

		assert(theLiaison != NULL);
		theDocument =
				theLiaison->createDocument(theXercesDocument, true, true, true);
		assert(theDocument != 0);

//		_tprintf(_T("err202:\n"));
		thePrefixResolver = new XalanDocumentPrefixResolver(theDocument);
		}
		catch (const XMLException& e)
		{
			_tprintf(_T("XMLException: %s\n"), e.getMessage());
		}
		catch (const DOMException& e)
		{
			_tprintf(_T("DOMException: %s\n"), e.getMessage());
		}
		catch (const SAXParseException& e)
		{
			_tprintf(_T("SAXParseException: %s\n"), e.getMessage());
		}
		catch (const XercesDOMException& e)
		{
			_tprintf(_T("XercesDOMException: %d\n"), e.getExceptionCode());
		}
		catch (const XalanDOMException& e)
		{
			_tprintf(_T("XalanDOMException: %d\n"), e.getExceptionCode());
		}
		
		catch(exception e) {
			_tprintf(_T("exception\n"));
			retCode = ERR_FAILED;
			goto cleanup;
//			printf(e.what());
		}
		catch(...) {
			_tprintf(_T("Uncatched exception!\n"));
			retCode = ERR_FAILED;
			goto cleanup;
		}

	}
	else {
//		_tprintf(_T("err2: %d %d\n"), errorsOccured, errHandler->getSawErrors());
		retCode = ERR_FAILED;
		goto cleanup;
	}
cleanup:
	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	return retCode;
}

// Get/Set m_arrayTag
_TCHAR const * CXMLProc::GetArrayTag()
{
	return &m_arrayTag[0];
}
void CXMLProc::SetArrayTag(const _TCHAR *arrayTag)
{
	::_tcscpy(m_arrayTag, arrayTag);
}

// Get/Set m_configFile
_TCHAR const * CXMLProc::GetConfigFile()
{
	return &m_configFile[0];
}
void CXMLProc::SetConfigFile(const _TCHAR *configFile)
{
	::_tcscpy(m_configFile, configFile);
}

// Get/Set m_useKey
bool const CXMLProc::IsUseKey()
{
	return m_useKey == true;
}
void CXMLProc::SetUseKey()
{
	m_useKey = true;
}

// Get/Set m_multiSep
_TCHAR const * CXMLProc::GetMultiSep()
{
	return &m_multiSep[0];
}
void CXMLProc::SetMultiSep(const _TCHAR *multiSep)
{
	::_tcscpy(m_multiSep, multiSep);
}

// Get/Set m_valueInMemory
bool const CXMLProc::IsValueInMemory()
{
	return m_valueInMemory == true;
}
void CXMLProc::SetValueInMemory()
{
	m_valueInMemory = true;
}

// Get/Set m_valuePtr
_TCHAR const * CXMLProc::GetValuePtr()
{
	return m_valuePtr;
}
void CXMLProc::SetValuePtr(_TCHAR const *value)
{
//	_tprintf(_T("len1: %d\n"), _tcslen(value));
	if (value != NULL) {
//		_tprintf(_T("len2: %d\n"), _tcslen(value));
		m_valuePtr = (_TCHAR *)malloc((_tcslen(value) + 1) * sizeof(_TCHAR));
		*m_valuePtr = _T('\0');
		_tcscpy(m_valuePtr, value);
//		memcpy(m_valuePtr, value, _tcslen(value) * sizeof(TCHAR));
//		*(m_valuePtr + _tcslen(value)) = _T('\0');
//		_tprintf(_T("len3: %d\n"), _tcslen(m_valuePtr));
		m_valueInMemory = true;
	}
}

// Get/Set m_versionInfo
bool const CXMLProc::IsVersionInfo()
{
	return m_versionInfo == true;
}
void CXMLProc::OmitVersionInfo()
{
	m_versionInfo = false;
}

// Get/Set m_selector
_TCHAR const * CXMLProc::GetSelector()
{
	return &m_selector[0];
}
void CXMLProc::SetSelector(const _TCHAR *selector)
{
	::_tcscpy(m_selector, selector);
}

// Get m_value
_TCHAR const * CXMLProc::GetValue()
{
	return &m_value[0];
}

// Get/Set m_verbose
bool const CXMLProc::GetVerbose()
{
	return m_verbose;
}
void CXMLProc::SetVerbose(const bool verbose)
{
	m_verbose = verbose;
}

int CXMLProc::PrepareValue()
{
	int retCode = STEP_SUCCESS;
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CXMLProc::PrepareValue()");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
		::LoadString(hInst, IDS_LOG_SELECTOR, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, this->GetSelector());
	}

	try {
	assert(theDocument != 0);
	assert(theDOMSupport != 0);
	assert(thePrefixResolver != 0);
	XPathEvaluator	theEvaluator;

	assert(theEvaluator != 0);

	XalanNode* const	theNode =
							theEvaluator.selectSingleNode(
							*theDOMSupport,
							theDocument,
							XalanDOMString(this->GetSelector()).c_str(),
							*thePrefixResolver);

	if (theNode != NULL)
	{
		const _TCHAR *value;
		if (!this->IsUseKey()) {
			// return as usual
			value = theNode->getNodeValue().c_str();
			if (value != NULL && _tcslen(value) > 0)
				_tcscpy(m_value, value);
		}
		else {
			// key is used, so take next element value and make all other assumptions
//			XalanNode *const theValueNode = theNode->getNextSibling();
			XalanNode* theValueNode =
								theEvaluator.selectSingleNode(
								*theDOMSupport,
								theNode,
								XalanDOMString(_T("following-sibling::node()[position()=2]")).c_str(),
								*thePrefixResolver);

			if (theValueNode == NULL) {
				::LoadString(hInst, IDS_ERR_KEYVALUENOTFOUND, usage, 256);
				::_tprintf(_TEXT("%s\n"), usage);
				retCode = STEP_KEYVALUENOTFOUND;
				goto done;
			}
			else {
				const _TCHAR *nodeName = theValueNode->getNodeName().c_str();
//				::_tprintf(_TEXT("nodeName: %s\n"), nodeName);
//				::_tprintf(_TEXT("nodeValue: %s\n"), theValueNode->getNodeValue().c_str());
				// check if we have array or not
				if (_tcscmp(nodeName, _T("array")) == 0) {
					const XalanNodeList* arrayItems = theValueNode->getChildNodes();
					if (arrayItems != NULL && arrayItems->getLength() > 0 ) {
//						::_tprintf(_TEXT("arrayItems->getLength(): %d\n"), arrayItems->getLength());
						int arrayItem = 0;
						for (int index=0; index < arrayItems->getLength(); index++ ) {
							theValueNode = arrayItems->item(index);
							if (theValueNode->getNodeType() == DOMNode.TEXT_NODE)
								continue;
							nodeName = theValueNode->getNodeName().c_str();
//							::_tprintf(_TEXT("2: %s\n"), nodeName);
							if (arrayItem == 0) {
								// special case for <false/> or <true/>
								if (_tcscmp(nodeName, _T("false")) == 0)
									_tcscpy(m_value, _T("0"));
								else if (_tcscmp(nodeName, _T("true")) == 0)
									_tcscpy(m_value, _T("1"));
								else {
//									_tcscpy(m_value, theValueNode->getNodeValue().c_str());
									XalanNode* theTextNode =
														theEvaluator.selectSingleNode(
														*theDOMSupport,
														theValueNode,
														XalanDOMString(_T("child::text()[position()=1]")).c_str(),
														*thePrefixResolver);
									_tcscpy(m_value, theTextNode->getNodeValue().c_str());
								}
								arrayItem++;
							}
							else {
								if (m_multiSep == NULL || _tcslen(m_multiSep) == 0)
									_tcscat(m_value, _T("\n"));
								else {
									// check value for possible \n, \r, \t symbols
									_TCHAR modified[2048];
									_TCHAR *ptr = m_multiSep;
									_TCHAR specStr[3];
									_TCHAR specOneStr[2];
									modified[0] = _T('\0');
									int offset = 0;
									while (ptr != NULL && _tcslen(ptr) > 0) {
										for (int specIndex = 0; specIndex < 3; specIndex++) {
											switch (specIndex) {
											case 0:
												_tcscpy(specStr, _T("\\n"));
												_tcscpy(specOneStr, _T("\n"));
												break;
											case 1:
												_tcscpy(specStr, _T("\\r"));
												_tcscpy(specOneStr, _T("\r"));
												break;
											case 2:
												_tcscpy(specStr, _T("\\t"));
												_tcscpy(specOneStr, _T("\t"));
												break;
											}
											if (_tcsncmp(ptr, specStr, 2) == 0)
												break;
											if (specIndex < 2)
												ptr = m_multiSep + offset;
											else
												ptr = NULL;
										}
										if (ptr == NULL) {
											ptr = m_multiSep + offset;
											_tcsncat(modified, ptr, 1);
											offset += 1;
										}
										else {
											_tcscat(modified, specOneStr);
											offset += 2;
										}
										ptr = m_multiSep + offset;
									}
									_tcscat(m_value, modified);
								}

								// special case for <false/> or <true/>
								if (_tcscmp(nodeName, _T("false")) == 0)
									_tcscat(m_value, _T("0"));
								else if (_tcscmp(nodeName, _T("true")) == 0)
									_tcscat(m_value, _T("1"));
								else {
									//_tcscat(m_value, theValueNode->getNodeValue().c_str());
									XalanNode* theTextNode =
														theEvaluator.selectSingleNode(
														*theDOMSupport,
														theValueNode,
														XalanDOMString(_T("child::text()[position()=1]")).c_str(),
														*thePrefixResolver);
									_tcscat(m_value, theTextNode->getNodeValue().c_str());
								}
							}
						}
					}
				}
				else {
					// special case for <false/> or <true/>
					if (_tcscmp(nodeName, _T("false")) == 0)
						_tcscpy(m_value, _T("0"));
					else if (_tcscmp(nodeName, _T("true")) == 0)
						_tcscpy(m_value, _T("1"));
					else {
//						_tcscpy(m_value, theValueNode->getNodeValue().c_str());
						XalanNode* theTextNode =
											theEvaluator.selectSingleNode(
											*theDOMSupport,
											theValueNode,
											XalanDOMString(_T("child::text()[position()=1]")).c_str(),
											*thePrefixResolver);
						int len = _tcslen(theTextNode->getNodeValue().c_str());
//						_tprintf(_T("len: %d\n"), len);
//						if (len > 2047) {
						if (_tcscmp(nodeName, _T("string")) == 0) {
							this->SetValuePtr(theTextNode->getNodeValue().c_str());
						}
						else
							_tcscpy(m_value, theTextNode->getNodeValue().c_str());
					}
				}
			}
		}

//		const XalanNamedNodeMap* attrs = theNode->getAttributes();
//		XalanNode* valueNode = attrs->getNamedItem(XalanDOMString(this->GetAttribute()));

	}
	else {
		// invalid selector
		::LoadString(hInst, IDS_ERR_SELECTOR, usage, 256);
		::_tprintf(_TEXT("%s\n"), usage);
		retCode = STEP_BADSELECTOR;
	}

	}
	catch (const XMLException& e)
	{
		_tprintf(_T("XMLException: %s\n"), e.getMessage());
	}
	catch (const DOMException& e)
	{
		_tprintf(_T("DOMException: %s\n"), e.getMessage());
	}
	catch (const SAXParseException& e)
	{
		_tprintf(_T("SAXParseException: %s\n"), e.getMessage());
	}
	catch (const XercesDOMException& e)
	{
		_tprintf(_T("XercesDOMException: %d\n"), e.getExceptionCode());
	}
	catch (const XalanDOMException& e)
	{
		_tprintf(_T("XalanDOMException: %d\n"), e.getExceptionCode());
	}
	catch (const XalanXPathException& e)
	{
		_tprintf(_T("XalanXPathException: %s\n"), e.getMessage().c_str());
	}
	catch (...)
	{
		_tprintf(_T("Uncatched exception!\n"));
	}

done:
	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	return retCode;
}
