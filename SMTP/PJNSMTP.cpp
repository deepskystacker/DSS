/*
Module : PJNSMTP.CPP
Purpose: Implementation for a MFC class encapsulation of the SMTP protocol
Created: PJN / 22-05-1998
History: PJN / 15-06-1998 1) Fixed the case where a single dot occurs on its own
                             in the body of a message
				              	  2) Class now supports Reply-To Header Field
                          3) Class now supports file attachments
   		   PJN / 18-06-1998 1) Fixed a memory overwrite problem which was occurring 
	  		                 with the buffer used for encoding base64 attachments
         PJN / 27-06-1998 1) The case where a line begins with a "." but contains
                          other text is now also catered for. See RFC821, Section 4.5.2
                          for further details.
                          2) m_sBody in CPJNSMTPMessage has now been made protected.
                          Client applications now should call AddBody instead. This
                          ensures that FixSingleDot is only called once even if the 
                          same message is sent a number of times.
                          3) Fixed a number of problems with how the MIME boundaries
                          were defined and sent.
                          4) Got rid of an unreferenced formal parameter 
                          compiler warning when doing a release build
         PJN / 11-09-1998 1) VC 5 project file is now provided
                          2) Attachment array which the message class contains now uses
                          references instead of pointers.
                          3) Now uses Sleep(0) to yield our time slice instead of Sleep(100),
                          this is the preferred way of writting polling style code in Win32
                          without serverly impacting performance.
                          4) All Trace statements now display the value as returned from
                          GetLastError
                          5) A number of extra asserts have been added
                          6) A AddMultipleRecipients function has been added which supports added a 
                          number of recipients at one time from a single string
                          7) Extra trace statements have been added to help in debugging
         PJN / 12-09-98   1) Removed a couple of unreferenced variable compiler warnings when code
                          was compiled with Visual C++ 6.0
                          2) Fixed a major bug which was causing an ASSERT when the CSMTPAttachment
                          destructor was being called in the InitInstance of the sample app. 
                          This was inadvertingly introduced for the 1.2 release. The fix is to revert 
                          fix 2) as done on 11-09-1998. This will also help to reduce the number of 
                          attachment images kept in memory at one time.
         PJN / 18-01-99   1) Full CC & BCC support has been added to the classes
         PJN / 22-02-99   1) Addition of a Get and SetTitle function which allows a files attachment 
                          title to be different that the original filename
                          2) AddMultipleRecipients now ignores addresses if they are empty.
                          3) Improved the reading of responses back from the server by implementing
                          a growable receive buffer
                          4) timeout is now 60 seconds when building for debug
         PJN / 25-03-99   1) Now sleeps for 250 ms instead of yielding the time slice. This helps 
                          reduce CPU usage when waiting for data to arrive in the socket
         PJN / 14-05-99   1) Fixed a bug with the way the code generates time zone fields in the Date headers.
         PJN / 10-09-99   1) Improved CPJNSMTPMessage::GetHeader to include mime field even when no attachments
                          are included.
         PJN / 16-02-00   1) Fixed a problem which was occuring when code was compiled with VC++ 6.0.
         PJN / 19-03-00   1) Fixed a problem in GetHeader on Non-English Windows machines
                          2) Now ships with a VC 5 workspace. I accidentally shipped a VC 6 version in one of the previous versions of the code.
                          3) Fixed a number of UNICODE problems
                          4) Updated the sample app to deliberately assert before connecting to the author's SMTP server.
         PJN / 28-03-00   1) Set the release mode timeout to be 10 seconds. 2 seconds was causing problems for slow dial
                          up networking connections.
         PJN / 07-05-00   1) Addition of some ASSERT's in CPJNSMTPSocket::Connect
		     PP  / 16-06-00   The following modifications were done by Puneet Pawaia
						              1) Removed the base64 encoder from this file
						              2) Added the base64 encoder/decoder implementation in a separate 
						              file. This was done because base64 decoding was not part of 
							            the previous implementation
						              3) Added support for ESMTP connection. The class now attempts to 
						              authenticate the user on the ESMTP server using the username and
							            passwords supplied. For this connect now takes the username and 
							            passwords as parameters. These can be null in which case ESMTP 
							            authentication is not attempted
						              4) This class can now handle AUTH LOGIN and AUTH LOGIN PLAIN authentication
						              schemes on 
		     PP  / 19-06-00   The following modifications were done by Puneet Pawaia
				         		      1) Added the files md5.* containing the MD5 digest generation code
						              after modifications so that it compiles with VC++ 6
						              2) Added the CRAM-MD5 login procedure.
         PJN / 10-07-00   1) Fixed a problem with sending attachments > 1K in size
                          2) Changed the parameters to CPJNSMTPConnection::Connect
         PJN / 30-07-00   1) Fixed a bug in AuthLogin which was transmitting the username and password
                          with an extra "=" which was causing the login to failure. Thanks to Victor Vogelpoel for
                          finding this.
         PJN / 05-09-00   1) Added a CSMTP_NORSA preprocessor macro to allow the CPJNSMTPConnection code to be compiled
                          without the dependence on the RSA code.
         PJN / 28-12-2000 1) Removed an unused variable from ConnectESMTP.
                          2) Allowed the hostname as sent in the HELO command to be specified at run time 
                          in addition to using the hostname of the client machine
                          3) Fixed a problem where high ascii characters were not being properly encoded in
                          the quoted-printable version of the body sent.
                          4) Added support for user definable charset's for the message body.
                          5) Mime boundaries are now always sent irrespective if whether attachments are included or
                          not. This is required as the body is using quoted-printable.
                          6) Fixed a bug in sendLines which was causing small message bodies to be sent incorrectly
                          7) Now fully supports custom headers in the SMTP message
                          8) Fixed a copy and paste bug where the default port for the SMTP socket class was 110.
                          9) You can now specify the address on which the socket is bound. This enables the programmer
                          to decide on which NIC data should be sent from. This is especially useful on a machine
                          with multiple IP addresses.
                          10) Addition of functions in the SMTP connection class to auto dial and auto disconnect to 
                          the Internet if you so desire.
                          11) Sample app has been improved to allow Auto Dial and binding to IP addresses to be configured.
         PJN / 26-02-2001 1)  Charset now defaults to ISO 8859-1 instead of us-ascii
                          2)  Removed a number of unreferrenced variables from the sample app.
                          3)  Headers are now encoded if they contain non ascii characters.
                          4)  Fixed a bug in getLine, Thanks to Lev Evert for spotting this one.
                          5)  Made the charset value a member of the message class instead of the connection class
                          6)  Sample app now fully supports specifying the charset of the message
                          7)  Added a AddMultipleAttachments method to CPJNSMTPMessage
                          8)  Attachments can now be copied to each other via new methods in CSMTPAttachment
                          9)  Message class now contains copies of the attachments instead of pointers to them
                          10) Sample app now allows multiple attachments to be added
                          11) Removed an unnecessary assert in QuotedPrintableEncode
                          12) Added a Mime flag to the CPJNSMTPMessage class which allows you to decide whether or not a message 
                          should be sent as MIME. Note that if you have attachments, then mime is assumed.
                          13) CSMTPAttachment class has now become CPJNSMTPBodyPart in anticipation of full support for MIME and 
                          MHTML email support
                          14) Updated copright message in source code and documentation
                          15) Fixed a bug in GetHeader related to _tsetlocale usage. Thanks to Sean McKinnon for spotting this
                          problem.
                          16) Fixed a bug in SendLines when sending small attachments. Thanks to Deng Tao for spotting this
                          problem.
                          17) Removed the need for SendLines function entirely.
                          18) Now fully supports HTML email (aka MHTML)
                          19) Updated copyright messages in code and in documentation
         PJN / 17-06-2001 1) Fixed a bug in CPJNSMTPMessage::HeaderEncode where spaces were not being interpreted correctly. Thanks
                          to Jim Alberico for spotting this.
                          2) Fixed 2 issues with ReadResponse both having to do with multi-line responses. Thanks to Chris Hanson 
                          for this update.
         PJN / 25-06-2001 1) Code now links in Winsock and RPCRT40 automatically. This avoids client code having to specify it in 
                          their linker settings. Thanks to Malte and Phillip for spotting this issue.
                          2) Updated sample code in documentation. Thanks to Phillip for spotting this.
                          3) Improved the code in CPJNSMTPBodyPart::SetText to ensure lines are correctly wrapped. Thanks to 
                          Thomas Moser for this fix.
         PJN / 01-07-2001 1) Modified QuotedPrintableEncode to prevent the code to enter in an infinite loop due to a long word i.e. 
                          bigger than SMTP_MAXLINE, in this case, the word is breaked. Thanks to Manuel Gustavo Saraiva for this fix.
                          2) Provided a new protected variable in CPJNSMTPBodyPart called m_bQuotedPrintable to bypass the 
                          QuotedPrintableEncode function in cases that we don't want that kind of correction. Again thanks to 
                          Manuel Gustavo Saraiva for this fix.
         PJN / 15-07-2001 1) Improved the error handling in the function CPJNSMTPMessage::AddMultipleAttachments. In addition the 
                          return value has been changed from BOOL to int
         PJN / 11-08-2001 1) Fixed a bug in QuotedPrintableEncode which was wrapping encoding characters across multiple lines. 
                          Thanks to Roy He for spotting this.
                          2) Provided a "SendMessage" method which sends a email directly from disk. This allows you 
                          to construct your own emails and the use the class just to do the sending. This function also has the 
                          advantage that it efficiently uses memory and reports progress.
                          3) Provided support for progress notification and cancelling via the "OnSendProgress" virtual method.
         PJN / 29-09-2001 1) Fixed a bug in ReadResponse which occured when you disconnected via Dial-Up Networking
                          while a connection was active. This was originally spotted in my POP3 class.
                          2) Fixed a problem in CPJNSMTPBodyPart::GetHeader which was always including the "quoted-printable" even when 
                          m_bQuotedPrintable for that body part was FALSE. Thanks to "jason" for spotting this.
         PJN / 12-10-2001 1) Fixed a problem where GetBody was reporting the size as 1 bigger than it should have been. Thanks
                          to "c f" for spotting this problem.
                          2) Fixed a bug in the TRACE statements when a socket connection cannot be made.
                          3) The sample app now displays a filter of "All Files" when selecting attachments to send
                          4) Fixed a problem sending 0 byte attachments. Thanks to Deng Tao for spotting this problem.
         PJN / 11-01-2002 1) Now includes a method to send a message directly from memory. Thanks to Tom Allebrandi for this
                          suggestion.
                          2) Change function name "IsReadible" to be "IsReadable". I was never very good at English!.
                          3) Fixed a bug in CPJNSMTPBodyPart::QuotedPrintableEncode. If a line was exactly 76 characters long plus 
                          \r\n it produced an invalid soft linebreak of "\r=\r\n\n". Thanks to Gerald Egert for spotting this problem.
         PJN / 29-07-2002 1) Fixed an access violation in CPJNSMTPBodyPart::QuotedPrintableEncode. Thanks to Fergus Gallagher for 
                          spotting this problem.
                          2) Fixed a problem where in very rare cases, the QuotedPrintableEncode function produces a single dot in a 
                          line, when inserting the "=" to avoid the mail server's maxline limit. I inserted a call to FixSingleDot 
                          after calling the QuotedPrintableEncode function in GetBody. Thanks to Andreas Kappler for spotting this
                          problem.
                          3) Fixed an issue in CPJNSMTPBodyPart::GetHeader where to ensure some mail clients can correctly handle
                          body parts and attachments which have a filename with spaces in it. Thanks to Andreas kappler for spotting
                          this problem.
                          4) QP encoding is now only used when you specify MIME. This fixes a bug as reported by David Terracino.
                          5) Removed now unused "API Reference" link in HTML file supported the code.
         PJN / 10-08-2002 1) Fixed a number of uncaught file exceptions in CPJNSMTPBodyPart::GetBody (Tick), CPJNSMTPMessage::SaveToDisk (Tick), and 
                          CPJNSMTPConnection::SendMessage. Thanks to John Allan Miller for reporting this problem.
                          2) The CPJNSMTPConnection::SendMessage version of the method which sends a file directly from disk, now fails if the
                          file is empty.
                          3) Improved the sample app by displaying a wait cursor while a message from file is being sent
                          4) Improved the sample app by warning the user if mail settings are missing and then bringing up the configuration
                          dialog.
         PJN / 20-09-2002 1) Fixed a problem where the code "Coder.EncodedMessage" was not being converted from an ASCII string to a UNICODE 
                          string in calls to CString::Format. This was occurring when sending the username and password for "AUTH LOGIN" support
                          in addition to sending the "username digest" for "AUTH CRAM-MD5" support. Thanks to Serhiy Pavlov for reporting this
                          problem.
                          2) Removed a number of calls to CString::Format and instead replaced with string literal CString constructors instead.
         PJN / 03-10-2002 1) Quoted printable encoding didn't work properly in UNICODE. (invalid conversion from TCHAR to BYTE). Thanks to
                          Serhiy Pavlov for reporting this problem.
                          2) Subject encoding didn't work properly in UNICODE. (invalid conversion from TCHAR to BYTE). Thanks to Serhiy Pavlov
                          for reporting this problem.
                          3) It didn't insert "charset" tag into root header for plain text messages (now it includes "charset" into plain text 
                          messages too). Thanks to Serhiy Pavlov for reporting this problem.
         PJN / 04-10-2002 1) Fixed an issue where the header / body separator was not being sent correctly for mails with attachments or when
                          the message is MIME encoded. Thanks to Serhiy Pavlov for reporting this problem.
                          2) Fixed a bug in QuotedPrintableEncode and HeaderEncode which was causing the errors. Thanks to Antonio Maiorano 
                          for reporting this problem.
                          3) Fixed an issue with an additional line separator being sent between the header and body of emails. This was only
                          evident in mail clients if a non mime email without attachments was sent.
         PJN / 11-12-2002 1) Review all TRACE statements for correctness
                          2) Provided a virtual OnError method which gets called with error information 
         PJN / 07-02-2003 1) Addition of a "BOOL bGracefully" argument to Disconnect so that when an application cancels the sending of a 
                          message you can pass FALSE and close the socket without properly terminating the SMTP conversation. Thanks to
                          "nabocorp" for this nice addition.
         PJN / 19-03-2003 1) Addition of copy constructors and operator= to CPJNSMTPMessage class. Thanks to Alexey Vasilyev for this suggestion.
         PJN / 13-04-2003 1) Fixed a bug in the handling of EHLO responses. Thanks to "Senior" for the bug report and the fix.
         PJN / 16-04-2003 1) Enhanced the CPJNSMTPAddress constructor to parse out the email address and friendly name rather than assume
                          it is an email address.
                          2) Reworked the syntax of the CPJNSMTPMessage::ParseMultipleRecipients method. Also now internally this function
                          uses the new CPJNSMTPAddress constructor
         PJN / 19-04-2003 1) Fixed a bug in the CPJNSMTPAddress constructor where I was mixing up the friendly name in the "<>" separators,
                          when it should have been the email address. 
         PJN / 04-05-2003 1) Fixed an issue where the class doesn't convert the mail body and subject to the wanted encoding but rather changes 
                          the encoding of the email in the email header. Since the issue of supporting several languages is a complicated one 
                          I've decided that we could settle for sending all CPJNSMTPConnection emails in UTF-8. I've added some conversion functions 
                          to the class that - at this point - always converts the body and subject of the email to UTF-8. A big thanks to Itamar 
                          Kerbel for this nice addition.
                          2) Moved code and sample app to VC 6.
         PJN / 05-05-2003 1) Reworked the way UTF8 encoding is now done. What you should do if you want to use UTF-8 encoding is set the charset
                          to "UTF-8" and use either QP or base 64 encoding.
                          2) Reworked the automatic encoding of the subject line to use the settings as taken from the root SMTP body part
                          3) Only the correct headers according to the MIME RFC are now encoded.
                          4) QP encoding is the encoding mechanism now always used for headers.
                          5) Headers are now only encoded if required to be encoded.
         PJN / 12-05-2003 1) Fixed a bug where the X-Mailer header was being sent incorrectly. Thanks to Paolo Vernazza for reporting this problem.
                          2) Addition of X-Mailer header is now optional. In addition sample app now does not send the X-Mailer header.
                          3) The sample app now does not send the Reply-To header.
         PJN / 18-08-2003 1) Modified the return value from the ConnectToInternet method. Instead of it being a boolean, it is now an enum. This
                          allows client code to differentiate between two conditions that it couldn't previously, namely when an internet connection
                          already existed or if a new connection (presumably a dial-up connection was made). This allows client code to then
                          disconnect if a new connection was required. Thanks to Pete Arends for this nice addition.
         PJN / 05-10-2003 1) Reworked the CPJNSMTPConnection::ReadResponse method to use the timeout provided by IsReadable rather than calling sleep.
                          Thanks to Clarke Brunt for reporting this issue.
         PJN / 03-11-2003 1) Simplified the code in CPJNSMTPConnection::ReadResponse. Thanks to Clarke Brunt for reporting this issue.
         PJN / 03-12-2003 1) Made code which checks the Login responses which contain "Username" and "Password" case insensitive. Thanks to 
                          Zhang xiao Pan for reporting this problem.
         PJN / 11-12-2003 1) Fixed an unreferrenced variable in CPJNSMTPConnection::OnError as reported by VC.Net 2003
                          2) DEBUG_NEW macro is now only used on VC 6 and not on VC 7 or VC 7.1. This avoids a problem on these compilers
                          where a conflict exists between it and the STL header files. Thanks to Alex Evans for reporting this problem.
         PJN / 31-01-2004 1) Fixed a bug in CPJNSMTPBodyPart::GetBody where the size of the body part was incorrectly calculating the size if the
                          encoded size was an exact multiple of 76. Thanks to Kurt Emanuelson and Enea Mansutti for reporting this problem.
         PJN / 07-02-2004 1) Fixed a bug in CPJNSMTPBodyPart::SetText where the code would enter an endless loop in the Replace function. It has now 
                          been replaced with CString::Replace. This now means that the class will not now compile on VC 5. Thanks to Johannes Philipp 
                          Grohs for reporting this problem.
                          2) Fixed a number of warnings when the code is compiled with VC.Net 2003. Thanks to Edward Livingston for reporting
                          this issue.
         PJN / 18-02-2004 1) You can now optionally set the priority of an email thro the variable CPJNSMTPMessage::m_Priority. Thanks to Edward 
                          Livingston for suggesting this addition.
         PJN / 04-03-2004 1) To avoid conflicts with the ATL Server class of the same name, namely "CSMTPConnection", the class is now called 
                          "CPJNSMTPConnection". To provide for easy upgrading of code, "CSMTPConnection" is now defined to be "CPJSMTPConnection" 
                          if the code detects that the ATL Server SMTP class is not included. Thanks to Ken Jones for reporting this issue.
         PJN / 13-03-2004 1) Fixed a problem where the CPJNSMTPBodyPart::m_dwMaxAttachmentSize value was not being copied in the CPJNSMTPBodyPart::operator=
                          method. Thanks to Gerald Egert for reporting this problem and the fix.
         PJN / 05-06-2004 1) Fixed a bug in CSMTPConnection::ReadResponse, where the wrong parameters were being null terminated. Thanks to "caowen" 
                          for this update.
                          2) Updated the CSMTPConnection::ReadResponse function to handle multiline responses in all cases. Thanks to Thomas Liebethal
                          for reporting this issue.
         PJN / 07-06-2004 1) Fixed a potential buffer overflow issue in CPJNSMTPConnection::ReadResponse when certain multi line responses are received
         PJN / 30-09-2004 1) Fixed a parsing issue in CPJNSMTPConnection::ReadResponse when multi line responses are read in as multiple packets. 
                          Thanks to Mark Smith for reporting this problem.
                          2) Reworked the code which supports the various authentication mechanisms to support the correct terms. What was called 
                          "AUTH LOGIN PLAIN" support now more correctly uses the term "AUTH PLAIN". The names of the functions and enums have now 
                          also been reworked.
                          3) Did a review of the sample app code provided with the class so that the name of the modules, projects, exe etc is now
                          "PJNSMTPApp". Also reworked the name of some helper classes as well as the module name which supports the main class.
                          4) Reworked CPJNSMTPConnection::AuthLoginPlain (now called AuthPlain) to correctly handle the case where an invalid
                          response is received when expecting the username: response. Thanks to Mark Smith for reporting this problem.
         PJN / 23-12-2004 "Name" field in Content-Type headers is now quoted just like the filename field. Thanks to Mark Smith for reporting this
                          issue in conjunction with the mail client Eudora.
         PJN / 23-01-2005 1) All classes now uses exceptions to indicate errors. This means the whole area of handling errors in the code a whole lot
                          simpler. For example the OnError mechanism is gone along with all the string literals in the code. The actual code itself
                          is a whole lot simpler also. You should carefully review your code as a lot of the return values from methods (especially
                          CPJNSMTPConnection) are now void and will throw CSMTPException's. Thanks to Mark Smith for prompting this update.
                          2) General tidy up of the code following the testing of the new exception based code.
         PJN / 06-04-2005 1) Addition of PJNSMTP_EXT_CLASS define to the classes to allow the classes to be easily incorporated into extension DLLs.
                          Thanks to Arnaud Faucher for suggesting this addition.
                          2) Now support NTLM authentication. Thanks to Arnaud Faucher for this nice addition. NTLM Authentication is provided by
                          a new reusable class called "CNTLMClientAuth" in PJNNTLMAuth.cpp/h".
                          3) Fixed a bug in the sample app in the persistence of the Authentication setting. Thanks to Arnaud Faucher for reporting
                          this issue.
         PJN / 26-03-2005 1) Fixed compile problems with the code when the Force Conformance In For Loop Scope compile setting is used in 
                          Visual Studio .NET 2003. Thanks to Alexey Kuznetsov for reporting this problem.
                          2) Fixed compile problems when the code is compiled using the Detect 64-bit Portability Issues setting in Visual Studio
                          .NET 2003.
         PJN / 18-04-2005 1) Addition of a simple IsConnected() function inline with the author's CPop3Connection class. Thanks to Alexey Kuznetsov
                          for prompting this addition.
                          2) Addition of a MXLookup function which provides for convenient DNS lookups of MX records (the IP addresses of SMTP mail 
                          servers for a specific host domain). Internally this new function uses the new DNS functions which are provided with 
                          Windows 2000 or later. To ensure that the code continues to work correctly on earlier versions of Windows, the function 
                          pointers for the required functions are constructed at runtime using GetProcAddress. You can use this function to 
                          discover the IP address of the mail servers responsible for a specific domain. This allows you to deliver email directly 
                          to a domain rather than through a third party mail server. Thanks to Hans Dietrich for suggesting this nice addition.
         PJN / 23-04-2005 1. The code now uses a MS Crypto API implementation of the MD5 HMAC algorithm instead of custom code based on the RSA 
                          MD5 code. This means that there is no need for the RSA MD5.cpp, MD5.h and glob-md5.h to be included in client applications. 
                          In addition the build configurations for excluding the RSA MD5 code has been removed from the sample app. The new classes to 
                          perform the MD5 hashing are contained in PJNMD5.h and are generic enough to be included in client applications in their
                          own right.
         PJN / 01-05-2005 1. Now uses the author'ss CWSocket sockets wrapper class.
                          2. Added support for connecting via Socks4, Socks5 and HTTP proxies 
                          3. The last parameter to Connect has now been removed and is configured via an accessor / mutator pair Get/SetBoundAddress.
                          4. CSMTPException class is now called CPJNSMTPException.
                          5. Class now uses the Base64 class as provided with CWSocket. This means that the modules Base64Coder.cpp/h are no longer
                          distributed or required and should be removed from any projects which use the updated SMTP classes.
                          6. Fixed a bug in SaveToDisk method whereby existing files were not being truncated prior to saving the email message.
                          7. All calls to allocate heap memory now generate a CPJNSMTPException if the allocation fails.
                          8. Addition of a MXLookupAvailable() function so that client code can know a priori if DNS MX lookups are supported.
                          9. Sample app now allows you to use the MXLookup method to determine the mail server to use based on the first email address
                          you provide in the To field.
         PJN / 03-05-2005 1. Fixed a number of warnings when the code is compiled in Visual Studio .NET 2003. Thanks to Nigel Delaforce for
                          reporting these issues.
         PJN / 24-07-2005 1. Now includes support for parsing "(...)" comments in addresses. Thanks to Alexey Kuznetsov for this nice addition.
                          2. Fixed an issue where the mail header could contain a "Content-Transfer-Encoding" header. This header should only be 
                          applied to headers of MIME body parts rather than the mail header itself. Thanks to Jeroen van der Laarse, Terence Dwyer
                          and Bostjan Erzen for reporting this issue and the required fix.
                          3. Now calling both AddTextBody and AddHTMLBody both set the root body part MIME type to multipart/mixed. Previously
                          the code comments said one thing and did another (set it to "multipart/related"). Thanks to Bostjan Erzen for reporting
                          this issue.
         PJN / 14-08-2005 1. Minor update to include comments next to various locations in the code to inform users what to do if they get compile
                          errors as a result of compiling the code without the Platform SDK being installed. Thanks to Gudjon Adalsteinsson for 
                          prompting this update.
                          2. Fixed an issue when the code calls the function DnsQuery when the code is compiled for UNICODE. This was the result of 
                          a documentation error in the Platform SDK which incorrectly says that DnsQuery_W takes a ASCII string when in actual fact 
                          it takes a UNICODE string. Thanks to John Oustalet III for reporting this issue.
         PJN / 05-09-2005 1. Fixed an issue in MXLookup method where memory was not being freed correctly in the call to DnsRecordListFree. Thanks 
                          to Raimund Mettendorf for reporting this bug.
                          2. Fixed an issue where lines which had a single dot i.e. "." did not get mailed correctly. Thanks to Raimund Mettendorf
                          for reporting this issue.
                          3. Removed the call to FixSingleDot from SetText method as it is done at runtime as the email as being sent. This was 
                          causing problems for fixing up the dot issue when MIME encoded messages were being sent.
                          4. Fixed an issue where the FixSingleDot function was being called when saving a message to disk. This was incorrect
                          and it should only be done when actually sending an SMTP email.
         PJN / 07-09-2005 1. Fixed another single dot issue, this time where the body is composed of just a single dot. Thanks to Raimund Mettendorf
                          for reporting this issue.
                          2. Fixed a typo in the accessor function CPJNSMTPConnection::GetBoundAddress which was incorrectly called 
                          CPJNSMTPConnection::SetBoundAddress.
         PJN / 29-09-2005 1. Removed linkage to secur32.lib as all SSPI functions are now constructed at runtime using GetProcAddress. Thanks to 
                          Emir Kapic for this update.
                          2. 250 or 251 is now considered a successful response to the RCPT command. Thanks to Jian Peng for reporting this issue.
                          3. Fixed potential memory leaks in the implementation of NTLM authentication as the code was throwing exceptions
                          which the authentication code was not expected to handle.
         PJN / 02-10-2005 1. MXLookup functionality of the class can now be excluded via the preprocessor define "CPJNSMTP_NOMXLOOKUP". Thanks to
                          Thiago Luiz for prompting this update. 
                          2. Sample app now does not bother asking for username and password if authentication type is NTLM. Thanks to Emir Kapic
                          for spotting this issue.
                          3. Documentation has been updated to refer to issues with NTLM Authentication and SSPI. Thanks to Emir Kapic for prompting
                          this update.
                          4. Fixed an update error done for the previous version which incorrectly supported the additional 251 command for the "MAIL"
                          command instead of more correctly the "RCPT" command. Thanks to Jian Peng for reporting this issue.
         PJN / 07-10-2005 1. Fixed an issue where file attachments were incorrectly including a charset value in their headers. Thanks to Asle Rokstad
                          for reporting this issue.
         PJN / 16-11-2005 1. Now includes full support for connecting via SSL. For example sending mail thro Gmail using the server 
                          "smtp.gmail.com" on port 465. Thanks to Vladimir Sukhoy for providing this very nice addition (using of course the author's 
                          very own OpenSSL wrappers!).
         PJN / 29-06-2006 1. Code now uses new C++ style casts rather than old style C casts where necessary. 
                          2. Integrated ThrowPJNSMTPException function into CPJNSMTPConnection class and renamed to ThrowPJNSMTPException
                          3. Combined the functionality of the _PJNSMTP_DATA class into the main CPJNSMTPConnection class.
                          4. Updated the code to clean compile on VC 2005
                          5. Updated copyright details.
                          6. Removed unnecessary checks for failure to allocate via new since in MFC these will throw CMemoryExceptions!.
                          7. Optimized CPJNSMTPException constructor code
         PJN / 14-07-2006 1. Fixed a bug in CPJNSMTPConnection where when compiled in UNICODE mode, NTLM authentication would fail. Thanks to 
                          Wouter Demuynck for providing the fix for this bug.
                          2. Reworked the CPJNSMTPConnection::NTLMAuthPhase(*) methods to throw standard CPJNSMTPException exceptions if there are any 
                          problems detected. This can be done now that the CNTLMClientAuth class is resilient to exceptions being thrown.
         PJN / 17-07-2006 1. Updated the code in the sample app to allow > 30000 characters to be entered into the edit box which contains the body
                          of the email. This is achieved by placing a call to CEdit::SetLimitText in the OnInitDialog method of the dialog. Thanks to 
                          Thomas Noone for reporting this issue.
         PJN / 13-10-2006 1. Fixed an issue in Disconnect where if the call to ReadCommandResponse threw an exception, we would not close down the 
                          socket or set m_bConnected to FALSE. The code has been updated to ensure these are now done. This just provides a more
                          consistent debugging experience. Thanks to Alexey Kuznetsov for reporting this issue.
                          2. CPJNSMTPException::GetErrorMessage now uses the user default locale for formatting error strings.
                          3. CPJNSMTPMessage::ParseMultipleRecipients now supports email addresses of the form ""Friendly, More Friendly" <emailaddress>"
                          Thanks to Xiao-li Ling for this very nice addition.
                          4. Fixed some issues with cleaning up of header and body parts in CPJNSMTPConnection::SendBodyPart and 
                          CPJNSMTPMessage::WriteToDisk. Thanks to Xiao-li Ling for reporting this issue.
         PJN / 09-11-2006 1. Reverted CPJNSMTPException::GetErrorMessage to use the system default locale. This is consistent with how MFC
                          does its own error handling.
                          2. Now includes comprehensive support for DSN's (Delivery Status Notifications) as specified in RFC 3461. Thanks to 
                          Riccardo Raccuglia for prompting this update.
                          3. CPJNSMTPBodyPart::GetBody and CPJNSMTPConnection::SendMessage(const CString& sMessageOnFile... now does not open the disk
                          file for exclusive read access, meaning that other apps can read from the file. Thanks to Wouter Demuynck for reporting
                          this issue.
                          4. Fixed an issue in CPJNSMTPConnection::SendMessage(const CString& sMessageOnFile... where under certain circumstances
                          we could end up deleting the local "pSendBuf" buffer twice. Thanks to Izidor Rozman for reporting this issue.
         PJN / 30-03-2007 1. Fixed a bug in CPJNSMTPConnection::SetHeloHostname where an unitialized stack variable could potentially be used. Thanks
                          to Anthony Kowalski for reporting this bug.
                          2. Updated copyright details.
         PJN / 01-08-2007 1. AuthCramMD5, ConnectESMTP, ConnectSMTP, AuthLogin, and AuthPlain methods have now been made virtual.
                          2. Now includes support for a Auto Authentication protocol. This will detect what authentication methods the SMTP server 
                          supports and uses the "most secure" method available. If you do not agree with the order in which the protocol is chosen,
                          a virtual function called "ChooseAuthenticationMethod" is provided. Thanks to "zhangbo" for providing this very nice 
                          addition to the code.
                          3. Removed the definition for the defunct "AuthNTLM" method.
         PJN / 13-11-2007 1. Major update to the code to use VC2005 specific features. This version of the code and onwards will be supported only
                          on VC 2005 or later. Thanks to Andrey Babushkin for prompting this update. Please note that there has been breaking changes
                          to the names and layout of various methods and member variables. You should carefully analyze the sample app included in the
                          download as well as your client code for the required updates you will need to make.
                          2. Sample app included in download is now build for Unicode and links dynamically to MFC. It also links dynamically to the 
                          latest OpenSSL 0.9.8g dlls.
                          
Copyright (c) 1998 - 2007 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

Please note that I have been informed that C(PJN)SMTPConnection was being used to develop and send unsolicted bulk mail. 
This was not the intention of the code and the author explicitly forbids use of the code for any software of this kind without 
my explicit written consent.

*/


