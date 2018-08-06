#ifndef ENTERTHREEDWIDGET_H
#define ENTERTHREEDWIDGET_H

#include <QWidget>
#include <QGLWidget>
#include <QOpenGLWidget>
//http://www.qtcentre.org/threads/52875-Rotate-opengl-view-with-mouse-position
//https://qt.developpez.com/doc/4.7/opengl-hellogl/
//#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QMouseEvent>
#include <GL/glu.h>
#include <QTimer>
#include <QThread>
#include <QCursor>
#include <QWheelEvent>
#include <math.h>
//#include <GL/glfw.h>
//#include "glm.h"
#include <QMatrix4x4>

typedef struct tagVERTEX
{
    float x,y,z;
    float u,v;
}VERTEX;

typedef struct tagTRIANGLE
{
    VERTEX vertexs[3];
}TRIANGLE;

typedef struct tagSECTOR
{
    int numtriangles;
    QVector<TRIANGLE> vTriangle;
}SECTOR;

struct VERTEXO
{
    float x,y,z;
};

struct OBJECT
{
    int verts;
    QVector<VERTEXO> vPoints;
};

class EnterThreeDWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit EnterThreeDWidget(QWidget *parent = 0);
    ~EnterThreeDWidget();


    void DrawSector(SECTOR sec,GLuint a_Texture);
    SECTOR loadObject(QString filename);

    int time_count =0;

    void BatForce();


protected:
    void initializeGL();
    void resizeGL(int w,int h);
    void paintGL();

    void keyPressEvent(QKeyEvent *event);

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
//    void wheelEvent(QWheelEvent *event);

private:
    bool fullscreen;
    QPoint lastPos;
    QCursor cursor;
    //QVector3D transVec = QVector3D(0.0f,0.0f,0.0f);

//    QString m_Filename;
//    GLuint m_Texture;
//    QString m_WorldFile;
//    SECTOR m_Sector;
    QString Sky_Filename;
    GLuint Sky_Texture;
    QString m_SkyFile;
    SECTOR Sky_Sector;


    QString Grass_Filename;
    GLuint Grass_Texture;
    QString m_GrassFile;
    SECTOR Grass_Sector;

//    QString GS_Filename;
//    GLuint GS_Texture;
//    QString m_GSFile;
//    SECTOR GS_Sector;

//    QString Quad_Filename;
//    GLuint Quad_Texture;

    //static
    const float m_PIOVER180 ;
    GLfloat k_xPos;
    GLfloat k_zPos;
    GLfloat m_yPos;
    GLfloat k_LookUpDown;

    //GLfloat m_rquad;
    GLfloat m_xRot;
    GLfloat m_zRot;
    GLfloat k_yRot;

    GLfloat m_xmove;
    GLfloat m_ymove;
    GLfloat m_zmove;

    GLfloat ForceApply;
    GLfloat Force_y;
    GLfloat Force_z;


    GLfloat fricy;
    GLfloat fricz;
    GLfloat ay;
    GLfloat az;
    GLfloat rou;
    GLfloat vy;
    GLfloat vz;
    GLfloat vy0;
    GLfloat vz0;
    GLfloat Fy_total;
    GLfloat Fz_total;
    GLfloat mass;
    GLfloat beta;
    GLfloat  time_c;

    //SECTOR loadObject(QString filename, OBJECT *k);

//    OBJECT m_obj1;
    QMatrix4x4 matrix;

};

#endif // ENTERTHREEDWIDGET_H
