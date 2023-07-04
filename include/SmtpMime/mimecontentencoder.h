#ifndef MIMEENCODER_H
#define MIMEENCODER_H

#include <QObject>
#include <QByteArray>

class MimeContentEncoder : public QObject
{
public:
    virtual QByteArray encode(const QByteArray &data) =0;

protected:
    MimeContentEncoder();
};

#endif // MIMEENCODER_H
