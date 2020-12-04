#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init_ui();
    init_event_bind();
    clear_all();
}

void MainWindow::init_ui()
{
    this->setWindowTitle("算符优先文法");
    ui->splitter->setStretchFactor(0,49);
    ui->splitter->setStretchFactor(1,78);
    this->setStyleSheet("QMainWindow#MainWindow,QMessageBox{background-image:url(:/img/imgs/bg.jpg)}");

    ui->table_firstvt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_firstvt->horizontalHeader()->setDefaultSectionSize(60);
    ui->table_firstvt->verticalHeader()->setDefaultSectionSize(60);
    ui->table_firstvt->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    ui->table_firstvt->verticalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::AlignHCenter);

    ui->table_lastvt->horizontalHeader()->setDefaultSectionSize(60);
    ui->table_lastvt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_lastvt->verticalHeader()->setDefaultSectionSize(60);
    ui->table_lastvt->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    ui->table_lastvt->verticalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::AlignHCenter);


    ui->table_precedence->horizontalHeader()->setDefaultSectionSize(60);
    ui->table_precedence->verticalHeader()->setDefaultSectionSize(60);
    ui->table_precedence->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    ui->table_precedence->verticalHeader()->setDefaultAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    ui->table_precedence->setEditTriggers(QAbstractItemView::NoEditTriggers);


    ui->table_process->verticalHeader()->setDefaultSectionSize(40);
    ui->table_process->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->table_process->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_process->verticalHeader()->setVisible(false);
}

void MainWindow::init_event_bind()
{
    connect(ui->btnStart, &QPushButton::released, this, &MainWindow::slot_btn_start_released);
    connect(ui->btnParse, &QPushButton::released, this, &MainWindow::slot_btn_parse_released);

}

void MainWindow::clear_all()
{
    ui->table_firstvt->setRowCount(0);
    ui->table_firstvt->setColumnCount(0);


    ui->table_lastvt->setRowCount(0);
    ui->table_lastvt->setColumnCount(0);

    ui->table_precedence->setRowCount(0);
    ui->table_precedence->setColumnCount(0);

    ui->table_process->setRowCount(0);
    ui->table_process->setColumnCount(0);

}

void MainWindow::show_firstvt(INFO_BLOCK &info_block)
{
    int i,j;
    QTableWidgetItem *item;
    ui->table_firstvt->setRowCount(info_block.vnNum);
    ui->table_firstvt->setColumnCount(info_block.vtNum);
    // 显示horizontal header
    QStringList strL1;
    for(i = 0; i < info_block.vtNum; i++)
        strL1.append(QString(info_block.VT[i]));
    //qDebug() << strL1;
    ui->table_firstvt->setHorizontalHeaderLabels(strL1);

    // 显示vertical header
    QStringList strL2;
    for(i = 0; i < info_block.vnNum; i++)
        strL2.append(QString(info_block.VN[i]));
    //qDebug() << strL2;
    ui->table_firstvt->setVerticalHeaderLabels(strL2);

    // 显示每个item
    for(i = 0; i < info_block.vnNum; i++)
    {
        for(j = 0; j < info_block.vtNum; j++)
        {
            item = new QTableWidgetItem(QString("%1").arg(info_block.firstvt[i][j]));
            item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->table_firstvt->setItem(i, j, item);
        }
    }

    ui->table_firstvt->horizontalHeader()->setFixedHeight(40);
    ui->table_firstvt->verticalHeader()->setFixedWidth(40);
}

void MainWindow::show_lastvt(INFO_BLOCK &info_block)
{
    int i,j;
    QTableWidgetItem *item;
    ui->table_lastvt->setRowCount(info_block.vnNum);
    ui->table_lastvt->setColumnCount(info_block.vtNum);
    // 显示horizontal header
    QStringList strL1;
    for(i = 0; i < info_block.vtNum; i++)
        strL1.append(QString(info_block.VT[i]));
    //qDebug() << strL1;
    ui->table_lastvt->setHorizontalHeaderLabels(strL1);

    // 显示vertical header
    QStringList strL2;
    for(i = 0; i < info_block.vnNum; i++)
        strL2.append(QString(info_block.VN[i]));
    //qDebug() << strL2;
    ui->table_lastvt->setVerticalHeaderLabels(strL2);

    // 显示每个item
    for(i = 0; i < info_block.vnNum; i++)
    {
        for(j = 0; j < info_block.vtNum; j++)
        {
            item = new QTableWidgetItem(QString("%1").arg(info_block.lastvt[i][j]));
            item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->table_lastvt->setItem(i, j, item);
        }
    }

    ui->table_lastvt->horizontalHeader()->setFixedHeight(40);
    ui->table_lastvt->verticalHeader()->setFixedWidth(40);
}

