#ifndef MIMEQPFORMATTER_H
#define MIMEQPFORMATTER_H

#include "mimecontentformatter.h"

class MimeQPFormatter : public MimeContentFormatter
{
public:
    MimeQPFormatter(QIODevice*);

protected:
    virtual qint64 writeData(const char *data, qint64 len);
};

#endif // MIMEQPFORMATTER_H
