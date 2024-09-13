#include "Chat.h"
Chat::Chat()
{
    comMap["REGIS"] = COMMAND::REGIS;
    comMap["SIGNIN"] = COMMAND::SIGNIN;
    comMap["CHATW"] = COMMAND::CHATW;
    comMap["ALLUSER"] = COMMAND::ALLUSER;
    comMap["ALLCHATS"] = COMMAND::ALLCHATS;
    comMap["ADDMES"] = COMMAND::ADDMES;
    comMap["ISBAN"] = COMMAND::ISBAN;
}

Chat::~Chat()
{
}


Chat::COMMAND Chat::getCommand(std::string& commandFromUser)
{
    std::string cmdFromUSer = parsedPart(commandFromUser);
    return comMap.find(cmdFromUSer)->second;
}

std::string Chat::parsedPart(std::string& stringLine)
{
    std::string parsedPart = stringLine.substr(0,stringLine.find(delim));
    stringLine.erase(0,stringLine.find(delim) + 1);
    return parsedPart;
}

std::string Chat::regis(std::string& commandFromUser)
{
    std::string login = parsedPart(commandFromUser);
    std::string name = parsedPart(commandFromUser);
    std::string tmppasw = parsedPart(commandFromUser);
    return servDB.regis(login,name,tmppasw);
}

std::string Chat::auth(std::string& commandFromUser)
{
    std::string login = parsedPart(commandFromUser);
    std::string tmppasw = parsedPart(commandFromUser);
    return servDB.auth(login, tmppasw);
}

std::string Chat::chatw(std::string& commandFromUser)
{
    std::string userLogin = parsedPart(commandFromUser);
    std::string friendLogin = parsedPart(commandFromUser);
    return servDB.chatw(userLogin,friendLogin);
}

void Chat::addmes(std::string& commandFromUser)
{
    std::string userLogin = parsedPart(commandFromUser);
    std::string friendLogin = parsedPart(commandFromUser);
    std::string text = parsedPart(commandFromUser);
    servDB.addmes(userLogin, friendLogin, text);
    //return servDB.chatw(userLogin,friendLogin);
}

std::string Chat::alluser(std::string& commandFromUser)
{
    std::string tmpLogin = parsedPart(commandFromUser);
    return servDB.alluser(tmpLogin);
}

std::string Chat::allchats(std::string& commandFromUser)
{

    std::string tmpLogin = parsedPart(commandFromUser);
    return servDB.allchats(tmpLogin);
}

std::string Chat::userstatus(std::string &commandFromUser)
{
    std::string tmpLogin = parsedPart(commandFromUser);
    return servDB.getUserStatus(tmpLogin);
}

QVector<QString> Chat::showAllUsers()
{
    return servDB.showAllUsers();
}

QVector<QString> Chat::usersStatus(QString &command)
{
    return servDB.usersStatus(command);
}


QVector<QString> Chat::showUserMessages(QString &userLog)
{
    return servDB.showUserMsg(userLog);
}

void Chat::changeUsrStsatus(QString &command)
{
    servDB.changeUserStatus(command);
}