//////////////// Includes ////////////////////////////////////////////

#include "stdafx.h"
#include "PJNSMTP.h"
#include "resource.h"
#include "PJNMD5.h"

#ifndef _INC_LOCALE
#pragma message("To avoid this message, please put locale.h in your pre compiled header (usually stdafx.h)")
#include <locale.h>
#endif


//////////////// Macros / Locals /////////////////////////////////////

const int PJNSMTP_MAXLINE = 76;

#pragma comment(lib, "wsock32.lib") //Automatically link in the Winsock dll
#pragma comment(lib, "rpcrt4.lib")  //Automatically link in the RPC runtime dll

#ifndef CPJNSMTP_NONTLM
#ifndef SEC_SUCCESS
#define SEC_SUCCESS(Status) ((Status) >= 0)
#endif
#endif


//////////////// Implementation //////////////////////////////////////

CPJNSMPTBase64::CPJNSMPTBase64() : m_pBuf(NULL), 
                                   m_nSize(0)
{
}

CPJNSMPTBase64::~CPJNSMPTBase64()
{
  if (m_pBuf)
    delete [] m_pBuf;
}

void CPJNSMPTBase64::Encode(const BYTE* pData, int nSize, DWORD dwFlags)
{
  //Tidy up any heap memory we have been using
  if (m_pBuf)
    delete [] m_pBuf;

  //Calculate and allocate the buffer to store the encoded data
  m_nSize = EncodeGetRequiredLength(nSize, dwFlags) + 1; //We allocate an extra byte so that we can null terminate the result
  m_pBuf = new char[m_nSize];

  //Finally do the encoding
  if (!CBase64::Encode(pData, nSize, m_pBuf, &m_nSize, dwFlags))
    CPJNSMTPConnection::ThrowPJNSMTPException(IDS_PJNSMTP_FAIL_BASE64_ENCODE, FACILITY_ITF);

  m_pBuf[m_nSize] = '\0';
}

