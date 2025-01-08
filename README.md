# XClock
![Overview](resource\Overview\IMG_20241222_154540.jpg)
![PCB Main Board1](resource\Overview\PixPin_2024-12-28_16-02-57.jpg)
![PCB Main Board2](resource\Overview\PixPin_2024-12-28_16-05-12.jpg)
![PCB Display Board1](resource\Overview\PixPin_2024-12-28_16-07-08.jpg)
![PCB Display Board2](resource\Overview\PixPin_2024-12-28_16-16-11.jpg)
## Background
目前网上有很多开源的点阵时钟项目，很多厉害的功能，比如可以通过小程序控制。但是大多在实用性上差那么点意思，在我的使用中最重要的一点就是需要电池供电可移动使用，我主要给我儿子写作业或者早上当闹钟使用。并且网上很多项目成品太过巨大，不够小巧。于是我萌生了自己制作一个的想法。我对点阵时钟一直有种莫名的喜爱，刚大学毕业时就打算做一个，那是还没发现有ws2812这种可以串行控制的led，现在各种硬件方案很丰富，这个项目我使用ESP32作为主控，ws2812—2020作为灯珠，能够实现非常多的功能。

## Features
* 时钟，日历，可联网显示天气
* 闹钟，可设置多个
* 秒表
* 计时器
* 音乐律动
* 显示电池电量
* 可根据环境音睡眠唤醒显示
* 可根据环境亮度自动调节显示亮度

## Usage
此时钟有3个操作按键（另外两个小按键为刷写按键），分别为左键，确认键，右键。左右键默认状态下可以切换功能，确认键可以进入每个功能的设置状态，进入设置状态之后，左右键就变为当前功能设置按键有些功能支持左右键长按操作，比如时间的加减键，功能设置状态下双击确认键可以退出设置状态。

## Hardware
硬件开源地址
[立创开源广场-XClock](https://oshwhub.com/polestar_xsx/matrixclock)

## Software
* Develop tool: VS Code + PlatformIO(vs code Extension)
* Toolchain Install：  
  1.Install VS Code  
  2.Install VS Code extension PlatformIO
  3.Git Clone soruce code  
  4.Run PlatformIO:build verify the toolchain
* 软件开发框架基于PlatformIO espressif32 arduino， PlatformIO工程中选择的开发板是esp32doit-devkit-v1。
* 使用的UI库是B站up主：创客尹白猿 开源的Dot2D库，在此表示感谢！
* 代码中使用的图片和声音资源都在resource目录下面
* 所有图片声音资源都是通过Winhex软件导出成C数组编译进ROM
* 软件支持两种图片格式：BMP，GIF。BMP用于静态图片显示，GIF用于实现动画效果。
* GIF图片使用[Pixilart](https://www.pixilart.com/ )网站编辑像素图片，导出后使用Ulead GIF Animator 5软件制作gif文件
* 声音文件使用WAV格式

## Assembly
* 外壳文件有三个，使用FreeCAD软件绘制，使用3D打印，已导出stl文件，所有文件在<resource\3DFiles\外壳>下  
  内腔.stl 前面板.stl 后壳1.stl
* 外壳使用m2x5x5平头螺丝以及配套M2x4注塑热熔铜螺母固定
* 所有连接线都是自己压线制作，连接头是2.0尺寸，当然也可以选择直接焊接不使用接插件连接
* 内置电池为两节18650，18650使用淘宝购买弹片正极端作为电极连接
* 面板淘宝定制的2mm灰色透明亚克力，尺寸38x146。亚克力后面放了一块导光膜，可以用白纸代替但是效果没那么好。
* 原理图中没有3个元件：喇叭(立创商城商品编号C969973)，光敏电阻(小于5mm)，麦克风咪头(直径9.7mm)。可根据外壳尺寸自行采购
  ![组装图1](resource\Overview\IMG_20241222_163108.jpg)

  ![组装图1](resource\Overview\IMG_20241222_163325.jpg)

  ![组装图1](resource\Overview\IMG_20241222_163348.jpg)

  ![组装图1](resource\Overview\IMG_20241222_163357.jpg)

  ![组装图1](resource\Overview\IMG_20241222_163426.jpg)

  ![组装图1](resource\Overview\IMG_20241222_163448.jpg)

  ![组装图1](resource\Overview\IMG_20241222_163651.jpg)

  ![组装图1](resource\Overview\IMG_20241222_163654.jpg)

  ![组装图1](resource\Overview\IMG_20241222_163716.jpg)

  ![组装图1](resource\Overview\IMG_20241222_164114.jpg)