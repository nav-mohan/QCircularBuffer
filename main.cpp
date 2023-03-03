#include "qcircularbuffer.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc,argv);
    QCircularBuffer *cbuf = new QCircularBuffer(1024);
    cbuf->initialize();
    cbuf->registerConsumer(1);
    char *data = (char*)malloc(1024);
    memset(data,1,102);
    cbuf->write(data,102);

    app.exec();
}