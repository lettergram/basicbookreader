#include "statsviewer.h"
#include "ui_statsviewer.h"

statsviewer::statsviewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::statsviewer)
{
    ui->setupUi(this);
    this->dateflag = false;
    generateLifeLogGraph();
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
 * @param title - the title of the book to be logged
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
            this->datesRead.push_back(std::make_pair(logline[1], logline[3].toInt()));
        }
    }
    if(date.size() > 5)
        date.append("  to " + last);
    file.close();
    return date;
}

/**
 * Private function of the statsviewer class
 *
 * @brief statsviewer::generateLogGraph - Graphs the
 *      number of pages read and diplays a chart
 */
void statsviewer::generateLogGraph(){
    int width = ui->graphicsView->size().width();
    int height = ui->graphicsView->size().height();
    QGraphicsScene * scene = new QGraphicsScene();

    QFont f;
    f.setPointSize(7);
    QPixmap p(5, 5);
    p.fill(Qt::blue);

    // TODO: Add predictive reading chart
    QPainterPath pagepath(QPointF(-width / 2, height/4));
    scene->addPixmap(p)->setPos(-width / 2, height/4);
    scene->addText(QString("Start"), f)->setPos(-width / 2, height/4);

    int hadj = height/4;
    int wadj = -width/2;

    this->datesRead.size();
    for(int i = 0; i < this->datesRead.size(); i++){
        int timestamp = (width/this->datesRead.size()) * i;
        int pagesRead = this->datesRead[i].second * 2;
        pagepath.lineTo(QPointF(wadj + timestamp, hadj - pagesRead));
        if(dateflag){
            scene->addText(this->datesRead[i].first, f)->setPos(wadj + timestamp, hadj - pagesRead);
        }else{
            scene->addText(QString::number(this->datesRead[i].second), f)->setPos(wadj + timestamp, hadj - pagesRead);
        }
        (scene->addPixmap(p))->setPos(wadj + timestamp, hadj - pagesRead);
    }

    QPen r(Qt::red);
    r.setWidth(2);
    scene->addPath( pagepath, r );
    ui->graphicsView->setScene(scene);
}

/**
 * Private function of the statsviewer class
 *
 * @brief statsviewer::generateLifeLogGraph - Generates a graph
 *      of the total number of pages read for each month.
 */
void statsviewer::generateLifeLogGraph(){

    QDir dir(QApplication::applicationDirPath());
    for(int i = 0; !dir.cd("stats") && i < 5; i++){ dir.cdUp(); }
    QFile file(QString(dir.absolutePath() + "/") + "journal.log");
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());
    QTextStream log(&file);

    QString month("1");
    QString year("1");
    QStringList check;
    QStringList date;
    QString label = ("Pages Read By Month | ");
    int total = 0;

    while(!log.atEnd()){
        check = log.readLine().split(",", QString::SkipEmptyParts);
        date = check[1].split(" ", QString::SkipEmptyParts);

        if(date[1].compare(month) != 0){
            if(month.compare("1") != 0)
                this->datesRead.push_back(std::make_pair(QString(month + " " + year), total));
            else
                label.append(QString(date[1] + " " + date[4]));
            total = 0;
        }
        total += check[3].toInt();
        month = date[1];
        year = date[4];
    }
    this->datesRead.push_back(std::make_pair(QString(month + " " + year), total));

    label.append(QString("  to  " + month + " " + year));
    const QString s(label);

    file.close();
    generateLogGraph();
    ui->titleDateLabel->setText(s);
}

/**
 * Private Slot of the statsviewer class
 *
 * @brief statsviewer::on_titleBox_activated - if the title
 *      of a book is chosen, the book statistics are loaded
 *      into the label as well as any graphics displayed.
 * @param arg1 - the title chosen
 *
 */
void statsviewer::on_titleBox_activated(const QString &arg1){

    this->datesRead.clear();

    QStringList t = arg1.split(".", QString::SkipEmptyParts);
    QString dates = logParser(t[0]);
    QString title = t[0].replace("-", " ", Qt::CaseSensitive);
    const QString s(title + " | " + dates);
    ui->titleDateLabel->setText(s);
    generateLogGraph();
}

/**
 * Privat Slot of the statsviewer class
 *
 * @brief statsviewer::on_datesToggle_clicked -
 */
void statsviewer::on_datesToggle_clicked(){

    this->dateflag ^= true;
    generateLogGraph();
    if(this->dateflag)
        ui->datesToggle->setText(QString("View Pages"));
    else
        ui->datesToggle->setText(QString("View Dates"));
}