void CPJNSMPTBase64::Decode(LPCSTR pData, int nSize)
{
  //Tidy up any heap memory we have been using
  if (m_pBuf)
    delete [] m_pBuf;

  //Calculate and allocate the buffer to store the encoded data
  m_nSize = DecodeGetRequiredLength(nSize) + 1;
  m_pBuf = new char[m_nSize];

  //Finally do the encoding
  if (!CBase64::Decode(pData, nSize, (BYTE*) m_pBuf, &m_nSize))
    CPJNSMTPConnection::ThrowPJNSMTPException(IDS_PJNSMTP_FAIL_BASE64_DECODE, FACILITY_ITF);

  m_pBuf[m_nSize] = '\0';
}

void CPJNSMPTBase64::Encode(LPCSTR pszMessage, DWORD dwFlags)
{
  Encode(reinterpret_cast<const BYTE*>(pszMessage), static_cast<int>(strlen(pszMessage)), dwFlags);
}

void CPJNSMPTBase64::Decode(LPCSTR pszMessage)
{
  Decode(pszMessage, static_cast<int>(strlen(pszMessage)));
}

CPJNSMTPException::CPJNSMTPException(HRESULT hr, const CString& sLastResponse) : m_hr(hr), 
                                                                                 m_sLastResponse(sLastResponse)
{
}

CPJNSMTPException::CPJNSMTPException(DWORD dwError, DWORD dwFacility, const CString& sLastResponse) : m_hr(MAKE_HRESULT(SEVERITY_ERROR, dwFacility, dwError)),
                                                                                                      m_sLastResponse(sLastResponse)
{
}

BOOL CPJNSMTPException::GetErrorMessage(LPTSTR pstrError, UINT nMaxError, PUINT pnHelpContext)
{
	ASSERT(pstrError != NULL && AfxIsValidString(pstrError, nMaxError));

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

  BOOL bRet = FALSE;
  if (HRESULT_FACILITY(m_hr) == FACILITY_ITF)
  {
    //Simply load up the string from the string table
    CString sError;
    AfxFormatString1(sError, HRESULT_CODE(m_hr), m_sLastResponse);
    lstrcpyn(pstrError, sError, nMaxError);
    bRet = TRUE;
  }
  else
  {
  	LPTSTR lpBuffer;
	  bRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			                   NULL, HRESULT_CODE(m_hr), MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			                   reinterpret_cast<LPTSTR>(&lpBuffer), 0, NULL);

	  if (bRet == FALSE)
		  *pstrError = '\0';
	  else
	  {
		  lstrcpyn(pstrError, lpBuffer, nMaxError);
		  LocalFree(lpBuffer);
	  }
  }

	return bRet;
}

CString CPJNSMTPException::GetErrorMessage()
{
  CString rVal;
  LPTSTR pstrError = rVal.GetBuffer(4096);
  GetErrorMessage(pstrError, 4096, NULL);
  rVal.ReleaseBuffer();
  return rVal;
}

IMPLEMENT_DYNAMIC(CPJNSMTPException, CException)

#ifdef _DEBUG
void CPJNSMTPException::Dump(CDumpContext& dc) const
{
  //Let the base class do its thing
	CObject::Dump(dc);

	dc << "m_hr = " << m_hr;
}
#endif


CPJNSMTPAddress::CPJNSMTPAddress() 
{
}

CPJNSMTPAddress::CPJNSMTPAddress(const CPJNSMTPAddress& address)
{
  *this = address;
}

CPJNSMTPAddress::CPJNSMTPAddress(const CString& sAddress)
{
  //The local variable which we will operate on
  CString sTemp(sAddress);
  sTemp.Trim();

	//divide the substring into friendly names and e-mail addresses
	int nMark = sTemp.Find(_T('<'));
	int nMark2 = sTemp.Find(_T('>'));
	if ((nMark != -1) && (nMark2 != -1) && (nMark2 > (nMark+1)))
	{
		m_sEmailAddress = sTemp.Mid(nMark+1, nMark2 - nMark - 1);
		m_sFriendlyName = sTemp.Left(nMark);
    m_sFriendlyName.Trim();
  }
	else
	{
		nMark = sTemp.Find(_T('('));
		nMark2 = sTemp.Find(_T(')'));
		if ((nMark != -1) && (nMark2 != -1) && (nMark2 > (nMark+1)))
		{
			m_sEmailAddress = sTemp.Left(nMark);
			m_sEmailAddress.Trim();
			m_sFriendlyName = sTemp.Mid(nMark+1, nMark2 - nMark - 1);
			m_sFriendlyName.Trim();
		}
		else

		m_sEmailAddress = sTemp;
	}
}

CPJNSMTPAddress::CPJNSMTPAddress(const CString& sFriendly, const CString& sAddress) : m_sFriendlyName(sFriendly), 
                                                                                      m_sEmailAddress(sAddress) 
{
  ASSERT(m_sEmailAddress.GetLength()); //An empty address is not allowed
}

CPJNSMTPAddress& CPJNSMTPAddress::operator=(const CPJNSMTPAddress& r) 
{ 
  m_sFriendlyName = r.m_sFriendlyName; 
	m_sEmailAddress = r.m_sEmailAddress; 
	return *this;
}

CString CPJNSMTPAddress::GetRegularFormat(BOOL bEncode, const CString& sCharset) const
{
  ASSERT(m_sEmailAddress.GetLength()); //Email Address must be valid

  CString sAddress;
  if (m_sFriendlyName.IsEmpty())
    sAddress = m_sEmailAddress;  //Just transfer the address across directly
  else
  {
    if (bEncode)
    {
      CString sEncodedFriendly(CPJNSMTPBodyPart::HeaderEncode(m_sFriendlyName, sCharset));
      sAddress.Format(_T("%s <%s>"), sEncodedFriendly.operator LPCTSTR(), m_sEmailAddress.operator LPCTSTR());
    }
    else
      sAddress.Format(_T("%s <%s>"), m_sFriendlyName.operator LPCTSTR(), m_sEmailAddress.operator LPCTSTR());
  }

  return sAddress;
}


CPJNSMTPBodyPart::CPJNSMTPBodyPart() : m_sCharset(_T("iso-8859-1")), 
                                       m_sContentType(_T("text/plain")), 
                                       m_pParentBodyPart(NULL), 
                                       m_bQuotedPrintable(TRUE), 
                                       m_bBase64(FALSE)
{
  //Automatically generate a unique boundary separator for this body part by creating a guid
  m_sBoundary = CreateGUID();
}

CPJNSMTPBodyPart::CPJNSMTPBodyPart(const CPJNSMTPBodyPart& bodyPart)
{
  *this = bodyPart;
}

CPJNSMTPBodyPart::~CPJNSMTPBodyPart()
{
  //Free up the array memory
  for (INT_PTR i=0; i<m_ChildBodyParts.GetSize(); i++)
    delete m_ChildBodyParts.GetAt(i);
  m_ChildBodyParts.RemoveAll();
}

CPJNSMTPBodyPart& CPJNSMTPBodyPart::operator=(const CPJNSMTPBodyPart& bodyPart)
{
  m_sFilename           = bodyPart.m_sFilename;
  m_sText               = bodyPart.m_sText;       
  m_sTitle              = bodyPart.m_sTitle;      
  m_sContentType        = bodyPart.m_sContentType;
  m_sCharset            = bodyPart.m_sCharset;
  m_sContentBase        = bodyPart.m_sContentBase;
  m_sContentID          = bodyPart.m_sContentID;
  m_sContentLocation    = bodyPart.m_sContentLocation;
  m_pParentBodyPart     = bodyPart.m_pParentBodyPart;
  m_sBoundary           = bodyPart.m_sBoundary;
  m_bQuotedPrintable    = bodyPart.m_bQuotedPrintable;
  m_bBase64             = bodyPart.m_bBase64;

  //Free up the array memory
  for (INT_PTR i=0; i<m_ChildBodyParts.GetSize(); i++)
    delete m_ChildBodyParts.GetAt(i);
  m_ChildBodyParts.RemoveAll();

  //Now copy over the new object
  for (INT_PTR i=0; i<bodyPart.m_ChildBodyParts.GetSize(); i++)
  {
    CPJNSMTPBodyPart* pBodyPart = new CPJNSMTPBodyPart(*bodyPart.m_ChildBodyParts.GetAt(i));
    pBodyPart->m_pParentBodyPart  = this;
    m_ChildBodyParts.Add(pBodyPart);
  }

  return *this;
}

CString CPJNSMTPBodyPart::CreateGUID()
{
  UUID uuid;
  memset(&uuid, 0, sizeof(uuid));
  UuidCreate(&uuid);
  
  //Convert it to a string
#ifdef _UNICODE
  unsigned short* pszGuid = NULL;
#else
  unsigned char* pszGuid = NULL;
#endif
  UuidToString(&uuid, &pszGuid);

  //What will be the return value from this function
  CString sGUID(reinterpret_cast<TCHAR*>(pszGuid));

  //Free up the temp memory
  RpcStringFree(&pszGuid);

	return sGUID;
}

BOOL CPJNSMTPBodyPart::SetFilename(const CString& sFilename)
{
  //Validate our parameters
  ASSERT(sFilename.GetLength());  //You need to a valid Filename!

	//Hive away the filename and form the title from the filename
  TCHAR sPath[_MAX_PATH];
  TCHAR sFname[_MAX_FNAME];
  TCHAR sExt[_MAX_EXT];
  _tsplitpath_s(sFilename, NULL, 0, NULL, 0, sFname, sizeof(sFname)/sizeof(TCHAR), sExt, sizeof(sExt)/sizeof(TCHAR));
  _tmakepath_s(sPath, sizeof(sPath)/sizeof(TCHAR), NULL, NULL, sFname, sExt);
	m_sFilename = sFilename;
  m_sTitle = sPath;

  //Also sent the content type to be appropiate for an attachment
  m_sContentType = _T("application/octet-stream");

  return TRUE;
}

void CPJNSMTPBodyPart::SetText(const CString& sText)
{
  m_sText = sText;

  //Ensure lines are correctly wrapped
  m_sText.Replace(_T("\r\n"), _T("\n"));
  m_sText.Replace(_T("\r"), _T("\n"));
  m_sText.Replace(_T("\n"), _T("\r\n"));

  //Also set the content type while we are at it
  m_sContentType = _T("text/plain");
}

void CPJNSMTPBodyPart::SetContentID(const CString& sContentID) 
{
  m_sContentLocation.Empty();
  m_sContentID = sContentID; 
}

CString CPJNSMTPBodyPart::GetContentID() const 
{ 
  return m_sContentID; 
}

void CPJNSMTPBodyPart::SetContentLocation(const CString& sContentLocation) 
{ 
  m_sContentID.Empty();
  m_sContentLocation = sContentLocation; 
}

CString CPJNSMTPBodyPart::GetContentLocation() const 
{ 
  return m_sContentLocation; 
}

char CPJNSMTPBodyPart::HexDigit(int nDigit)
{
  if (nDigit < 10)
    return static_cast<char>(nDigit + '0');
  else
    return static_cast<char>(nDigit - 10 + 'A');
}

//Converts text to its Quoted printable equivalent according to RFC 2045
CStringA CPJNSMTPBodyPart::QuotedPrintableEncode(const CStringA& sText)
{
  CStringA sTemp;
  for (int i=0; i<sText.GetLength(); i++)
  {
    //Pull out the character to operate on
    char c = sText.GetAt(i);
    
    if (((c >= 33) && (c <= 60)) || ((c >= 62) && (c <= 126)) || (c == '\r') || (c == '\n') || (c == '\t') || (c == ' '))
      sTemp += c;
    else
    {
      //otherwise must quote the text
      sTemp += '=';
      sTemp += HexDigit((c & 0xF0) >> 4);
      sTemp += HexDigit(c & 0x0F);
    }
  }

  //Now insert soft line breaks where appropiate
  CStringA sOut;
  int nStartLine = 0;
  int nLen = sTemp.GetLength();
  for (int i=0; i<nLen; i++)
  {
    //Pull out the character to operate on
    char c = sTemp.GetAt(i);
    
    if (c == '\n' || c == '\r' || i == (nLen-1))
    {
      sOut += sTemp.Mid(nStartLine, i-nStartLine+1);
      nStartLine = i+1;
      continue;
    }

    if ((i - nStartLine) > PJNSMTP_MAXLINE)
    {
      BOOL bInWord = TRUE;
      while (bInWord)
      {
        if (i>1)
          bInWord = (!isspace(c) && sTemp[i-2] != _T('='));
        if (bInWord)
        {
          --i;
          c = static_cast<BYTE>(sTemp.GetAt(i));
        }

		    if (i == nStartLine)
		    {
			    i = nStartLine + PJNSMTP_MAXLINE;
			    break;
		    }
      }

      sOut += sTemp.Mid(nStartLine, i-nStartLine+1);
      sOut += "=\r\n";
      nStartLine = i+1;
    }
  }

  return sOut;
}

CStringA CPJNSMTPBodyPart::ConvertToUTF8(const CString& sText)
{
  //What will be the return value from this function
  CStringA sOutput;

#ifdef _UNICODE
  int nInputLen = sText.GetLength();
  int nChars = WideCharToMultiByte(CP_UTF8, 0, sText, nInputLen, NULL, 0, NULL, NULL); 
  if (nChars)
  {
    char* pszUTF8 = new char[nChars+1];
    nChars = WideCharToMultiByte(CP_UTF8, 0, sText, nInputLen, pszUTF8, nChars, NULL, NULL);
    if (nChars)
    {
      pszUTF8[nChars] = '\0';
      sOutput = pszUTF8;
    }
    delete [] pszUTF8;
  }
#else
  //If the code is built for ASCII then the data is already expected to be UTF8 encoded
  sOutput = sText;  
#endif

  return sOutput;
}

