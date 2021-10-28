//
// Created by tokareva on 27.10.2021.
//
#include <tsdbconnector.h>
#include <QStringList>

int main(int argc, char** argv) {
    tsdbconnector conn;
    conn.setConnectParameters("127.0.0.1", 5433, "root", ".root");
    conn.setDatabase("DynData");
    conn.connect();

    conn.insert("mdata", QStringList{"ts", "tag", "textval", "value"},
                QStringList{"NOW()", "'mytag'", "'description'", QString::number(33.21)});

    conn.setTablename("mdata");
    conn.setFields(QStringList{"ts", "tag", "textval", "value"});
    conn.setFormat("'%1', '%2', '%3', %4");

    conn.preloadValues(QStringList{"NOW()", "mytag", "description", QString::number(22.12)});
    conn.preloadValue("NOW(), 'mytag', 'description', 11.11");
    conn.insertPreloaded();
    
    conn.select("mdata",QStringList{"ts", "tag", "textval", "value"}, 5, " ORDER BY ts DESC " );
}