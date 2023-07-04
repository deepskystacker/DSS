/*
  Copyright (c) 2011-2012 - Tőkés Attila

  This file is part of SmtpClient for Qt.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  See the LICENSE file for more details.
*/

#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <QObject>
#include <QtNetwork/QSslSocket>
#include <QEventLoop>
#include "smtpmime_global.h"
#include "mimemessage.h"


class SMTP_MIME_EXPORT SmtpClient : public QObject
{
    Q_OBJECT
    Q_ENUMS (AuthMethod SmtpError ConnectionType ClientState)
public:

    /* [0] Enumerations */

    enum AuthMethod
    {
        AuthPlain,
        AuthLogin
    };

    enum SmtpError
    {
        ConnectionTimeoutError = 0,
        ResponseTimeoutError = 1,
        AuthenticationError = 2,
        MailSendingError = 3,
        ServerError = 4,    // 4xx smtp error
        ClientError = 5,    // 5xx smtp error
        SocketError = 6
    };

    enum ConnectionType
    {
        TcpConnection = 0,
        SslConnection = 1,
        TlsConnection = 2      // STARTTLS
    };

    enum ClientState {
        UnconnectedState = 0,
        ConnectingState = 1,
        ConnectedState = 2,
        ReadyState = 3,
        AuthenticatingState = 4,
        MailSendingState = 5,
        DisconnectingState = 6,
        ResetState = 7,

        /* Internal States */
        _EHLO_State = 50,
        _TLS_State = 51,

        _READY_Connected = 52,
        _READY_Authenticated = 53,
        _READY_MailSent = 54,
        _READY_Encrypted = 55,

        /* Internal Substates */

        // TLS
        _TLS_0_STARTTLS = 60,
        _TLS_1_ENCRYPT = 61,
        _TLS_2_EHLO = 62,

        // AUTH
        _AUTH_PLAIN_0 = 70,
        _AUTH_LOGIN_0 = 71,
        _AUTH_LOGIN_1_USER = 72,
        _AUTH_LOGIN_2_PASS = 73,

        // MAIL
        _MAIL_0_FROM = 81,
        _MAIL_1_RCPT_INIT = 82,
        _MAIL_2_RCPT = 83,
        _MAIL_3_DATA = 84,
        _MAIL_4_SEND_DATA = 85
    };

    /* [0] --- */


    /* [1] Constructors and Destructors */

    SmtpClient(const QString & host = "localhost", int port = 25, ConnectionType ct = TcpConnection);

    ~SmtpClient();

    /* [1] --- */


    /* [2] Getters and Setters */

    QString getHost() const;
    int getPort() const;
    ConnectionType getConnectionType() const;

    QString getName() const;
    void setName(const QString &name);

    QString getResponseText() const;
    int getResponseCode() const;

    QTcpSocket* getSocket();

    /* [2] --- */


    /* [3] Public methods */

    void connectToHost();
    void login(const QString &user, const QString &password, AuthMethod method = AuthLogin);
    void sendMail(const MimeMessage & email);
    void quit();
    void reset();

    bool waitForReadyConnected(int msec = 30000);
    bool waitForAuthenticated(int msec = 30000);
    bool waitForMailSent(int msec = 30000);
    bool waitForReset(int msec = 30000);

    /* [3] --- */

protected:

    /* [4] Protected members */

    struct AuthInfo {
        QString username;
        QString password;
        AuthMethod authMethod;

        AuthInfo(const QString & username = "", const QString &password = "", AuthMethod authMethod = AuthPlain) :
            username(username), password(password), authMethod(authMethod) {}
    };

    QTcpSocket *socket;
    ClientState state;

    const QString host;
    const int port;
    ConnectionType connectionType;

    QString name;

    AuthInfo authInfo;

    QString responseText;
    QString tempResponse;
    int responseCode;

    bool isReadyConnected;
    bool isAuthenticated;
    bool isMailSent;
    bool isReset;

    const MimeMessage *email;

    int rcptType;
    enum _RcptType { _TO = 1, _CC = 2, _BCC = 3};

    QList<EmailAddress>::const_iterator addressIt;
    QList<EmailAddress>::const_iterator addressItEnd;

    /* [4] --- */


    /* [5] Protected methods */
    void login();
    void setConnectionType(ConnectionType ct);
    void changeState(ClientState state);
    void processResponse();
    void sendMessage(const QString &text);
    void emitError(SmtpClient::SmtpError e);
    void waitForEvent(int msec, const char *successSignal);

    /* [5] --- */

protected slots:

    /* [6] Protected slots */

    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketError(QAbstractSocket::SocketError error);
    void socketReadyRead();
    void socketEncrypted();

    /* [6] --- */


signals:

    /* [7] Signals */

    void error(SmtpClient::SmtpError e);
    void stateChanged(SmtpClient::ClientState s);
    void connected();
    void readyConnected();
    void authenticated();
    void mailSent();
    void mailReset();
    void disconnected();

    /* [7] --- */

};

#endif // SMTPCLIENT_H