CStringA CPJNSMTPBodyPart::GetHeader()
{
  CString sHeaderT;
  if (m_sFilename.GetLength())
  {
    //Ok, it's a file  

    //Form the header to go along with this body part
    if (GetNumberOfChildBodyParts())
		  sHeaderT.Format(_T("\r\n\r\n--%s\r\nContent-Type: %s; name=\"%s\"; Boundary=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment; filename=\"%s\"\r\n"), 
                     m_pParentBodyPart->m_sBoundary.operator LPCTSTR(), m_sContentType.operator LPCTSTR(), m_sTitle.operator LPCTSTR(), m_sBoundary.operator LPCTSTR(), m_sTitle.operator LPCTSTR());
    else
		  sHeaderT.Format(_T("\r\n\r\n--%s\r\nContent-Type: %s; name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment; filename=\"%s\"\r\n"), 
                     m_pParentBodyPart->m_sBoundary.operator LPCTSTR(), m_sContentType.operator LPCTSTR(), m_sTitle.operator LPCTSTR(), m_sTitle.operator LPCTSTR());

  }
  else
  {
    //ok, it's some text

    //Form the header to go along with this body part
    ASSERT(m_pParentBodyPart);
    if (GetNumberOfChildBodyParts())
    {
      if (m_bBase64)
        sHeaderT.Format(_T("\r\n--%s\r\nContent-Type: %s; charset=%s; Boundary=\"%s\"\r\nContent-Transfer-Encoding: base64\r\n"),
                       m_pParentBodyPart->m_sBoundary.operator LPCTSTR(), m_sContentType.operator LPCTSTR(), m_sCharset.operator LPCTSTR(), m_sBoundary.operator LPCTSTR());
      else if (m_bQuotedPrintable)
        sHeaderT.Format(_T("\r\n--%s\r\nContent-Type: %s; charset=%s; Boundary=\"%s\"\r\nContent-Transfer-Encoding: quoted-printable\r\n"),
                       m_pParentBodyPart->m_sBoundary.operator LPCTSTR(), m_sContentType.operator LPCTSTR(), m_sCharset.operator LPCTSTR(), m_sBoundary.operator LPCTSTR());
      else
        sHeaderT.Format(_T("\r\n--%s\r\nContent-Type: %s; charset=%s; Boundary=\"%s\"\r\n"),
                       m_pParentBodyPart->m_sBoundary.operator LPCTSTR(), m_sContentType.operator LPCTSTR(), m_sCharset.operator LPCTSTR(), m_sBoundary.operator LPCTSTR());
    }
    else
    {
      if (m_bBase64)
        sHeaderT.Format(_T("\r\n--%s\r\nContent-Type: %s; charset=%s\r\nContent-Transfer-Encoding: base64\r\n"),
                       m_pParentBodyPart->m_sBoundary.operator LPCTSTR(), m_sContentType.operator LPCTSTR(), m_sCharset.operator LPCTSTR());
      else if (m_bQuotedPrintable)
        sHeaderT.Format(_T("\r\n--%s\r\nContent-Type: %s; charset=%s\r\nContent-Transfer-Encoding: quoted-printable\r\n"),
                       m_pParentBodyPart->m_sBoundary.operator LPCTSTR(), m_sContentType.operator LPCTSTR(), m_sCharset.operator LPCTSTR());
      else
        sHeaderT.Format(_T("\r\n--%s\r\nContent-Type: %s; charset=%s\r\n"),
                       m_pParentBodyPart->m_sBoundary.operator LPCTSTR(), m_sContentType.operator LPCTSTR(), m_sCharset.operator LPCTSTR());
    }
  }

  //Add the other headers
  if (m_sContentBase.GetLength())
  {
    CString sLine;
    sLine.Format(_T("Content-Base: %s\r\n"), m_sContentBase.operator LPCTSTR());
    sHeaderT += sLine;
  }
  if (m_sContentID.GetLength())
  {
    CString sLine;
    sLine.Format(_T("Content-ID: %s\r\n"), m_sContentID.operator LPCTSTR());
    sHeaderT += sLine;
  }
  if (m_sContentLocation.GetLength())
  {
    CString sLine;
    sLine.Format(_T("Content-Location: %s\r\n"), m_sContentLocation.operator LPCTSTR());
    sHeaderT += sLine;
  }
  sHeaderT += _T("\r\n");

  return CStringA(sHeaderT);
}

CStringA CPJNSMTPBodyPart::GetBody(BOOL bDoSingleDotFix)
{
	//if the body is text we must convert it to the declared encoding, this could create some
	//problems since windows conversion functions (such as WideCharToMultiByte) uses UINT
	//code page and not a String like HTTP uses.
	//For now we will add the support for UTF-8, to support other encodings we have to 
	//implement a mapping between the "internet name" of the encoding and its LCID(UINT)

  //What will be the return value from this function
  CStringA sBodyA;
	
  if (m_sFilename.GetLength())
  {
    //Ok, it's a file  
    HANDLE hFile = CreateFile(m_sFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
      //Get the length of the file (we only support sending files less than 4GB!)
      DWORD dwFileSizeHigh = 0;
      DWORD dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);
      if (dwFileSizeHigh)
      {
        //Close the handler before we throw the exception
        CloseHandle(hFile);
        
        CPJNSMTPConnection::ThrowPJNSMTPException(IDS_PJNSMTP_FILE_SIZE_TO_SEND_TOO_LARGE, FACILITY_ITF);
      }
      else if (dwFileSizeLow != INVALID_FILE_SIZE)
      {
        if (dwFileSizeLow)
        {
          //read in the contents of the input file
          BYTE* pszIn = new BYTE[dwFileSizeLow];

          //Read in the contents of the file
          DWORD dwBytesWritten = 0;
          if (ReadFile(hFile, pszIn, dwFileSizeLow, &dwBytesWritten, NULL))
          {
            //Do the encoding
            m_Coder.Encode(pszIn, dwBytesWritten, BASE64_FLAG_NONE);

            //Form the body for this body part
            sBodyA = m_Coder.Result();
          }

          //delete the input buffer
          delete [] pszIn;
        }
      }
      else
      {
        //Close the handler before we throw the exception
        CloseHandle(hFile);
        
        CPJNSMTPConnection::ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);
      }

      //Close the file now that we are finished with it
      CloseHandle(hFile);
    }
    else
      CPJNSMTPConnection::ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);
  }
  else
  {
    //ok, it's some text
    if (m_bBase64)
    {
      //Do the UTF8 conversion if necessary
  	  CStringA sBuff;
      if (m_sCharset.CompareNoCase(_T("UTF-8")) == 0)
	      sBuff = ConvertToUTF8(m_sText);
      else
        sBuff = CT2A(m_sText);

      //Do the encoding
      m_Coder.Encode(reinterpret_cast<const BYTE*>(sBuff.operator LPCSTR()), static_cast<int>(sBuff.GetLength()), BASE64_FLAG_NONE);

      //Form the body for this body part
      sBodyA = m_Coder.Result();
    }
    else if (m_bQuotedPrintable)
    {
      //Do the UTF8 conversion if necessary
  	  CStringA sBuff;
      if (m_sCharset.CompareNoCase(_T("UTF-8")) == 0)
	      sBuff = ConvertToUTF8(m_sText);
      else
        sBuff = CT2A(m_sText);

      //Do the encoding
	    sBodyA = QuotedPrintableEncode(sBuff);
      if (bDoSingleDotFix)
        FixSingleDotA(sBodyA);
    }
    else
    {
      //Do the UTF8 conversion if necessary
      if (m_sCharset.CompareNoCase(_T("UTF-8")) == 0)
	      sBodyA = ConvertToUTF8(m_sText);
      else
        sBodyA = CT2A(m_sText);

      //No encoding to do
      if (bDoSingleDotFix)
        FixSingleDotA(sBodyA);
    }
  }

  return sBodyA;
}

CStringA CPJNSMTPBodyPart::GetFooter()
{
  //Form the MIME footer
	CStringA sFooter;
  sFooter.Format("\r\n--%s--", CT2A(m_sBoundary).operator LPSTR());

  return sFooter;
}

INT_PTR CPJNSMTPBodyPart::GetNumberOfChildBodyParts() const
{
  return m_ChildBodyParts.GetSize();
}

INT_PTR CPJNSMTPBodyPart::AddChildBodyPart(CPJNSMTPBodyPart& bodyPart)
{
  CPJNSMTPBodyPart* pNewBodyPart = new CPJNSMTPBodyPart(bodyPart);
  pNewBodyPart->m_pParentBodyPart = this;
  ASSERT(m_sContentType.GetLength()); //Did you forget to call SetContentType

  return m_ChildBodyParts.Add(pNewBodyPart);
}

void CPJNSMTPBodyPart::RemoveChildBodyPart(INT_PTR nIndex)
{
  CPJNSMTPBodyPart* pBodyPart = m_ChildBodyParts.GetAt(nIndex);
  delete pBodyPart;
  m_ChildBodyParts.RemoveAt(nIndex);
}

CPJNSMTPBodyPart* CPJNSMTPBodyPart::GetChildBodyPart(INT_PTR nIndex)
{
  return m_ChildBodyParts.GetAt(nIndex);
}

CPJNSMTPBodyPart* CPJNSMTPBodyPart::GetParentBodyPart()
{
  return m_pParentBodyPart;
}

void CPJNSMTPBodyPart::FixSingleDotT(CString& sBody)
{
  //Handle a dot on the first line on its own (by replacing it with a double dot as referred to in 
  //section 4.5.2 of the SMTP RFC)  
  if ((sBody.Find(_T(".\r\n")) == 0) || (sBody == _T(".")))
    sBody.Insert(0, _T("."));

  //Also handle any subsequent single dot lines
  sBody.Replace(_T("\r\n."), _T("\r\n.."));
}

void CPJNSMTPBodyPart::FixSingleDotA(CStringA& sBody)
{
  //Handle a dot on the first line on its own (by replacing it with a double dot as referred to in 
  //section 4.5.2 of the SMTP RFC)  
  if ((sBody.Find(".\r\n") == 0) || (sBody == "."))
    sBody.Insert(0, ".");

  //Also handle any subsequent single dot lines
  sBody.Replace("\r\n.", "\r\n..");
}

CPJNSMTPBodyPart* CPJNSMTPBodyPart::FindFirstBodyPart(const CString sContentType)
{
  for (INT_PTR i=0; i<m_ChildBodyParts.GetSize(); i++)
  {
    CPJNSMTPBodyPart* pBodyPart = m_ChildBodyParts.GetAt(i);
    if (pBodyPart->m_sContentType == sContentType)
      return pBodyPart;
  }
  return NULL;
}

//Converts header text to its encoded form according to RFC 2047
CStringA CPJNSMTPBodyPart::QEncode(LPCSTR sText, LPCSTR sCharset)
{
  //Determine if a translation is needed
  BOOL bTranslationNeeded = FALSE;
  size_t nSize = strlen(sText);
  for (size_t i=0; i<nSize && !bTranslationNeeded; i++)
  {
    BYTE c = static_cast<BYTE>(sText[i]);
    bTranslationNeeded = (c > 127);
  }

  CStringA sOut;
  if (bTranslationNeeded)
  {
    sOut = "=?";
    sOut += sCharset;
    sOut += "?q?";
    for (size_t i=0; i<nSize; i++)
    {
      BYTE c = sText[i];
      
      if (c == ' ') // A space
        sOut += _T('_');
      else if ((c > 127) || (c == '=') || (c == '?') || (c == '_'))
      {
        //Must Quote the text
        sOut += _T('=');
        sOut += CPJNSMTPBodyPart::HexDigit((c & 0xF0) >> 4);
        sOut += CPJNSMTPBodyPart::HexDigit(c & 0x0F);
      }
      else
        sOut += c;
    }
    sOut += "?=";
  }
  else
  {
    //Just pass the text thro unmodified
    sOut = sText;
  }

  return sOut;
}

CStringA CPJNSMTPBodyPart::HeaderEncode(const CString& sText, const CString& sCharset)
{
  //Do the UTF8 conversion if necessary
	CStringA sLocalText;
  if (sCharset.CompareNoCase(_T("UTF-8")) == 0)
	  sLocalText = CPJNSMTPBodyPart::ConvertToUTF8(sText);
  else
    sLocalText = CT2A(sText);

  CT2A szAsciiCharset(sCharset);

  CStringA sOut;
	LPCSTR itr = sLocalText.operator LPCSTR();
  size_t nCurrentLineLength = strlen(itr);
	while (nCurrentLineLength > 70)
	{
		//copy the current line and move the pointer forward
		size_t length = (70 > nCurrentLineLength) ? nCurrentLineLength : 70;

		char buf[71];
    strncpy_s(buf, sizeof(buf), itr, length);
    buf[length] = '\0';

    //Prepare for the next time around
		itr += length;
    nCurrentLineLength = strlen(itr);

    //Encode the current
		sOut += QEncode(buf, szAsciiCharset);
	}
	sOut += QEncode(itr, szAsciiCharset);

  return sOut;
}


CPJNSMTPMessage::CPJNSMTPMessage() : m_sXMailer(_T("CPJNSMTPConnection v2.70")), 
                                     m_bMime(FALSE), 
                                     m_Priority(NO_PRIORITY),
                                     m_DSNReturnType(HEADERS_ONLY),
                                     m_DSN(static_cast<DWORD>(DSN_NOT_SPECIFIED))
{
}

CPJNSMTPMessage::CPJNSMTPMessage(const CPJNSMTPMessage& message)
{
  *this = message;
}

CPJNSMTPMessage& CPJNSMTPMessage::operator=(const CPJNSMTPMessage& message) 
{ 
  m_From          = message.m_From;
  m_sSubject      = message.m_sSubject;
  m_sXMailer      = message.m_sXMailer;
  m_ReplyTo       = message.m_ReplyTo;
  m_RootPart      = message.m_RootPart;
  m_Priority      = message.m_Priority;
  m_DSNReturnType = message.m_DSNReturnType;
  m_DSN           = message.m_DSN;
  m_sENVID        = message.m_sENVID;

  m_To.Copy(message.m_To);
  m_CC.Copy(message.m_CC);
  m_BCC.Copy(message.m_BCC);
  m_CustomHeaders.Copy(message.m_CustomHeaders);
  m_bMime = message.m_bMime;

	return *this;
}

void CPJNSMTPMessage::SetCharset(const CString& sCharset)
{
  m_RootPart.SetCharset(sCharset);
}

CString CPJNSMTPMessage::GetCharset() const
{
  return m_RootPart.GetCharset();
}

INT_PTR CPJNSMTPMessage::AddBodyPart(CPJNSMTPBodyPart& Attachment)
{
  SetMime(TRUE); //Body parts implies Mime
	return m_RootPart.AddChildBodyPart(Attachment);
}

void CPJNSMTPMessage::RemoveBodyPart(INT_PTR nIndex)
{
	m_RootPart.RemoveChildBodyPart(nIndex);
}

CPJNSMTPBodyPart* CPJNSMTPMessage::GetBodyPart(INT_PTR nIndex)
{
	return m_RootPart.GetChildBodyPart(nIndex);
}

INT_PTR CPJNSMTPMessage::GetNumberOfBodyParts() const
{
	return m_RootPart.GetNumberOfChildBodyParts();
}

void CPJNSMTPMessage::AddCustomHeader(const CString& sHeader)
{
  m_CustomHeaders.Add(sHeader);
}

CString CPJNSMTPMessage::GetCustomHeader(int nIndex)
{
  return m_CustomHeaders.GetAt(nIndex);
}

INT_PTR CPJNSMTPMessage::GetNumberOfCustomHeaders() const
{
  return m_CustomHeaders.GetSize();
}

void CPJNSMTPMessage::RemoveCustomHeader(int nIndex)
{
  m_CustomHeaders.RemoveAt(nIndex);
}

CStringA CPJNSMTPMessage::GetHeader()
{
  //Hive away the locale so that we can restore it later. We
  //require the English locale to ensure the date header is
  //formed correctly
  CString sOldLocale(_tsetlocale(LC_TIME, NULL));
  _tsetlocale(LC_TIME, _T("english"));

  //Form the Timezone info which will form part of the Date header
  TIME_ZONE_INFORMATION tzi;
  int nTZBias;
  if (GetTimeZoneInformation(&tzi) == TIME_ZONE_ID_DAYLIGHT)
    nTZBias = tzi.Bias + tzi.DaylightBias;
  else
    nTZBias = tzi.Bias;
  CString sTZBias;
  sTZBias.Format(_T("%+.2d%.2d"), -nTZBias/60, nTZBias%60);

  //Create the "Date:" part of the header
  CTime now(CTime::GetCurrentTime());
  CString sDate(now.Format(_T("%a, %d %b %Y %H:%M:%S ")));
  sDate += sTZBias;

  CString sCharset(m_RootPart.GetCharset());

  //Create the "To:" part of the header
  CString sTo;
  for (INT_PTR i=0; i<m_To.GetSize(); i++)
  {
    CPJNSMTPAddress& recipient = m_To.ElementAt(i);
    if (i)
 		  sTo += _T(",");
    sTo += recipient.GetRegularFormat(TRUE, sCharset);
  }

  //Create the "Cc:" part of the header
  CString sCc;
  for (INT_PTR i=0; i<m_CC.GetSize(); i++)
  {
    CPJNSMTPAddress& recipient = m_CC.ElementAt(i);
    if (i)
 		  sCc += _T(",");
    sCc += recipient.GetRegularFormat(TRUE, sCharset);
  }

  //No Bcc info added in header

  //Add the From and To fields
  CString sHeader(_T("From: "));
  sHeader += m_From.GetRegularFormat(TRUE, sCharset);
  sHeader += _T("\r\nTo: ");
  sHeader += sTo;
  sHeader += _T("\r\n");

  //Add the CC field if there is any
  if (sCc.GetLength())
  {
    sHeader += _T("Cc: ");
    sHeader += sCc;
    sHeader += _T("\r\n");
  }

  //add the subject
  sHeader += _T("Subject: ");
  sHeader += CPJNSMTPBodyPart::HeaderEncode(m_sSubject, sCharset);

  //X-Mailer fields
  if (m_sXMailer.GetLength())
  {
    sHeader += _T("\r\nX-Mailer: ");
    sHeader += m_sXMailer;
  }
  sHeader += _T("\r\n");

  //Add the Mime header if needed
  BOOL bHasChildParts = (m_RootPart.GetNumberOfChildBodyParts() != 0);

  if (m_bMime)
  {
    if (bHasChildParts)
    {
      CString sReply;
      sReply.Format(_T("MIME-Version: 1.0\r\nContent-Type: %s; boundary=\"%s\"\r\n"), m_RootPart.GetContentType().operator LPCTSTR(), m_RootPart.GetBoundary().operator LPCTSTR());
      sHeader += sReply;
    }
    else
    {
      CString sReply;
      sReply.Format(_T("MIME-Version: 1.0\r\nContent-Type: %s\r\n"), m_RootPart.GetContentType().operator LPCTSTR());
      sHeader += sReply;
    }
  }
  else
  {
    CString sReply;
    sReply.Format(_T("Content-Type: %s;\r\n\tcharset=%s\r\n"), m_RootPart.GetContentType().GetString(), m_RootPart.GetCharset().GetString());
    sHeader += sReply;
  }
  
	//Add the optional Reply-To Field
	if (m_ReplyTo.m_sEmailAddress.GetLength())
	{
		sHeader += _T("Reply-To: ");
		sHeader += m_ReplyTo.GetRegularFormat(TRUE, sCharset);
    sHeader += _T("\r\n");
	}

  //Date header
  sHeader += _T("Date: ");
  sHeader += sDate;
  sHeader += _T("\r\n");

  //The Priorty header
  switch (m_Priority)
  {
    case NO_PRIORITY:
    {
      break;
    }
    case LOW_PRIORITY:
    {
      sHeader += _T("X-Priority: 5\r\n");
      break;
    }
    case NORMAL_PRIORITY:
    {
      sHeader += _T("X-Priority: 3\r\n");
      break;
    }
    case HIGH_PRIORITY:
    {
      sHeader += _T("X-Priority: 1\r\n");
      break;
    }
    default:
    {
      ASSERT(FALSE);
      break;
    }
  }

  //Add the custom headers
  INT_PTR nCustomHeaders = m_CustomHeaders.GetSize();
  for (INT_PTR i=0; i<nCustomHeaders; i++)
  {
    sHeader += m_CustomHeaders.GetAt(i);
    
    //Add line separators for each header
    sHeader += _T("\r\n");
  }

  // restore original locale
  _tsetlocale(LC_TIME, sOldLocale);

	//Return the result
	return CStringA(sHeader);
}

