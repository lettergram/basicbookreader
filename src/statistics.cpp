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
statistics::statistics(QString book, int numberOfPages){

    disable_flag = false;

    QDir dir;
    int i = 0;
    while(!dir.cd("stats")){
        dir.cdUp();
        if(i++ == 7){ break; }
    }
    QString location(dir.absolutePath() + "/");

    this->file_loc = new QString(location + book + QString(".stat"));

    this->bookSize = numberOfPages;
    this->pageTimes.resize(numberOfPages);

    for(int i = 0; i < numberOfPages; i++)
        this->pageTimes[i].resize(MAXREAD, 0);

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
}

/**
 * Public Function
 *
 * @brief statistics::startPage
 * @param pagenum - the page that the stats are recording
 * @return index - the locaiton in the row
 */
void statistics::startPage(int pagenum){

    if(disable_flag){ return; }

    clock_t t;
    t = clock();

    for(int i = 0; i < MAXREAD; i++){
        if(this->pageTimes[pagenum][i] == 0){
            this->pageTimes[pagenum][i] = t;
            this->index = i;
            return;
        }
    }
    // Way to handle when someone reads over MAXREAD times on a page
    srand(time(NULL));
    this->index = rand() % MAXREAD;
    this->pageTimes[pagenum][this->index] = t;
}

/**
 * Public Function
 *
 * @brief statistics::endPage
 */
void statistics::endPage(int pagenum){

    if(disable_flag){ return; }

    clock_t t;
    t = clock();

    int diff = t - this->pageTimes[pagenum][this->index];

    // clearly they didn't read it if it was less than a second
    if(diff < 1 || diff < 0){
        this->pageTimes[pagenum][this->index] = 0;
        return;
     }

    this->pageTimes[pagenum][this->index] = (double(diff))/CLOCKS_PER_SEC;
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
 * Private Function
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

            // Leaving zeros as place holders for page numbers
            if(this->pageTimes[i][j] == 0){ break; }
        }
        stream << endl;
    }

    file.close();
}

/**
 * Private Function
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

    for(int page = 0; !stream.atEnd(); page++){

        QString pagestat = stream.readLine();

        QStringList list = pagestat.split(",", QString::SkipEmptyParts);

        for(int i = 0; i < list.count(); i++)
            this->pageTimes[page][i] = list[i].toDouble();
    }
    file.close();
}
