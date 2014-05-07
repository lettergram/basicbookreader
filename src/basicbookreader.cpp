#include "basicbookreader.h"
#include "ui_basicbookreader.h"
#include "library.h"
#include "statistics.h"
#include <iostream>
#include <QFileDialog>
#include <QInputDialog>
#include <QCursor>
#include <QPoint>

/**
 *  Constructor of the BasicBookReader class
 *
 * @brief BasicBookReader::BasicBookReader - creates a qt window to display the books
 * @param parent - The qt widget object
 */
BasicBookReader::BasicBookReader(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BasicBookReader){

        this->grabKeyboard();

        ui->setupUi(this);

        QDir dir(QApplication::applicationDirPath());

        int i = 0;
        while(!dir.cd("books")){
            dir.cdUp();
            if(i++ == 5){ break; }
        }

        this->lib_loc = new QString(dir.absolutePath() + "/");

        this->lib = new library(*this->lib_loc);
        this->book = new current_book();
        this->stats = NULL;
        this->book->open = false;

        this->book->file_location = new QString(*this->lib_loc);
        this->search = QString("Pages");
        this->grabKeyboard();
}

/**
 *  Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::~BasicBookReader Deconstructor, called when the ui is destroyed
 * Deletes all excess data
 */
BasicBookReader::~BasicBookReader(){

    this->releaseKeyboard();

    if(this->book != NULL){ lib->closeBook(this->book); }
  
    if(this->stats!= NULL){
        this->stats->endPage(this->book->pagenum);
        delete this->stats;
    }

    this->lib->save_bookinfo_to_database(*this->lib_loc);

    delete book;
    delete ui;
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_nextbutton_clicked - loads next page
 */
void BasicBookReader::on_nextButton_clicked(){
    if(this->book == NULL){ return; }
    if(this->book->pagenum >= this->book->page.size() - 1){ return; }
    this->stats->endPage(this->book->pagenum);
    (this->book->pagenum)++;
    this->grabKeyboard();
    loadpage();
}

/**
 * Public Function of the BasicBookReader
 *
 * @brief BasicBookReader::on_prevButton_clicked - s previous page
 */
void BasicBookReader::on_prevButton_clicked(){

    if(this->book == NULL){ return; }
    if((this->book->pagenum) > 0){
        this->stats->endPage(this->book->pagenum);
        (this->book->pagenum)--;
    }

    this->grabKeyboard();
    loadpage();
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_saveBookButton_clicked - saves the book to the library
 */
void BasicBookReader::on_saveBookButton_clicked(){
    if(book == NULL){ return; }
    for(int i = 0; i < lib->books.size(); i++){
        if(book->title->compare(lib->books[i].title, Qt::CaseInsensitive) == 0){
            lib->books[i].pagenum = book->pagenum;
        }
    }
    lib->save_bookinfo_to_database(*lib_loc);
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_lineEdit_page_textEdited - changes the book to that particular page
 * @param arg1 - the text entered converted to page number and loads
 */
void BasicBookReader::on_lineEdit_page_textEdited(const QString &arg1){

    if(this->book == NULL){ return; }
    this->stats->endPage((this->book->pagenum));

    if(arg1.toInt() > this->book->page.count())
        QMessageBox::information(0, "Error", "Pages out of bounds");

    if((this->search).compare(QString("Chapters"), Qt::CaseInsensitive) == 0){
        if(arg1.toInt() > this->book->chapter.count() - 1){ return; }
        this->book->pagenum = this->book->chapter[arg1.toInt()];
    }else{
        if(arg1.toInt() > this->book->page.count() - 1){ return; }
        this->book->pagenum = arg1.toInt();
    }

    this->stats->endPage(this->book->pagenum);
    loadpage();
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_toolButton_clicked - if the tool button is clicked
 *      a window appears where you can choose what book to load. This function is called
 *      and will parse the file and load the book
 */
void BasicBookReader::on_toolButton_clicked(){

    if(this->stats != NULL){
        if(this->book != NULL){
            this->stats->endPage(this->book->pagenum);
        }
        delete this->stats;
    }

    if(book->open)
      this->lib->closeBook(this->book);

    QString fileName = QFileDialog::getOpenFileName(this, "Select a file to open...", (*this->book->file_location));
    if(fileName == NULL){ return; }

    QStringList list = fileName.split("/", QString::SkipEmptyParts);
    this->book->title = new QString(list[list.count() - 1]);

    this->book->file_location = new QString(fileName.remove((*this->book->title), Qt::CaseInsensitive));

    loadNewBook();
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_enable_stats_button_clicked - enables/disables statistics
 *              (this is default).
 */
void BasicBookReader::on_enable_stats_button_clicked(){
    if(this->stats== NULL){ return; }
    if((ui->enable_stats_button->text()).compare(QString("Enable  Statistics"), Qt::CaseInsensitive) == 0){
        this->stats->enableStats();
        QMessageBox::information(0, "Statistics", "Statistics Enabled");
        ui->enable_stats_button->setText("Disable Statistics");
    }else{
        this->stats->disableStats();
        QMessageBox::information(0, "Statistics", "Statistics Disabled");
        ui->enable_stats_button->setText("Enable  Statistics");
    }
}


/**
 * Private Function of the BasicBookReader class
 *
 * @brief BasicBookReader::next_page - loads the next page (30 lines) from the current book
 * into the window.
 */
void BasicBookReader::loadpage(){

    QString str("");
    this->highlight.clear();

    QFile file(*book->file_location + *book->title);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());
    QTextStream stream(&file);

    if(!stream.seek(this->book->page[this->book->pagenum])){
        stream.seek(0);
        this->book->pagenum = 0;
    }
    for(int i = 0; i < LINESPERPAGE; i++){
        QString toadd(stream.readLine(85));
        if(toadd.size() > 84)
            str.append(toadd + '-' + '\n');
        else
            str.append(toadd + '\n');
    }

    str.append(QString(QString("Page: ") + QString::number(this->book->pagenum)
               + " / " + QString::number(this->book->page.count() - 1) + '\n').rightJustified(135, ' '));

    const QString * display = &str;

    this->stats->startPage(this->book->pagenum);
    ui->textBrowser->setText(*display);
    file.close();
}

/**
 * Private Function of BasicBookReader class
 *
 * @brief BasicBookReader::loadNewBook - loads a book already in the library into memory
 * Closing the previously open book as well as the accompaning statistics
 */
void BasicBookReader::loadNewBook(){

    for(int i = 0; i < this->lib->books.count(); i++){
        if(this->book->title->compare(this->lib->books[i].title, Qt::CaseInsensitive) == 0){
            this->lib->loadbook(i, this->book);
            this->stats= new statistics(*this->book->title, this->book->page.count(), LINESPERPAGE);
            loadpage();
            return;
        }
    }

    this->book->open = true;

    this->lib->init_book(this->book);
    this->stats= new statistics(*this->book->title, this->book->page.count(), LINESPERPAGE);
    loadpage();
}

/**
 * Public Function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_textBrowser_selectionChanged - what has been
 *          highlighted for notes
 */
void BasicBookReader::on_textBrowser_selectionChanged(){

    if(this->book == NULL || this->book->page.size() == 0){ return; }

    QPoint pos = QCursor::pos();

    int y = int(double(pos.y() - 130) / 16.8);
    int x = int(double(pos.x() - 357));             // TODO save between to save particular words

    QFile file(*book->file_location + *book->title);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());
    QTextStream stream(&file);
    stream.seek(this->book->page[this->book->pagenum]);

    if( this->highlight.size() == 0) { this->highlight.resize(LINESPERPAGE); }

    for(int i = 0; i < LINESPERPAGE; i++){
        QString line(stream.readLine(85) + '\n');
        if(i == y){
            this->stats->xcursor[i].push_back(x);
            if(this->highlight[i] == NULL){ this->highlight[i].append(line); }
        }
    }

    stream.seek(this->book->page[this->book->pagenum]);
    file.close();
}


/**
 * Public funcion of BasicBookReader class
 *
 * @brief BasicBookReader::on_pushNoteButton_clicked - adds a highlighted section to the "<bookname>.notes"
 * The user can then add a specific note.
 */
void BasicBookReader::on_pushNoteButton_clicked(){

    if(this->book == NULL){ return; }
    if(this->stats== NULL){ return; }
    this->releaseKeyboard();
    bool ok;
    QStringList title = (*this->book->title).split(".", QString::SkipEmptyParts);
    QString note = QInputDialog::getText(this, tr("Highlights and Notes"),
                                         tr("Note"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);

    QDir dir(QApplication::applicationDirPath());
    int j = 0;
    while(!dir.cd("notes")){ dir.cdUp(); if(j++ == 7){ break; } }

    QString location(dir.absolutePath() + "/");
    QFile file(QString(location + title[0] + ".notes"));

    if(!file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text))
        QMessageBox::information(0, "Error", "Writing in journal");

    QTextStream stream(&file);

    stream << title[0] << ", page " << this->book->pagenum << ": " << endl;
    stream << "Quote:\n'";
    for(unsigned long i = 0; i < this->highlight.size(); i++)
        stream << this->highlight[i];
    stream << "'" << endl;
    stream << "Note: " << note << endl;
    stream << "---------------------------\n" << endl;

    this->highlight.clear();

    file.close();
}

/**
 * Public function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_comboBox_currentIndexChanged - This combobox is used
 *      to change what you are using the search bar for
 * @param arg1 - the item you are searching for
 */
void BasicBookReader::on_search_type_currentIndexChanged(const QString &arg1){
    this->search = QString(arg1);
    if((this->search).compare(QString("Chapters"), Qt::CaseInsensitive) == 0)
        QMessageBox::information(0, "Experimental", "This is experimental and may not work");
}

/**
 * Public function of the BasicBookReader class
 *
 * @brief BasicBookReader::on_user_rating_currentIndexChanged - This combobox
 *      is used to select how many stars a user would rate the book
 * @param index - the number of stars the user would give a book
 */
void BasicBookReader::on_user_rating_currentIndexChanged(int index){

    if(this->book == NULL){ return; }
    this->stats->usrsrating(index);
}

/**
 * Public function of the BasicBookReader class
 *
 * @brief BasicBookReader::keyPressEvent - is called when grabKeyboard()
 *      is in effect. It will determine which key has been pressed and call that function
 * @param k - the key pressed.
 */
void BasicBookReader::keyPressEvent( QKeyEvent *k ){

    if(k->key() == Qt::Key_Right)
        on_nextButton_clicked();
    else if(k->key() == Qt::Key_Left)
        on_prevButton_clicked();
    else if(k->key() == Qt::Key_Backspace)
        this->releaseKeyboard();
    else if(k->key() == Qt::Key_O)
        on_toolButton_clicked();
    else if(k->key() == Qt::Key_B)
        on_saveBookButton_clicked();
    else if(k->key() == Qt::Key_S)
        on_viewStats_clicked();
    if(k->key() == Qt::Key_0
       || k->key() == Qt::Key_1
       || k->key() == Qt::Key_2
       || k->key() == Qt::Key_3
       || k->key() == Qt::Key_4
       || k->key() == Qt::Key_5
       || k->key() == Qt::Key_6
       || k->key() == Qt::Key_7
       || k->key() == Qt::Key_8
       || k->key() == Qt::Key_9){
        this->releaseKeyboard();
    }
}


/**
 * Public function of the BasicBookReader Class
 *
 * @brief BasicBookReader::on_viewStats_clicked - if
 *      statistics is clicked open the view statistics window
 */
void BasicBookReader::on_viewStats_clicked(){

    sv = new statsviewer();
    QStringList titles;
    for(int i = 0; i < this->lib->books.size(); i++)
        titles.append(*this->lib->books[i].title);
    sv->initTitle(titles);
    sv->show();
}
