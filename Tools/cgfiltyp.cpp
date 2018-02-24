//- -----------------------------------------------------------------
//- (C) Copyright 2000 Blake V. Miller
//- All Rights Reserved.
//- -----------------------------------------------------------------
//- File	:	CGFILTYP.CPP
//-	Author	:	Blake Miller
//-	Version	:	June 16, 2000
//-	Purpose	:	File Type Access
//- -----------------------------------------------------------------

#include "stdafx.h"
#include "winerror.h"

#ifndef __CGFILTYP_H__
#include "CGFILTYP.H"	//	CGCFileTypeAccess
#endif

#undef THIS_FILE
static char THIS_FILE[] = "CGFILTYP.CPP";

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CGCFileTypeAccess, CObject)

CGCFileTypeAccess::CGCFileTypeAccess()
{
}

// virtual 
CGCFileTypeAccess::~CGCFileTypeAccess()
{
}

//-	*****************************************************************
//- Functions Affecting Registry
//-	*****************************************************************

// virtual
BOOL CGCFileTypeAccess::RegSetAllInfo(void)
{
	RegSetExtension();
	RegSetDocumentType();
	RegSetCLSID();
	RegSetShellInfo();

	return TRUE;
}

// HKEY_CLASSES_ROOT\.<Extension>
// virtual
BOOL CGCFileTypeAccess::RegSetExtension(void)
{
	if( m_csExtension.IsEmpty() ){
		return TRUE;
	}

	CString csKey = "." + m_csExtension;

	SetRegistryValue(HKEY_CLASSES_ROOT, csKey, "", m_csDocumentClassName);

	if( !m_csShellOpenCommand.IsEmpty() ){
		csKey += "\\shell\\open\\command";
		SetRegistryValue(HKEY_CLASSES_ROOT, csKey, "", m_csShellOpenCommand);
	}

	return TRUE;
}

// HKEY_CLASSES_ROOT\.<Document Type>
// virtual
BOOL CGCFileTypeAccess::RegSetDocumentType(void)
{
	if( m_csDocumentClassName.IsEmpty()){
		return TRUE;
	}

	CString csKey = m_csDocumentClassName;

	SetRegistryValue(HKEY_CLASSES_ROOT, csKey, "", m_csDocumentDescription);

	// DefaultIcon
	if( !m_csDocumentDefaultIcon.IsEmpty() ){
		csKey  = m_csDocumentClassName;
		csKey += "\\DefaultIcon";
		SetRegistryValue(HKEY_CLASSES_ROOT, csKey, "", m_csDocumentDefaultIcon);
	}

	// shell\open\command
	if( !m_csShellOpenCommand.IsEmpty() ){
		csKey  = m_csDocumentClassName;
		csKey += "\\shell\\open\\command";
		SetRegistryValue(HKEY_CLASSES_ROOT, csKey, "", m_csShellOpenCommand);
	}

	return TRUE;
}

// HKEY_CLASSES_ROOT\CLSID\<GUID>
// virtual
BOOL CGCFileTypeAccess::RegSetCLSID(void)
{
	return TRUE;
}

// 
// virtual
BOOL CGCFileTypeAccess::RegSetShellInfo(void)
{
	return TRUE;
}

//-	*****************************************************************
//- Member Variable Get/Set Methods
//-	*****************************************************************

void CGCFileTypeAccess::SetExtension(
	LPCTSTR szExtension
){
	m_csExtension = szExtension;
}
void CGCFileTypeAccess::GetExtension(
	CString& csExtension
) const {
	csExtension = m_csExtension;
}

void CGCFileTypeAccess::SetContentType(
	LPCTSTR szContentType
){
	m_csContentType = szContentType;
}
void CGCFileTypeAccess::GetContentType(
	CString& csContentType
) const {
	csContentType = m_csContentType;
}

void CGCFileTypeAccess::SetShellOpenCommand(
	LPCTSTR szShellOpenCommand
){
	m_csShellOpenCommand = szShellOpenCommand;
}
void CGCFileTypeAccess::GetShellOpenCommand(
	CString& csShellOpenCommand
) const {
	csShellOpenCommand = m_csShellOpenCommand;
}

void CGCFileTypeAccess::SetShellNewCommand(
	LPCTSTR szShellNewCommand
){
	m_csShellNewCommand = szShellNewCommand;
}
void CGCFileTypeAccess::GetShellNewCommand(
	CString& csShellNewCommand
) const {
	csShellNewCommand = m_csShellNewCommand;
}

void CGCFileTypeAccess::SetShellNewFileName(
	LPCTSTR szShellNewFileName
){
	m_csShellNewFileName = szShellNewFileName;
}
void CGCFileTypeAccess::GetShellNewFileName(
	CString& csShellNewFileName
) const {
	csShellNewFileName = m_csShellNewFileName;
}

