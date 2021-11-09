//
// Created by tokareva on 02.11.2021.
//
#include <tsdbconnector.h>
#include <iTSDB/TsdbInterface.h>
#include <QStringList>
#include <QDateTime>
#include <QElapsedTimer>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <QDataStream>

int main(int argc, char** argv) {
    tsdbconnector conn;
    conn.setConnectParameters("127.0.0.1", 5433, "root", ".root");
    conn.setDatabase("DynData");
    conn.connect();
//!=================================================================
    //! Variants tests for converting milliseconds into timestamp

    const QString dtFormat{"yyyy-MM-dd HH:mm:ss.zzz"};
    quint64 timNorm{0}, timQt{0}, timPg{0};
    QElapsedTimer tim;
    for (int j = 0; j < 1; ++j) {
        tim.restart();
        for (int i = 0; i < 100; ++i) {
            auto curDT = QDateTime::currentDateTime().toString(dtFormat);
            conn.insert("mdata", QStringList{"ts", "tag", "textval", "value"},
                        QStringList{"'2021-01-01 10:00:00.000'", "'mytag'", "'description'", QString::number(33.21)});
        }
        timNorm += tim.nsecsElapsed() / 1000;
        //qDebug() << "Elapsed with now insert: " << tim.nsecsElapsed() / 1000;
        tim.restart();
        for (int i = 0; i < 100; ++i) {
            auto curDT = QDateTime::currentDateTime().toString(dtFormat);
            conn.insert("mdata", QStringList{"ts", "tag", "textval", "value"},
                        QStringList{"'" + curDT + "'", "'mytag'", "'description'", QString::number(33.21)});
        }
        timQt += tim.nsecsElapsed() / 1000;
        //qDebug() << "Elapsed with qt converting: " << tim.nsecsElapsed() / 1000;
        tim.restart();
        for (int i = 0; i < 100; ++i) {
            auto curDT = QDateTime::currentDateTime().toTime_t();
            conn.insert("mdata", QStringList{"ts", "tag", "textval", "value"},
                        QStringList{"'epoch'::timestamptz + " + QString::number(curDT) + " * INTERVAL '1 second' ",
                                    "'mytag'", "'description'", QString::number(33.21)});
        }
        timPg += tim.nsecsElapsed() / 1000;
        //qDebug() << "Elapsed with pqsql converting: " << tim.nsecsElapsed() / 1000;
    }
    qDebug() << "Elapsed with constant insert: " << timNorm / 10;
    qDebug() << "Elapsed with Qt converting: " << timQt / 10;
    qDebug() << "Elapsed with Pg converting: " << timPg / 10;

    conn.select("mdata",QStringList{"ts", "tag", "textval", "value"}, 1, " ORDER BY ts DESC " );

//! ===============================================================
    //! Almost real tests. Writing raw bytes into created pipe and processing then inside TimeScale Interface.

    //! Connection parameters will be set from constructor. Constant for MVP
    TsdbInterface iface;
    char* fifoPath{"/opt/psatoolkit/share/mPipe"};
    mkfifo(fifoPath, 0666);

    iface.setPipeName(fifoPath);
    iface.start();
    auto wPipe = open(fifoPath, O_WRONLY);

    //! Pipe size can be adjusted with next command. Last arg is bytes count, up to /proc/sys/fs/pipe-max-size
    //! fcntl(wPipe, F_SETPIPE_SZ, 1048576);
    //fcntl(wPipe, F_SETPIPE_SZ, 240000); // 10k entries
    fcntl(wPipe, F_SETPIPE_SZ, 1048576); // 43690 entries

    QByteArray arr;
    quint64 ts;
    int cnt = 0;
    //! 100 times...
     while (++cnt < 100) {
        arr.clear();
        QDataStream str(&arr, QIODevice::WriteOnly);
        str.setByteOrder(QDataStream::ByteOrder::LittleEndian);
    //! ... 10000 'messages' will be created...
        int numEntries{10000};
        for (int i = 0; i< numEntries; ++i) {
            str << (unsigned int)i;
            str << (unsigned int)0;
            ts = QDateTime::currentDateTime().toMSecsSinceEpoch();
            str << ts;
            str << (double)(ts % 10000);
        }
    //! ... and written to the pipe
        write(wPipe, arr.toStdString().c_str(), arr.size());
    //! ... with 10ms delay between message bunches
        usleep(10000);
    }
    close(wPipe);
     conn.disconnect();
     iface.stop();
    return 0;
}