#include "library.h"
#include <iostream>

/**
 * Constructor for the library class
 */
library::library(QString loc){
    load_database(loc);
}

/**
 * Public Function of the library class
 *
 * @brief library::load_database - loads the data from /books/library.txt
 * @param lib_loc - the library location, it will search all parent directories
 */
void library::load_database(QString lib_loc){

    QFile file(lib_loc + QString("library.txt"));

    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());

    QTextStream database(&file);

    while(!database.atEnd()){
        current_book b;
        b.title = new QString(database.readLine());
        b.file_location = new QString(database.readLine());

        QString open(*b.file_location);
        open.append(*b.title);
        b.file = new QFile(open);

        b.pagenum = new int(database.readLine().toInt());

        QString pages = database.readLine();

        QStringList list = pages.split(",", QString::SkipEmptyParts);

        for(int i = 0; i < list.count(); i++){
            b.page.append(list[i].toInt());
        }

        this->books.append(b);
    }
    file.close();
}

/**
 * Public Function of the library class
 *
 * @brief library::loadbook - loads a particular book from the this->books
 *        QList (aka the library), it will load the book into *book and the display
 * @param index - The index of the book in the list
 * @param book - The pointer to the current book to be displayed
 */
void library::loadbook(int index, current_book * book){

    book->file = this->books[index].file;
    book->pagenum = this->books[index].pagenum;
    book->title = this->books[index].title;
    book->file_location = this->books[index].file_location;

    for(int i = 0; i < this->books[index].page.count(); i++)
        book->page.append(this->books[index].page[i]);

    if(!book->file->open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", book->file->errorString());

    book->stream = new QTextStream(book->file);

    if(!book->stream->seek(book->page.value(*book->pagenum))){
        book->stream->seek(0);
        (*book->pagenum) = 0;
    }
}

/**
 * Public Function of the library class
 *
 * @brief library::save_bookinfo_to_database - Saves the current book data to
 *          books/library.txt, which will be loaded on startup.
 * @param lib_loc - the locaiton of library.txt in the file system
 */
void library::save_bookinfo_to_database(QString lib_loc){

    QFile file(lib_loc + QString("library.txt"));

    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        QMessageBox::information(0, "Error", file.errorString());

    QTextStream database(&file);

    for(int i = 0; i < books.count(); i++){
        database << *books[i].title << "\n";
        database << *books[i].file_location << "\n";
        database << *books[i].pagenum << "\n";

        for(int j = 0; j < books[i].page.count(); j++){
            database << QString::number(books[i].page.value(j)) << ",";
        }
        database << "\n";
    }
}

/**
 * Public Function of the library class
 *
 * @brief init_book - This function is a helper function used to initialize the book struct
 * @param book - The book which needs an index
 */
void library::index_book(current_book* book){

    book->page.push_back(0);
    while(!book->stream->atEnd()){
        for(int i = 0; i < 50; i++){
            book->stream->readLine(85);
        }
        book->page.push_back(book->stream->pos());
    }

    book->stream->seek(0);
    book->stream->resetStatus();
}

/**
 * Public Function of the library class
 *
 * @brief library::closeBook - use if you are attempted to grab a new book,
 *          closes current book.
 * @param book - book to be closed
 */
void library::closeBook(current_book* book){
    if(book == NULL || book->file == NULL)
        return;
    if(book->file->isOpen())
        book->file->close();
    book->page.clear();
}

/**
 * Public Function of the library class
 *
 * @brief init_book - This function is function used to initialize the book struct
 * @param book - The book which needs to be initalized
 */
void library::init_book(current_book * book){

    for(int i = 0; i < this->books.count(); i++){
        if(book->title->compare(this->books[i].title, Qt::CaseInsensitive) == 0){
            loadbook(i, book);
            return;
        }
    }

    QString open(*book->file_location);
    open.append(*book->title);

    if(book->file_location->size() > 0 || book->title->size() > 4)
        book->file = new QFile(open);

    if(!book->file->open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", book->file->errorString());

    book->pagenum = new int(0);
    book->stream = new QTextStream(book->file);

    /* Index book pages */
    index_book(book);

    /* Add book to Library */
    books.append(*book);
}