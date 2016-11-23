#include "operatesql.h"
#include "operatedefine.h"
#include "../security/sha256.h"

using namespace std;

Dbsql::Dbsql()
{

}

Dbsql::~Dbsql()
{
    mysql_close(&my_connection);
}

void Dbsql::CloseConnecion()
{
    mysql_close(&my_connection);
}

bool Dbsql::VerifyPWD(string name, string pwd)
{
    int iRet;
    int iTableRow, iTableCol, i, j;

    vector<string> later;
    NormalSearch(TABLE_ACCOUNT, ACCOUNT_USERNAME, name, later);
    if(later.size() == 0)
    {
        return false;
    }

    string Theselect= "select password, salt from Account where username = '";

    Theselect += name + "'";

    const char *p = Theselect.c_str();

    iRet = mysql_query(&my_connection, p);
    if( iRet )
    {
        //        fprintf(stderr,"select error %d: %s !\n", mysql_errno(&my_connection),mysql_error(&my_connection));//打印错误处理具体信息
        return false;
    }

    vector<string> my;
    res_ptr = mysql_store_result(&my_connection);
    if( res_ptr )
    {
        iTableRow = mysql_num_rows(res_ptr);
        iTableCol = mysql_num_fields(res_ptr);

        for(i = 0; i < iTableRow; i++)
        {
            sqlrow = mysql_fetch_row(res_ptr);
            for(j = 0; j < iTableCol; j++)
            {
                my.push_back(sqlrow[j]);
            }
        }

        mysql_free_result(res_ptr);
    }

    string salt_pwd = my[1];
    salt_pwd += pwd + '\0';

    char ciphertext[65] = {0};
    int num_salt_pwd = salt_pwd.size();
    char *saltPLUSpwd = new char(num_salt_pwd);
    memset(saltPLUSpwd, 0, sizeof(char) * num_salt_pwd);

    for(int i = 0; i < num_salt_pwd; ++i)
    {
        saltPLUSpwd[i] = salt_pwd[i];
    }

    sha_256(ciphertext, saltPLUSpwd);

    string aa = ciphertext;
    if(aa == my[0])
    {
        return true;
    }
    return false;
}

// 修改密码
int Dbsql::ModifyPWD(string name, string pwd)
{
    int iRet;

    char asc[62] = {0};

    for(int i = 0; i < 26; ++i)
    {
        asc[i] = 97 + i;
    }
    for(int i = 26; i < 36; ++i)
    {
        asc[i] = 48 + i - 26;
    }
    for(int i = 36; i < 62; ++i)
    {
        asc[i] = 65 + i - 36;
    }

    string id_pwd = name + pwd;
    int num_id_pwd = id_pwd.size();
    unsigned char *salt = new unsigned char(num_id_pwd + 1);
    memset(salt, 0, sizeof(unsigned char) * num_id_pwd + 1);

    for(int i = 0; i < num_id_pwd; ++i)
    {
        salt[i] = asc[id_pwd[i] % 62];
    }

    string salt_pwd = (char*)salt;
    salt_pwd += pwd + '\0';

    int num_salt_pwd = salt_pwd.size();
    char *str = new char(num_salt_pwd);
    memset(str, 0, num_salt_pwd);

    for(int i = 0; i < num_salt_pwd; ++i)
    {
        str[i] = salt_pwd[i];
    }

    char ciphertext[65] = {0};
    sha_256(ciphertext, str);

    string str_salt = (char*)salt;
    string real_my = ciphertext;

    string Theselect= "update Account set password = ";
    Theselect += "'" + real_my + "' , salt = ";
    Theselect += "'" + str_salt + "' where username = ";
    Theselect += "'" + name + "'";

    const char *p = Theselect.c_str();

    iRet = mysql_query(&my_connection, p);

    delete salt;
    delete str;
    if( iRet )
    {
        return -1;
    }

    return 0;
}


