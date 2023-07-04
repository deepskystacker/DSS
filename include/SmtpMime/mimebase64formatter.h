#ifndef MIMEBASE64FORMATTER_H
#define MIMEBASE64FORMATTER_H

#include "mimecontentformatter.h"

class MimeBase64Formatter : public MimeContentFormatter
{
public:
    MimeBase64Formatter(QIODevice*);

protected:
    virtual qint64 writeData(const char *data, qint64 len);    
};

#endif // MIMEBASE64FORMATTER_H
