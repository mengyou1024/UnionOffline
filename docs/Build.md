### 依赖

* `Qt=5.15.x` 
  * `QQuick`
  * `QWidgets`
  * `QCharts`
  * `QCharts`
  * `QSerialPort`
* `InonSetup >= 6`
* `QtCreator`

### 编译并打包

1. 下载归档文件并解压

2. 打开`QtCreator`

3. 点击打开项目

   ![image-20240523113944095](./assets/image-20240523113944095.png)

4. 选择文件中的`CMakeList.txt`文件

   ![image-20240523114109658](./assets/image-20240523114109658.png)

5. 选择`Qt 5.15.2 MSVC2019 64bit`再点击`Configure Project`

   ![image-20240523114249501](./assets/image-20240523114249501.png)

6. 选择`Release`模式

   ![image-20240523114518151](./assets/image-20240523114518151.png)

7. 点击`项目`-`构建`-`构建的步骤`, 勾选`generate_exe_installer`

   ![image-20240523114912764](./assets/image-20240523114912764.png)

8. 点击按钮

   ![image-20240523114949150](./assets/image-20240523114949150.png)

9. 编译成功如下: 

   ![image-20240523115154324](./assets/image-20240523115154324.png)

   生成的安装文件在源码目录中的output目录中

### 特化版本编译

1. `git clone <repo url> --recursive`完整克隆本仓库
2. `git apply patchs/<patch name>.patch`应用补丁
3. 剩余步骤和普通版本类似

> 特化版本无法使用自动化流程

