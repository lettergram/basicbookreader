#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_saveBookButton_clicked();

    void on_lineEdit_page_textEdited(const QString &arg1);

    void on_toolButton_clicked();

    void on_disable_stats_button_clicked();

    void on_enable_stats_button_clicked();

private:
    Ui::MainWindow *ui;
    void loadpage();
    void loadNewBook();

};

#endif // MAINWINDOW_H
