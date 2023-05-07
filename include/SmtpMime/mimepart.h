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

#ifndef MIMEPART_H
#define MIMEPART_H

#include "smtpmime_global.h"
#include <QByteArray>
#include <QString>

class QIODevice;

class SMTP_MIME_EXPORT MimePart
{
public:

    /* [0] Enumerations */
    enum Encoding {        
        _7Bit,
        _8Bit,
        Base64,
        QuotedPrintable
    };


    /* [0] --- */


    /* [1] Constructors and Destructors */

    MimePart();
    virtual ~MimePart();

    /* [1] --- */


    /* [2] Getters and Setters */

    void setContent(const QByteArray & content);
    QByteArray getContent() const;

    void setHeader(const QString & headerLines);
    QString getHeader() const;

    void addHeaderLine(const QString & line);

    void setContentId(const QString & cId);
    QString getContentId() const;

    void setContentName(const QString & cName);
    QString getContentName() const;

    void setContentType(const QString & cType);
    QString getContentType() const;

    void setCharset(const QString & charset);
    QString getCharset() const;

    void setEncoding(Encoding enc);
    Encoding getEncoding() const;

    void setMaxLineLength(const int length);
    int getMaxLineLength() const;

    /* [2] --- */


    /* [3] Public methods */

    virtual QString toString() const;
    void writeToDevice(QIODevice &device) const;

    /* [3] --- */

protected:

    /* [4] Protected members */

    QString headerLines;
    QByteArray content;

    QString cId;
    QString cName;
    QString cType;
    QString cCharset;
    QString cBoundary;
    Encoding cEncoding;

    int maxLineLength;

    QString mimeString;
    bool prepared;

    /* [4] --- */

    virtual void writeContent(QIODevice &device) const;
    void writeContent(QIODevice &device, const QByteArray &content) const;
};

#endif // MIMEPART_H
