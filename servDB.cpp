#include "servDB.h"


servDB::servDB()
{
    myDB = QSqlDatabase::addDatabase("QSQLITE");
    myDB.setDatabaseName("testDB.db");
    myDB.open();
    if(!myDB.isOpen()){
        std::cout << "can't open database";
    }
    else{
        query = new QSqlQuery(myDB);
        query->exec("CREATE TABLE IF NOT EXISTS users(login VARCHAR(25), name VARCHAR(25), password VARCHAR(32), status INT)");
        query->exec("CREATE TABLE IF NOT EXISTS messages(id_sender INT, id_getter INT, time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, text TEXT)");
        delete query;
    }
    myDB.close();
}

servDB::~servDB()
{

}

std::string servDB::regis(std::string &login, std::string &name, std::string &passw)
{
    myDB.open();
    query = new QSqlQuery(myDB);
    QString cmd = "SELECT * FROM " + QString::fromStdString(users) + " WHERE login = '"+QString::fromStdString(login)+"'";
    query->exec(cmd);
    if(query->next()){
        std::string notSucces = "Login is busy, try another one!\n";
        delete query;
        myDB.close();
        return notSucces;
    }
    cmd.clear();
    cmd = "INSERT INTO " + QString::fromStdString(users) + " (login, name, password,status) values('" + QString::fromStdString(login) + "',"
          " '" + QString::fromStdString(name) + "', '" + QString::fromStdString(passw) + "', 0)";
    query->exec(cmd);
    delete query;
    myDB.close();
    std::string succes = "succes";
    return succes;
}



std::string servDB::auth(std::string &login, std::string &passw)
{
    myDB.open();
    query = new QSqlQuery(myDB);
    QString check_user_existanse = "SELECT * FROM " + QString::fromStdString(users) + " WHERE login = '" + QString::fromStdString(login) + "'";
    query->exec(check_user_existanse);
    if(query->next()){
        check_user_existanse = "SELECT * FROM " + QString::fromStdString(users) + " WHERE login = '" + QString::fromStdString(login) + "' "
                               "AND password = '" + QString::fromStdString(passw) + "'";
        query->exec(check_user_existanse);
        if(query->next()){
            std::string succes = "succes";
            delete query;
            myDB.close();
            return succes;
        }
        else{
            std::string wrongPasw = "Incorrect password!\n";
            delete query;
            myDB.close();
            return wrongPasw;
        }
    }
    std::string nonExistUser = "There is no user with this login!\n";
    delete query;
    myDB.close();
    return nonExistUser;
}



std::string servDB::chatw(std::string &userLogin, std::string &friendLogin)
{
    myDB.open();
    query = new QSqlQuery(myDB);
    QString response;
    QString getMessages = "SELECT (select login from users where messages.id_sender = users.rowid) AS senderlog, time, text FROM messages WHERE id_sender = "
                              "(select rowid from users where login = '"+QString::fromStdString(userLogin)+"') "
                              "AND id_getter = (select rowid from users where login = '"+QString::fromStdString(friendLogin)+"')"
                              " OR id_sender = (select rowid from users where login = '"+QString::fromStdString(friendLogin)+"')"
                              " AND id_getter = (select rowid from users where login = '"+QString::fromStdString(userLogin)+"')";
    query->exec(getMessages);
    if(query->first()){
        QSqlRecord rec = query->record();
        int rowCounter = 0;
        rowCounter++;
        response += query->value(rec.indexOf("senderlog")).toString() + QString::fromStdString(delim) + query->value(rec.indexOf("time")).toString() +
                    QString::fromStdString(delim) + query->value(rec.indexOf("text")).toString() + QString::fromStdString(delim);
        while (query->next()) {
            rowCounter++;
            response += query->value(rec.indexOf("senderlog")).toString() + QString::fromStdString(delim) + query->value(rec.indexOf("time")).toString() +
                        QString::fromStdString(delim) + query->value(rec.indexOf("text")).toString() + QString::fromStdString(delim);
        }
        delete query;
        myDB.close();
        return "1%" + std::to_string(rowCounter) + delim + response.toStdString();
    }
    else{
        response += "0%There's no messages with this user\n";
        delete query;
        myDB.close();
        return response.toStdString();
    }
}


void servDB::addmes(std::string &senderLogin, std::string &getterLogin, std::string &text)
{
    myDB.open();
    query = new QSqlQuery(myDB);
    QString addMes = "INSERT INTO messages(id_sender, id_getter, text) values((SELECT rowid FROM " + QString::fromStdString(users) +
                     " WHERE login = '" + QString::fromStdString(senderLogin) + "'), (SELECT rowid FROM users WHERE login = '" +
                     QString::fromStdString(getterLogin) + "'), '" + QString::fromStdString(text) + "')";
    query->exec(addMes);
    delete query;
    myDB.close();
}


std::string servDB::alluser(std::string &userLogin)
{
    myDB.open();
    query = new QSqlQuery(myDB);
    QString response;
    int counter = 0;
    QString getUsers = "SELECT login, name FROM " + QString::fromStdString(users) + " WHERE login <> '" + QString::fromStdString(userLogin) + "'";
    query->exec(getUsers);
    if(query->first()){
        QSqlRecord rec = query->record();
        counter++;
        response += QString::number(counter) + QString::fromStdString(delim) +
                    query->value(rec.indexOf("login")).toString() + QString::fromStdString(delim) +
                    query->value(rec.indexOf("name")).toString() + QString::fromStdString(delim);
        while (query->next()){
            counter++;
            response += QString::number(counter) + QString::fromStdString(delim) +
                        query->value(rec.indexOf("login")).toString() + QString::fromStdString(delim) +
                        query->value(rec.indexOf("name")).toString() + QString::fromStdString(delim);
        }
        delete query;
        myDB.close();
        return std::to_string(counter) + delim + response.toStdString();
    }
    else{
        delete query;
        myDB.close();
        response += QString::number(counter) + QString::fromStdString(delim) + "You are the only user\n";
        return response.toStdString();
    }
}