INT_PTR CPJNSMTPMessage::ParseMultipleRecipients(const CString& sRecipients, CPJNSMTPAddressArray& recipients)
{
	ASSERT(sRecipients.GetLength()); //An empty string is now allowed

  //Empty out the array
  recipients.SetSize(0);
	
	//Loop through the whole string, adding recipients as they are encountered
	int length = sRecipients.GetLength();
	TCHAR* buf = new TCHAR[length + 1];	//Allocate a work area (don't touch parameter itself)
  _tcscpy_s(buf, length+1, sRecipients);

  BOOL bLeftQuotationMark=FALSE;
	for (int pos=0, start=0; pos<=length; pos++)
	{
		if (bLeftQuotationMark && buf[pos] != _T('"') )
			continue;
		if (bLeftQuotationMark && buf[pos] == _T('"'))
		{
			bLeftQuotationMark = FALSE;
			continue;
		}
		if (buf[pos] == _T('"'))
		{
			bLeftQuotationMark = TRUE;
			continue;
		}
	
		//Valid separators between addresses are ',' or ';'
		if ((buf[pos] == _T(',')) || (buf[pos] == _T(';')) || (buf[pos] == 0))
		{
			buf[pos] = 0;	//Redundant when at the end of string, but who cares.
      CString sTemp(&buf[start]);
      sTemp.Trim();

      //Let the CPJNSMTPAddress constructor do its work
      CPJNSMTPAddress To(sTemp);
      if (To.m_sEmailAddress.GetLength())
        recipients.Add(To);

      //Move on to the next position
			start = pos + 1;
		}
	}
    
  //Tidy up the heap memory we have used
	delete [] buf;

  //Return the number of recipients parsed
	return recipients.GetSize();
}

int CPJNSMTPMessage::AddMultipleAttachments(const CString& sAttachments)
{
  //What will be the return value from this function
  int nAttachments = 0;

	//Loop through the whole string, adding attachments as they are encountered
	int length = sAttachments.GetLength();
	TCHAR* buf = new TCHAR[length + 1];	// Allocate a work area (don't touch parameter itself)
  _tcscpy_s(buf, length+1, sAttachments);
	for (int pos=0, start=0; pos<=length; pos++)
	{
		//Valid separators between attachments are ',' or ';'
		if ((buf[pos] == _T(',')) || (buf[pos] == _T(';')) || (buf[pos] == 0))
		{
			buf[pos] = 0;	//Redundant when at the end of string, but who cares.
      CString sTemp(&buf[start]);

      //Finally add the new attachment to the array of attachments
      CPJNSMTPBodyPart attachment;
			sTemp.Trim();
      if (sTemp.GetLength())
      {
        BOOL bAdded = attachment.SetFilename(sTemp);
        if (bAdded)
        {
          ++nAttachments;
          AddBodyPart(attachment);
        }
      }

      //Move on to the next position
			start = pos + 1;
		}
	}

  //Tidy up the heap memory we have used
	delete [] buf;

	return nAttachments;
}

CString CPJNSMTPMessage::ConvertHTMLToPlainText(const CString& sHtml)
{
  //First pull out whats within the body tags
  CString sRet(sHtml);
  sRet.MakeUpper();
  int nStartCut = -1;
  int nStartBody = sRet.Find(_T("<BODY"));
  if (nStartBody != -1)
  {
    sRet = sHtml.Right(sHtml.GetLength() - nStartBody - 5);
    int nTemp = sRet.Find(_T('>'));
    nStartCut = nStartBody + nTemp + 6;
    sRet = sRet.Right(sRet.GetLength() - nTemp - 1);
  }
  sRet.MakeUpper();
  int nLength = sRet.Find(_T("</BODY"));

  //Finally do the actual cutting
  if (nLength != -1)
    sRet = sHtml.Mid(nStartCut, nLength);
  else
    sRet = sHtml;

  //Now strip all html tags
  int nStartTag = sRet.Find(_T('<'));
  int nEndTag = sRet.Find(_T('>'));
  while (nStartTag != -1 && nEndTag != -1)
  {
    sRet = sRet.Left(nStartTag) + sRet.Right(sRet.GetLength() - nEndTag - 1);
    nStartTag = sRet.Find(_T('<'));
    nEndTag = sRet.Find(_T('>'));
  }

  sRet.Trim();
  return sRet;
}

void CPJNSMTPMessage::AddTextBody(const CString& sBody)
{
  if (m_bMime)
  {
    CPJNSMTPBodyPart* pTextBodyPart = m_RootPart.FindFirstBodyPart(_T("text/plain"));
    if (pTextBodyPart)
      pTextBodyPart->SetText(sBody);
    else
    {
      //Create a text body part
      CPJNSMTPBodyPart oldRoot(m_RootPart);

      //Reset the root body part to be multipart/mixed
      m_RootPart.SetCharset(oldRoot.GetCharset());
      m_RootPart.SetText(_T("This is a multi-part message in MIME format"));
      m_RootPart.SetContentType(_T("multipart/mixed"));

      //Just add the text/plain body part (directly to the root)
      CPJNSMTPBodyPart text;
      text.SetCharset(oldRoot.GetCharset());
      text.SetText(sBody);

      //Hook everything up to the root body part
      m_RootPart.AddChildBodyPart(text);
    }
  }
  else
  {
    //Let the body part class do all the work
    m_RootPart.SetText(sBody);
  }
}

void CPJNSMTPMessage::AddHTMLBody(const CString& sBody, const CString& sContentBase)
{
  ASSERT(m_bMime); //You forgot to make this a MIME message using SetMime(TRUE)

  CPJNSMTPBodyPart* pHtmlBodyPart = m_RootPart.FindFirstBodyPart(_T("text/html"));
  if (pHtmlBodyPart)
    pHtmlBodyPart->SetText(sBody);
  else
  {
    //Remember some of the old root settings before we write over it
    CPJNSMTPBodyPart oldRoot = m_RootPart;

    //Reset the root body part to be multipart/mixed
    m_RootPart.SetCharset(oldRoot.GetCharset());
    m_RootPart.SetText(_T("This is a multi-part message in MIME format"));
    m_RootPart.SetContentType(_T("multipart/mixed"));

    //Just add the text/html body part (directly to the root)
    CPJNSMTPBodyPart html;
    html.SetCharset(oldRoot.GetCharset());
    html.SetText(sBody);
    html.SetContentType(_T("text/html"));
    html.SetContentBase(sContentBase);

    //Hook everything up to the root body part
    m_RootPart.AddChildBodyPart(html);
  }
}

CString CPJNSMTPMessage::GetHTMLBody()
{
  //What will be the return value from this function
  CString sRet;

  if (m_RootPart.GetNumberOfChildBodyParts())
  {
    CPJNSMTPBodyPart* pHtml = m_RootPart.GetChildBodyPart(0);
    if (pHtml->GetContentType() == _T("text/html"))
      sRet = pHtml->GetText();
  }
  return sRet;
}

CString CPJNSMTPMessage::GetTextBody()
{
  return m_RootPart.GetText();
}

void CPJNSMTPMessage::SetMime(BOOL bMime)
{
  if (m_bMime != bMime)
  {
    m_bMime = bMime;

    //Reset the body body parts
    for (INT_PTR i=0; i<m_RootPart.GetNumberOfChildBodyParts(); i++)
      m_RootPart.RemoveChildBodyPart(i);

    if (bMime)
    {
      CString sText(GetTextBody());

      //Remember some of the old root settings before we write over it
      CPJNSMTPBodyPart oldRoot(m_RootPart);

      //Reset the root body part to be multipart/mixed
      m_RootPart.SetCharset(oldRoot.GetCharset());
      m_RootPart.SetText(_T("This is a multi-part message in MIME format"));
      m_RootPart.SetContentType(_T("multipart/mixed"));

      //Also readd the body if non - empty
      if (sText.GetLength())
        AddTextBody(sText);
    }
  }
}

void CPJNSMTPMessage::SaveToDisk(const CString& sFilename)
{
  //Open the file for writing
  HANDLE hFile = CreateFile(sFilename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    CPJNSMTPConnection::ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);

  //Set the file back to 0 in size
  if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    CPJNSMTPConnection::ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);
  if (!SetEndOfFile(hFile))
    CPJNSMTPConnection::ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);

  try
  {
    //Write out the Message Header
    CStringA sHeader(GetHeader());
    DWORD dwWritten = 0;
    if (!WriteFile(hFile, sHeader.operator LPCSTR(), sHeader.GetLength(), &dwWritten, NULL))
      CPJNSMTPConnection::ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);

	  //Write out the separator
    const char* pszBodyHeader = "\r\n";
    if (!WriteFile(hFile, pszBodyHeader, 2, &dwWritten, NULL))
      CPJNSMTPConnection::ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);

    //Write out the rest of the message
    BOOL bHasChildParts = (m_RootPart.GetNumberOfChildBodyParts() != 0);
    if (bHasChildParts || m_bMime)
    {
      //Write the root body part (and all its children)
      WriteToDisk(hFile, &m_RootPart, TRUE);
    }
    else
    {
      //Send the body
      CT2A szBody(m_RootPart.GetText());

      //Send the body
      if (!WriteFile(hFile, szBody.operator LPSTR(), static_cast<DWORD>(strlen(szBody)), &dwWritten, NULL))
        CPJNSMTPConnection::ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);
    }
  }
  catch(CPJNSMTPException* pEx)
  {
    //Close the file before we rethrow the exception
    CloseHandle(hFile);

    HRESULT hr = pEx->m_hr;
    CString sLastResponse(pEx->m_sLastResponse);
    pEx->Delete();

    //rethrow the exception
    CPJNSMTPConnection::ThrowPJNSMTPException(hr, sLastResponse);
  }

  CloseHandle(hFile);
}

void CPJNSMTPMessage::WriteToDisk(HANDLE hFile, CPJNSMTPBodyPart* pBodyPart, BOOL bRoot)
{
  //validate our parameters
  ASSERT(pBodyPart);

  if (!bRoot)
  {
    //First send this body parts header
    CStringA sHeader(pBodyPart->GetHeader());
    DWORD dwWritten = 0;
		if (!WriteFile(hFile, sHeader.operator LPCSTR(), sHeader.GetLength(), &dwWritten, NULL))
    {
      //Throw the exception
      CPJNSMTPConnection::ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);
    }
  }
  
  //Then the body parts body
  CStringA sBody(pBodyPart->GetBody(FALSE));
  DWORD dwWritten = 0;
	if (!WriteFile(hFile, sBody.operator LPCSTR(), sBody.GetLength(), &dwWritten, NULL))
    CPJNSMTPConnection::ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);

  //Recursively save all the child body parts
  INT_PTR nChildBodyParts = pBodyPart->GetNumberOfChildBodyParts();
  for (INT_PTR i=0; i<nChildBodyParts; i++)
  {
    CPJNSMTPBodyPart* pChildBodyPart = pBodyPart->GetChildBodyPart(i);
    WriteToDisk(hFile, pChildBodyPart, FALSE);
  }

  //Then the MIME footer if need be
  BOOL bSendFooter = (pBodyPart->GetNumberOfChildBodyParts() != 0);
  if (bSendFooter)
  {
    CStringA sFooter(pBodyPart->GetFooter());
  	if (!WriteFile(hFile, sFooter.operator LPCSTR(), sFooter.GetLength(), &dwWritten, NULL))
      CPJNSMTPConnection::ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);
  }
}


CPJNSMTPConnection::CPJNSMTPConnection() : m_bConnected(FALSE), 
                                           m_nLastCommandResponseCode(0), 
                                           m_bSSL(FALSE)
{
#ifdef _DEBUG
  m_dwTimeout = 90000; //default timeout of 90 seconds when debugging
#else
  m_dwTimeout = 60000;  //default timeout of 60 seconds for normal release code
#endif
  SetHeloHostname(_T("auto"));
  m_ProxyType = ptNone;
  m_nProxyPort = 1080;
  
  m_hWininet = LoadLibrary(_T("WININET.DLL"));
  if (m_hWininet)
  {
    m_lpfnInternetGetConnectedState = reinterpret_cast<LPINTERNETGETCONNECTEDSTATE>(GetProcAddress(m_hWininet, "InternetGetConnectedState"));
    m_lpfnInternetAutoDialHangup = reinterpret_cast<LPINTERNETAUTODIALHANGUP>(GetProcAddress(m_hWininet, "InternetAutodialHangup"));
    m_lpfnInternetAttemptConnect = reinterpret_cast<LPINTERNETATTEMPCONNECT>(GetProcAddress(m_hWininet, "InternetAttemptConnect"));
  }
  else
  {
    m_lpfnInternetGetConnectedState = NULL;
    m_lpfnInternetAutoDialHangup = NULL;
    m_lpfnInternetAttemptConnect = NULL;
  }
#ifndef CPJNSMTP_NOMXLOOKUP
  m_hDnsapi = LoadLibrary(_T("DNSAPI.DLL"));
  if (m_hDnsapi)
  {
    m_lpfnDnsRecordListFree = reinterpret_cast<LPDNSRECORDLISTFREE>(GetProcAddress(m_hDnsapi, "DnsRecordListFree"));
    #ifdef _UNICODE
    m_lpfnDnsQuery = reinterpret_cast<LPDNSQUERY>(GetProcAddress(m_hDnsapi, "DnsQuery_W"));
    #else
    m_lpfnDnsQuery = reinterpret_cast<LPDNSQUERY>(GetProcAddress(m_hDnsapi, "DnsQuery_A"));
    #endif
  }
  else
  {
    m_lpfnDnsRecordListFree = NULL;
    m_lpfnDnsQuery = NULL; 
  }
#endif
}

CPJNSMTPConnection::~CPJNSMTPConnection()
{
  if (m_bConnected)
  {
    //Deliberately handle exceptions here, so that the destructor does not throw exception
    try
    {
      Disconnect(TRUE);
    }
    catch(CPJNSMTPException* pEx)
    {
      pEx->Delete();
    }
  }
  
  if (m_hWininet)
  {
    FreeLibrary(m_hWininet);
    m_hWininet = NULL;
  }
#ifndef CPJNSMTP_NOMXLOOKUP
  if (m_hDnsapi)
  {
    FreeLibrary(m_hDnsapi);
    m_hDnsapi = NULL;
  }
#endif
}

void CPJNSMTPConnection::ThrowPJNSMTPException(DWORD dwError, DWORD dwFacility, const CString& sLastResponse)
{
	if (dwError == 0)
		dwError = ::WSAGetLastError();

	CPJNSMTPException* pException = new CPJNSMTPException(dwError, dwFacility, sLastResponse);

	TRACE(_T("Warning: throwing CPJNSMTPException for error %08X\n"), pException->m_hr);
	THROW(pException);
}