void MainWindow::show_precedenceTable(INFO_BLOCK &info_block)
{
    int i,j;
    QTableWidgetItem *item;
    ui->table_precedence->setRowCount(info_block.vtNum+1);
    ui->table_precedence->setColumnCount(info_block.vtNum+1);
    // 显示horizontal header
    QStringList strL;
    for(i = 0; i < info_block.vtNum; i++)
        strL.append(QString(info_block.VT[i]));
    strL.append("#");
    //qDebug() << strL1;
    ui->table_precedence->setHorizontalHeaderLabels(strL);

    // 显示vertical header
    ui->table_precedence->setVerticalHeaderLabels(strL);

    // 显示每个item
    for(i = 0; i <= info_block.vtNum; i++)
    {
        for(j = 0; j <= info_block.vtNum; j++)
        {
            if(info_block.precedenceTable[i][j] == ' ') continue;
            item = new QTableWidgetItem(QString("%1").arg(info_block.precedenceTable[i][j]));
            item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->table_precedence->setItem(i, j, item);
        }
    }

    ui->table_precedence->horizontalHeader()->setFixedHeight(40);
    ui->table_precedence->verticalHeader()->setFixedWidth(40);
}

void MainWindow::show_process(QList<QStringList> &process_info)
{
    int i,j;
    QTableWidgetItem *item;

    ui->table_process->setRowCount(process_info.size()-1);
    ui->table_process->setColumnCount(process_info[0].size());
    ui->table_process->setHorizontalHeaderLabels(process_info[0]);
    // 显示每个item
    for(i = 0; i < process_info.size()-1; i++)
    {
        for(j = 0; j < process_info[0].size(); j++)
        {
           item = new QTableWidgetItem(QString("%1").arg(process_info[i+1][j]));
           if(j == 0) //居中
               item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
           if(j == 1 || j ==4) //居左
               item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
           if(j == 2 || j ==3) //居右
               item->setTextAlignment(Qt::AlignRight |Qt::AlignVCenter);

           ui->table_process->setItem(i, j, item);
        }
    }
    ui->table_process->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_process->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

void MainWindow::slot_btn_start_released()
{
    this->clear_all();

    bool b = opp.start(ui->textEdit->toPlainText().split("\n"));
    if(!b)
    {
        QMessageBox::warning(this, "ERROR", QString("%1").arg(opp.get_error_msg()), QMessageBox::Ok);
        return;
    }

    INFO_BLOCK info_block;
    b = opp.get_result_info(&info_block);
    if(!b)
    {
        QMessageBox::warning(this, "ERROR", QString("%1").arg(opp.get_error_msg()), QMessageBox::Ok);
        return;
    }
    this->show_firstvt(info_block);
    this->show_lastvt(info_block);
    this->show_precedenceTable(info_block);

//    int i;
//    for(i = 0; i < info_block.vnNum; i++)
//    {
//      printf("%c ", info_block.VN[i].toLatin1());
//    }
//    printf("\n");
//    for(i = 0; i < info_block.vtNum; i++)
//    {
//        printf("%c ", info_block.VT[i].toLatin1());
//    }
//    printf("\n");

//    for(i = 0; i < info_block.vnNum; i++)
//    {
//        for(int j = 0; j < info_block.vtNum; j++)
//        {
//            printf("%d ", info_block.firstvt[i][j]);
//        }
//        printf("\n");
//    }
//    printf("\n");
//    for(i = 0; i < info_block.vnNum; i++)
//    {
//        for(int j = 0; j < info_block.vtNum; j++)
//        {
//            printf("%d ", info_block.lastvt[i][j]);
//        }
//        printf("\n");
    //    }
}

void MainWindow::slot_btn_parse_released()
{
    QList<QStringList> process_info;
    bool b = opp.get_process_info(&process_info,ui->lineEdit->text());
    if(!b)
    {
        ui->table_process->setRowCount(0);
        ui->table_process->setColumnCount(0);
        QMessageBox::warning(this, "ERROR", QString("%1").arg(opp.get_error_msg()), QMessageBox::Ok);
        return;
    }
    this->show_process(process_info);

}

MainWindow::~MainWindow()
{
    delete ui;
}