void CGCFileTypeAccess::SetDocumentClassName(
	LPCTSTR szDocumentClassName
){
	m_csDocumentClassName = szDocumentClassName;
}
void CGCFileTypeAccess::GetDocumentClassName(
	CString& csDocumentClassName
) const {
	csDocumentClassName = m_csDocumentClassName;
}

void CGCFileTypeAccess::SetDocumentDescription(
	LPCTSTR szDocumentDescription
){
	m_csDocumentDescription = szDocumentDescription;
}
void CGCFileTypeAccess::GetDocumentDescription(
	CString& csDocumentDescription
) const {
	csDocumentDescription = m_csDocumentDescription;
}

void CGCFileTypeAccess::SetDocumentCLSID(
	LPCTSTR szDocumentCLSID
){
	m_csDocumentCLSID = szDocumentCLSID;
}
void CGCFileTypeAccess::GetDocumentCLSID(
	CString& csDocumentCLSID
) const {
	csDocumentCLSID = m_csDocumentCLSID;
}

void CGCFileTypeAccess::SetDocumentCurrentVersion(
	LPCTSTR szDocumentCurrentVersion
){
	m_csDocumentCurrentVersion = szDocumentCurrentVersion;
}
void CGCFileTypeAccess::GetDocumentCurrentVersion(
	CString& csDocumentCurrentVersion
) const {
	csDocumentCurrentVersion = m_csDocumentCurrentVersion;
}

void CGCFileTypeAccess::SetDocumentDefaultIcon(
	LPCTSTR szDocumentDefaultIcon
){
	m_csDocumentDefaultIcon = szDocumentDefaultIcon;
}
void CGCFileTypeAccess::GetDocumentDefaultIcon(
	CString& csDocumentDefaultIcon
) const {
	csDocumentDefaultIcon = m_csDocumentDefaultIcon;
}

void CGCFileTypeAccess::SetDocumentShellOpenCommand(
	LPCTSTR szDocumentShellOpenCommand
){
	m_csDocumentShellOpenCommand = szDocumentShellOpenCommand;
}
void CGCFileTypeAccess::GetDocumentShellOpenCommand(
	CString& csDocumentShellOpenCommand
) const {
	csDocumentShellOpenCommand = m_csDocumentShellOpenCommand;
}

// virtual 
void CGCFileTypeAccess::ClearAllData(void)
{
	m_csExtension.Empty();
	m_csContentType.Empty();
	m_csShellOpenCommand.Empty();
	m_csShellNewCommand.Empty();
	m_csShellNewFileName.Empty();

	m_csDocumentClassName.Empty();
	m_csDocumentDescription.Empty();
	m_csDocumentCLSID.Empty();
	m_csDocumentCurrentVersion.Empty();
	m_csDocumentDefaultIcon.Empty();
	m_csDocumentShellOpenCommand.Empty();
}

//- -----------------------------------------------------------------
//- set string value into registry

BOOL CGCFileTypeAccess::SetRegistryValue(
	HKEY	hOpenKey,
	LPCTSTR szKey,
	LPCTSTR szValue,
	LPCTSTR szData
){
	// validate input
	if( !hOpenKey || !szKey || !szKey[0] || 
		!szValue || !szData ){
		::SetLastError(E_INVALIDARG);
		return FALSE;
	}

	BOOL 	bRetVal = FALSE;
	DWORD	dwDisposition;
	DWORD	dwReserved = 0;
	HKEY  	hTempKey = (HKEY)0;

	// length specifier is in bytes, and some TCHAR 
	// are more than 1 byte each
	DWORD	dwBufferLength = lstrlen(szData) * sizeof(TCHAR);

	// Open key of interest
	// Assume all access is okay and that all keys will be stored to file
	// Utilize the default security attributes
	if( ERROR_SUCCESS == ::RegCreateKeyEx(hOpenKey, szKey, dwReserved,
		(LPTSTR)0, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, 0,
		&hTempKey, &dwDisposition) ){
		
		// dwBufferLength must include size of terminating nul 
		// character when using REG_SZ with RegSetValueEx function
		dwBufferLength += sizeof(TCHAR);
		
		if( ERROR_SUCCESS == ::RegSetValueEx(hTempKey, (LPTSTR)szValue,
			dwReserved, REG_SZ, (LPBYTE)szData, dwBufferLength) ){
			bRetVal = TRUE;
		}
	}

	// close opened key
	if( hTempKey ){
		::RegCloseKey(hTempKey);
	}

	return bRetVal;
}

//-	-----------------------------------------------------------------
//- END CGFILTYP.CPP Source File
//-	-----------------------------------------------------------------
