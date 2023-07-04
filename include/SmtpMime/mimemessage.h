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

#ifndef MIMEMESSAGE_H
#define MIMEMESSAGE_H

#include <QObject>
#include <QStringList>
#include <QTextStream>

#include "smtpmime_global.h"
#include "mimepart.h"
#include "mimemultipart.h"
#include "emailaddress.h"

class SMTP_MIME_EXPORT MimeMessage : public QObject
{
public:

    enum RecipientType {
        To,                 // primary
        Cc,                 // carbon copy
        Bcc                 // blind carbon copy
    };

    /* [1] Constructors and Destructors */

    MimeMessage(bool createAutoMimeContent = true);
    ~MimeMessage();

    /* [1] --- */


    /* [2] Getters and Setters */

    void setSender(const EmailAddress &sndr);
    void addRecipient(const EmailAddress &rcpt, RecipientType type = To);
    void addTo(const EmailAddress &rcpt);
    void addCc(const EmailAddress &rcpt);
    void addBcc(const EmailAddress &rcpt);
    void addCustomHeader(const QString &hdr);
    void setSubject(const QString &subject);
    void addPart(MimePart* part);

    void setHeaderEncoding(MimePart::Encoding);

    EmailAddress getSender() const;
    const QList<EmailAddress> &getRecipients(RecipientType type = To) const;
    QString getSubject() const;
    const QStringList &getCustomHeaders() const;
    const QList<MimePart*> & getParts() const;

    MimePart& getContent();
    void setContent(MimePart *content);
    /* [2] --- */


    /* [3] Public methods */

    virtual QString toString() const;
    void writeToDevice(QIODevice &device) const;

    /* [3] --- */

protected:

    /* [4] Protected members */

    EmailAddress sender;
    QList<EmailAddress> recipientsTo, recipientsCc, recipientsBcc;
    QString subject;
    QStringList customHeaders;
    MimePart *content;

    MimePart::Encoding hEncoding;

    static QByteArray format(const QString &text, MimePart::Encoding encoding);
    static QByteArray formatAddress(const EmailAddress &address, MimePart::Encoding encoding);

    /* [4] --- */


};

#endif // MIMEMESSAGE_H
