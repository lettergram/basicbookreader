#include "BasicBookReader.h"
#include "ui_BasicBookReader.h"
#include "library.h"
#include "statistics.h"
#include <iostream>
#include <QFileDialog>
#include <QInputDialog>
#include <QCursor>
#include <QPoint>

current_book * book;
library * lib;
QString * lib_loc;
statistics * stats;
std::vector< QString > highlight;

/**
 *  Constructor of the BasicBookReader class
 *
 * @brief BasicBookReader::BasicBookReader - creates a qt window to display the books
 * @param parent - The qt widget object
 */
BasicBookReader::BasicBookReader(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BasicBookReader){
        ui->setupUi(this);

        QDir dir(QApplication::applicationDirPath());

        int i = 0;
        while(!dir.cd("books")){
            dir.cdUp();
            if(i++ == 7){ break; }
        }
        lib_loc = new QString(dir.absolutePath() + "/");

        lib = new library(*lib_loc);
        book = new current_book();

        book->file_location = new QString(*lib_loc);
}

/**
 *  Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::~BasicBookReader Deconstructor, called when the ui is destroyed
 * Deletes all excess data
 */
BasicBookReader::~BasicBookReader(){

    if(stats != NULL){
        stats->endPage(*book->pagenum);
        delete stats;
    }

    lib->save_bookinfo_to_database(*lib_loc);

    delete book;
    delete ui;
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_nextbutton_clicked - loads next page
 */
void BasicBookReader::on_nextButton_clicked(){
    if(book->stream == NULL){ return; }
    stats->endPage(*book->pagenum);
    (*book->pagenum)++;
    loadpage();
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_prevButton_clicked - s previous page
 */
void BasicBookReader::on_prevButton_clicked(){

    if(book->stream == NULL){ return; }
    if((*book->pagenum) > 0){
        stats->endPage(*book->pagenum);
        (*book->pagenum)--;
    }

    if(!book->stream->seek(book->page[*book->pagenum])){
        book->stream->seek(0);
        (*book->pagenum) = 0;
    }
    // save stats to file
    loadpage();
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_saveBookButton_clicked - saves the book to the library
 */
void BasicBookReader::on_saveBookButton_clicked(){
    lib->save_bookinfo_to_database(*lib_loc);
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_lineEdit_page_textEdited - changes the book to that particular page
 * @param arg1 - the text entered converted to page number and loads
 */
void BasicBookReader::on_lineEdit_page_textEdited(const QString &arg1){

    if(book->stream == NULL){ return; }

    if(arg1.toInt() > book->page.count())
        QMessageBox::information(0, "Error", "Pages out of bounds");

    stats->endPage(*book->pagenum);
    *book->pagenum = arg1.toInt();
    book->stream->seek(book->page[*book->pagenum]);
    loadpage();
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_toolButton_clicked - if the tool button is clicked
 * a window appears where you can choose what book to load. This function is called
 * and will parse the file and load the book
 */
void BasicBookReader::on_toolButton_clicked(){

    QString fileName = QFileDialog::getOpenFileName(this, "Select a file to open...", (*book->file_location));
    if(fileName == NULL){ return; }

    QStringList list = fileName.split("/", QString::SkipEmptyParts);
    book->title = new QString(list[list.count() - 1]);

    book->file_location = new QString(fileName.remove((*book->title), Qt::CaseInsensitive));

    loadNewBook();
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_disable_stats_button_clicked - disables statistics
 *
 * ==================== WARNING =======================
 * They must disable it every time they load the book
 * or statistics will be kept. The assumption being
 * some people will click it and forget.
 *
 */
void BasicBookReader::on_disable_stats_button_clicked(){
    if(stats == NULL){ return; }
    QMessageBox::information(0, "Statistics", "Statistics Disabled");
    stats->disableStats();
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_enable_stats_button_clicked - enables statistics
 *              (this is default).
 */
void BasicBookReader::on_enable_stats_button_clicked(){
    if(stats == NULL){ return; }
    QMessageBox::information(0, "Statistics", "Statistics Enabled");
    stats->enableStats();
}


/**
 * Private Function of the BasicBookReader class
 *
 * @brief BasicBookReader::next_page - loads the next page (30 lines) from the current book
 * into the window.
 */
void BasicBookReader::loadpage(){

    QString str("");
    highlight.clear();

    for(int i = 0; i < LINESPERPAGE; i++){
        QString toadd(book->stream->readLine(85));
        if(toadd.size() > 84)
            str.append(toadd + '-' + '\n');
        else
            str.append(toadd + '\n');
    }

    str.append(QString(QString("Page: ") + QString::number(*book->pagenum)
               + " / " + QString::number(book->page.count()) + '\n').rightJustified(150, ' '));

    const QString * display = &str;

    stats->startPage(*book->pagenum);
    ui->textBrowser->setText(*display);
}

/**
 * Private Function of BasicBookReader class
 *
 * @brief BasicBookReader::loadNewBook - loads a book already in the library into memory
 * Closing the previously open book as well as the accompaning statistics
 */
void BasicBookReader::loadNewBook(){
    if(stats != NULL){ delete stats; }
    lib->closeBook(book);

    for(int i = 0; i < lib->books.count(); i++){
        if(book->title->compare(lib->books[i].title, Qt::CaseInsensitive) == 0){
            lib->loadbook(i, book);
            stats = new statistics(*book->title, book->page.count());
            loadpage();
            return;
        }
    }

    /* If no book can be found in library, create a new entry! */
    lib->init_book(book);
    stats = new statistics(*book->title, book->page.count());
    loadpage();

}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_textBrowser_selectionChanged
 */
void BasicBookReader::on_textBrowser_selectionChanged(){

    if(book == NULL || book->stream == NULL){ return; }

    QPoint pos = QCursor::pos();

    int y = int(double(pos.y() - 134) / 15.8);

    std::cout << QString::number(y).toStdString() << std::endl;

    book->stream->seek(book->page[*book->pagenum]);

    if( highlight.size() == 0) { highlight.resize(LINESPERPAGE); }

    for(int i = 0; i < LINESPERPAGE; i++){
        if(i == y && highlight[i] == NULL)
            highlight[i].append(book->stream->readLine(85) + '\n');
        else
            book->stream->readLine(85);
    }

    book->stream->seek(book->page[*book->pagenum]);

}


/**
 * Public funcion of BasicBookReader class
 *
 * @brief BasicBookReader::on_pushButton_clicked - adds a highlighted section to the "<bookname>.notes"
 * The user can then add a specific note.
 */
void BasicBookReader::on_pushButton_clicked(){

    if(book == NULL){ return; }

    bool ok;
    QStringList title = (*book->title).split(".", QString::SkipEmptyParts);

    QString note = QInputDialog::getText(this, tr("Highlights and Notes"),
                                         tr("Note"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);

    QDir dir;
    int j = 0;
    while(!dir.cd("notes")){ dir.cdUp(); if(j++ == 7){ break; } }

    QString location(dir.absolutePath() + "/");
    QFile file(QString(location + title[0] + ".notes"));

    /* open file for appending */
    if(!file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text))
        QMessageBox::information(0, "Error", "Writing in journal");

    QTextStream stream(&file);

    stream << title[0] << ", page " << *book->pagenum << ": " << endl;
    stream << "Quote:\n'";
    for(unsigned long i = 0; i < highlight.size(); i++)
        stream << highlight[i];
    stream << "'" << endl;
    stream << "Note: " << note << endl;
    stream << "---------------------------\n" << endl;

    highlight.clear();

    file.close();
}
