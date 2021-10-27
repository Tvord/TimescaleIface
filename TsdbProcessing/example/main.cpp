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

    conn.insert("mdata", QStringList{"ts", "tag", "textval"}, QStringList{"NOW()", "'mytag'", "'description'"});
    conn.select("mdata",QStringList{"ts", "tag", "textval"}, 5 );
}