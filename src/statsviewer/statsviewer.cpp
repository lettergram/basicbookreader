#include "statsviewer.h"
#include "ui_statsviewer.h"

/**
 * Public function of the statsviewer class
 *
 * @brief statsviewer::statsviewer - constructor for the
 *          stats viewer class
 */
statsviewer::statsviewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::statsviewer){

    ui->setupUi(this);

    QDir dir(QApplication::applicationDirPath());
    for(int i = 0; !dir.cd("stats") && i < 5; i++){ dir.cdUp(); }
    this->statspath = QString(dir.absolutePath() + "/");

    this->dateflag = false;
    this->toggleOp1 = QString("View Pages");
    this->toggleOp2 = QString("View Dates");
    this->stretch = 1;

    generateLifeLogGraph();
}

/**
 * Public function of the statsviewer class
 *
 * @brief statsviewer::~statsviewer - Deconstructor for the
 *          statsviewer class
 */
statsviewer::~statsviewer(){

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
 * @brief statsviewer::openfile - helper function which
 *          finds stats folder and returns file handle
 * @param title - title of file to open
 * @return pointer to the file it was called to open
 */
QFile * statsviewer::openfile(QString title){
    QFile * file = new QFile(this->statspath + title);
    if(!file->open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file->errorString());
    return file;
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

    QFile * file = openfile("journal.log");
    QTextStream log( file );

    this->datesRead.clear();
    this->toggleOp1 = QString("View Pages");
    this->toggleOp2 = QString("View Dates");
    ui->timesToggle->setText(this->toggleOp2);

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
    file->close(); delete file;
    ui->statsTypeBox->setCurrentIndex(1);
    return date;
}

/**
 * Private function of the statsviewer class
 *
 * @brief statsviewer::statsParser - Parses .stats file given an input title,
 *      the input title must be in the form <first word>-<second word>-<etc.>.
 *      The vector this->datesRead will be filled with the appropriate values
 * @param title - the title of the .stats to parse
 */
void statsviewer::statsParser(QString title){
    QFile * file = openfile(title + ".stat");
    QTextStream stats( file );

    this->datesRead.clear();

    this->toggleOp1 = QString("Page Times");
    this->toggleOp2 = QString("View Pages");
    ui->timesToggle->setText(this->toggleOp1);

    QStringList times;
    int total = 0;

    while(!stats.atEnd()){
        times = (stats.readLine()).split(",", QString::SkipEmptyParts);
        for(int i = 0; i < times.size(); i++){
            if(i == 0){ total = times[i].toInt(); }
            else{ total += times[i].toInt(); }
        }
        this->datesRead.push_back( std::make_pair(QString("p." + QString::number(this->datesRead.size())), total) );
        total = 0;
    }
    ui->statsTypeBox->setCurrentIndex(2);
    file->close(); delete file;
}

/**
 * Private function of the statsviewer class
 *
 * @brief statsviewer::generateGraph - Graphs the
 *      number of pages read and diplays a chart
 */
void statsviewer::generateGraph(){

    int width = ui->graphicsView->size().width();
    int height = ui->graphicsView->size().height();
    QGraphicsScene * scene = new QGraphicsScene();

    QFont f;
    f.setPointSize(7);
    QPixmap p(6, 6);
    p.fill(Qt::blue);

    // TODO: Add predictive reading chart
    QPainterPath pagepath(QPointF(-width / 2, height/4));
    scene->addPixmap(p)->setPos(-width / 2 - 3, height/4 - 3);

    int hadj = height/4;
    int wadj = -width/2;

    this->datesRead.size();
    for(int i = 0; i < (int)this->datesRead.size(); i++){
        int timestamp = (width/this->datesRead.size()) * (i * stretch);
        int pagesRead = this->datesRead[i].second * 2;
        if(dateflag){
            scene->addText(this->datesRead[i].first, f)->setPos(wadj + timestamp, hadj - pagesRead);
        }else{
            scene->addText(QString::number(this->datesRead[i].second), f)->setPos(wadj + timestamp, hadj - pagesRead);
        }
        pagepath.lineTo(QPointF(wadj + timestamp, hadj - pagesRead));
        (scene->addPixmap(p))->setPos(wadj + timestamp - 3, hadj - pagesRead - 3);
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

    QFile * file = openfile("journal.log");
    QTextStream log( file );

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

    file->close(); delete file;
    generateGraph();
    ui->titleDateLabel->setText(s);
    ui->statsTypeBox->setCurrentIndex(0);
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
    this->bookfile = t[0];

    QString dates = logParser(t[0]);
    QString title = t[0].replace("-", " ", Qt::CaseSensitive);
    const QString s(title + " | " + dates);
    ui->titleDateLabel->setText(s);
    generateGraph();
}

/**
 * Private Slot of the statsviewer class
 *
 * @brief statsviewer::on_datesToggle_clicked -switches between
 *          view dates and number of pages
 */
void statsviewer::on_timesToggle_clicked(){

    this->dateflag ^= true;
    generateGraph();
    if(this->dateflag)
        ui->timesToggle->setText(this->toggleOp1);
    else
        ui->timesToggle->setText(this->toggleOp2);
}

/**
 * Private Slot of the satatsviewer class
 *
 * @brief statsviewer::on_statsTypeBox_activated - selects the type
 *          of statistic to review
 * @param arg1 - the selection on the combobox
 */
void statsviewer::on_statsTypeBox_activated(const QString &arg1){

    if(arg1.compare("Journal", Qt::CaseInsensitive) == 0)
        logParser(this->bookfile);
    else if(arg1.compare("Times Per Page", Qt::CaseInsensitive) == 0)
        statsParser(this->bookfile);
    else
        generateLifeLogGraph();

    this->stretch = this->datesRead.size() >> 5 | 1;
    generateGraph();
}

/**
 * Private Slot of the statsviewer class
 *
 * @brief statsviewer::on_zoomSlider_valueChanged - Allows the user
 *      to zoom in and out of the data
 * @param value - the value/current location of the slidebar.
 */
void statsviewer::on_zoomSlider_valueChanged(int value){
    QMatrix matrix;
    double zoom = 5.01 - (double)((double)value / 20.0);
    matrix.scale(zoom, zoom);
    ui->graphicsView->setMatrix(matrix);
}
