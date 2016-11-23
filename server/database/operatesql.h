#ifndef OPERATESQL_H
#define OPERATESQL_H

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include "mysql/include/mysql.h"

class Dbsql
{
private:
    MYSQL my_connection, *conn_ptr;
    MYSQL_RES *res_ptr;
    MYSQL_ROW sqlrow;

    int GetColsName(std::string table, std::vector<std::string> &my);

public:
    void CloseConnecion();          //关闭连接
    int Connetion(const char *host, const char *user, const char *passwd, const char *db, int uiTimeOut);      //连接数据库

    bool VerifyPWD(std::string name, std::string pwd);  // 验证密码是否正确
    int ModifyPWD(std::string name, std::string pwd);       // 修改密码
    int Insert(std::string table, std::vector<std::string> &my);   //插入
    int Delete(std::string table, std::string condition, std::string wanna);   //删除
    int NormalSearch(std::string table, std::string condition, std::string wanna, std::vector<std::string> &my);   //普通查询
    int AllSearch(std::string table, std::vector<std::string> &my);            //查询全部
    int Alter(std::string table, std::string condition, std::string wanna, std::vector<std::string> &my);      //更改

    Dbsql();
    ~Dbsql();
};

#endif // OPERATESQL_H
