#ifndef STATISTICS_H
#define STATISTICS_H

#include <Qfile>
#include <QTextStream>
#include <QString>
#include <QMessageBox>
#include <QDir>
#include <QApplication>

#include <iostream>
#include <vector>
#include <sys/time.h>

class statistics{

public:

/*=== Public Statistic Functions ===*/

    statistics(const QString book, int numberOfPages);
    ~statistics();
    void startPage(int pagenum);
    void endPage(int pagenum);
    void disableStats();
    void enableStats();

private:

/*=== Private Statistics Variables ===*/

    QString * file_loc;
    QString * date_loc;
    QString * logFile;

    int * pageVists;
    int bookSize;

    std::vector< std::vector<double> > pageTimes;
    int index;

    bool disable_flag;

/*=== Private Statistics Functions ===*/

    void loadStatsDocument();
    void generateStatsDocument();
    void openJournal();
    void closeJournal();

};

#endif // STATISTICS_H
