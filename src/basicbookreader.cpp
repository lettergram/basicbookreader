#include "basicbookreader.h"
#include "ui_basicbookreader.h"
#include "library.h"
#include "statistics.h"
#include <iostream>
#include <QFileDialog>
#include <QInputDialog>
#include <QCursor>
#include <QPoint>
#include <QDesktopServices>

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

        this->fontsize = 13;
        this->start = 0;
        this->end = 0;


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
    if(book == NULL || book->page.size() < 1){ return; }
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
 * Private function of the BasicBookReader class
 *
 * @brief BasicBookReader::parseImage - parses a given <img src=''>
 *          and adds the image to the scene if it exists. Currently
 *          Only works for following format <img src='location of
 *          image in books directory'>
 * @param y - current y-line postion
 * @param line - the line text to search for parsable image file
 * @param scene - scene to add image too
 * @return - 0 if no image found, the number of height (in pixels) of the image
 *          if the image exists.
 */
int BasicBookReader::parseImage(int y, QString line, QGraphicsScene * scene){

    QPixmap pixel_image;

    //TODO: Add basicbookreader private variable to hold y

    if(line.indexOf("img src=", 0, Qt::CaseInsensitive) != -1){
        QStringList imageName = line.split("\"");
        QPixmap pixel_image;

        pixel_image.load(*this->lib_loc + "images/" + imageName[1]);
        if(pixel_image.isNull())
            return 0;
        QGraphicsPixmapItem * item = new QGraphicsPixmapItem(pixel_image);
        item->setY(y);
        scene->addItem(item);
        return pixel_image.height();
    }
    return 0;
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

    QGraphicsScene * scene = new QGraphicsScene();
    QFont f;
    f.setPointSize(this->fontsize);
    int curline_pos = 0;
    int imgline = 0;

    for(int i = 0; i < LINESPERPAGE; i++){
        QString toadd(stream.readLine(85)); // TODO: Improve line and page parse
        imgline = this->parseImage(curline_pos, toadd, scene);
        if(imgline == 0){
            if(toadd.size() > 84)
                scene->addText(toadd + '-', f)->setPos(0, curline_pos);
            else
                scene->addText(toadd, f)->setPos(0, curline_pos);
        }
        curline_pos += (imgline | this->fontsize) + 3; // Not exactly inline but close enough
    }

    scene->addText(QString(QString("Page: ") + QString::number(this->book->pagenum)+ " / "
    + QString::number(this->book->page.count() - 1)).rightJustified(135, ' '))->setPos(0, curline_pos);
    ui->graphicsView->setScene(scene);

    this->stats->startPage(this->book->pagenum);
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
    stream << "Quote:\n\n";
    for(unsigned long i = 0; i < this->highlight.size(); i++)
        stream << "     " + this->highlight[i] + '\n';
    stream << "\n\nNote:\n" << note << endl;
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

/**
 * Private Slot of the BasicBookReader
 *
 * @brief BasicBookReader::on_graphicsView_rubberBandChanged - Saves the position
 *          the start and end point of the highlightes section
 * @param viewportRect - Current viewport rectangle - NOT USED
 * @param fromScenePoint - end point of highlighted box
 * @param toScenePoint - start point of highlighted box
 */
void BasicBookReader::on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint){

    if(book == NULL || book->page.size() < 1){ return; }

    if(fromScenePoint.y() != 0 && toScenePoint.y() != 0){

        this->start = toScenePoint.y();
        this->end = fromScenePoint.y();

        if(this->start/(this->fontsize + 3) == this->end/(this->fontsize + 3)){
            this->start_search = fromScenePoint.x();
            this->end_search = toScenePoint.x();
        }
        return;
    }

    if(this->start/(this->fontsize + 3) == this->end/(this->fontsize + 3))
        this->searchWord();
    else
        this->saveHighlightedSection();
}

/**
 * Private Function of BasicBookReader
 *
 * @brief BasicBookReader::searchWord - opens a google webpage with yoursearch
 *      it's awful, but it works (kinda)...
 *
 *  You just highlight the word(s) in the line
 *
 * FOR THE FUTURE: I will make it so if you click shift,
 *      all of the words will highlight so you can pick a specific word
 */
void BasicBookReader::searchWord(){

    QFile file(*book->file_location + *book->title);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());
    QTextStream stream(&file);
    stream.seek(this->book->page[this->book->pagenum]);

    if(this->end_search < this->start_search){
        int temp = this->start_search;
        this->start_search = this->end_search;
        this->end_search = temp;
    }

    this->start_search /= (7 * this->fontsize / 15);
    this->end_search /= (7 * this->fontsize / 15);
    QString line;

    for(int i = 0; i < LINESPERPAGE; i++){
        line = stream.readLine(85);
        if(this->start / (this->fontsize + 3) == i){
            int space = (line.count(' ', Qt::CaseInsensitive));
            line.remove(this->end_search, 85);
            line.remove(0, this->start_search - space);
            QDesktopServices::openUrl(QUrl("https://www.google.com/#q=define+" + line));
            break;
        }
    }
    this->stats->reviewed(line, this->book->pagenum);
}

/**
 * Private Function of BasicBookReader
 *
 *
 * WARNING: CURRENTLY INACCURATE ON PAGES WITH IMAGES - CRITICAL FIX COMING SOON
 *
 *
 * @brief BasicBookReader::saveHighlightedSection - Saves the highlighted
 *      section in the basicbookreaders qscene into this->heighlight[i]
 */
void BasicBookReader::saveHighlightedSection(){

    QFile file(*book->file_location + *book->title);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());
    QTextStream stream(&file);
    stream.seek(this->book->page[this->book->pagenum]);

    if(this->end < this->start){
        int temp = this->start;
        this->start = this->end;
        this->end = temp;
    }

    this->start /= (this->fontsize + 3);
    this->end /= (this->fontsize + 3);

    /* Iterate to start */
    for(int i = 0; i < this->start; i++){ stream.readLine(85); }

    /* Add to highlighted section */
    for(int i = this->start; i < this->end; i++)
        this->highlight.push_back(stream.readLine(85));
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
