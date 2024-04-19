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



