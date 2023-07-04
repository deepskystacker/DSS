#ifndef MIMEQPENCODER_H
#define MIMEQPENCODER_H

#include "mimecontentencoder.h"

class MimeQpEncoder : public MimeContentEncoder
{
public:
    MimeQpEncoder();

    QByteArray encode(const QByteArray &data);
};

#endif // MIMEQPENCODER_H
