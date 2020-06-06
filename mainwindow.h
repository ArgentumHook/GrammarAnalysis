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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_getFirstSet_clicked();

    void on_getFollowSet_clicked();

    void on_enterInput_clicked();

    void on_getLLTable_clicked();

    void on_getLRTable_clicked();

    void on_getSLRTable_clicked();

private:
    Ui::MainWindow *ui;
    QString rawInput;//获取输入文本
    QStringList exprSet;//输入的产生式集合
};

#endif // MAINWINDOW_H
