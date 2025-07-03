# v2.2.250703 - 2025-07-03

## 🎉 New Features
- 重构工艺参数界面
- 修改配色及菜单栏按钮
- 增加T8钢轨功能(仿真图)
- 软件更名为
390N/T8离线分析软件
- 更新软件名为390N、T8离线分析软件并启用钢轨焊缝特化接口
- 修改软件图标
- 添加设置功能，打开和关闭软件升级功能
- **AScan:** 添加文件名列表动态启用、禁用功能
- 修改软件关于软件界面的显示, 修改Toast的样式
- **AScan:** 新增A扫类型:R系列
- **config:** 启用仪器通讯功能
- **AScan:** 支持 T8 钢轨特化版本
- **resource:** 添加文件对话框缓存功能
- **SerialRunner:** 修改串口列表显示格式
- A扫连续数据上一帧、下一帧按钮增加长按功能
- **AScan:** 添加390N/T8的B、C扫功能
- **win32:** 添加生成调试信息和异常捕获功能
- **关联文件:** 实现文件关联打开功能
- **View:** 修改 BScanView 和 CScanView 的颜色表
- **PXUT-T8:** 创建 T8 钢轨特殊版本
- **AScan:** 添加工件厚度设置功能
- **设置:** 添加 Mdat 设置功能
- **AScan:** 新增波门重绘功能
- **BScanView:** 实现 B 扫描视图的测量线功能

## 🐛 Bugfixes
- **resource:** 修复工艺参数窗口相关问题
- **resource:** 修复下拉框颜色和启用状态问题
- **resource:** 修复 JsonTable 组件表格展示不完整
- 修复某些PXUT-F4仪器保存的连续图像打开程序崩溃
- **communicate:** 修复如果当前已在A扫界面, 通讯后无法打开
- **ascan:** 播放暂停后，波门数据未使用最新值
- 修复T8摄像头上下翻转、N系列DAC不显示、T8钢轨报表模板使用错误
- R系列无法读取文件名
- **AScan:** 更新钢轨仿真图显示逻辑, 修复连续记录过程中打开仿真图后不显示
- **components:** 修复 B 扫描视图的越界问题
- **components:** 修复 C 扫描和 B 扫描中 A 扫描光标同步问题

## 🚀 Performance Improvements
- **AppUpdater:** 优化 channel 属性的设置逻辑
- **view:** 优化 B 扫描和 C 扫描视图的创建和更新

## 🧱 Build System
- 更新 Gitee action 版本并优化工作流
- 重构项目构建配置和文件结构
- **installer:** 禁用根据运行键决定是否需要重启
- 移除 MSVC 构建的特定标识
- 修改安装程序名称以Setup为中缀
- 添加工作流编译标识
- **cmake:** 更新 morose_copy 函数并修正安装程序生成路径
- 更新配置文件处理逻辑
- **CMakeLists.txt:** 优化 lrelease 和 lupdate 的依赖关系
- **cmake:** 发布脚本增加调试数据库复制
- 移除文件拷贝相关的依赖关系
- **CMakeLists.txt:** 更新项目构建配置
- 更新 CMakeLists.txt 文件

## 🪚 Refactors
- **installer:** 重构安装程序脚本
- **updater:** 重构升级模块中的文件下载逻辑
- **installer:** 屏蔽卸载 vc_redist.x64 库
- **qml:** 重构A扫界面的波门数据显示布局
- **AScan:** 重构 AScan 组件的文件读取逻辑
- **report:** 重构报表生成逻辑
- **TofdPe:** 重构 TOFD_PE 模块
- **AScan:** 优化 AScanInteractor 中的特殊扫描处理逻辑
- **AScan:** 重构 AScanInteractor 中的 CScan 和 BScan 逻辑
- **AScan:** 更新距离模式枚举值
- 重构N系列和R系列DAC、AVG、DGS曲线的绘制
- 重构设置界面和控制UI的布局
- **AScan:** 重构 B 扫描和 C 扫描视图的更新逻辑
- **AScan:** 重构 AScanInteractor 类
- **AScan:** 重构 B/C 扫描视图的生成逻辑
- 更新 union 组件的提交引用
- **AScan:** 重构 B/C 扫描视图显示逻辑
- **src:** 重构 AScanInteractor 类
- **AScan:** 优化 AScanInteractor 中的 gate 相关操作
- **View:** 重构 AxisView 类
- **view:** 优化 BScanView 和 CScanView 的图像处理和颜色映射, 添加图像颜色平滑

## 💅 Code Style Changes
- **AScanIntf:** 修复 AScanInteractor 类的变量命名
- **main:** 优化代码格式和变量命名

## Patchs
- update railweld specialization patch
- **RailWeldSpecial:** 更新 PXUT-390 钢轨焊缝离线分析软件补丁
- 修复钢轨焊缝特化补丁失效问题
- disable upgrade featrue
- **PXUT-T8:** 创建 PXUT-T8 钢轨焊缝离线分析软件

## Refector
- **updater:** 重构软件更新功能

## Broken
- 修改软件名称为通用离线分析软件

## Beta
- 移除B、C扫滚动条, 直接缩放

## Fix-patch
- **AScan:** 滤除错误的编码器数据 x 轴数值


# v2.0.240830 - 2024-09-24

## 🎉 New Features
- [CMP000]可使用超过3条的DAC曲线，最大6条、增加B门类型显示

## 🐛 Bugfixes
- 修复U盘拔插后FolderListModel的内容未正常刷新，导致不能打开U盘中的新增文件
- 无网络情况下检查更新导致软件启动缓慢
- **390N:** 打开含有AVG曲线的数据崩溃
- 保存的报表缺少A扫图像


# v2.0.240712 - 2024-07-12

## 🎉 New Features
- 为优化软件在高DPI显示器下的UI增加控件隐藏和展开listview和ctrl界面
- 安装包安装时不再检查文件的时间戳
- A扫增加.mdat格式读取接口
- 增加TOFD/PE的帮助
- 限制A扫图像播放时波门数据的刷新频率

## 🐛 Bugfixes
- 校准后撤销不正常
- tofd/pe直通波校准错误
- 打包生成的目录名称未强制设置
- 波们参数在高DPI模式下占据屏幕的高度过大
- 修复波门结果显示UI存在的问题
- 修复打开AScan文件失败后调整软件增益触发异常
- 钢轨焊缝示意图在切换文件后未刷新
- release 缺少Config.json

## 🧱 Build System
- 移除子模块NumCpp
- morose_add_subdirectory_path会搜索压缩文件并解压缩(减少预编译库的大小)
- 添加newtwork modules
- 修复clean无法删除目录
- 增加用于软件升级的安装包发布流程
- 修改发布流程

## Patchs
- update railweld specialization patch
- 更新钢轨焊缝特化补丁


# v2.0.240604 - 2024-06-04

## 🎉 New Features
- Loader使用异步方式加载

## 🐛 Bugfixes
- 修复config file 修改后不会重新拷贝
- **TOFD/PE:** 设置间距后A扫图像未设置正确的偏移
- **ASCAN:** 当子文件列表过长时无法滚动
- 使用打开按钮打开同类型的文件时会失败
- TOFD A扫图像正版波被削减80%
- 390N AVG曲线和DAC曲线的问题


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



