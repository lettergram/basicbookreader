#ifndef BASICBOOKREADER_H
#define BASICBOOKREADER_H

#include "statsviewer/statsviewer.h"

#include <QMainWindow>
#include <QKeyEvent>

namespace Ui {
class BasicBookReader;
}

class BasicBookReader : public QMainWindow
{
    Q_OBJECT

public:
    explicit BasicBookReader(QWidget *parent = 0);
    ~BasicBookReader();

private slots:

    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_saveBookButton_clicked();

    void on_lineEdit_page_textEdited(const QString &arg1);
    void on_toolButton_clicked();
    void on_enable_stats_button_clicked();
    void on_pushNoteButton_clicked();

    void on_search_type_currentIndexChanged(const QString &arg1);
    void on_user_rating_currentIndexChanged(int index);
    void keyPressEvent( QKeyEvent *k );

    void on_viewStats_clicked();

    void on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint);

private:

    Ui::BasicBookReader *ui;
    statsviewer * sv;

    int fontsize;
    int start;
    int end;
    int start_search;
    int end_search;

    current_book * book;
    library * lib;
    QString * lib_loc;
    statistics * stats;
    std::vector< QString > highlight;
    QString search;

    void loadpage();
    void loadNewBook();
    int parseImage(int, QString, QGraphicsScene *);

    void saveHighlightedSection();
    void searchWord();
};

#endif // BASICBOOKREADER_H
