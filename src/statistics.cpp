#include "statistics.h"
#include <stdlib.h>

static int MAXREAD = 12;

/**
 * Constructor for the statistics class
 * 
 * @brief statistics::statistics - Constructor for the statistics object
 * @param book - Qstring title for the book
 * @param location - QString location the stats file will be stored
 */
statistics::statistics(QString book, int numberOfPages, int linesPerPage){

    disable_flag = false;

    QDir dir(QApplication::applicationDirPath());
    int i = 0;
    while(!dir.cd("stats")){
        dir.cdUp();
        if(i++ == 7){ break; }
    }

    QString location(dir.absolutePath() + "/");
    QStringList title = book.split(".", QString::SkipEmptyParts);

    this->file_loc = new QString(location + title[0] + QString(".stat"));
    this->reviewed_loc = new QString(location + title[0] + QString(".interest"));
    this->date_loc = new QString(location + QString("journal.log"));

    this->bookSize = numberOfPages;
    this->pageTimes.resize(numberOfPages);

    for(int i = 0; i < numberOfPages; i++)
        this->pageTimes[i].resize(MAXREAD, 0);

    this->xcursor.resize(linesPerPage);
    openJournal();
    loadStatsDocument();
}

/**
 * Deconstructor for the statistics class
 * 
 * @brief statistics::~statistics - deconstructor for statistics class,
 * saves out to file first, before deconstructing.
 */
statistics::~statistics(){
    if(disable_flag){ return; }
    generateStatsDocument();
    closeJournal();
}

/**
 * Public Function
 *
 * @brief statistics::startPage
 * @param pagenum - the page that the stats are recording
 */
void statistics::startPage(int pagenum){

    if(disable_flag){ return; }

    this->start = time(NULL);
    (*this->pageVists)++;
    for(int i = 0; i < MAXREAD; i++){
        if(this->pageTimes[pagenum][i] == 0){
            this->index = i;
            return;
        }
    }

    srand(time(NULL));
    this->index = rand() % MAXREAD;
}

/**
 * Public Function
 *
 * @brief statistics::endPage - called when a page is about to be closed
 *      this function will add the page time to the pageTimes array
 * @param pagenum - the current page, that is about to be closed
 */
void statistics::endPage(int pagenum){

    if(disable_flag){ return; }

    reviewed(pagenum);

    double diff = difftime(time(NULL), this->start);

    if(diff < 2 ){ return; }

    this->pageTimes[pagenum][this->index] = diff;
}

/**
 * Public Function for the statistics class
 *
 * @brief statistics::disableStats - disables all statistics functions
 */
void statistics::disableStats(){
    this->disable_flag = true;
}

/**
 * Public Function for the statistics class
 *
 * @brief statistics::dEnableStats - Enables all statistics functions (default)
 */
void statistics::enableStats(){
     this->disable_flag = false;
}


/**
 * Private Function of the statistics class
 *
 * @brief generateStatsDocument - This function will generate a statistics
 * document for a given book.
 */
void statistics::generateStatsDocument(){

    if(disable_flag){ return; }

    QFile file(*this->file_loc);
    QTextStream stream(&file);

    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        QMessageBox::information(0, "Error", file.errorString());

    if(this->pageTimes.size() < 1)
        return;

    for(int i = 0; i < this->bookSize; i++){
        for(int j = 0; j < MAXREAD; j++){
            stream << this->pageTimes[i][j] << ",";

            if(this->pageTimes[i][j] == 0){ break; }
        }
        stream << endl;
    }

    file.close();
}

/**
 * Private Function of the statistics class
 *
 * @brief statistics::loadStatsDocument - this function will load a
 * statistics document for the book the statistics object is being used for.
 *
 * WARNING - this function requires location of the stats folder + filename FIRST
 */
void statistics::loadStatsDocument(){

    if(disable_flag){ return; }

    if(this->file_loc == NULL){ return; }

    QFile file(*this->file_loc);

    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", "Loading new Stats file");

    QTextStream stream(&file);
    QString pagestat;

    for(int page = 0; !stream.atEnd(); page++){

        pagestat = stream.readLine();

        QStringList list = pagestat.split(",", QString::SkipEmptyParts);

        for(int i = 0; i < list.count(); i++)
            this->pageTimes[page][i] = list[i].toDouble();
    }
    file.close();
}

/**
 * Private Function of the statistics class
 *
 * @brief statistics::openJournal - This function will
 * save the begining time that a book has been opened, as well as
 * initialize the pageVisits variable.
 */
void statistics::openJournal(){

    if(disable_flag){ return; }

    if(this->date_loc == NULL){ return; }

    QFile file(*this->date_loc);

    QStringList list = (*this->file_loc).split("/", QString::SkipEmptyParts);
    QStringList title = list[list.count() - 1].split(".", QString::SkipEmptyParts);

    time_t current_time = time(NULL);
    this->logFile = new QString(title[0] + ", " + QString(ctime(&current_time)).remove(24, 1) + ", ");
    this->pageVists = new int(0);
}

/**
 * Private Function of the statistics class
 *
 * @brief statistics::closeJournal - This function will save to file
 * the "Title, begin time, end time, pagevists" to the jounral.log file
 *
 * WARNING - this function requires location of the stats folder + filename FIRST
 */
void statistics::closeJournal(){

    if(disable_flag){ return; }

    if(this->date_loc == NULL){ return; }

    QFile file(*this->date_loc);

    if(!file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text))
        QMessageBox::information(0, "Error", "Writing in journal");

    QTextStream stream(&file);

    time_t current_time = time(NULL);

    this->logFile = new QString((*this->logFile) + QString(ctime(&current_time)).remove(24, 1) + ", " + QString::number(*this->pageVists));
    stream << (*this->logFile) << endl;
    file.close();
}

/**
 * Public Function of the statistics class
 *
 * @brief statistics::reviewed - If a user highlights a secion
 *      the page, line#, and every 8th x position will be saved.
 *      This is called every time the user clicks "next."
 *
 *      This is not ideal, however it will provide insight to a given
 *      word that is "searched" for example
 *
 * @param pagenum - the page number that the user is currently on
 */
void statistics::reviewed(int pagenum){

    if(disable_flag){ return; }

    if(this->reviewed_loc == NULL){ return; }

    QFile file(*this->reviewed_loc);

    if(!file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text))
        QMessageBox::information(0, "Error", "Writing in journal");

    QTextStream stream(&file);

    for(unsigned long j = 0; j < this->xcursor.size(); j++){
        if(this->xcursor[j].size() > 0){ stream << "\n" << pagenum << ", " << j << ", "; }
        for(unsigned long i = 0; i < this->xcursor[j].size(); i += 8)
            stream << this->xcursor[j][i] << ", ";
        this->xcursor[j].clear();
    }
}
