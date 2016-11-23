#ifndef COMMANDS_H
#define COMMANDS_H


// 命令类型定义
enum commandType {
    cmd_default,    // 缺省命令

    cmd_signIn,     // 登陆请求
    cmd_signUp,     // 注册请求
    cmd_getUserInfo,        // 获得用户信息
    cmd_changeUserInfo, // 更改信息
    cmd_changePwd,  // 修改密码

    cmd_preLoad,      // 预加载地图信息
    cmd_getTile,        //获取切片
    cmd_getSymbol,  // 获取标记

    cmd_getTask,        // 客户端获取任务
    cmd_acceptTask,   // 客户端接受任务
    cmd_posChange,       // 客户端发送当前位置
    cmd_taskFinish,     // 客户端完成任务
    cmd_taskFail,       // 任务失败

    cmd_submitRoad,     //客户端提交路况


    cmd_closeClient,    // 服务器切断连接
};

#define SEND_ERROR 0x101    //接受数据错误
#define RECV_ERROR 0x102    //发送数据错误
#define LOGIN_YES 0x103     // 登陆成功
#define LOGIN_NO 0x104      // 登录失败
#define LOGIN_OL 0x105      // 帐号已登录
#define USR_REPEAT  0x106       // 用户名重复
#define REG_OK    0x107            // 注册成功
#define NO_FILE 0x108       // 文件错误
#define NO_TASK 0x109      // 当前无任务
#define PWD_RIGHT 0x110     // 密码正确
#define PWD_WRONG   0x111   // 密码错误

#endif // COMMANDS_H
