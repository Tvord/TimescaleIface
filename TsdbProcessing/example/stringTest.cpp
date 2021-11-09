//
// Created by tokareva on 08.11.2021.
//
#include <string>
#include <queue>
#include <vector>
#include <QElapsedTimer>
#include <QDebug>

struct trendValue {
    unsigned int tagId;
    unsigned int placeholder;
    unsigned long ts;
    double value;
};

void fillQueue(std::queue<trendValue>& container) {
    for (unsigned int i = 0; i < 10000; ++i) {
        container.push(trendValue{i, i, i, (double)i});
    }
}

int main(int argc, char** argv)
{
    unsigned int count{0};
    const char* fmt{"('epoch'::timestamptz + %lu * INTERVAL '1 millisecond' + INTERVAL '3 HOURS', "
                    "'tag%ui', 'Temp Description', %0.2f)"};
    std::string sinEntry{""};
    char buff[150];
    std::queue<trendValue> mData;
    QElapsedTimer tim;
    fillQueue(mData);

    tim.start();
    while (!mData.empty()) {
        sinEntry = "";
        auto val = mData.front();
        mData.pop();
        snprintf(buff, sizeof(buff), fmt, val.ts, val.tagId, val.value);
        sinEntry = buff;
        sinEntry.size();
    }
    qDebug() << "Filling buffer of char: " << tim.nsecsElapsed() / 10000;

    fillQueue(mData);
    tim.restart();
    while (!mData.empty()) {
        auto val = mData.front();
        mData.pop();
        std::string mString = "'epoch'::timestamptz + " + std::to_string(val.ts) + " * INTERVAL '1 millisecond' + INTERVAL '3 HOURS', "
                               "'tag" + std::to_string(val.tagId) + "', "
                              "'Temp Description', " +
                              std::to_string(val.value);
        mString.size();
    }
    qDebug() << "Building string: " << tim.nsecsElapsed() / 10000;

    fillQueue(mData);
    tim.restart();
    while (!mData.empty()) {
        auto val = mData.front();
        mData.pop();
        auto mVec = std::vector<std::string>{
                "'epoch'::timestamptz + " + std::to_string(val.ts) + " * INTERVAL '1 millisecond' + INTERVAL '3 HOURS'",
                "'tag" + std::to_string(val.tagId) + "'",
                "'Temp Description'",
                std::to_string(val.value)
            };
        mVec.size();
    }
    qDebug() << "Building vector of strings: " << tim.nsecsElapsed() / 10000;

    return 0;
}