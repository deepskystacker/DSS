#ifndef MIMEBYTEARRAYATTACHMENT_H
#define MIMEBYTEARRAYATTACHMENT_H

#include "smtpmime_global.h"
#include "mimepart.h"

class SMTP_MIME_EXPORT MimeByteArrayAttachment : public MimePart
{
public:    
    /* [1] Constructors and Destructors */

    MimeByteArrayAttachment(const QString& name, const QByteArray &content);
    ~MimeByteArrayAttachment();

    /* [1] --- */


};

#endif // MIMEBYTEARRAYATTACHMENT_H
