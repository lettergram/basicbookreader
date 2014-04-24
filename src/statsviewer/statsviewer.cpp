#include "statsviewer.h"
#include "ui_statsviewer.h"

statsviewer::statsviewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::statsviewer)
{
    ui->setupUi(this);
}

statsviewer::~statsviewer()
{
    delete ui;
}
