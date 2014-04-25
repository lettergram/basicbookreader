#ifndef STATSVIEWER_H
#define STATSVIEWER_H

#include <QWidget>
#include <QStringList>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPointF>

#include <vector>
#include <utility>

#include "library.h"
#include "statistics.h"

namespace Ui {
class statsviewer;
}

class statsviewer : public QWidget
{
    Q_OBJECT

public:
    explicit statsviewer(QWidget *parent = 0);

    void initUsers(QStringList);
    void initTitle(QStringList);

    ~statsviewer();

private slots:
    void on_titleBox_activated(const QString &arg1);

    void on_datesToggle_clicked();

private:
    std::vector< std::pair<QString, int > > datesRead; // Stores time read and number of pages
    Ui::statsviewer *ui;

    bool dateflag;

    void generateLogGraph();
    QString logParser(QString title);
};

#endif // STATSVIEWER_H
