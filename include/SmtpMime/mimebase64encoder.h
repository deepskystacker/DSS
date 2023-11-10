#ifndef MIMEBASE64ENCODER_H
#define MIMEBASE64ENCODER_H

#include "mimecontentencoder.h"

class MimeBase64Encoder : public MimeContentEncoder
{
public:
    MimeBase64Encoder();

    QByteArray encode(const QByteArray &data);
};

#endif // MIMEBASE64ENCODER_H
