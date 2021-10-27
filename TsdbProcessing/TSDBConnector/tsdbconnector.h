//
// Created by tokareva on 27.10.2021.
//

#ifndef TSDBPROCESSING_TSDBCONNECTOR_H
#define TSDBPROCESSING_TSDBCONNECTOR_H

#include <QString>
#include <QStringList>
#include <QDebug>
#include <libpq-fe.h>

class tsdbconnector {
public:
    tsdbconnector();
    void setConnectParameters(const QString &mHost, const int mPort, const QString &mUser, const QString &mPass);
    void setDatabase(const QString& database);
    void connect();
    void disconnect();
    bool select(const QString& table, const QStringList& fields, int limit, const QString& args = "");
    bool insert(const QString& table, const QStringList& fields, const QStringList& vals);
private:
    PGconn *conn{nullptr};
    QString hostname;
    QString user;
    QString pass;
    int port;
    QString dbName;
    QString format;
    QString fields;
public:
    void setFields(const QString &fields);
};


#endif //TSDBPROCESSING_TSDBCONNECTOR_H
