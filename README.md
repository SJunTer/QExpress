# QExpress
A smart logistics distribution system developed for practice

车行千里
车行千里是一款智能化物流配送系统

支持平台
普华桌面操作系统 V3.0 或更高、Centos 7

编译条件
Qt 5.0 或更高，GCC 4.7 或更高（需支持C++11）
注意：必须在release模式下编译！编译后将maps文件拷贝到release目录下。

使用说明
程序主体可分为4个部分：地图界面、配送界面、货物界面、车辆界面。
用户可以在货物界面和车辆界面对货物或车辆信息进行管理（增删存取）。
用户可以在配送界面选择“添加”，之后通过下拉列表选择配送车辆，单击”选择货物“按钮跳转到货物界
面选择需要配送的货物，点击“地图选点”按钮后自动调转到地图界面，通过右键菜单标记配送点和仓库，
选择“计算路径”菜单后将自动返回配送界面，最后点击”确定配送“，即可完成任务添加，并在地图界面
观察行车信息。

文件说明
./images  包含程序用到的图片文件
./maps  包含显示的地图文件
./mitab  第三方库，用于地图信息的读取
./mapviewer  地图界面类目录
./mapviewer/mapfield.h ./mapviewer/mapfield.cpp  定义图形字段
./mapviewer/mapfile.h ./mapviewer/mapfile.cpp  定义图形信息读取操作
./mapviewer/maplayer.h ./mapviewer/maplayer.cpp  定义图层信息
./mapviewer/mapshape.h ./mapviewer/mapshape.cpp  定义图形类别
./mapviewer/mapview.h ./mapviewer/mapview.cpp  定义地图视图框架
./mapviewer/mapwidget.h ./mapviewer/mapwidget.cpp  定义地图界面
./deliverypath.h ./deliverypath.cpp  定义配送路径信相关信息
./deliverywidget.h ./deliverywidget.cpp  定义配送界面
./des.h ./des.cpp  加密算法类
./inventorywidget.h ./inventorywidget.cpp  定义货物界面
./mainwindow.h ./mainwindow.cpp  定义主窗口类
./nofocusdelegate.h ./nofocusdelegate.cpp  用于消除虚框的辅助类
./optimumPath.h ./optimumPath.cpp  路径算法类
./tabwidget.h ./tabwidget.cpp  自定义标签框架
./truckwidget.h ./truckwidget.cpp  定义货车界面
./main.cpp  主函数入口
./map.qrc  资源文件
