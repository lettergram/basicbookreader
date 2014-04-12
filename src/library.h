#ifndef LIBRARY_H
#define LIBRARY_H

#include <Qfile>
#include <QTextStream>
#include <QString>
#include <QMessageBox>
#include <QList>
#include <QStringList>
#include <QDir>

typedef struct _current_book{

    const QString * file_location;
    const QString * title;
    QFile * file;

    QTextStream * stream;
    int * pagenum;

    QList<int> page;

} current_book;

class library{

public:

    library(QString loc);
    void init_book(current_book * book);
    void save_bookinfo_to_database(QString loc);
    void load_database(QString loc);
    void loadbook(int index, current_book * book);
    void closeBook(current_book * book);
    QList<current_book> books;

private:
    void index_book(current_book * book);
};


#endif // LIBRARY_H