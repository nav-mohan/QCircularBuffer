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
    QCircularBuffer(qint64 bufferSize) : m_bufferSize(bufferSize), QIODevice(){};
    ~QCircularBuffer();

    QReadWriteLock m_lock;
    char *m_buffer;
    qint64 m_writeHead;
    qint64 m_bufferSize;
    QMap<qint64,qint64> m_readTails;
    QMap<qint64,qint64> m_validData;

public slots:
    void initialize();
    void registerConsumer(qint64 timeStamp);
    void removeConsumer(qint64 timeStamp);
    void resetBuffer();
    qint64 readTail(char *data, qint64 maxSize, qint64 timeStamp);

protected:
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);

signals:
#if defined(IS_TESTING)
    void producerLoop();
    void consumerLoop(qint64 consumerID);
    void consumerReset(qint64 consumerID);
    void doneRead(qint64 consumerID);
#endif // IS_TEST_ENV


};

#endif // QCIRCULARBUFFER_H
