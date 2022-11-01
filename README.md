### 地形数据三维可视化 程序说明
关

#### 实习要求

一、地形数据三维可视化（GRID DEM）

1. 数据源
	1. QQ群上传的DEM文件：test.asc文件
	2. 自己下载的一块高分辨率影像数据，这里我用的SPOT5.tif，由ARCGIS转asc与bmp格式导入

2. 技术要求
    1. 基于MFC + OpenGL，编写单视窗文档，初始化OpenGL环境（蓝色底色）；（5分）
    2. 读取并且三维可视化DEM地形，分别采用正射投影和透视投影模式；（10分）
    3. 正射投影和透视投影下，可以中心旋转三维地形，可以拉近拉远地形，可以进行地形高度夸张；（10分）
    4. 自己从网站上下载一块高分辨率影像数据和矢量地图栅格化数据，作为地形的纹理进行渲染。（15分）


#### 代码说明

1. 运行环境说明：
    - 程序编写基于VS2019的MFC单视窗环境，OpenGL环境基于Nupengl库或自行通过glut配置的最新OpenGL，均可正常运行。  

2. 主要代码说明：
    - pch.h/pch.cpp: 预编译文件
    - CMFCDemOpenGLDoc.h/CMFCDemOpenGLDoc.cpp: 文档类，主要负责文件读取
    - CMFCDemOpenGLView.h/CMFCDemOpenGLView.cpp: 视图类，主要负责图像与操作显示
    - ……

3. 实现功能：
    基于OpenGL实现的DEM操作包括如下：
    - 读取并且三维可视化DEM地形
    - 中心旋转三维地形漫游
    - 绕X/Y轴顺时针/逆时针旋转场景
    - 缩小/放大场景（即拉近拉远地形）
    - 增加/减小高度比（即地形高度夸张）
    - 向左/右平移场景
    - 粘贴渲染地形纹理
    - 填充地形模式切换
    - 正射投影和透视投影模式切换显示


#### 程序运行操作说明

1. 配置好OpenGL/Nupengl环境后运行exe或在解决方案中直接运行代码。  

2. 打开运行程序MFC窗口，通过`打开文件`读入test.asc或其他DEM文件，通过菜单中的`DEM操作`下的功能按钮或快捷键实现各类DEM操作功能。  

3. 具体MFC运行截图如效果下：
![screenshot1](.\MFCDemOpenGL\res\screenshot1.png)
![screenshot2](.\MFCDemOpenGL\res\screenshot2.png)