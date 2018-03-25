/*
Module : OpenSSLMfc.cpp
Purpose: Implementation for wrapper classes for the OpenSSL C variable types
Created: PJN / 24-05-2002
History: PJN / 29-12-2004 1. Updated to suit new layout of CWSocket methods
         PJN / 19-02-2005 1. Provided a derived implementation of CSSLSocket::IsReadible.
         PJN / 16-11-2005 1. CSSLSocket::Send now uses a const void* parameter.
         PJN / 27-11-2005 1. Updated comments in the header file to accompany inclusion of the OpenSSL header files


Copyright (c) 2002 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/



//////////////// Includes ////////////////////////////////////////////

#include "stdafx.h"
#include "OpenSSLMfc.h"



//////////////// Macros //////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////// Implementation //////////////////////////////////////

CSSLContext::CSSLContext()
{
  //Initialize our member variable to sane defaults
  m_pSSLContext = NULL;
}

CSSLContext::CSSLContext(SSL_CTX* pSSLContext)
{
  m_pSSLContext = pSSLContext;
}

CSSLContext::CSSLContext(const CSSLContext& SSLContext)
{
  *this = SSLContext;
}

CSSLContext::~CSSLContext()
{
  Close();
}

void CSSLContext::Close()
{
  if (m_pSSLContext)
  {
    SSL_CTX_free(m_pSSLContext);
    m_pSSLContext = NULL;
  }
}

CSSLContext::operator SSL_CTX*() const
{
  return m_pSSLContext;
}

void CSSLContext::Attach(SSL_CTX* pSSLContext)
{
  Close();
  m_pSSLContext = pSSLContext;
}

CSSLContext& CSSLContext::operator=(const CSSLContext& SSLContext)
{
  Close();
  m_pSSLContext = SSLContext;
  return *this;
}

SSL_CTX* CSSLContext::Detach()
{
  SSL_CTX* pTemp = m_pSSLContext;
  m_pSSLContext = NULL;
  return pTemp;
}



CSSL::CSSL()
{
  //Initialize our member variable to sane defaults
  m_pSSL = NULL;
}

CSSL::CSSL(SSL* pSSL)
{
  m_pSSL = pSSL;
}

CSSL::CSSL(const CSSL& ssl)
{
  *this = ssl;
}

CSSL::~CSSL()
{
  Close();
}

void CSSL::Close()
{
  if (m_pSSL)
  {
    for (int i=0; i<3 && SSL_shutdown(m_pSSL)== 0; i++) 
      Sleep(0); 

    SSL_free(m_pSSL);
    m_pSSL = NULL;
  }
}

CSSL::operator SSL*() const
{
  return m_pSSL;
}

void CSSL::Attach(SSL* pSSL)
{
  Close();
  m_pSSL = pSSL;
}

CSSL& CSSL::operator=(const CSSL& ssl)
{
  Close();
  m_pSSL = ssl;
  return *this;
}

SSL* CSSL::Detach()
{
  SSL* pTemp = m_pSSL;
  m_pSSL = NULL;
  return pTemp;
}



CSSLSocket::CSSLSocket()
{
  m_pSocket = NULL;
}

CSSLSocket::~CSSLSocket()
{
  Close();
}

BOOL CSSLSocket::Create(CSSLContext& sslContext, CWSocket& socket)
{
  //Create the SSL object
  SSL* pSSL = SSL_new(sslContext);
  if (pSSL == NULL)
    return FALSE;
  else
    m_SSL.Attach(pSSL);

  //Associate the socket with the SSL connection object
  if (SSL_set_fd(m_SSL, socket) != 1)
  {
    Close();
    return FALSE;
  }

  //Hive away the socket pointer
  m_pSocket = &socket;

  return TRUE;
}

BOOL CSSLSocket::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
  //Validate our parameters
  ASSERT(m_pSocket);

  //Call the low level socket connect
  m_pSocket->Connect(lpszHostAddress, nHostPort);

  //Just call the SSL_accept function
  int nSSLConnect = SSL_connect(m_SSL);
  return (nSSLConnect == 1);
}

BOOL CSSLSocket::ConnectViaSocks4(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, DWORD dwConnectionTimeout)
{
  //Validate our parameters
  ASSERT(m_pSocket);

  //Call the low level socket connect
  m_pSocket->ConnectViaSocks4(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, dwConnectionTimeout);

  //Just call the SSL_accept function
  int nSSLConnect = SSL_connect(m_SSL);
  return (nSSLConnect == 1);
}

BOOL CSSLSocket::ConnectViaSocks5(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwConnectionTimeout, BOOL bUDP)
{
  //Validate our parameters
  ASSERT(m_pSocket);

  //Call the low level socket connect
  m_pSocket->ConnectViaSocks5(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, lpszUserName, lpszPassword, dwConnectionTimeout, bUDP);

  //Just call the SSL_accept function
  int nSSLConnect = SSL_connect(m_SSL);
  return (nSSLConnect == 1);
}

BOOL CSSLSocket::ConnectViaHTTPProxy(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszHTTPServer, UINT nHTTPProxyPort, CString& sProxyResponse, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwConnectionTimeout, LPCTSTR lpszUserAgent)
{
  //Validate our parameters
  ASSERT(m_pSocket);

  //Call the low level socket connect
  m_pSocket->ConnectViaHTTPProxy(lpszHostAddress, nHostPort, lpszHTTPServer, nHTTPProxyPort, sProxyResponse, lpszUserName, lpszPassword, dwConnectionTimeout, lpszUserAgent);

  //Just call the SSL_accept function
  int nSSLConnect = SSL_connect(m_SSL);
  return (nSSLConnect == 1);
}

BOOL CSSLSocket::Accept(DWORD dwSSLNegotiationTimeout)
{
  //Validate our parameters
  ASSERT(m_pSocket);

  //Then do the SSL accept
  BOOL bNegotiationComplete = FALSE;
  while (!bNegotiationComplete)
  {
    int nSSLAccept = SSL_accept(m_SSL);
    if (nSSLAccept != 1)
    {
      BOOL bRetry = FALSE;
      int nSSL_get_error = SSL_get_error(m_SSL, nSSLAccept);
      if (nSSL_get_error == SSL_ERROR_WANT_READ)
      {
        if (m_pSocket->IsReadible(dwSSLNegotiationTimeout))
          bRetry = TRUE;
      }

      if (!bRetry)
        return FALSE;
    }
    else
      bNegotiationComplete = TRUE;
  }

  return TRUE;
}

void CSSLSocket::Close()
{
  //Close down the SSL connection
  if (m_SSL.operator SSL*())
    m_SSL.Close();

  //Close down the socket connection
  if (m_pSocket)
  {
    m_pSocket->Close();
    m_pSocket = NULL;
  }
}

int CSSLSocket::Send(const void* pBuffer, int nBuf)
{
  //Validate our parameters
  ASSERT(m_SSL.operator SSL*());

  //Just call the SSL_write function
  return SSL_write(m_SSL, pBuffer, nBuf);
}

int CSSLSocket::Receive(void* pBuffer, int nBuf)
{
  //Validate our parameters
  ASSERT(m_SSL.operator SSL*());

  //Just call the SSL_read function
  return SSL_read(m_SSL, pBuffer, nBuf);
}

CSSLSocket::operator SSL*() const
{
  return m_SSL.operator SSL*();
}

CSSLSocket::operator CWSocket&() const
{
  //validate our parameters
  ASSERT(m_pSocket);

  return *m_pSocket;
}

BOOL CSSLSocket::IsReadible(DWORD dwTimeout)
{
  //Validate our parameters
  ASSERT(m_pSocket);

  //Try SSL_pending before we defer to our socket implementation
  if (m_SSL.m_pSSL)
  {
    if (SSL_pending(m_SSL))
      return TRUE;
  }

  return m_pSocket->IsReadible(dwTimeout);
}