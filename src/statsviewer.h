#ifndef STATSVIEWER_H
#define STATSVIEWER_H

#include <QWidget>
#include <QStringList>
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


private:
    Ui::statsviewer *ui;
    QString logParser(QString title);
};

#endif // STATSVIEWER_H
