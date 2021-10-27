//
// Created by tokareva on 27.10.2021.
//

#include "tsdbconnector.h"
#include <iostream>
#include <QThread>

tsdbconnector::tsdbconnector() {

}

void tsdbconnector::setConnectParameters(const QString &mHost, const int mPort, const QString &mUser,
                                         const QString &mPass) {
    hostname = mHost;
    this->port = mPort;
    this->user = mUser;
    this->pass = mPass;
}

void tsdbconnector::setDatabase(const QString &database) {
    this->dbName = database;
}

void tsdbconnector::connect() {
    conn = PQconnectdb(QString("hostaddr=%1 port=%2 dbname=%3 user=%4 password=%5").arg(
                                        hostname, QString::number(port), dbName, user, pass).toStdString().c_str());
//    while (PQstatus(conn) == ConnStatusType::CONNECTION_STARTED) {
//        qDebug() << "Waiting...";
//        QThread::sleep(1);
//    }
}

void tsdbconnector::disconnect() {
    PQfinish(conn);
}

bool tsdbconnector::select(const QString &table, const QStringList &fields, int limit, const QString &args) {
    if (PQstatus(conn) != ConnStatusType::CONNECTION_OK) {
        qDebug() << "Unable to execute query. Status: " << (int) PQstatus(conn) << ", message: " << PQerrorMessage(conn);
        return false;
    }
    auto result = PQexec(conn, QString("SELECT %1 FROM %2 LIMIT %3").arg(
            fields.join(','), table, QString::number(limit)
            ).toStdString().c_str());
    if (PQresultStatus(result) != ExecStatusType::PGRES_TUPLES_OK) {
        qDebug() << "Error on select. Status: " << (int)PQresultStatus(result) << ", message : " << PQresultErrorMessage(result);
        return false;
    }
    auto nVals = PQntuples(result);
    qDebug() << "Select done, count: " << nVals;
    for (int index = 0; index < nVals; ++index) {
        for (auto colId = 0; colId < fields.count(); ++colId) {
            std::cout << PQgetvalue(result, index, colId) << "\t";
        }
        std::cout << std::endl;
    }
    return true;
}

bool tsdbconnector::insert(const QString &table, const QStringList &fields, const QStringList &vals) {

    if (PQstatus(conn) != ConnStatusType::CONNECTION_OK) {
        qDebug() << "Unable to execute query. Status: " << (int) PQstatus(conn) << ", message: " << PQerrorMessage(conn);
        return false;
    }
    auto result = PQexec(conn, QString("INSERT INTO %1 %2 VALUES %3;").arg(
            table,  fields.join(',').prepend('(').append(')'),
                        vals.join(',').prepend('(').append(')')).toStdString().c_str());
    if (PQresultStatus(result) != ExecStatusType::PGRES_COMMAND_OK) {
        qDebug() << "Error on inserting: " << PQresultErrorMessage(result);
        return false;
    }
    //qDebug() << "Insert done";
    return true;
}

void tsdbconnector::setFields(const QStringList &fields) {
    tsdbconnector::fields = fields.join(',');
    this->format =
}

