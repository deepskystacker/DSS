/*
Module : PJNNTLMAuth.CPP
Purpose: Implementation for a simple wrapper class for client NTLM authentication via SSPI
Created: PJN / 06-04-2005
History: PJN / 05-09-2005 1. Function pointer to CompleteAuthToken is now constructed at runtime. This means
                          that NTLM client authentication will work correctly on Win9x as used by the class
                          CPJNSMTPConnection and any other class instead of bringing up a "Failed to load
                          due to missing export..." message from the Windows loader. If you want to remove
                          the NTLM support from CPJNSMTPConnection then you can continue to use the CPJNSMTP_NONTLM
                          preprocessor define. Thanks to "Lior" for reporting this problem.
         PJN / 29-09-2005 1. All SDK function pointers used by the class are now implemented using GetProcAddress.
                          Thanks to Emir Kapic for this update.
         PJN / 29-06-2006 1. Combined the functionality of the _PJNNTLMCLIENTAUTH_DATA class into the main 
                          CNTLMClientAuth class.
         PJN / 14-07-2006 1. Updated CNTLMClientAuth to use newer C++ style casts.
                          2. Made the NTLMAuthenticate function "exception" safe, that is derived classes are free to throw 
                          exceptions in their implementations of NTLMAuthPhase1, NTLMAuthPhase2 or NTLMAuthPhase3 and the
                          CNTLMClientAuth class will properly clean up its resources. This is achieved by making the CredHandle
                          and SecHandle values member variables of the class.
                          3. Replaced all calls to ZeroMemory with memset.
                          4. Now the public NTLMAuthenticate function allows you to pass a user name and password pair to 
                          do NTLM authentication using a specified account instead of the credentials of the current user. If 
                          you leave these values as NULL, then you will get the old behaviour which is to use the current
                          user credentials. Thanks to Wouter Demuynck for this very nice addition.
         PJN / 13-10-2006 1. Code now initializes the Domain name value in NTLMAuthenticate to an empty string instead of a NULL
                          string. This avoids NTLM authentication issues when authenticating as a non-domain user with certain
                          mail servers. Thanks to Wouter Demuynck for reporting this issue.

Copyright (c) 2005 - 2007 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


//////////////// Includes /////////////////////////////////////////////////////

#include "stdafx.h"
#include "PJNNTLMAuth.h"
#ifndef __SSPI_H__
#pragma message("To avoid this message, please put Sspi.h in your pre compiled header (usually stdafx.h)")
#include <Sspi.h>
#endif


//////////////// Macros / Locals //////////////////////////////////////////////

#ifndef SEC_SUCCESS
#define SEC_SUCCESS(Status) ((Status) >= 0)
#endif


//////////////// Implementation ///////////////////////////////////////////////

CNTLMClientAuth::CNTLMClientAuth()
{
  //Set our credentials handles to default values
  memset(&m_hCred, 0, sizeof(m_hCred));
  memset(&m_hContext, 0, sizeof(m_hContext));

  m_hSecur32 = LoadLibrary(_T("SECUR32.DLL"));
  if (m_hSecur32)
  {
    m_lpfnCompleteAuthToken = reinterpret_cast<COMPLETE_AUTH_TOKEN_FN>(GetProcAddress(m_hSecur32, "CompleteAuthToken"));
    m_lpfnFreeCredentialsHandle = reinterpret_cast<FREE_CREDENTIALS_HANDLE_FN>(GetProcAddress(m_hSecur32, "FreeCredentialsHandle"));
    m_lpfnDeleteSecurityContext = reinterpret_cast<DELETE_SECURITY_CONTEXT_FN>(GetProcAddress(m_hSecur32, "DeleteSecurityContext"));

    #ifdef _UNICODE
      m_lpfnInitializeSecurityContext = reinterpret_cast<INITIALIZE_SECURITY_CONTEXT_FN>(GetProcAddress(m_hSecur32, "InitializeSecurityContextW"));
      m_lpfnAcquireCredentialsHandle = reinterpret_cast<ACQUIRE_CREDENTIALS_HANDLE_FN>(GetProcAddress(m_hSecur32, "AcquireCredentialsHandleW")); 
    #else
      m_lpfnInitializeSecurityContext = reinterpret_cast<INITIALIZE_SECURITY_CONTEXT_FN>(GetProcAddress(m_hSecur32, "InitializeSecurityContextA"));
      m_lpfnAcquireCredentialsHandle = reinterpret_cast<ACQUIRE_CREDENTIALS_HANDLE_FN>(GetProcAddress(m_hSecur32, "AcquireCredentialsHandleA"));
    #endif

    //Note we allow "CompleteAuthToken" to be not implemented. this gives us at least a runtime chance of using NTLM authentication on Win 9x.
    if (m_lpfnFreeCredentialsHandle == NULL || m_lpfnDeleteSecurityContext == NULL ||
        m_lpfnInitializeSecurityContext == NULL || m_lpfnAcquireCredentialsHandle == NULL)
    {
      m_lpfnFreeCredentialsHandle     = NULL;
      m_lpfnDeleteSecurityContext     = NULL;
      m_lpfnInitializeSecurityContext = NULL;
      m_lpfnAcquireCredentialsHandle  = NULL;
    } 
  }
  else
  {
    m_lpfnCompleteAuthToken         = NULL;
    m_lpfnFreeCredentialsHandle     = NULL;
    m_lpfnDeleteSecurityContext     = NULL;
    m_lpfnInitializeSecurityContext = NULL;
    m_lpfnAcquireCredentialsHandle  = NULL;
  }
}

CNTLMClientAuth::~CNTLMClientAuth()
{
  ReleaseHandles();

  if (m_hSecur32)
  {
    FreeLibrary(m_hSecur32);
    m_hSecur32 = NULL;
  }
}

void CNTLMClientAuth::ReleaseHandles()
{
  //Free up the security context if valid
  if (m_hContext.dwLower != 0 || m_hContext.dwUpper != 0) 
  {
    ASSERT(m_lpfnDeleteSecurityContext);
    m_lpfnDeleteSecurityContext(&m_hContext);
    memset(&m_hContext, 0, sizeof(m_hContext));
  }

  //Free up the credentials handle if valid
  if (m_hCred.dwLower != 0 || m_hCred.dwUpper != 0) 
  {
    ASSERT(m_lpfnFreeCredentialsHandle);
    m_lpfnFreeCredentialsHandle(&m_hCred);
    memset(&m_hCred, 0, sizeof(m_hCred));
  }
}

SECURITY_STATUS CNTLMClientAuth::NTLMAuthenticate(LPCTSTR pszUserName, LPCTSTR pszPassword)
{
  //Note we do the check for these 2 function pointers at the start, as otherwise we could (very unlikely
  //but possibly) end up with resource handles but with no means to deallocate them!.
  if ((m_lpfnDeleteSecurityContext == NULL) || (m_lpfnFreeCredentialsHandle == NULL))
    return SEC_E_UNSUPPORTED_FUNCTION;

  //allow "UserName" to be of the format DomainName\UserName
  LPCTSTR pszDomain = _T("");
  CString sUserName(pszUserName);
  int nSlashSeparatorOffset = sUserName.Find(_T('\\'));
  CString sDomain;
  CString sUserNameWithoutDomain;
  if (nSlashSeparatorOffset != -1)
  {
	  sDomain = sUserName.Left(nSlashSeparatorOffset);
	  pszDomain = sDomain.operator LPCTSTR();
	  sUserNameWithoutDomain = sUserName.Mid(nSlashSeparatorOffset + 1);
	  pszUserName = sUserNameWithoutDomain.operator LPCTSTR();
  }

  //Release the handles before we try to authenticate (we do this here to ensure any previous calls
  //to NTLMAuthenticate which throw exceptions are cleaned up prior to any new calls to DoNTLMAuthentication)
  ReleaseHandles();

  //Call the helper function which does all the work
  SECURITY_STATUS ss = DoNTLMAuthentication(pszUserName, pszPassword, pszDomain);

  //Now free up the handles now that we are finished the authentication (note it is not critical that this code is
  //called since the various NTLMAuthPhase(*) functions may throw exceptions
  ReleaseHandles();

  return ss;
}

SECURITY_STATUS CNTLMClientAuth::DoNTLMAuthentication(LPCTSTR pszUserName, LPCTSTR pszPassword, LPCTSTR pszDomain)
{
  BOOL fDone = FALSE;
  DWORD cbIn = 0;
  BYTE InBuf[12000];
  BYTE OutBuf[12000];
  DWORD cbMaxMessage = sizeof(InBuf);
  DWORD cbOut = cbMaxMessage;

  SECURITY_STATUS ss = GenClientContext(NULL, 0, OutBuf, &cbOut, &fDone, pszUserName, pszPassword, pszDomain);
  if (!SEC_SUCCESS(ss))
    return ss;

  ss = NTLMAuthPhase1(OutBuf, cbOut);
  if (!SEC_SUCCESS(ss))
    return ss;

  while (!fDone) 
  {
    ss = NTLMAuthPhase2(InBuf, sizeof(InBuf), &cbIn);
    if (!SEC_SUCCESS(ss))
      return ss;

    cbOut = cbMaxMessage;

    ss = GenClientContext(InBuf, cbIn, OutBuf, &cbOut, &fDone, pszUserName, pszPassword, pszDomain);
    if (!SEC_SUCCESS(ss))
      return ss;

    ss = NTLMAuthPhase3(OutBuf, cbOut);
    if (!SEC_SUCCESS(ss))
      return ss;
  }

  return ss;
}

SECURITY_STATUS CNTLMClientAuth::GenClientContext(BYTE* pIn, DWORD cbIn, BYTE* pOut, DWORD* pcbOut, BOOL* pfDone, LPCTSTR pszUserName, LPCTSTR pszPassword, LPCTSTR pszDomain)
{
  TimeStamp Lifetime;
  SECURITY_STATUS ss;
  if (NULL == pIn)  
  {   
    if (m_lpfnAcquireCredentialsHandle)
    {
      SEC_WINNT_AUTH_IDENTITY authInfo;
      authInfo.Domain = NULL;
      authInfo.User = NULL;
      authInfo.Password = NULL;
      void* pvLogonID = NULL;
      if ((pszUserName != NULL) && (lstrlen(pszUserName)))
      {
        authInfo.UserLength = lstrlen(pszUserName);
        authInfo.DomainLength = lstrlen(pszDomain);
        authInfo.PasswordLength = lstrlen(pszPassword);
      #ifdef _UNICODE  
        authInfo.User = reinterpret_cast<unsigned short*>(const_cast<LPTSTR>(pszUserName));
        authInfo.Domain = reinterpret_cast<unsigned short*>(const_cast<LPTSTR>(pszDomain));
        authInfo.Password = reinterpret_cast<unsigned short*>(const_cast<LPTSTR>(pszPassword));
        authInfo.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
      #else
        authInfo.User = reinterpret_cast<unsigned char*>(const_cast<LPTSTR>(pszUserName));
        authInfo.Domain = reinterpret_cast<unsigned char*>(const_cast<LPTSTR>(pszDomain));
        authInfo.Password = reinterpret_cast<unsigned char*>(const_cast<LPTSTR>(pszPassword));
        authInfo.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
      #endif
        pvLogonID = &authInfo;
      }

      wchar_t ntlm[5] = _T("NTLM");
      wchar_t* pntlm = &ntlm[0];
      ss = m_lpfnAcquireCredentialsHandle(NULL, pntlm, SECPKG_CRED_OUTBOUND, NULL, pvLogonID, NULL, NULL, &m_hCred, &Lifetime);
      if (!SEC_SUCCESS(ss))
        return ss;
    }
    else
      return SEC_E_UNSUPPORTED_FUNCTION;
  }

  //Prepare the buffers
  SecBufferDesc OutBuffDesc;
  OutBuffDesc.ulVersion = 0;
  OutBuffDesc.cBuffers  = 1;
  SecBuffer OutSecBuff;
  OutBuffDesc.pBuffers  = &OutSecBuff;
  OutSecBuff.cbBuffer   = *pcbOut;
  OutSecBuff.BufferType = SECBUFFER_TOKEN;
  OutSecBuff.pvBuffer   = pOut;

  //The input buffer is created only if a message has been received from the server.
  SecBufferDesc InBuffDesc;
  SecBuffer InSecBuff;
  if (pIn)   
  {
    InBuffDesc.ulVersion = 0;
    InBuffDesc.cBuffers  = 1;
    InBuffDesc.pBuffers  = &InSecBuff;
    InSecBuff.cbBuffer   = cbIn;
    InSecBuff.BufferType = SECBUFFER_TOKEN;
    InSecBuff.pvBuffer   = pIn;

    ULONG ContextAttributes;
    if (m_lpfnInitializeSecurityContext)
      ss = m_lpfnInitializeSecurityContext(&m_hCred, &m_hContext, NULL, 0, 0, SECURITY_NATIVE_DREP, &InBuffDesc, 0, &m_hContext, &OutBuffDesc, &ContextAttributes, &Lifetime);
    else
      return SEC_E_UNSUPPORTED_FUNCTION;
  }
  else
  {
    ULONG ContextAttributes;
    if (m_lpfnInitializeSecurityContext)
      ss = m_lpfnInitializeSecurityContext(&m_hCred, NULL, NULL, 0, 0, SECURITY_NATIVE_DREP, NULL, 0, &m_hContext, &OutBuffDesc, &ContextAttributes, &Lifetime);
    else
      return SEC_E_UNSUPPORTED_FUNCTION;
  }

  if (!SEC_SUCCESS(ss))
    return ss;

  //If necessary, complete the token.
  if ((SEC_I_COMPLETE_NEEDED == ss) || (SEC_I_COMPLETE_AND_CONTINUE == ss))  
  {
    //Check if CompleteAuthToken is available at runtime
    if (m_lpfnCompleteAuthToken)
    {
      ss = m_lpfnCompleteAuthToken(&m_hContext, &OutBuffDesc);
      if (!SEC_SUCCESS(ss))  
        return ss;
    }
    else
      return SEC_E_UNSUPPORTED_FUNCTION;
  }

  *pcbOut = OutSecBuff.cbBuffer;
  *pfDone = !((SEC_I_CONTINUE_NEEDED == ss) || (SEC_I_COMPLETE_AND_CONTINUE == ss));

  return ss;
}
