#ifndef COMMANDS_H
#define COMMANDS_H


// 命令类型定义
enum commandType {
    cmd_default,    // 缺省命令
    cmd_signIn,     // 登陆请求
    cmd_signUp,     // 注册请求
    cmd_preLoad,    // 预加载地图信息
    cmd_getTile,    //获取切片
    cmd_closeClient,    // 服务器切断连接
};

#define SEND_ERROR 0x101    //接受数据错误
#define RECV_ERROR 0x102    //发送数据错误
#define LOGIN_YES 0x103     // 登陆成功
#define LOGIN_NO 0x104      // 登录失败
#define REG_YES 0x105       // 注册成功
#define REG_NO 0x106        //注册失败
#define NO_FILE 0x107       // 文件错误

#endif // COMMANDS_H
