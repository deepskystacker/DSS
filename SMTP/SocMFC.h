/*
Module : SocMFC.H
Purpose: Interface for an MFC wrapper class for sockets
Created: PJN / 05-08-1998

Copyright (c) 2002 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Defines ///////////////////////////////////////

#ifndef __SOCMFC_H__
#define __SOCMFC_H__

#ifndef SOCKMFC_EXT_CLASS
#define SOCKMFC_EXT_CLASS
#endif



////////////////////////////// Includes ///////////////////////////////////////

#ifndef _WINSOCKAPI_
#pragma message("You need to add Winsock support to your PCH (normally stdafx.h), Use either afxsock.h, winsock.h or Winsock2.h")
#endif


////////////////////////////// Classes ////////////////////////////////////////

class SOCKMFC_EXT_CLASS CWSocketException : public CException
{
public:
//Constructors / Destructors
	CWSocketException(int nError);

//Methods
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL GetErrorMessage(LPTSTR lpstrError, UINT nMaxError,	PUINT pnHelpContext = NULL);
	CString GetErrorMessage();

//Data members
	int m_nError;

protected:
	DECLARE_DYNAMIC(CWSocketException)
};


class SOCKMFC_EXT_CLASS CWSocket
{
public:
//Constructors / Destructors
  CWSocket();
  virtual ~CWSocket();

//Attributes
  void    Attach(SOCKET hSocket);
  SOCKET  Detach(); 
  void    GetPeerName(CString& sPeerAddress, UINT& nPeerPort);
  void    GetPeerName(SOCKADDR* lpSockAddr, int* lpSockAddrLen);
  void    GetSockName(CString& sSocketAddress, UINT& nSocketPort);
  void    GetSockName(SOCKADDR* lpSockAddr, int* lpSockAddrLen);
  void    SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel = SOL_SOCKET);
  void    GetSockOpt(int nOptionName, void* lpOptionValue, int* lpOptionLen, int nLevel = SOL_SOCKET);
  BOOL    IsCreated() const; 
  BOOL    IsReadible(DWORD dwTimeout);
  BOOL    IsWritable(DWORD dwTimeout);

//Methods
  void    Create(BOOL bUDP = FALSE);
  void    Create(int nSocketType, int nProtocolType, int nAddressFormat);
  void    Accept(CWSocket& connectedSocket, sockaddr_in& clientAddress);
  void    Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL);
  void    Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen);
  void    Close();
  void    Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);
  void    Connect(LPCTSTR lpszHostAddress, UINT nHostPort);
#ifdef _WINSOCK2API_ //Connect methods which have a timeout parameter are only provided if we are using WinSock2
                     //because we are making use of Winsock2 functionality such as WSAEventSelect
  void    Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen, DWORD dwConnectionTimeout, BOOL bResetToBlockingMode = TRUE);
  void    Connect(LPCTSTR lpszHostAddress, UINT nHostPort, DWORD dwConnectionTimeout, BOOL bResetToBlockingMode = TRUE);
#endif
  void    ConnectViaSocks4(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, DWORD dwConnectionTimeout = 5000);
  void    ConnectViaSocks5(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, LPCTSTR lpszUserName = NULL, LPCTSTR lpszPassword = NULL, DWORD dwConnectionTimeout = 5000, BOOL bUDP = FALSE);
  void    ConnectViaHTTPProxy(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszHTTPServer, UINT nHTTPProxyPort, CString& sProxyResponse, LPCTSTR lpszUserName = NULL, LPCTSTR pszPassword = NULL, DWORD dwConnectionTimeout = 5000, LPCTSTR lpszUserAgent = NULL);
  void    IOCtl(long lCommand, DWORD* lpArgument);
  void    Listen(int nConnectionBacklog = SOMAXCONN);
  int     Receive(void* lpBuf, int nBufLen, int nFlags = 0);
  int     ReceiveFrom(void* lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags = 0);
  int     ReceiveFrom(void* lpBuf, int nBufLen, CString& sSocketAddress, UINT& nSocketPort, int nFlags = 0);
  int     Send(const void* pBuffer, int nBufLen, int nFlags = 0);
  int     SendTo(const void* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags = 0);
  int     SendTo(const void* lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress = NULL, int nFlags = 0);
  enum { receives = 0, sends = 1, both = 2 };
  void    ShutDown(int nHow = sends);

//Operators
  operator SOCKET();

//Static methods
  static void ThrowWSocketException(int nError = 0);

protected:
//Methods
  void ReadHTTPProxyResponse(DWORD dwTimeout, CString& sResponse);
  void ReadSocks5ConnectReply(DWORD dwTimeout);

//Member variables
  SOCKET m_hSocket;
};

#endif //__SOCMFC_H__
