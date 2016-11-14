#ifndef COMMANDS_H
#define COMMANDS_H


// 命令类型定义
enum commandType {
    cmd_default,    // 缺省命令
    cmd_signIn,     // 登陆请求
    cmd_signUp,     // 注册请求
    cmd_preLoad,    // 预加载地图信息
    cmd_getTile,    //获取切片
    cmd_pic,        // 传输图片
};

#define SEND_ERROR 0x101
#define RECV_ERROR 0x102
#define LOGIN_YES 0x103
#define LOGIN_NO 0x104
#define NO_FILE 0x105

#endif // COMMANDS_H
