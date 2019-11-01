//
//  main.cpp
//  LearnOpenGL
//
//  Created by changbo on 2019/10/12.
//  Copyright © 2019 CB. All rights reserved.
//

#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"
#include "GLBatch.h"
#include "StopWatch.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


// 视景体-投影矩阵通过它来设置
GLFrustum           viewFrustum;

// 固定着色管理器
GLShaderManager     shaderManager;

// 三角形批次类-绘制图形-大球自转
GLTriangleBatch     sphereBatch;

//窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素
void ChangeSize(int w,int h)
{
    if (h==0) {
        h = 1; // 防止为0
    }
    
    // 窗口尺寸
    glViewport(0,0, w, h);
    
    // 设置透视投影
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 1000.0f);
}

//为程序作一次性的设置
void SetupRC()
{
    // 设置背影颜色
    glClearColor(0.67f,1.0f,0.83f,1.0f);
    
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 初始化管理器
    shaderManager.InitializeStockShaders();
    
    // 球体
    gltMakeSphere(sphereBatch, 0.4f, 10, 20);
    
    // 正背面剔除，填充方式，线
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


//开始渲染
void RenderScene(void)
{
    // 建立基于时间变化的动画
    static CStopWatch rotTimer;
    
    // 当前时间 * 60s
    float yRot = rotTimer.GetElapsedSeconds() * 60; //yRot根据经过时间设置动画帧率
    
    // 清除缓冲区
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    // 矩阵变量
    M3DMatrix44f mTranslate, mRotate, mModelView, mModelViewProjection;
    
    // 往屏幕内(z轴方向)移动2.5个单位长度
    m3dTranslationMatrix44(mTranslate, 0.0f, 0.0f, -2.5f);
    
    // 做动画-绕着y轴旋转yRot度
    m3dRotationMatrix44(mRotate, m3dDegToRad(yRot), 0.0f, 1.0f, 0.0f);
    
    // 矩阵相乘（将平移和旋转结合矩阵相乘后放置模型视图矩阵中）
    m3dMatrixMultiply44(mModelView, mTranslate, mRotate);
    
    // 投影矩阵 * 模型视图矩阵 ，将变化结果通过矩阵乘法应用到mModelViewProjection矩阵上
    m3dMatrixMultiply44(mModelViewProjection, viewFrustum.GetProjectionMatrix(), mModelView);
    
    // 设置颜色
    GLfloat vBlack[] = {0.0f, 0.0f, 0.0f, 1.0f};
    // 渲染几何图形
    shaderManager.UseStockShader(GLT_SHADER_FLAT,mModelViewProjection, vBlack);
    
    sphereBatch.Draw();
    
    // 将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
    // 刷新
    glutPostRedisplay();
}



int main(int argc,char* argv[])
{
    //设置当前工作目录，针对MAC OS X
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库
    glutInit(&argc, argv);
    
    /*初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区*/
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    //GLUT窗口大小，标题窗口
    glutInitWindowSize(800,500);
    
    glutCreateWindow("Triangle");
    
    //注册回调函数
    glutReshapeFunc(ChangeSize);
    
    glutDisplayFunc(RenderScene);
    
    //驱动程序的初始化中没有出现任何问题。
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        return 1;
    }
    
    //调用SetupRC
    SetupRC();
    
    glutMainLoop();
    
    return 0;
}
