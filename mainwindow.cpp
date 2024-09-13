#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    connect(this, &MainWindow::startThreadFunc, &serv, &myServer::threadFunc);
    serv.moveToThread(&_thread);
    _thread.start();
    emit startThreadFunc();

    ui->setupUi(this);
    ui->statusbar->showMessage(serv.getIP());

}

MainWindow::~MainWindow()
{
    _thread.terminate();
    _thread.wait();
    delete ui;
}

void MainWindow::on_ShowMsg_clicked()
{
    QDialog dial(this);
    dial.setModal(true);
    auto l = new QVBoxLayout();
    dial.setLayout(l);
    auto userListWgt = new QListWidget(&dial);
    l->addWidget(userListWgt);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dial);
    l->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, &dial, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dial, &QDialog::reject);
    auto userList = serv.showAllUsers();
    for(auto user : userList){
        userListWgt->addItem(user);
    }
    userListWgt->setCurrentRow(0);
    auto result = dial.exec();
    if(result == QDialog::Accepted && userListWgt->currentItem()){
        QString dialog;
        auto msgList = serv.showUserMessages(userListWgt->currentItem()->text());
        for(const auto &msg : msgList){
            dialog.append(msg + "\n");
        }
        if(ui->MsgBrowser->toPlainText() != dialog){
            ui->MsgBrowser->setText(dialog);
        }
    }
}


void MainWindow::on_BanUser_clicked()
{
    QString cmd = "SELECT login FROM users WHERE status = 0";
    QDialog dial(this);
    dial.setModal(true);
    auto l = new QVBoxLayout();
    dial.setLayout(l);
    auto userListWgt = new QListWidget(&dial);
    l->addWidget(userListWgt);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dial);
    l->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, &dial, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dial, &QDialog::reject);
    auto userList = serv.showUsersStatus(cmd);
    for(auto user : userList){
        userListWgt->addItem(user);
    }
    userListWgt->setCurrentRow(0);
    auto result = dial.exec();
    if(result == QDialog::Accepted && userListWgt->currentItem()){
        cmd = "UPDATE users SET status = 1 WHERE login = '" + userListWgt->currentItem()->text() + "'";
        serv.banOrUnban(cmd);
        QString text = "User: <" + userListWgt->currentItem()->text() + "> is banned.";
        ui->MsgBrowser->setText(text);
    }
}


void MainWindow::on_UnbanUser_clicked()
{
    QString cmd = "SELECT login FROM users WHERE status = 1";
    QDialog dial(this);
    dial.setModal(true);
    auto l = new QVBoxLayout();
    dial.setLayout(l);
    auto userListWgt = new QListWidget(&dial);
    l->addWidget(userListWgt);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dial);
    l->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, &dial, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dial, &QDialog::reject);
    auto userList = serv.showUsersStatus(cmd);
    for(auto user : userList){
        userListWgt->addItem(user);
    }
    userListWgt->setCurrentRow(0);
    auto result = dial.exec();
    if(result == QDialog::Accepted && userListWgt->currentItem()){
        cmd = "UPDATE users SET status = 0 WHERE login = '" + userListWgt->currentItem()->text() + "'";
        serv.banOrUnban(cmd);
        QString text = "User: <" + userListWgt->currentItem()->text() + "> is unbanned.";
        ui->MsgBrowser->setText(text);
    }
}

