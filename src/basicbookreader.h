#ifndef BASICBOOKREADER_H
#define BASICBOOKREADER_H

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

    void on_textBrowser_selectionChanged();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void keyPressEvent( QKeyEvent *k );

private:
    Ui::BasicBookReader *ui;
    void loadpage();
    void loadNewBook();

};

#endif // BASICBOOKREADER_H
