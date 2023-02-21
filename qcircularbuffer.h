#if !defined(QCIRCULARBUFFER_H)
#define QCIRCULARBUFFER_H
#include <QDebug>

#include <QObject>
#include  <QIODevice>
#include <QReadWriteLock>
#include <QMap>

class QCircularBuffer : public QIODevice {
Q_OBJECT
public:
    QCircularBuffer() : QIODevice(){};
    ~QCircularBuffer();

    QReadWriteLock m_lock;
    char *m_buffer;
    qint64 m_headIndex;
    qint64 m_bufferSize;
    QMap<qint64,qint64> m_tailMap;

    void initialize(qint64);
    void registerConsumer(qint64);
    int removeConsumer(qint64);
    void resetBuffer();

protected:
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);

};

#endif // QCIRCULARBUFFER_H
