# QCircularBuffer
A re-implementation of the `QIODevice` class from the `Qt` framework. The `readData` and `writeData` functions have been overridden for cyclic indexing.

* It supports multiple-consumers and a single-producer.
* This is not Lock-Free. It uses a `QReadWriteLock`.
* The producer writes to the `head_index`.
* Each consumer has a unique `tail_index`.
* When a consumer is newly registered, it's `tail_index` is equal to 0.
* The `QCircularBuffer` manages a `QMap` of each consumer's `tail_index`.
* When the producer's `head_index` overtakes a consumer's`tail_index`, that consumer's `tail_index` is reset to the last `writeHead`.
* _For now lets assume that consumers cannot overtake producer (which is reasonable because consumers are CPU intensive audio encoders)._
* consumers read data by copying (and not by popping). 
* The producer overwrites old data if there is no space left. 
* If the producer has to write more than the available space (`buffer_size` - `head_index`) then it loops around for writing the remaining data (`data_size` - `buffer_size` + `head_index`) and updates the `head_index`
* New data has higher priority than preservation of old data. If producer has to write `n * buffer_size` bytes of data, then only the last `buffer_size` bytes of data is written to the `m_buffer`.
* If the consumer has to read more data than is available then it reads just the available amount of data and updates the `tail_index`. The amount of available data is equal to
    * ( `head_index` - `tail_index` ) if `head_index` > `tail_index`.
    * ( `head_index` + `buffer_size` - `tail_index` ) if `tail_index` > `head_index`. _assuming the producer is always ahead of the consumer_

## Member-Variables
* _`qint64`_ `m_headIndex`              : The producer's `head_index`
* _`QMap<qint64,qint64>`_ `m_tailMap`   : Each consumer's `tail_index`. The `key` is the creation timestamp of the consumer.
* _`char*`_ `m_buffer`                  : The buffer of bytes
* _`qint64`_ `m_bufferSize`             : Number of `bytes` in `m_buffer` 

## Public/Private Member-Methods
* _`void`_ `initialize()`               : To initialize the `m_buffer` and open the `QIODevice` in `ReadWrite` mode.
* _`void`_ `registerConsumer(qint64)`   : To register a new consumer to the `m_tailMap`
* _`int`_ `removeConsumer(qint64)`     : To remove an exisintg consumer from the `m_tailMap`. Returns the number of consumers removed (1 if correct. Not 1 if wrong)
* _`void`_ `resetBuffer()`              : To reset the buffers (not sure if I need this but it's good to have it in-case)

## Protected Member Methods
* _`qint64`_ `readData(`_`char`_ `*data`, _`qint64`_ `maxSize)`: Try to copy `maxSize` bytes of data from `m_buffer` into `data`. if `maxSize` bytes of valid data is not available, then copy just teh available bytes of data and return `n_available`. 
* _`qint64`_ `writeData(`_`const char`_ `*data`, _`qint64`_ `maxSize)` : Copy `maxSize` bytes of data from `m_data` into `m_buffer`. 



## Writing Tests
* Implement `SIGNALS` for 
    * `emit producerLoop()` : Producer looping around          
    * `emit consumerLoop(consumerID)` : Consumer(consumerID) looping around   
    * `emit consumerReset(consumerID)` : Consumer(consumerID) tail is reset    
    * `emit QIODevice::readyRead()` : Producer is done writing         
    * `emit doneRead(consumerID)` : Consumer(consumerID) is done reading  
* There must be some relationship between these `SIGNALS`