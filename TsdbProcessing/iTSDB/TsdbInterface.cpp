//
// Created by tokareva on 02.11.2021.
//

#include "TsdbInterface.h"
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <iostream>
#include <QDateTime>
#include <QElapsedTimer>
//#include <sys/stat.h>
//#include <sys/types.h>

TsdbInterface::TsdbInterface() {
    conn.setConnectParameters("127.0.0.1", 5433, "root", ".root");
    conn.setDatabase("DynData");
    conn.connect();

    conn.setTablename("mdata");
    conn.setFields(QStringList{"ts", "tag", "textval", "value"});
    //! TODO: Check is it necessary
    conn.setFormat("%1, %2, %3, %4");
}

void TsdbInterface::listen() {
    struct pollfd polling[1];
    int bytesReady{0};
    auto pRead = open(pipeName, O_RDWR | O_NONBLOCK);
    std::cout << "Opening: " << pipeName << std::endl;
    if (pRead < 1) {
        std::cout << "Err on opening " << pipeName << "! Err is: " << strerror(errno) << std::endl;
    }
    polling->fd = pRead;
    polling->events = POLLIN;
    int structCount{0};
    while (true) {
        if (breakLoop) break;
        auto ret = poll(polling, 1, 5000); //! 5 seconds
        if (ret == 0) {
            std::cout << "Timeout" << std::endl;
            ;//! timeout
        } else if (ret < 0) {
            std::cout << "Err" << std::endl;
            ;//! Error!
        } else {
            ioctl(pRead, FIONREAD, &bytesReady);
            if (bytesReady < 1) continue;

            //! Now we receive all bytes ready in pipe into array of structures.
            //! Memory is allocated continuously - there will be not any problem with reading to array's pointer.
            //! We really need in allocating memory every read, cause in any other way - we will need to use mutex
            structCount = bytesReady / (int)sizeof(trendValue);
            auto trendData = new trendValue[structCount];
            read(polling->fd, trendData, sizeof(trendValue) * structCount);
            ParseNewData(trendData, structCount);
            bytesReady -= structCount * 24;

            polling[0].revents = 0; // Drop events count
            usleep(10000);
        }
    }
    close(pRead);
}

TsdbInterface::~TsdbInterface() {
//    delete pipeName;
}

void TsdbInterface::setPipeName(const char *p_name) {
    pipeName = const_cast<char*>(p_name);
}

void TsdbInterface::start() {
    pipeListener_ptr = new std::thread(&TsdbInterface::listen, this);
    queueWatcher_ptr = new std::thread(&TsdbInterface::watch, this);
}

void TsdbInterface::ParseNewData(trendValue *dataArray, int structCount) {
    for (auto index = 0; index < structCount; index++) {
        mQueue.push(dataArray[index]);
    }
}

void TsdbInterface::watch() {
    unsigned int count{0};
    const char* fmt{"('epoch'::timestamptz + %lu * INTERVAL '1 millisecond' + INTERVAL '3 HOURS', "
                          "'tag%ui', 'Temp Description', %0.2f)"};
    std::vector<std::string> values;
    std::string sinEntry;
    char buff[150];
    QElapsedTimer tim;
    std::vector<quint64> preload_t{0}, insert_t{0}, build_t{0};
    while (true) {
        if (breakLoop) break;
        count = 0;
        tim.restart();
        values.clear();
        //! Collect messages buffer locally before preload/insert them to database interface
        while (!mQueue.empty()) {
            sinEntry = "";
            auto val = mQueue.front();
            mQueue.pop();
            snprintf(buff, sizeof(buff), fmt, val.ts, val.tagId, val.value);
            values.emplace_back(buff);
            //conn.preloadValue(std::string(buff));
            ++count;
        }
        if (count) {
            conn.preloadValues(values);
            conn.insertPreloaded();
        }
        usleep(1000);
    }
}

void TsdbInterface::stop() {
    breakLoop = true;

    pipeListener_ptr->join();
    queueWatcher_ptr->join();
}