int Dbsql::Connetion(const char *host, const char *user, const char *passwd, const char *db, int uiTimeOut)
{
    int iRet;

    conn_ptr = mysql_init(&my_connection);
    if( !conn_ptr )
    {
        return -1;
    }

    iRet = mysql_options(&my_connection,MYSQL_OPT_CONNECT_TIMEOUT,(const char *)&uiTimeOut);//设置连接超时
    if( iRet )
    {
        return -1;
    }

    conn_ptr = mysql_real_connect(&my_connection, host, user, passwd, db, 0, NULL, 0);//连接数据库
    if( !conn_ptr )
    {
        return -1;
    }
    return 0;
}

int Dbsql::NormalSearch(string table, string condition, string wanna, vector<string> &my)
{
    int iRet;
    int iTableRow, iTableCol, i, j;

    string Theselect= "select * from ";

    Theselect += table + " where " + condition + " = '" + wanna + "'";

    const char *p = Theselect.c_str();

    mysql_query(&my_connection, "SET NAMES utf8");
    iRet = mysql_query(&my_connection, p);
    if( iRet )
    {
//        fprintf(stderr,"select error %d: %s !\n", mysql_errno(&my_connection),mysql_error(&my_connection));//打印错误处理具体信息
        return -1;
    }

    res_ptr = mysql_store_result(&my_connection);
    if( res_ptr )
    {
        iTableRow = mysql_num_rows(res_ptr);
        iTableCol = mysql_num_fields(res_ptr);

        for(i = 0; i < iTableRow; i++)
        {
            sqlrow = mysql_fetch_row(res_ptr);
            for(j = 0; j < iTableCol; j++)
            {
                my.push_back(sqlrow[j]);
            }
        }

        mysql_free_result(res_ptr);
    }
    return 0;
}


int Dbsql::AllSearch(string table, vector<string> &my)
{
    int iRet;
    int iTableRow, iTableCol, i, j;

    string Theselect= "select * from ";
    Theselect += table;

    const char *p = Theselect.c_str();

    mysql_query(&my_connection, "SET NAMES utf8");
    iRet = mysql_query(&my_connection, p);
    if( iRet )
    {
//        fprintf(stderr,"select error %d: %s !\n", mysql_errno(&my_connection),mysql_error(&my_connection));//打印错误处理具体信息
        return -1;
    }

    res_ptr = mysql_store_result(&my_connection);
    if( res_ptr )
    {
        iTableRow = mysql_num_rows(res_ptr);
        iTableCol = mysql_num_fields(res_ptr);

        for(i = 0; i < iTableRow; i++)
        {
            sqlrow = mysql_fetch_row(res_ptr);
            for(j = 0; j < iTableCol; j++)
            {
                my.push_back(sqlrow[j]);
            }
        }

        mysql_free_result(res_ptr);
    }
    return 0;
}

int Dbsql::Delete(string table, string condition, string wanna)
{
    int iRet;

    string Theselect= "delete from ";
    Theselect += table + " where " + condition + " = " + "'" + wanna + "'";

    const char *p = Theselect.c_str();

    iRet = mysql_query(&my_connection, p);
    if( iRet )
    {
//        fprintf(stderr,"select error %d: %s !\n", mysql_errno(&my_connection),mysql_error(&my_connection));//打印错误处理具体信息
        return -1;
    }

    return 0;
}