void CPJNSMTPConnection::ThrowPJNSMTPException(HRESULT hr, const CString& sLastResponse)
{
	CPJNSMTPException* pException = new CPJNSMTPException(hr, sLastResponse);

	TRACE(_T("Warning: throwing CPJNSMTPException for error %08X\n"), pException->m_hr);
	THROW(pException);
}

void CPJNSMTPConnection::SetHeloHostname(const CString& sHostname)
{
	//retrieve the localhost name if we are supplied the magic "auto" host name
	if (sHostname == _T("auto"))
	{
		//retrieve the localhost name
		char sHostName[_MAX_PATH] = "\0";
		if (gethostname(sHostName, sizeof(sHostName)) == 0)
		  m_sHeloHostname = CA2T(sHostName);
    else
      m_sHeloHostname = sHostname;
	}
  else
    m_sHeloHostname = sHostname;
}

void CPJNSMTPConnection::_CreateSocket()
{
	m_Socket.Create();
#ifndef CPJNSMTP_NOSSL
	if (m_bSSL) 
  {
		m_SSLCtx.Attach(SSL_CTX_new(SSLv23_client_method()));
		if (!m_SSL.Create(m_SSLCtx, m_Socket))
      ThrowPJNSMTPException(IDS_PJNSMTP_FAIL_CREATE_SSL_SOCKET, FACILITY_ITF, GetOpenSSLError());
	}
#endif
}

#ifndef CPJNSMTP_NOSSL
CString CPJNSMTPConnection::GetOpenSSLError()
{
  //What will be the return value
  CString sError;

  char szError[1024];
  unsigned long nError;
  do
  { 
    nError = ERR_get_error();
    if (nError)
    {
      ERR_error_string(nError, szError);
      sError += szError;
      sError += _T("\r\n");
    }
  }
  while (nError);

  return sError;
}
#endif

