# v2.0.240523 - 2024-05-23

## 🎉 New Features
- 添加串口读取330A扫数据
- **tofd/pe:** 添加垂直线、水平线联动功能
- 连续图像新增播放所有文件的选项
- 新增对390N/T8摄像头文件(.VDATA)的支持
- Ascan ControlUI中的部分控件文件更改为子文件,避免歧义
- 报表中也附带摄像头图像
- 添加钢轨焊缝特化版本补丁
- **TOFD/PE:** 由于鼠标右键双击和单击打开popup冲突, 快速移动线条修改为Ctrl+鼠标
- 支持.tof格式
- **TOFD/PE:** 完善调校功能

## 🐛 Bugfixes
- 330连续图谱切换文件时, 数据指针未能正确设置为0
- tofd/peA扫图像在drawAble区域变化后，背景大小不跟随变化
- 通过文件列表打开TOFD/PE图像时，D扫数据选择线条和指针未重新初始化
- 打开错误的DAT文件有可能崩溃
- **tofd/pe:** 撤销功能未刷新D扫界面
- 底部状态栏颜色显示不对
- getCameraImage可能存在的空指针异常
- 330 DAC/AVG曲线不显示
- 主界面ListView内元素超出范围
- 390N/T8波门数据显示不正确
- **TOFD/PE:** 重新读取文件时TOFD/PE间距未置为0


# v2.0.240419 - 2024-04-19

## 🎉 New Features
- 合并TOFD/PE的部分功能
- 支持330的A扫文件(.das、.dat)

## 🐛 Bugfixes
- 修复createAScanSeries创建的曲线高度不对
- 修复打开错误的JSON格式文件会崩溃
- ldat格式文件读取不全
- 连续图像中ADC和DAC在消失时未隐藏
- 打开大小为0的ldat文件会崩溃

## 🪚 Refactors
- 使用Qt5.15.2重构, 确保win7平台的支持


# v2.0.240319 - 2024-03-19

## 🐛 Bugfixes
- release 无法正常输出日志
- 修复安装脚本存在的一些问题
- 修复波门参数显示的波高不正确

## 🧱 Build System
- 先提交CHANGELOG再发布归档


# v2.0.0 - 2024-03-08

## 🎉 New Features
- 导出报表添加A扫截图
- 工艺参数添加角度和抑制


# v0.1.0 - 2024-03-07

## 🎉 New Features
- 完善A扫界面、T8和390N的数据读取



