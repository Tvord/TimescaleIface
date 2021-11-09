//
// Created by tokareva on 27.10.2021.
//

#include "tsdbconnector.h"
#include <iostream>
#include <QThread>
#include <QElapsedTimer>
#include <sstream>

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
    auto result = PQexec(conn, QString("SELECT %1 FROM %2 %3 LIMIT %4").arg(
            fields.join(','), table, args, QString::number(limit)
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

void tsdbconnector::setFormat(const QString &mFormat) {
    format.clear();
    format = mFormat;
}

void tsdbconnector::setFields(const QStringList &fields) {
    tsdbconnector::fields = fields.join(',').prepend('(').append(')');

}

void tsdbconnector::setTablename(const QString &tablename) {
    tsdbconnector::tablename = tablename;
}

bool tsdbconnector::insert(const QStringList &values) {
    return false;
}

bool tsdbconnector::insertPreloaded() {
    if (valuesQueue.isEmpty()) return false;
    qDebug() << "Inserting " << valuesQueue.count() << " values!";
    if (Q_UNLIKELY(PQstatus(conn) != ConnStatusType::CONNECTION_OK)) {
        qDebug() << "Unable to execute query. Status: " << (int) PQstatus(conn) << ", message: " << PQerrorMessage(conn);
        return false;
    }
    const char* const delim = ", ";

    std::ostringstream joinerStr;
    std::copy(valuesQueue.begin(), valuesQueue.end(),
              std::ostream_iterator<std::string>(joinerStr, delim));
    auto strVals = joinerStr.str();
    strVals.resize(strVals.size() - 2);
    PQsendQuery(conn, QString("INSERT INTO %1 %2 VALUES %3;").arg(
            tablename, fields, QString::fromStdString(strVals)).toStdString().c_str());
    valuesQueue.clear();

    return true;
}

void tsdbconnector::preloadValues(std::vector<std::string> values) {
    valuesQueue.append(QList<std::string>::fromStdList(std::list<std::string>(values.begin(), values.end())));
//    QStringList qValues{};
//    for (auto& fieldVal : values) {
//        qValues.append(QString::fromStdString(fieldVal));
//    }
//    preloadValues(qValues);
}

void tsdbconnector::preloadValues(std::vector<std::vector<std::string>> values) {
//    for (auto& val : values) {
//        preloadValues(val);
//    }
}

void tsdbconnector::preloadValues(QStringList values) {
    valuesQueue.append(prepareValuesString(values).prepend('(').append(')').toStdString());
}

void tsdbconnector::preloadValues(QList<QStringList> &values) {
    for (auto& val : values) {
        preloadValues(val);
    }
}

void tsdbconnector::preloadValue(QString value) {
    if (!value.startsWith('(')) value.prepend('(');
    if (!value.endsWith(')')) value.append(')');
    valuesQueue.append(value.toStdString());
}

QString tsdbconnector::prepareValuesString(QStringList &valuesList) {
    QString newVal{format};
    for (auto& val : valuesList) {
        newVal = newVal.arg(val);
    }
    return newVal;
}

//inline void tsdbconnector::preloadValue(const std::string &value) {
//    //auto val = const_cast<std::string&>(value);
//    //val.insert(0, 1, '(');
//    //val.append(")");
//
//    valuesQueue.append(value);
//    //preloadValue(QString::fromStdString(value));
//}



