#ifndef STATISTICS_H
#define STATISTICS_H

#include <Qfile>
#include <QTextStream>
#include <QString>
#include <QMessageBox>
#include <QDir>

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

    int bookSize;

    std::vector< std::vector<double> > pageTimes;
    int index;

    bool disable_flag;

/*=== Private Statistics Functions ===*/

    void loadStatsDocument();
    void generateStatsDocument();

};

#endif // STATISTICS_H
