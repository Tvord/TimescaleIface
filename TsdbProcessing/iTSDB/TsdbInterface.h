//
// Created by tokareva on 02.11.2021.
//

#ifndef TSDB_PROCESS_TSDBINTERFACE_H
#define TSDB_PROCESS_TSDBINTERFACE_H

//! 1. Open pipe
//! 2. Listen for new messages and store them
//! 3. Insert into database by timer or count of stored messages

#include <thread>
#include <queue>
#include "../TSDBConnector/tsdbconnector.h"

struct trendValue {
    unsigned int tagId;
    unsigned int placeholder;
    unsigned long ts;
    double value;
};

class TsdbInterface {
public:
    TsdbInterface();
    ~TsdbInterface();

    void setPipeName(const char* p_name);

    TsdbInterface(const TsdbInterface& other ) = delete;
    TsdbInterface(TsdbInterface&& other ) = delete;
    TsdbInterface& operator=(const TsdbInterface& other ) = delete;
    TsdbInterface& operator=(TsdbInterface&& other ) = delete;
    void start();
    void stop();
private:
    std::thread *pipeListener_ptr;
    std::thread *queueWatcher_ptr;
    std::queue<trendValue> mQueue;
    tsdbconnector conn;
    char data[24];
    char* pipeName{nullptr};
    bool breakLoop{false};
    void ParseNewData(trendValue *dataArray, int structCount);
    void listen(); //! Threaded func
    void watch(); //! Threaded func
};


#endif //TSDB_PROCESS_TSDBINTERFACE_H
