#ifndef MIMECONTENTFORMATTER_H
#define MIMECONTENTFORMATTER_H

#include <QObject>
#include <QIODevice>

class MimeContentFormatter : public QIODevice
{
    Q_OBJECT
public:
    MimeContentFormatter(QIODevice *device, int lineLength = 76);

    int getLineLength() const;
    void setLineLength(int l);

protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len) = 0;

    QIODevice *output;
    int lineLength;
};

#endif // MIMECONTENTFORMATTER_H
