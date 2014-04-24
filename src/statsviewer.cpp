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

/**
 * Public funciton of the statsviewer class
 *
 * @brief statsviewer::initTitle - Initalizes
 *      the title comboBox with the titles in the library.
 *      Called upon statsviewer creation.
 * @param list - the list of titles
 */
void statsviewer::initTitle(QStringList list){
    ui->titleBox->addItems(list);
}

/**
 * Private function of the statsviewer class
 *
 * @brief statsviewer::logParser - Parses the journal.log file
 *      gathering all the information on the "title"
 * @param title
 * @return - Returns the first and last access date
 */
QString statsviewer::logParser(QString title){

    QDir dir(QApplication::applicationDirPath());
    for(int i = 0; !dir.cd("stats") && i < 5; i++){ dir.cdUp(); }
    QFile file(QString(dir.absolutePath() + "/") + "journal.log");
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());
    QTextStream log(&file);

    QString check;
    QString date("");
    QString last("");
    QStringList logline;

    while(!log.atEnd()){
        check = log.readLine();
        logline = check.split(",", QString::SkipEmptyParts);
        if(logline[0].compare(title) == 0){
            if(date.compare("") == 0)
                date.append(logline[1]);
            last = logline[2];
        }
    }
    if(date.size() > 5)
        date.append("  to " + last);
    return date;
}

/**
 * Private Slot of the statsviewr class
 *
 * @brief statsviewer::on_titleBox_activated - if the title
 *      of a book is chosen, the book statistics are loaded
 *      into the label as well as any graphics displayed.
 * @param arg1 - the title chosen
 *
 */
void statsviewer::on_titleBox_activated(const QString &arg1){

    QStringList t = arg1.split(".", QString::SkipEmptyParts);
    QString dates = logParser(t[0]);
    QString title = t[0].replace("-", " ", Qt::CaseSensitive);
    const QString s(title + " | " + dates);
    ui->titleDateLabel->setText(s);
}

