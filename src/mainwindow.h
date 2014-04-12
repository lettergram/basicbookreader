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

    void on_addtoLibButton_clicked();
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_saveBookButton_clicked();
    void on_loadBookButton_clicked();

    void on_lineEdit_page_textEdited(const QString &arg1);
    void on_book_name_textEdited(const QString &arg1);
    void on_file_location_textEdited(const QString &arg1);

    void on_toolButton_clicked();

private:
    Ui::MainWindow *ui;
    void loadpage();
    void loadNewBook();

};

#endif // MAINWINDOW_H