void CPJNSMTPConnection::_ConnectViaSocks4(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, DWORD dwConnectionTimeout)
{
#ifndef CPJNSMTP_NOSSL
	if (m_bSSL) 
  {
		if(!m_SSL.ConnectViaSocks4(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, dwConnectionTimeout)) 
			ThrowPJNSMTPException(IDS_PJNSMTP_FAIL_CONNECT_SOCKS4_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
		m_Socket.ConnectViaSocks4(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, dwConnectionTimeout);
}

void CPJNSMTPConnection::_ConnectViaSocks5(LPCTSTR lpszHostAddress, UINT nHostPort , LPCTSTR lpszSocksServer, UINT nSocksPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwConnectionTimeout, BOOL bUDP)
{
#ifndef CPJNSMTP_NOSSL
	if (m_bSSL) 
  {
		if (!m_SSL.ConnectViaSocks5(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, lpszUserName, lpszPassword, dwConnectionTimeout, bUDP)) 
			ThrowPJNSMTPException(IDS_PJNSMTP_FAIL_CONNECT_SOCKS5_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
		m_Socket.ConnectViaSocks5(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, lpszUserName, lpszPassword, dwConnectionTimeout, bUDP);
}

void CPJNSMTPConnection::_ConnectViaHTTPProxy(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszHTTPServer, UINT nHTTPProxyPort, CString & sProxyResponse, 
                                              LPCTSTR lpszUserName, LPCTSTR pszPassword, DWORD dwConnectionTimeout, LPCTSTR lpszUserAgent)
{
#ifndef CPJNSMTP_NOSSL
	if (m_bSSL) 
  {
		if (!m_SSL.ConnectViaHTTPProxy(lpszHostAddress, nHostPort, lpszHTTPServer, nHTTPProxyPort, sProxyResponse, lpszUserName, pszPassword, dwConnectionTimeout, lpszUserAgent))
			ThrowPJNSMTPException(IDS_PJNSMTP_FAIL_CONNECT_HTTPPROXY_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
		m_Socket.ConnectViaHTTPProxy(lpszHostAddress, nHostPort, lpszHTTPServer, nHTTPProxyPort, sProxyResponse, lpszUserName, pszPassword, dwConnectionTimeout, lpszUserAgent);
}

void CPJNSMTPConnection::_Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
#ifndef CPJNSMTP_NOSSL
	if (m_bSSL) 
  {
		if (!m_SSL.Connect(lpszHostAddress, nHostPort)) 
			ThrowPJNSMTPException(IDS_PJNSMTP_FAIL_CONNECT_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
		m_Socket.Connect(lpszHostAddress, nHostPort);
}

int CPJNSMTPConnection::_Send(const void* pBuffer, int nBuf)
{
#ifndef CPJNSMTP_NOSSL
	if (m_bSSL) 
  {
		int nSSLWrite = m_SSL.Send(pBuffer, nBuf);
    if (nSSLWrite < 1)
      ThrowPJNSMTPException(IDS_PJNSMTP_FAIL_SEND_VIASSL, FACILITY_ITF, GetOpenSSLError());
    return nSSLWrite;
  }
	else 
#endif
		return m_Socket.Send(pBuffer, nBuf);
}

int CPJNSMTPConnection::_Receive(void *pBuffer, int nBuf)
{
#ifndef CPJNSMTP_NOSSL
	if (m_bSSL) 
  {
		int nSSLReceive = m_SSL.Receive(pBuffer, nBuf);
    if (nSSLReceive <= 0)
    {
      int nSSLError = SSL_get_error(m_SSL, nSSLReceive);
      if (nSSLError != SSL_ERROR_WANT_READ)
        ThrowPJNSMTPException(IDS_PJNSMTP_FAIL_RECEIVE_VIASSL, FACILITY_ITF, GetOpenSSLError());
    }
    return nSSLReceive;
  }
	else 
#endif
		return m_Socket.Receive(pBuffer, nBuf);
}

void CPJNSMTPConnection::_Close()
{
#ifndef CPJNSMTP_NOSSL
	if (m_bSSL)
		m_SSL.Close();
	else 
#endif
		m_Socket.Close();
}

BOOL CPJNSMTPConnection::_IsReadible(DWORD dwTimeout)
{
#ifndef CPJNSMTP_NOSSL
	if (m_bSSL) 
		return m_SSL.IsReadible(dwTimeout);
	else 
#endif
		return m_Socket.IsReadible(dwTimeout);
}

#ifndef CPJNSMTP_NOSSL
void CPJNSMTPConnection::Connect(LPCTSTR pszHostName, AuthenticationMethod am, LPCTSTR pszUsername, LPCTSTR pszPassword, int nPort, BOOL bSSL)
#else
void CPJNSMTPConnection::Connect(LPCTSTR pszHostName, AuthenticationMethod am, LPCTSTR pszUsername, LPCTSTR pszPassword, int nPort)
#endif
{
	//Validate our parameters
  ASSERT(pszHostName);
  ASSERT(!m_bConnected);

#ifndef CPJNSMTP_NOSSL
  m_bSSL = bSSL;
#else
  m_bSSL = FALSE;
#endif

  //Create the socket
  try
  {
    _CreateSocket();
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();

    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  try
  {
    //Connect to the SMTP server

    //Bind if required
    if (m_sLocalBoundAddress.GetLength())
      m_Socket.Bind(0, m_sLocalBoundAddress);

    switch (m_ProxyType)
    {
      case ptSocks4:
      {
        _ConnectViaSocks4(pszHostName, nPort, m_sProxyServer, m_nProxyPort, m_dwTimeout);
        break;
      }
      case ptSocks5:
      {
        if (m_sProxyUserName.GetLength())
          _ConnectViaSocks5(pszHostName, nPort, m_sProxyServer, m_nProxyPort, m_sProxyUserName, m_sProxyPassword, m_dwTimeout, FALSE);
        else
          _ConnectViaSocks5(pszHostName, nPort, m_sProxyServer, m_nProxyPort, NULL, NULL, m_dwTimeout, FALSE);
        break;
      }
      case ptHTTP:
      {
        CString sProxyResponse;
        if (m_sProxyUserName.GetLength())
        {
          if (m_sUserAgent.GetLength())
            _ConnectViaHTTPProxy(pszHostName, nPort, m_sProxyServer, m_nProxyPort, sProxyResponse, m_sProxyUserName, m_sProxyPassword, m_dwTimeout, m_sUserAgent);
          else
            _ConnectViaHTTPProxy(pszHostName, nPort, m_sProxyServer, m_nProxyPort, sProxyResponse, m_sProxyUserName, m_sProxyPassword, m_dwTimeout, NULL);
        }
        else
        {
          if (m_sUserAgent.GetLength())
            _ConnectViaHTTPProxy(pszHostName, nPort, m_sProxyServer, m_nProxyPort, sProxyResponse, NULL, NULL, m_dwTimeout, m_sUserAgent);
          else
            _ConnectViaHTTPProxy(pszHostName, nPort, m_sProxyServer, m_nProxyPort, sProxyResponse, NULL, NULL, m_dwTimeout, NULL);
        }
        break;
      }
      case ptNone:
      {
        _Connect(pszHostName, nPort);
        break;
      }
      default:
      {
        ASSERT(FALSE);
        break;
      }
    }
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();

    //Disconnect before we rethrow the exception
    try
    {
      Disconnect(TRUE);
    }
    catch(CPJNSMTPException* pEx2)
    {
      pEx2->Delete();
    }

    //rethrow the exception
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //We're now connected
  m_bConnected = TRUE;

  try
  {
    //check the response to the login
    if (!ReadCommandResponse(220))
      ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_SMTP_LOGIN_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

	  //Negotiate Extended SMTP connection
	  if (am != AUTH_NONE)
		  ConnectESMTP(m_sHeloHostname, pszUsername, pszPassword, am);
    else
		  ConnectSMTP(m_sHeloHostname);
  }
  catch(CPJNSMTPException* pEx)
  {
    //Disconnect before we rethrow the exception
    try
    {
      Disconnect(TRUE);
    }
    catch(CPJNSMTPException* pEx2)
    {
      pEx2->Delete();
    }

    HRESULT hr = pEx->m_hr;
    CString sLastResponse = pEx->m_sLastResponse;
    pEx->Delete();

    //rethrow the exception
    ThrowPJNSMTPException(hr, sLastResponse);
  }
}

CPJNSMTPConnection::AuthenticationMethod CPJNSMTPConnection::ChooseAuthenticationMethod(const CString& sAuthMethods)
{
  //What will be the return value from this function
  AuthenticationMethod am = AUTH_AUTO;

  //decide which protocol to choose
	if (_tcsstr(sAuthMethods, _T("NTLM")) != NULL)
		am = AUTH_NTLM;
	else if (_tcsstr(sAuthMethods, _T("CRAM-MD5")) != NULL)
		am = AUTH_CRAM_MD5;
	else if (_tcsstr(sAuthMethods, _T("LOGIN")) != NULL)
		am = AUTH_LOGIN;
	else if (_tcsstr(sAuthMethods, _T("PLAIN")) != NULL)
		am = AUTH_PLAIN;
		
  return am;
}

//This function connects using one of the Extended SMTP methods i.e. EHLO
void CPJNSMTPConnection::ConnectESMTP(LPCTSTR pszLocalName, LPCTSTR pszUsername, LPCTSTR pszPassword, AuthenticationMethod am)
{
  //Validate our parameters
  ASSERT(pszUsername);
  ASSERT(pszPassword);
  ASSERT(am != AUTH_NONE);

  //Send the EHLO command
	CString sBuf;
	sBuf.Format(_T("EHLO %s\r\n"), pszLocalName);
  CT2A szBuf(sBuf);
  try
  {
    _Send(szBuf.operator LPSTR(), static_cast<int>(strlen(szBuf)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }
  
  //check the response to the EHLO command
  if (!ReadCommandResponse(250))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_EHLO_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

  //If we are doing "Auto authentication, then find the supported protocols of the SMTP server, then pick and use one
  if (am == AUTH_AUTO)
  {
    //Find the authentication methods supported by the SMTP server
    CString sResponse(GetLastCommandResponse());
		const TCHAR* sBegin = _T("250-AUTH ");
		const TCHAR* sEnd = _T("\r\n");
		int PosBegin = sResponse.Find(sBegin);
		if (PosBegin >= 0)
		{
			CString sAuthMethods;
			int PosEnd = sResponse.Find(sEnd, PosBegin);
			if (PosEnd >= 0)
				sAuthMethods = sResponse.Mid(PosBegin + 9, PosEnd - PosBegin - 9);
			else
				sAuthMethods = sResponse.Mid(PosBegin + 9);
			sAuthMethods.MakeUpper();

      //Now decide which protocol to choose via the virtual function to allow further client customization
			am = ChooseAuthenticationMethod(sAuthMethods);
		}
  }

  //What we do next depends on what authentication protocol we are using
  switch (am)
  {
    case AUTH_CRAM_MD5:
    {
      AuthCramMD5(pszUsername, pszPassword);
      break;
    }
    case AUTH_LOGIN:
    {
      AuthLogin(pszUsername, pszPassword);
      break;
    }
    case AUTH_PLAIN:
    {
      AuthPlain(pszUsername, pszPassword);
      break;
    }
    #ifndef CPJNSMTP_NONTLM
    case AUTH_NTLM:
    {
      SECURITY_STATUS ss = NTLMAuthenticate(pszUsername, pszPassword);
      if (!SEC_SUCCESS(ss))
        ThrowPJNSMTPException(ss, GetLastCommandResponse());
      break;
    }
    #endif  
    case AUTH_AUTO:
		{
			ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_EHLO_RESPONSE, FACILITY_ITF, GetLastCommandResponse());
			break;
		}    
    default:
    {
      ASSERT(FALSE);
      break;
    }
  }
}

//This function connects using standard SMTP connection i.e. HELO
void CPJNSMTPConnection::ConnectSMTP(LPCTSTR pszLocalName)
{
  //Send the HELO command
	CString sBuf;
	sBuf.Format(_T("HELO %s\r\n"), pszLocalName);
	CT2A szBuf(sBuf);
  try
  {
	  _Send(szBuf.operator LPSTR(), static_cast<int>(strlen(szBuf)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

	//check the response to the HELO command
	if (!ReadCommandResponse(250))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_HELO_RESPONSE, FACILITY_ITF, GetLastCommandResponse());
}

void CPJNSMTPConnection::Disconnect(BOOL bGracefully)
{
  //disconnect from the SMTP server if connected 
  HRESULT hr = S_OK;
  CString sLastResponse;
  if (m_bConnected)
  {
    if (bGracefully)
    {
      const char* sBuf = "QUIT\r\n";
      try
      {
        _Send(sBuf, static_cast<int>(strlen(sBuf)));
      }
      catch(CWSocketException* pEx)
      {
        hr = MAKE_HRESULT(SEVERITY_ERROR, pEx->m_nError, FACILITY_WIN32);
        pEx->Delete();
      }

      //Check the reponse
      try
      {
        if (!ReadCommandResponse(221))
        {
          hr = MAKE_HRESULT(SEVERITY_ERROR, IDS_PJNSMTP_UNEXPECTED_QUIT_RESPONSE, FACILITY_ITF);
          sLastResponse = GetLastCommandResponse();
        }
      }
      catch(CPJNSMTPException* pEx)
      {
        //Hive away the values in the local variables first. We do this to ensure that we 
        //end up executing the m_bConnected = FALSE and _Close code below
        hr = pEx->m_hr;
        sLastResponse = pEx->m_sLastResponse;
      
        pEx->Delete();
      }
    }

    //Reset the connected state
    m_bConnected = FALSE;
  }
 
  //free up our socket
  _Close();

  //Should we throw an expection
  if (FAILED(hr))
    ThrowPJNSMTPException(hr, sLastResponse);
}

void CPJNSMTPConnection::SendBodyPart(CPJNSMTPBodyPart* pBodyPart, BOOL bRoot)
{
  //Validate our parameters
  ASSERT(pBodyPart);

  if (!bRoot)
  {
    //First send this body parts header
    CStringA sHeader(pBodyPart->GetHeader());
    try
    {
		  _Send(sHeader.operator LPCSTR(), sHeader.GetLength());
    }
    catch(CWSocketException* pEx)
    {
      DWORD dwError = pEx->m_nError;
      pEx->Delete();
      ThrowPJNSMTPException(dwError, FACILITY_WIN32);
    }
  }
  
  //Then the body parts body
  CStringA sBody(pBodyPart->GetBody(TRUE));
  try
  {
    _Send(sBody.operator LPCSTR(), sBody.GetLength());
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //The recursively send all the child body parts
  INT_PTR nChildBodyParts = pBodyPart->GetNumberOfChildBodyParts();
  for (INT_PTR i=0; i<nChildBodyParts; i++)
  {
    CPJNSMTPBodyPart* pChildBodyPart = pBodyPart->GetChildBodyPart(i);
    SendBodyPart(pChildBodyPart, FALSE);
  }

  //Then the MIME footer if need be
  BOOL bSendFooter = (pBodyPart->GetNumberOfChildBodyParts() != 0);
  if (bSendFooter)
  {
    CStringA sFooter(pBodyPart->GetFooter());
    try
    {
	    _Send(sFooter.operator LPCSTR(), sFooter.GetLength());
    }
    catch(CWSocketException* pEx)
    {
      DWORD dwError = pEx->m_nError;
      pEx->Delete();
      ThrowPJNSMTPException(dwError, FACILITY_WIN32);
    }
  }
}

CString CPJNSMTPConnection::FormMailFromCommand(const CString& sEmailAddress, DWORD DSN, CPJNSMTPMessage::DSN_RETURN_TYPE DSNReturnType, CString& sENVID)
{
  //Validate our parameters
	ASSERT(sEmailAddress.GetLength());
	
	//What will be the return value from this function
  CString sBuf;
  
  if (DSN == CPJNSMTPMessage::DSN_NOT_SPECIFIED)
    sBuf.Format(_T("MAIL FROM:<%s>\r\n"), sEmailAddress.GetString());
  else
  {
    //Create an envelope ID if one has not been specified
    if (sENVID.IsEmpty())
      sENVID = CreateNEWENVID();
  
    if (DSNReturnType == CPJNSMTPMessage::HEADERS_ONLY)
      sBuf.Format(_T("MAIL FROM:<%s> RET=HDRS ENVID=%s\r\n"), sEmailAddress.GetString(), sENVID.GetString());
    else
      sBuf.Format(_T("MAIL FROM:<%s> RET=FULL ENVID=%s\r\n"), sEmailAddress.GetString(), sENVID.GetString());
  }

  return sBuf;  
}

void CPJNSMTPConnection::SendMessage(CPJNSMTPMessage& Message)
{
	//paramater validity checking
  ASSERT(m_bConnected); //Must be connected to send a message

  //Send the MAIL command
  CString sBuf(FormMailFromCommand(Message.m_From.m_sEmailAddress, Message.m_DSN, Message.m_DSNReturnType, Message.m_sENVID));
  CT2A szMailFrom(sBuf);
  try
  {
    _Send(szMailFrom, static_cast<int>(strlen(szMailFrom)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //check the response to the MAIL command
  if (!ReadCommandResponse(250))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_MAIL_FROM_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

  //Send the RCPT command, one for each recipient (includes the TO, CC & BCC recipients)

  //We must be sending the message to someone!
  ASSERT(Message.m_To.GetSize() + Message.m_CC.GetSize() + Message.m_BCC.GetSize());

  //First the "To" recipients
  for (INT_PTR i=0; i<Message.m_To.GetSize(); i++)
  {
    CPJNSMTPAddress& recipient = Message.m_To.ElementAt(i);
    SendRCPTForRecipient(Message.m_DSN, recipient);
  }

  //Then the "CC" recipients
  for (INT_PTR i=0; i<Message.m_CC.GetSize(); i++)
  {
    CPJNSMTPAddress& recipient = Message.m_CC.ElementAt(i);
    SendRCPTForRecipient(Message.m_DSN, recipient);
  }

  //Then the "BCC" recipients
  for (INT_PTR i=0; i<Message.m_BCC.GetSize(); i++)
  {
    CPJNSMTPAddress& recipient = Message.m_BCC.ElementAt(i);
    SendRCPTForRecipient(Message.m_DSN, recipient);
  }

  //Send the DATA command
  const char* szDataCommand = "DATA\r\n";
  try
  {
    _Send(szDataCommand, static_cast<int>(strlen(szDataCommand)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //check the response to the DATA command
  if (!ReadCommandResponse(354))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_DATA_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

  //Send the Message Header
  CStringA sHeader(Message.GetHeader());
  try
  {
    _Send(sHeader.operator LPCSTR(), sHeader.GetLength());
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

	//Send the Header / body Separator
  const char* szBodyHeader = "\r\n";
  try
  {
    _Send(szBodyHeader, static_cast<int>(strlen(szBodyHeader)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //Now send the contents of the mail    
  BOOL bHasChildParts = (Message.m_RootPart.GetNumberOfChildBodyParts() != 0);
  if (bHasChildParts || Message.m_bMime)
  {
    //Send the root body part (and all its children)
    SendBodyPart(&Message.m_RootPart, TRUE);
  }
  else
  {
    CString sBody(Message.m_RootPart.GetText());
    CPJNSMTPBodyPart::FixSingleDotT(sBody);
	  CT2A szBody(sBody);

    //Send the body
    try
    {
      _Send(szBody.operator LPSTR(), static_cast<int>(strlen(szBody)));
    }
    catch(CWSocketException* pEx)
    {
      DWORD dwError = pEx->m_nError;
      pEx->Delete();
      ThrowPJNSMTPException(dwError, FACILITY_WIN32);
    }
  }

  //Send the end of message indicator
  const char* szEOM = "\r\n.\r\n";
  try
  {
    _Send(szEOM, static_cast<int>(strlen(szEOM)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //check the response to the End of Message command
  if (!ReadCommandResponse(250))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_END_OF_MESSAGE_RESPONSE, FACILITY_ITF, GetLastCommandResponse());
}

BOOL CPJNSMTPConnection::OnSendProgress(DWORD /*dwCurrentBytes*/, DWORD /*dwTotalBytes*/)
{
  //By default just return TRUE to allow the mail to continue to be sent
  return TRUE; 
}

void CPJNSMTPConnection::SendMessage(BYTE* pMessage, DWORD dwTotalBytes, CPJNSMTPAddressArray& Recipients, const CPJNSMTPAddress& From, CString& sENVID, DWORD dwSendBufferSize, DWORD DSN, CPJNSMTPMessage::DSN_RETURN_TYPE DSNReturnType)
{
	//paramater validity checking
  ASSERT(m_bConnected); //Must be connected to send a message

  //Send the MAIL command
  CString sBuf(FormMailFromCommand(From.m_sEmailAddress, DSN, DSNReturnType, sENVID));
  CT2A szMailFrom(sBuf);
  try
  {
    _Send(szMailFrom.operator LPSTR(), static_cast<int>(strlen(szMailFrom)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //check the response to the MAIL command
  if (!ReadCommandResponse(250))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_MAIL_FROM_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

  //Must be sending to someone
  INT_PTR nRecipients = Recipients.GetSize();
  ASSERT(nRecipients);

  //Send the RCPT command, one for each recipient
  for (INT_PTR i=0; i<nRecipients; i++)
  {
    CPJNSMTPAddress& recipient = Recipients.ElementAt(i);
    SendRCPTForRecipient(DSN, recipient);
  }

  //Send the DATA command
  const char* szDataCommand = "DATA\r\n";
  try
  {
    _Send(szDataCommand, static_cast<int>(strlen(szDataCommand)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //check the response to the DATA command
  if (!ReadCommandResponse(354))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_DATA_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

  //Read and send the data a chunk at a time
  BOOL bMore = TRUE;
  BOOL bSuccess = TRUE;
  DWORD dwBytesSent = 0;
  BYTE* pSendBuf = pMessage; 
  do
  {
    DWORD dwRead = min(dwSendBufferSize, dwTotalBytes-dwBytesSent);
    dwBytesSent += dwRead;

    //Call the progress virtual method
    if (OnSendProgress(dwBytesSent, dwTotalBytes))
    {
      try
      {
        _Send(pSendBuf, dwRead);
      }
      catch(CWSocketException* pEx)
      {
        DWORD dwError = pEx->m_nError;
        pEx->Delete();
        ThrowPJNSMTPException(dwError, FACILITY_WIN32);
      }
    }
    else
    {
      //Abort the mail send (due to the progress virtual method returning FALSE
      bSuccess = FALSE;
    }

    //Prepare for the next time around
    pSendBuf += dwRead;
    bMore = (dwBytesSent < dwTotalBytes);
  }
  while (bMore && bSuccess);

  if (bSuccess)
  {
    //Send the end of message indicator
    const char* szEOM = "\r\n.\r\n";
    try
    {
      _Send(szEOM, static_cast<int>(strlen(szEOM)));
    }
    catch(CWSocketException* pEx)
    {
      DWORD dwError = pEx->m_nError;
      pEx->Delete();
      ThrowPJNSMTPException(dwError, FACILITY_WIN32);
    }

    //check the response to the End of Message command
    if (!ReadCommandResponse(250))
      ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_END_OF_MESSAGE_RESPONSE, FACILITY_ITF, GetLastCommandResponse());
  }
}

void CPJNSMTPConnection::SendMessage(const CString& sMessageOnFile, CPJNSMTPAddressArray& Recipients, const CPJNSMTPAddress& From, CString& sENVID, DWORD dwSendBufferSize, DWORD DSN, CPJNSMTPMessage::DSN_RETURN_TYPE DSNReturnType)
{
	//paramater validity checking
  ASSERT(m_bConnected); //Must be connected to send a message

  //Open up the file we want to send
  HANDLE hFile = CreateFile(sMessageOnFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);

  //Get the length of the file (we only support sending files less than 4GB)
  DWORD dwFileSizeHigh = 0;
  DWORD dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);
  DWORD dwError = GetLastError();
  if (dwFileSizeLow == INVALID_FILE_SIZE)
  {
    CloseHandle(hFile);
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }
  if (dwFileSizeHigh)
  {
    CloseHandle(hFile);
    ThrowPJNSMTPException(IDS_PJNSMTP_FILE_SIZE_TO_SEND_TOO_LARGE, FACILITY_ITF);
  }
  if (dwFileSizeLow == 0)
  {
    CloseHandle(hFile);
    ThrowPJNSMTPException(IDS_PJNSMTP_CANNOT_SEND_ZERO_BYTE_MESSAGE, FACILITY_ITF);
  }

  char* pSendBuf = NULL;

  try
  {
    //Send the MAIL command
	  ASSERT(From.m_sEmailAddress.GetLength());
    CString sBuf(FormMailFromCommand(From.m_sEmailAddress, DSN, DSNReturnType, sENVID));
    CT2A szMailFrom(sBuf);
    try
    {
      _Send(szMailFrom.operator LPSTR(), static_cast<int>(strlen(szMailFrom)));
    }
    catch(CWSocketException* pEx)
    {
      dwError = pEx->m_nError;
      pEx->Delete();
      ThrowPJNSMTPException(dwError, FACILITY_WIN32);
    }

    //check the response to the MAIL command
    if (!ReadCommandResponse(250))
      ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_MAIL_FROM_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

    //Must be sending to someone
    INT_PTR nRecipients = Recipients.GetSize();
    ASSERT(nRecipients);

    //Send the RCPT command, one for each recipient
    for (INT_PTR i=0; i<nRecipients; i++)
    {
      CPJNSMTPAddress& recipient = Recipients.ElementAt(i);
      SendRCPTForRecipient(DSN, recipient);
    }

    //Send the DATA command
    const char* szDataCommand = "DATA\r\n";
    try
    {
      _Send(szDataCommand, static_cast<int>(strlen(szDataCommand)));
    }
    catch(CWSocketException* pEx)
    {
      dwError = pEx->m_nError;
      pEx->Delete();
      ThrowPJNSMTPException(dwError, FACILITY_WIN32);
    }

    //check the response to the DATA command
    if (!ReadCommandResponse(354))
      ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_DATA_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

    //Allocate a buffer we will use in the sending
    pSendBuf = new char[dwSendBufferSize];

    //Read and send the data a chunk at a time
    BOOL bMore = TRUE;
    BOOL bSuccess = TRUE;
    DWORD dwBytesSent = 0; 
    do
    {
      //Read the chunk from file
      DWORD dwRead = 0;
      if (!ReadFile(hFile, pSendBuf, dwSendBufferSize, &dwRead, NULL))
        ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);
      bMore = (dwRead == dwSendBufferSize);

      //Send the chunk
      if (dwRead)
      {
        dwBytesSent += dwRead;

        //Call the progress virtual method
        if (OnSendProgress(dwBytesSent, dwFileSizeLow))
        {
          try
          {
            _Send(pSendBuf, dwRead);
          }
          catch(CWSocketException* pEx)
          {
            dwError = pEx->m_nError;
            pEx->Delete();
            ThrowPJNSMTPException(dwError, FACILITY_WIN32);
          }
        }
        else
          bSuccess = FALSE; //Abort the mail send (due to the progress virtual method returning FALSE)
      }
    }
    while (bMore && bSuccess);

    //Tidy up the heap memory we have used
    delete [] pSendBuf;
    pSendBuf = NULL;

    if (bSuccess)
    {
      //Send the end of message indicator
      const char* szEOM = "\r\n.\r\n";
      try
      {
        _Send(szEOM, static_cast<int>(strlen(szEOM)));
      }
      catch(CWSocketException* pEx)
      {
        dwError = pEx->m_nError;
        pEx->Delete();
        ThrowPJNSMTPException(dwError, FACILITY_WIN32);
      }

      //check the response to the End of Message command
      if (!ReadCommandResponse(250))
        ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_END_OF_MESSAGE_RESPONSE, FACILITY_ITF, GetLastCommandResponse());
    }
  }
  catch(CPJNSMTPException* pEx)
  {
    //Explicitly closing the file before we rethrow the exception
    CloseHandle(hFile);

    //Tidy up the heap memory we have used
    if (pSendBuf)
      delete [] pSendBuf;

    HRESULT hr = pEx->m_hr;
    CString sLastResponse = pEx->m_sLastResponse;
    pEx->Delete();

    //rethrow the exception
    ThrowPJNSMTPException(hr, sLastResponse);
  }

  //close the file we have been working on
  CloseHandle(hFile);
}

void CPJNSMTPConnection::SendRCPTForRecipient(DWORD DSN, CPJNSMTPAddress& recipient)
{
  //Validate our parameters
	ASSERT(recipient.m_sEmailAddress.GetLength()); //must have an email address for this recipient

  //form the command to send
  CString sBuf;
  if (DSN == CPJNSMTPMessage::DSN_NOT_SPECIFIED)
    sBuf.Format(_T("RCPT TO:<%s>\r\n"), recipient.m_sEmailAddress.GetString());
  else
  {
    sBuf.Format(_T("RCPT TO:<%s>\r\n"), recipient.m_sEmailAddress.GetString());
    
    CString sNotificationTypes;
    if (DSN & CPJNSMTPMessage::DSN_SUCCESS)
      sNotificationTypes = _T("SUCCESS");
    if (DSN & CPJNSMTPMessage::DSN_FAILURE)
    {
      if (sNotificationTypes.GetLength())
        sNotificationTypes += _T(",");
      sNotificationTypes += _T("FAILURE");
    }      
    if (DSN & CPJNSMTPMessage::DSN_DELAY)
    {
      if (sNotificationTypes.GetLength())
        sNotificationTypes += _T(",");
      sNotificationTypes += _T("DELAY");
    }  
    if (sNotificationTypes.IsEmpty()) //Not setting DSN_SUCCESS, DSN_FAILURE or DSN_DELAY in m_DSNReturnType implies we do not want DSN's
      sNotificationTypes += _T("NEVER");
    
    sBuf.Format(_T("RCPT TO:<%s> NOTIFY=%s\r\n"), recipient.m_sEmailAddress.GetString(), sNotificationTypes.GetString());
  }
  CT2A szRCPT(sBuf);

  //and send it
  try
  {
    _Send(szRCPT.operator LPSTR(), static_cast<int>(strlen(szRCPT)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //check the response to the RCPT command (250 or 251) is considered success
  if (!ReadCommandResponse(250, 251))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_RCPT_TO_RESPONSE, FACILITY_ITF, GetLastCommandResponse());
}

BOOL CPJNSMTPConnection::ReadCommandResponse(int nExpectedCode)
{
  LPSTR pszOverFlowBuffer = NULL;
  char sBuf[256];
  BOOL bSuccess = ReadResponse(sBuf, 256, &pszOverFlowBuffer, 4096);
  if (pszOverFlowBuffer)
    delete [] pszOverFlowBuffer;

  if (bSuccess)
    bSuccess = (m_nLastCommandResponseCode == nExpectedCode);

  return bSuccess;
}

BOOL CPJNSMTPConnection::ReadCommandResponse(int nExpectedCode1, int nExpectedCode2)
{
  LPSTR pszOverFlowBuffer = NULL;
  char sBuf[256];
  BOOL bSuccess = ReadResponse(sBuf, 256, &pszOverFlowBuffer, 4096);
  if (pszOverFlowBuffer)
    delete [] pszOverFlowBuffer;

  if (bSuccess)
    bSuccess = (m_nLastCommandResponseCode == nExpectedCode1) || (m_nLastCommandResponseCode == nExpectedCode2);

  return bSuccess;
}

BOOL CPJNSMTPConnection::ReadResponse(LPSTR pszBuffer, int nInitialBufSize, LPSTR* ppszOverFlowBuffer, int nGrowBy)
{
  //Validate our parameters
	ASSERT(ppszOverFlowBuffer);          //Must have a valid string pointer
	ASSERT(*ppszOverFlowBuffer == NULL); //Initially it must point to a NULL string

  //We always use a terminator of CR LF
  LPCSTR pszTerminator = "\r\n";
	int nTerminatorLen = 2;

	//must have been created first
	ASSERT(m_bConnected);

	//The local variables which will receive the data
	LPSTR pszRecvBuffer = pszBuffer;
	int nBufSize = nInitialBufSize;
  int nStartOfLastLine = 0;

	//retrieve the reponse until we get the terminator or a timeout occurs
	BOOL bFoundFullResponse = FALSE;
	int nReceived = 0;
	while (!bFoundFullResponse)
	{
		//check the socket for readability
    try
    {
		  if (!_IsReadible(m_dwTimeout)) //A timeout has occured so fail the function call
		  {
			  pszRecvBuffer[nReceived] = '\0';
			  m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
			  return FALSE;
		  }
    }
    catch(CWSocketException* pEx)
    {
      DWORD dwError = pEx->m_nError;
      pEx->Delete();
      ThrowPJNSMTPException(dwError, FACILITY_WIN32);
    }

		//receive the data from the socket
		int nBufRemaining = nBufSize-nReceived-1; //Allows allow one space for the NULL terminator
		if (nBufRemaining < 0)
			nBufRemaining = 0;
		int nData = 0;
    try
    {
      nData = _Receive(pszRecvBuffer+nReceived, nBufRemaining);
    }
    catch(CWSocketException* pEx)
    {
      DWORD dwError = pEx->m_nError;
      pEx->Delete();
      ThrowPJNSMTPException(dwError, FACILITY_WIN32);
    }

		//Reset the idle timeout if data was received
		if (nData > 0)
		{
			//Increment the count of data received
			nReceived += nData;							   
		}

		//If an error occurred receiving the data
		if (nData < 1)
		{
			//NULL terminate the data received
			if (pszRecvBuffer)
			  pszRecvBuffer[nReceived] = '\0';
			m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
			return FALSE; 
		}
		else
		{
			//NULL terminate the data received
			if (pszRecvBuffer)
			  pszRecvBuffer[nReceived] = '\0';
			if (nBufRemaining-nData == 0) //No space left in the current buffer
			{
				//Allocate the new receive buffer
				nBufSize += nGrowBy; //Grow the buffer by the specified amount
				LPSTR pszNewBuf = new char[nBufSize];

				//copy the old contents over to the new buffer and assign 
				//the new buffer to the local variable used for retreiving 
				//from the socket
				if (pszRecvBuffer)
				  strcpy_s(pszNewBuf, nBufSize, pszRecvBuffer);
				pszRecvBuffer = pszNewBuf;

				//delete the old buffer if it was allocated
				if (*ppszOverFlowBuffer)
				  delete [] *ppszOverFlowBuffer;

				//Remember the overflow buffer for the next time around
				*ppszOverFlowBuffer = pszNewBuf;        
			}
		}

		//Check to see if we got a full response yet
		BOOL bFoundTerminator = (strncmp(&pszRecvBuffer[nReceived - nTerminatorLen], pszTerminator, nTerminatorLen) == 0);
    if (bFoundTerminator && (nReceived > 5))
    {
      //Find the start of the last line we have received
      int i = nReceived - 6;
      int nOldStartOfLastLine = nStartOfLastLine;
      while ((i >= 0) && (nOldStartOfLastLine == nStartOfLastLine))
      {
        if ((pszRecvBuffer[i] == '\r') && (pszRecvBuffer[i+1] == '\n'))
          nStartOfLastLine = i + 2;

        i--;
      }
		  bFoundFullResponse = (pszRecvBuffer[nStartOfLastLine + 3] == ' ');
    }
	}

	//Remove the terminator from the response data
  pszRecvBuffer[nReceived - nTerminatorLen] = '\0';

	//determine if Numeric response code
	char sCode[4];
  strncpy_s(sCode, sizeof(sCode), &pszRecvBuffer[nStartOfLastLine], 3);
	sCode[3] = '\0';
  sscanf_s(sCode, "%d", &m_nLastCommandResponseCode);

	//Hive away the last command reponse
	m_sLastCommandResponse = pszRecvBuffer;

	return TRUE;
}

void CPJNSMTPConnection::AuthLogin(LPCTSTR pszUsername, LPCTSTR pszPassword)
{
  //Send the AUTH LOGIN command
	const char* szAuth = "AUTH LOGIN\r\n";
  try
  {
	  _Send(szAuth, static_cast<int>(strlen(szAuth)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //check the response to the AUTH LOGIN command
	if (!ReadCommandResponse(334))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_LOGIN_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

  //Check that the response has a username request in it
	CString sLastCommandString(m_sLastCommandResponse);
	sLastCommandString = sLastCommandString.Right(sLastCommandString.GetLength() - 4);
  CT2A szLastCommandString(sLastCommandString);
  CPJNSMPTBase64 Coder;
	Coder.Decode(szLastCommandString);
	if (_stricmp(Coder.Result(), "username:") != 0)
		ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_LOGIN_USERNAME_REQUEST, FACILITY_ITF, GetLastCommandResponse());

	//send base64 encoded username
	Coder.Encode(CT2A(pszUsername), BASE64_FLAG_NOCRLF);
	CStringA sUser;
	sUser.Format("%s\r\n", Coder.Result());
  try
  {
	  _Send(sUser.operator LPCSTR(), sUser.GetLength());
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

	//check the response to the username 
	if (!ReadCommandResponse(334))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_LOGIN_USERNAME_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

	//Check that the response has a password request in it
	sLastCommandString = m_sLastCommandResponse;
	sLastCommandString = sLastCommandString.Right(sLastCommandString.GetLength() - 4);
  CT2A szLastCommandString2(sLastCommandString);
	Coder.Decode(szLastCommandString2);
	if (_stricmp(Coder.Result(), "password:") != 0)
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_LOGIN_PASSWORD_REQUEST, FACILITY_ITF, GetLastCommandResponse());

  //send password as base64 encoded
	Coder.Encode(CT2A(pszPassword), BASE64_FLAG_NOCRLF);
	CStringA sPwd;
	sPwd.Format("%s\r\n", Coder.Result());
  try
  {
	  _Send(sPwd.operator LPCSTR(), sPwd.GetLength());
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

	//check if authentication is successful
	if (!ReadCommandResponse(235))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_LOGIN_PASSWORD_RESPONSE, FACILITY_ITF, GetLastCommandResponse());
}

void CPJNSMTPConnection::AuthPlain(LPCTSTR pszUsername, LPCTSTR pszPassword)
{
  //Send the AUTH PLAIN command
  const	char* szAuth = "AUTH PLAIN\r\n";
  try
  {
	  _Send(szAuth, static_cast<int>(strlen(szAuth)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //Check the response to the AUTH PLAIN command
	if (!ReadCommandResponse(334))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_PLAIN_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

  //Check that the response has a username request in it
	CString sLastCommandString(m_sLastCommandResponse);
	sLastCommandString = sLastCommandString.Right(sLastCommandString.GetLength() - 4);
  CT2A szLastCommandString(sLastCommandString);
	if (_stricmp(szLastCommandString, "username:") != 0)
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_PLAIN_USERNAME_REQUEST, FACILITY_ITF, GetLastCommandResponse());

	//send username in plain
	CStringA sUser;
	sUser.Format("%s\r\n", CT2A(pszUsername).operator LPSTR());
  try
  {
	  _Send(sUser.operator LPCSTR(), sUser.GetLength());
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

	//check the response to the username 
	if (!ReadCommandResponse(334))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_PLAIN_USERNAME_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

	//Check that the response has a password request in it
	sLastCommandString = m_sLastCommandResponse;
	sLastCommandString = sLastCommandString.Right(sLastCommandString.GetLength() - 4);
  CT2A szLastCommandString2(sLastCommandString);
	if (_stricmp(szLastCommandString2, "password:") != 0)
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_PLAIN_PASSWORD_REQUEST, FACILITY_ITF, GetLastCommandResponse());

  //send password in plain
  CStringA sPwd;
	sPwd.Format("%s\r\n", LPCTSTR(pszPassword));
  try
  {
	  _Send(sPwd.operator LPCSTR(), sPwd.GetLength());
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

	//check if authentication is successful
	if (!ReadCommandResponse(235))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_PLAIN_PASSWORD_RESPONSE, FACILITY_ITF, GetLastCommandResponse());
}

void CPJNSMTPConnection::AuthCramMD5(LPCTSTR pszUsername, LPCTSTR pszPassword)
{
  //Send the AUTH CRAM-MD5 command
  const	char* szAuth = "AUTH CRAM-MD5\r\n";
  try
  {
	  _Send(szAuth, static_cast<int>(strlen(szAuth)));
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

	//Check the response to the AUTH CRAM-MD5 command
	if (!ReadCommandResponse(334))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_CRAM_MD5_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

	//Get the base64 decoded challange 
	CString sLastCommandString(m_sLastCommandResponse);
	sLastCommandString = sLastCommandString.Right(sLastCommandString.GetLength() - 4);
  CT2A szLastCommandString(sLastCommandString);
	CPJNSMPTBase64 Coder;
	Coder.Decode(szLastCommandString);
	LPCSTR pszChallenge = Coder.Result();

	//generate the MD5 digest from the challenge and password
  CPJNMD5 hmac;
  CPJNMD5Hash hash;
  CT2A szAsciiPassword(pszPassword);
  if (!hmac.HMAC(reinterpret_cast<const BYTE*>(pszChallenge), static_cast<DWORD>(strlen(pszChallenge)), reinterpret_cast<const BYTE*>(szAsciiPassword.operator LPSTR()), static_cast<DWORD>(strlen(szAsciiPassword)), hash))
    ThrowPJNSMTPException(GetLastError(), FACILITY_WIN32);
  CString sRet(hash.Format(FALSE)); //CRAM-MD5 requires a lowercase hash
	
	//make the CRAM-MD5 response
	CString sCramDigest(pszUsername);
	sCramDigest += _T(" ");
  sCramDigest += hash.Format(FALSE); //CRAM-MD5 requires a lowercase hash
		
	//send the digest response
  CT2A szCramDigest(sCramDigest);
	Coder.Encode(szCramDigest, BASE64_FLAG_NOCRLF);
	CStringA sEncodedDigest;
	sEncodedDigest.Format("%s\r\n", Coder.Result());
  try
  {
	  _Send(sEncodedDigest.operator LPCSTR(), sEncodedDigest.GetLength());
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

	//check if authentication is successful
	if (!ReadCommandResponse(235))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_CRAMD_MD5_DIGEST_RESPONSE, FACILITY_ITF, GetLastCommandResponse());
}

CPJNSMTPConnection::ConnectToInternetResult CPJNSMTPConnection::ConnectToInternet()
{
  if ((m_lpfnInternetGetConnectedState != NULL) && (m_lpfnInternetAttemptConnect != NULL))
  {
    //Check to see if an internet connection already exists.
    //bInternet = TRUE  internet connection exists.
    //bInternet = FALSE internet connection does not exist
    DWORD dwFlags = 0;
    BOOL bInternet = m_lpfnInternetGetConnectedState(&dwFlags, 0);
    if (!bInternet)
    {
      //Attempt to establish internet connection, probably
      //using Dial-up connection. CloseInternetConnection() should be called when
      //as some time to drop the dial-up connection.
      DWORD dwResult = m_lpfnInternetAttemptConnect(0);
      if (dwResult != ERROR_SUCCESS)
      {
        SetLastError(dwResult);
        return CTIR_Failure;
      }
      else
        return CTIR_NewConnection;
    }
    else
      return CTIR_ExistingConnection;
  }
  else
  {
    //Wininet is not available. Do what would happen if the dll
    //was present but the function call failed
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);  
    return CTIR_Failure;
  }
}

BOOL CPJNSMTPConnection::CloseInternetConnection()
{
  if (m_lpfnInternetAutoDialHangup)
  {
    //Make sure any connection through a modem is 'closed'.
    return m_lpfnInternetAutoDialHangup(0);
  }
  else
  {
    //Wininet is not available. Do what would happen if the dll
    //was present but the function call failed
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);  
    return FALSE;
  }
}

#ifndef CPJNSMTP_NONTLM
SECURITY_STATUS CPJNSMTPConnection::NTLMAuthPhase1(PBYTE pBuf, DWORD cbBuf)
{
  //Send the AUTH NTLM command with the initial data
  CPJNSMPTBase64 Coder;
  Coder.Encode(pBuf, cbBuf, BASE64_FLAG_NOCRLF);
  
  CStringA sBuf;
  sBuf.Format("AUTH NTLM %s\r\n", Coder.Result());
  try
  {
    _Send(sBuf.operator LPCSTR(), sBuf.GetLength());
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  return SEC_E_OK;
}

SECURITY_STATUS CPJNSMTPConnection::NTLMAuthPhase2(PBYTE pBuf, DWORD cbBuf, DWORD* pcbRead)
{
  //check the response to the AUTH NTLM command
  if (!ReadCommandResponse(334))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_NTLM_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

  //Decode the last response
  CPJNSMPTBase64 Coder;
  CString sLastCommandString(m_sLastCommandResponse);
  sLastCommandString = sLastCommandString.Right(sLastCommandString.GetLength() - 4);
  CT2A szLastCommandString(sLastCommandString);
  Coder.Decode(szLastCommandString);

  //Store the results in the output parameters
  *pcbRead = Coder.ResultSize();
  if (*pcbRead >= cbBuf)
    return SEC_E_INSUFFICIENT_MEMORY;
  memcpy(pBuf, Coder.Result(), *pcbRead);

  return SEC_E_OK;
}

SECURITY_STATUS CPJNSMTPConnection::NTLMAuthPhase3(PBYTE pBuf, DWORD cbBuf)
{
  //send base64 encoded version of the data
  CPJNSMPTBase64 Coder;
  Coder.Encode(pBuf, cbBuf, BASE64_FLAG_NOCRLF);

  CStringA sBuf;
  sBuf.Format("%s\r\n", Coder.Result());
  try
  {
    _Send(sBuf.operator LPCSTR(), sBuf.GetLength());
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNSMTPException(dwError, FACILITY_WIN32);
  }

  //check if authentication is successful
  if (!ReadCommandResponse(235))
    ThrowPJNSMTPException(IDS_PJNSMTP_UNEXPECTED_AUTH_NTLM_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

  return SEC_E_OK;
}
#endif

#ifndef CPJNSMTP_NOMXLOOKUP
BOOL CPJNSMTPConnection::MXLookup(LPCTSTR lpszHostDomain,	CStringArray& arrHosts, CWordArray& arrPreferences, WORD fOptions, PIP4_ARRAY aipServers)
{
  //What will be the return value
  BOOL bSuccess = FALSE;

  //Reset the arrays
  arrHosts.RemoveAll();
  arrPreferences.RemoveAll();

  if ((m_lpfnDnsRecordListFree != NULL) && (m_lpfnDnsQuery != NULL))
  {
    //Do the DNS MX lookup  
	  PDNS_RECORD pRec = NULL;
	  if (m_lpfnDnsQuery(lpszHostDomain, DNS_TYPE_MX, fOptions, aipServers, &pRec, NULL) == ERROR_SUCCESS)
	  {
      bSuccess = TRUE;

      PDNS_RECORD pRecFirst = pRec;
		  while (pRec)
		  {
        //We're only interested in MX records
			  if (pRec->wType == DNS_TYPE_MX)
        {
				  arrHosts.Add(pRec->Data.MX.pNameExchange);
          arrPreferences.Add(pRec->Data.MX.wPreference);
        }
			  pRec = pRec->pNext;
		  }
		  m_lpfnDnsRecordListFree(pRecFirst, DnsFreeRecordList);
	  }
  }
  else
  {
    //Dnsapi is not available. Do what would happen if the dll
    //was present but the function call failed
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);  
  }

  return bSuccess;
}

BOOL CPJNSMTPConnection::MXLookupAvailable()
{
  return (m_lpfnDnsRecordListFree != NULL && m_lpfnDnsQuery != NULL);
}
#endif

CString CPJNSMTPConnection::CreateNEWENVID()
{
  //Call the helper function to do the main work
  CString sEnvID(CPJNSMTPBodyPart::CreateGUID());

  //Remove all but the hex digits
	sEnvID.Remove('{');
	sEnvID.Remove('}');
	sEnvID.Remove('-');
  sEnvID.MakeUpper();

	return sEnvID;
}
