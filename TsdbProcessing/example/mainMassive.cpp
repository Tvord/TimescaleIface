//
// Created by tokareva on 27.10.2021.
//

#include <tsdbconnector.h>
#include <QStringList>
#include <random>
#include <unistd.h>

int main(int argc, char** argv) {
    tsdbconnector conn;
    conn.setConnectParameters("127.0.0.1", 5433, "root", ".root");
    conn.setDatabase("DynData");
    conn.connect();

    QStringList tags{"Tag1", "NewTag", "Tag1234", "HOBA!"};
    QStringList descs{tags};
    for (auto& val : descs) {
        val.append("_DESC");
    }

    srand(time(NULL));

    while (true) {
        auto index = rand() % 4;
        conn.insert("mdata", QStringList{"ts", "value", "tag", "textval"},
                                    QStringList{"NOW()", QString::number((float)(rand() % 100) + ((float)(rand()%100)/100)),
                                                "'" + tags[index] + "'", "'" + descs[index] + "'"});

        usleep(1000);
    }
    conn.insert("mdata", QStringList{"ts", "tag", "textval"}, QStringList{"NOW()", "'mytag'", "'description'"});
    conn.select("mdata",QStringList{"ts", "tag", "textval"}, 5 );
}