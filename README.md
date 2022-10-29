# renderer 软渲染器
主要使用c++11和win32API，利用一些基本的图形学知识，实现了CPU渲染。代码简单易于阅读。
## 平台
- Windows
## 特性
- 可编程着色器
- 透视矫正
- 背面剔除
- Blinn-Phong着色模型
- Phong shading
- 第一人称可移动相机 (WASDQE,鼠标)
- 矩阵向量数学库 (实现类似Eigen操作)
## 编译
vscode + CMake + GCC(MinGW)
## 参考
- [使用Windows API进行窗口绘图](https://blog.csdn.net/celte/article/details/10243309)
- [tinyrender](https://github.com/ssloy/tinyrenderer/wiki)
- [mini3d](https://github.com/skywind3000/mini3d)
- [SRender](https://github.com/SunXLei/SRender)
- [Dazed](https://github.com/qiutanguu/SoftwareRenderer)
- [games101](http://games-cn.org/intro-graphics/)