int Dbsql::Insert(string table, vector<string> &my)
{
    int iRet;
    int num = my.size();

    if(table != TABLE_ACCOUNT)
    {
        string Theselect= "insert into ";
        Theselect += table + " values(";

        Theselect += my[0];
        for(int i = 1; i < num; ++i)
        {
            Theselect += ", '" + my[i] + "'";
        }
        Theselect += ")";

        const char *p = Theselect.c_str();

        iRet = mysql_query(&my_connection, p);
        if( iRet )
        {
            fprintf(stderr,"select error %d: %s !\n", mysql_errno(&my_connection),mysql_error(&my_connection));//打印错误处理具体信息
            return -1;
        }

        return 0;
    }
    else    // 密码字段通过hash算法加salt加密
    {
        char asc[62] = {0};

        for(int i = 0; i < 26; ++i)
        {
            asc[i] = 97 + i;
        }
        for(int i = 26; i < 36; ++i)
        {
            asc[i] = 48 + i - 26;
        }
        for(int i = 36; i < 62; ++i)
        {
            asc[i] = 65 + i - 36;
        }

        string id_pwd = my[1] + my[2];
        int num_id_pwd = id_pwd.size();
        unsigned char *salt = new unsigned char(num_id_pwd + 1);
        memset(salt, 0, sizeof(unsigned char) * num_id_pwd + 1);

        for(int i = 0; i < num_id_pwd; ++i)
        {
            salt[i] = asc[id_pwd[i] % 62];
        }

        string salt_pwd = (char*)salt;
        salt_pwd += my[2] + '\0';

        int num_salt_pwd = salt_pwd.size();
        char *str = new char(num_salt_pwd);
        memset(str, 0, num_salt_pwd);

        for(int i = 0; i < num_salt_pwd; ++i)
        {
            str[i] = salt_pwd[i];
        }

        char ciphertext[65] = {0};
        sha_256(ciphertext, str);

        string str_salt = (char*)salt;
        vector<string > real_my(my);    //real vector string

        real_my.insert(real_my.begin() + 3, str_salt);
        real_my[2] = ciphertext;

        for(unsigned int i = 0; i < real_my.size(); ++i)
        {
//            cout << real_my[i] << " ";
        }

        string Theselect= "insert into ";
        Theselect += table + " values(";

        Theselect += real_my[0];
        for(int i = 1; i < num + 1; ++i)
        {
            Theselect += ", '" + real_my[i] + "'";
        }
        Theselect += ")";

        const char *p = Theselect.c_str();

        iRet = mysql_query(&my_connection, p);
        if( iRet )
        {
            fprintf(stderr,"select error %d: %s !\n", mysql_errno(&my_connection),mysql_error(&my_connection));//打印错误处理具体信息
            return -1;
        }

        return 0;
    }
}

int Dbsql::Alter(string table, string condition, string wanna, vector<string> &my)
{
    int iRet;
    int num = my.size();

    vector<string> AllCols;
    vector<string> OriValue;

    GetColsName(table, AllCols);
    NormalSearch(table, condition, wanna, OriValue);

    string Theselect= "update ";
    Theselect += table + " set ";

    if(!my[0].empty())
    {
        Theselect += AllCols[0] + " = '" + my[0] + "'";
    }
    else
    {
        Theselect += AllCols[0] + " = '" + OriValue[0] + "'";
    }

    for(int i = 1; i < num; ++i)
    {
        if(!my[i].empty())
        {
            Theselect += ", " + AllCols[i] + " = '" + my[i] + "'";
        }
        else
        {
            Theselect += ", " + AllCols[i] + " = '" + OriValue[i] + "'";
        }
    }

    Theselect += " where " + condition + " = " + "'" + wanna + "'";

    const char *p = Theselect.c_str();

    iRet = mysql_query(&my_connection, p);
    if( iRet )
    {
//        fprintf(stderr,"select error %d: %s !\n", mysql_errno(&my_connection),mysql_error(&my_connection));//打印错误处理具体信息
        return -1;
    }

    return 0;
}

int Dbsql::GetColsName(string table, vector<string> &my)
{
    int iRet;
    int iTableRow, iTableCol, i, j;

    string Theselect= "select COLUMN_NAME from information_schema.COLUMNS where table_name = '";
    Theselect += table + "'";

    const char *p = Theselect.c_str();

    iRet = mysql_query(&my_connection, p);
    if( iRet )
    {
//        fprintf(stderr,"select error %d: %s !\n", mysql_errno(&my_connection),mysql_error(&my_connection));//打印错误处理具体信息
        return -1;
    }

    res_ptr = mysql_store_result(&my_connection);
    if( res_ptr )
    {
        iTableRow = mysql_num_rows(res_ptr);
        iTableCol = mysql_num_fields(res_ptr);

        for(i = 0; i < iTableRow; i++)
        {
            sqlrow = mysql_fetch_row(res_ptr);
            for(j = 0; j < iTableCol; j++)
            {
                my.push_back(sqlrow[j]);
            }
        }

        mysql_free_result(res_ptr);
    }
    return 0;
}
