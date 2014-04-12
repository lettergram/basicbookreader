#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "library.h"
#include "statistics.h"
#include <iostream>
#include <QFileDialog>

current_book * book;
library * lib;
QString * lib_loc;
statistics * stats;

/**
 *  Constructor of the MainWindow class
 *
 * @brief MainWindow::MainWindow - creates a qt window to display the books
 * @param parent - The qt widget object
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
        ui->setupUi(this);

        QDir dir;
        int i = 0;
        while(!dir.cd("books")){
            dir.cdUp();
            if(i++ == 7){ break; }
        }
        lib_loc = new QString(dir.absolutePath() + "/");

        lib = new library(*lib_loc);
        book = new current_book();

        QTextStream(stdout) << *lib_loc << endl;
        book->file_location = new QString(*lib_loc);
        book->title = new QString("The-Adventures-of-Tom-Sawyer.txt");
}

/**
 *  Public Function of the MainWindow class
 *
 * @brief MainWindow::~MainWindow Deconstructor, called when the ui is destroyed
 * Deletes all excess data
 */
MainWindow::~MainWindow(){
    stats->endPage(*book->pagenum);
    lib->save_bookinfo_to_database(*lib_loc);
    delete stats;
    delete book;
    delete ui;
}

/**
 * Public Function of the MainWindow class
 *
 * @brief MainWindow::on_addtoLibButton_clicked - Adds a new book to the library,
 * loads the first page.
 */
void MainWindow::on_addtoLibButton_clicked(){

    lib->init_book(book);
    if(stats != NULL){ delete stats; }
    stats = new statistics(*book->title, book->page.count());
    loadpage();
}

/* Loads the string in the file_location field into the book->file_location */
void MainWindow::on_file_location_textEdited(const QString &arg1){
    book->file_location = new QString(arg1);
}

/* Loads the string in the book_name field into the book->title */
void MainWindow::on_book_name_textEdited(const QString &arg1){
    book->title = new QString(arg1);
}

/**
 * Public Function of the MainWindow class
 *
 * @brief MainWindow::on_nextbutton_clicked - loads next page
 */
void MainWindow::on_nextButton_clicked(){
    if(book->stream == NULL){ return; }
    stats->endPage(*book->pagenum);
    (*book->pagenum)++;
    loadpage();
}

/**
 * Public Function of the MainWindow class
 *
 * @brief MainWindow::on_prevButton_clicked - s previous page
 */
void MainWindow::on_prevButton_clicked(){

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
 * Public Function of the MainWindow class
 *
 * @brief MainWindow::on_saveBookButton_clicked - saves the book to the library
 */
void MainWindow::on_saveBookButton_clicked(){
    lib->save_bookinfo_to_database(*lib_loc);
}

/**
 * Public Function of the MainWindow class
 *
 * @brief MainWindow::on_loadBookButton_clicked - loads the book from the library into the display
 */
void MainWindow::on_loadBookButton_clicked(){

    if(book->title == NULL){ return; }

    loadNewBook();
}

/**
 * Public Function of the MainWindow class
 *
 * @brief MainWindow::on_lineEdit_page_textEdited - changes the book to that particular page
 * @param arg1 - the text entered converted to page number and loads
 */
void MainWindow::on_lineEdit_page_textEdited(const QString &arg1){

    if(book->stream == NULL){ return; }

    if(arg1.toInt() > book->page.count()){
        QString err("Pages out of bounds");
        QMessageBox::information(0, "Error", err);
    }

    stats->endPage(*book->pagenum);
    *book->pagenum = arg1.toInt();
    book->stream->seek(book->page[*book->pagenum]);
    loadpage();
}

/**
 * Public Function of the MainWindow class
 *
 * @brief MainWindow::on_toolButton_clicked - if the tool button is clicked
 * a window appears where you can choose what book to load. This function is called
 * and will parse the file and load the book
 */
void MainWindow::on_toolButton_clicked(){

    QString fileName = QFileDialog::getOpenFileName(this, "Select a file to open...", (*book->file_location));
    if(fileName == NULL){ return; }

    QStringList list = fileName.split("/", QString::SkipEmptyParts);
    book->title = new QString(list[list.count() - 1]);

    book->file_location = new QString(fileName.remove((*book->title), Qt::CaseInsensitive));

    loadNewBook();
}

/**
 * Private Function of the MainWindow class
 *
 * @brief MainWindow::next_page - loads the next page (50 lines) from the current book
 * into the window.
 */
void MainWindow::loadpage(){

    QString str("");

    for(int i = 0; i < 50; i++){
        QString toadd(book->stream->readLine(85));
        if(toadd.size() > 84)
            str.append(toadd + '-' + '\n');
        else
            str.append(toadd + '\n');
    }

    str.append(QString("\nPage: ") + QString::number(*book->pagenum) + " / " + QString::number(book->page.count()) + '\n');

    const QString * display = &str;

    stats->startPage(*book->pagenum);
    ui->textBrowser->setText(*display);
}

/**
 * Private Function of MainWindow class
 *
 * @brief MainWindow::loadNewBook - loads a book already in the library into memory
 * Closing the previously open book as well as the accompaning statistics
 */
void MainWindow::loadNewBook(){
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
}