std::string servDB::allchats(std::string &userLogin)
{
    myDB.open();
    query = new QSqlQuery(myDB);
    int counter = 0;
    QString response;
    QString getChats = "SELECT (select login from users where users.rowid=gigatable.friendid) AS friendlog, (select name from users where users.rowid=gigatable.friendid)"
                           " AS friendname FROM (select *, IIF((SELECT users.rowid FROM users WHERE users.login = '" + QString::fromStdString(userLogin) + "')=messages.id_sender,"
                            "messages.id_getter, messages.id_sender) AS friendid from users join messages on users.rowid = messages.id_sender) AS gigatable WHERE gigatable.id_sender = "
                           "(SELECT users.rowid FROM users WHERE users.login = '" + QString::fromStdString(userLogin) + "') OR gigatable.id_getter = (SELECT users.rowid FROM users "
                           "WHERE users.login = '" + QString::fromStdString(userLogin) + "') GROUP BY friendlog, friendname";
    query->exec(getChats);
    if(query->first()){
        QSqlRecord rec = query->record();
        response += QString::number(++counter) + QString::fromStdString(delim) +
                    query->value(rec.indexOf("friendlog")).toString() + QString::fromStdString(delim) +
                    query->value(rec.indexOf("friendname")).toString() + QString::fromStdString(delim);
        while (query->next()) {
            response += QString::number(++counter) + QString::fromStdString(delim) +
                        query->value(rec.indexOf("friendlog")).toString() + QString::fromStdString(delim) +
                        query->value(rec.indexOf("friendname")).toString() + QString::fromStdString(delim);
        }
        delete query;
        myDB.close();
        return std::to_string(counter) + delim + response.toStdString();
    }
    else{
        delete query;
        myDB.close();
        response += std::to_string(counter) + delim + "There are no dialogues yet\n";
        return response.toStdString();
    }
}

std::string servDB::getUserStatus(std::string &userLogin)
{
    myDB.open();
    query = new QSqlQuery(myDB);
    QString getStatus = "SELECT status FROM users WHERE login = '" + QString::fromStdString(userLogin) + "'";
    query->exec(getStatus);
    if(query->first()){
        QString response;
        QSqlRecord rec = query->record();
        response += query->value(rec.indexOf("status")).toString();
        return response.toStdString();
        delete query;
        myDB.close();
    }
    delete query;
    myDB.close();
}

QVector<QString> servDB::showAllUsers()
{
    myDB.open();
    query = new QSqlQuery(myDB);
    QVector<QString> vctr;
    QString cmd = "SELECT login FROM users";
    query->exec(cmd);
    if(query->first()){
        QSqlRecord rec = query->record();
        vctr.push_back(query->value(rec.indexOf("login")).toString());
        while(query->next()){
            vctr.push_back(query->value(rec.indexOf("login")).toString());
        }
        delete query;
        myDB.close();
        return vctr;
    }
    delete query;
    myDB.close();
    return vctr;
}

QVector<QString> servDB::usersStatus(QString &command)
{
    myDB.open();
    query = new QSqlQuery(myDB);
    QVector<QString> vctr;
    query->exec(command);
    if(query->first()){
        QSqlRecord rec = query->record();
        vctr.push_back(query->value(rec.indexOf("login")).toString());
        while(query->next()){
            vctr.push_back(query->value(rec.indexOf("login")).toString());
        }
        delete query;
        myDB.close();
        return vctr;
    }
    delete query;
    myDB.close();
    return vctr;
}

QVector<QString> servDB::showUserMsg(QString &userLog)
{
    myDB.open();
    query = new QSqlQuery(myDB);
    QVector<QString> vctr;
    QString getUserMsg = "SELECT (select login from users where messages.id_sender = users.rowid) AS senderlog,"
                         "(select login from users where messages.id_getter = users.rowid) AS getterlog,"
                         " time, text FROM messages WHERE id_sender = (select rowid from users where login = '"+userLog+"')"
                         "OR id_getter = (select rowid from users where login = '"+userLog+"')";
    query->exec(getUserMsg);
    if(query->first()){
        QSqlRecord rec = query->record();
        vctr.push_back("from: " + query->value(rec.indexOf("senderlog")).toString() + " to: " + query->value(rec.indexOf("getterlog")).toString() +
                       " | " + query->value(rec.indexOf("time")).toString() + " | " + query->value(rec.indexOf("text")).toString());
        while(query->next()){
            vctr.push_back("from: " + query->value(rec.indexOf("senderlog")).toString() + " to: " + query->value(rec.indexOf("getterlog")).toString() +
                           " | " + query->value(rec.indexOf("time")).toString() + " | " + query->value(rec.indexOf("text")).toString());
        }
        delete query;
        myDB.close();
        return vctr;
    }
    delete query;
    myDB.close();
    return vctr;
}

void servDB::changeUserStatus(QString &command)
{
    myDB.open();
    query = new QSqlQuery(myDB);
    query->exec(command);
    delete query;
    myDB.close();
}
