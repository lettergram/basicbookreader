#ifndef STATSVIEWER_H
#define STATSVIEWER_H

#include <QWidget>

namespace Ui {
class statsviewer;
}

class statsviewer : public QWidget
{
    Q_OBJECT

public:
    explicit statsviewer(QWidget *parent = 0);
    ~statsviewer();

private:
    Ui::statsviewer *ui;
};

#endif // STATSVIEWER_H
