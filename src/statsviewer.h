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
#include "graphics_zoom.h"

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
    bool viewportEvent(QEvent *event);

    ~statsviewer();

    QTextStream openFile();
private slots:
    void on_titleBox_activated(const QString &arg1);

    void on_timesToggle_clicked();

    void on_statsTypeBox_activated(const QString &arg1);

private:

    std::vector< std::pair<QString, int > > datesRead; // Stores time read and number of pages
    Ui::statsviewer *ui;

    bool dateflag;
    QString bookfile;
    QString statspath;

    QString toggleOp1;
    QString toggleOp2;

    QFile * openfile(QString title);

    void generateGraph();
    void generateLifeLogGraph();

    void statsParser(QString title);
    QString logParser(QString title);
};

#endif // STATSVIEWER_H
