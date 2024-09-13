#pragma once
#include <iostream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#if defined(_WIN32)
#define clear_screen() system("cls")
#elif defined(_WIN64)
#define clear_screen() system("cls")
#elif defined(__linux__)
#define clear_screen() system("clear")
#endif

class servDB: public QObject{

    Q_OBJECT

public:
    servDB();
    ~servDB();
    std::string regis(std::string& login,std::string& name,std::string& passw);
    std::string auth(std::string& login,std::string& passw);
    std::string chatw (std::string& userLogin, std::string& friendLogin);
    void addmes (std::string& senderLogin, std::string& getterLogin, std::string& text);
    std::string alluser (std::string& userLogin);
    std::string allchats (std::string& userLogin);
    std::string getUserStatus (std::string& userLogin);


    QVector<QString> showAllUsers();
    QVector<QString> usersStatus(QString& command);
    QVector<QString> showUserMsg(QString& userLog);
    void changeUserStatus(QString& command);



private:
    std::string users = "users";
    std::string messages = "messages";
    std::string delim = "%";
    QSqlDatabase myDB;
    QSqlQuery* query;
};
