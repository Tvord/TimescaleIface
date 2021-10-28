//
// Created by tokareva on 27.10.2021.
//

#ifndef TSDBPROCESSING_TSDBCONNECTOR_H
#define TSDBPROCESSING_TSDBCONNECTOR_H

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QList>
#include <vector>
#include <string>
//#include <stdlib.h>
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
    bool insert(const QStringList& values);
    bool insertPreloaded();
    void setFields(const QStringList &fields);
    void setFormat(const QString& mFormat);
    //! Build and add values string from list of values
    void preloadValues(QStringList values);
    void preloadValues(std::vector<std::string> values);
    //! Build and add multiple values string from list of values list
    void preloadValues(QList<QStringList> &values);
    void preloadValues(std::vector<std::vector<std::string> > values);
    //! Add preformatted values string
    void preloadValue(QString value);

private:
    PGconn *conn{nullptr};
    QString hostname;
    QString user;
    QString pass;
    int port;
    QString dbName;
    QString tablename;
    QStringList valuesQueue{};
    QString prepareValuesString(QStringList& valuesList);
public:
    void setTablename(const QString &tablename);

private:
    QString format;
    QString fields;

};


#endif //TSDBPROCESSING_TSDBCONNECTOR_H
