#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "opp.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void init_ui();
    void init_event_bind();
    void clear_all();

    void show_firstvt(INFO_BLOCK &info_block);
    void show_lastvt(INFO_BLOCK &info_block);
    void show_precedenceTable(INFO_BLOCK &info_block);
    void show_process(QList<QStringList> &process_info);

    // 槽函数
    void slot_btn_start_released();
    void slot_btn_parse_released();

    ~MainWindow();

private:
    Ui::MainWindow *ui;

    OPP opp;
};
#endif // MAINWINDOW_H
