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

#ifndef EMAILADDRESS_H
#define EMAILADDRESS_H

#include "smtpmime_global.h"
#include <QString>

class SMTP_MIME_EXPORT EmailAddress
{
public:

    /* [1] Constructors and Destructors */

    EmailAddress(const QString & address = "", const QString & name = "");

    /* [1] --- */


    /* [2] Getters and Setters */

    QString getAddress() const;
    QString getName() const;

    /* [2] --- */


private:

    /* [3] Private members */

    QString address;
    QString name;

    /* [3] --- */
};

#endif // EMAILADDRESS_H
