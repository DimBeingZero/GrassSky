#include "enterthreedwidget.h"
#include<GL/glu.h>
#include<QKeyEvent>
#include<QTimer>
#include<QTextStream>
#include<QtMath>
#include<QDebug>
#include<QFile>
#include<QOpenGLContext>
#include<QtGlobal>
#include <QDateTime>
#include "MeshModel.h"

cMeshModel  *m_sphere;
extern int mWindowWidth;
extern int mWindowHeight;

EnterThreeDWidget::EnterThreeDWidget(QWidget *parent) :
    QGLWidget(parent),m_PIOVER180(0.174532925f)
{
    fullscreen = false;
    Sky_Filename = "C:/Users/h252114/Documents/QT_OpenGL/GrassSky/Sky2.bmp";
    m_SkyFile = "C:/Users/h252114/Documents/QT_OpenGL/GrassSky/Sky.txt";
    Grass_Filename = "C:/Users/h252114/Documents/QT_OpenGL/GrassSky/Grass2.bmp";
    m_GrassFile = "C:/Users/h252114/Documents/QT_OpenGL/GrassSky/Grass.txt";
    //m_WorldFile = "C:/Users/h252114/Documents/QT_OpenGL/GrassSky/World.txt";
    //Quad_Filename = "C:/Users/h252114/Documents/QT_OpenGL/GrassSky/SNOW.bmp";
//    GS_Filename = "C:/Users/h252114/Documents/QT_OpenGL/GrassSky/SkyGrass.bmp";
//    m_GSFile = "C:/Users/h252114/Documents/QT_OpenGL/GrassSky/GS.txt";

    Sky_Sector.numtriangles = 0;
    Grass_Sector.numtriangles = 0;
   // GS_Sector.numtriangles =0;
    //m_rquad = 0.0f;



    k_xPos =0.0f;
    k_yRot =0.0f;
    k_zPos =0.0f;
    m_xRot =0.0f;
    m_yPos =0.0f;
    m_zRot =0.0f;
    m_xmove = 0.0f;
    m_ymove = 0.0f;
    m_zmove = 0.0f;
    k_LookUpDown = 0.0f;

    fricy = 0.0;
    fricz = 0.0;
    ay = 0;
    az = 0.0;
    rou = 0.0;
    vy0 = 15.0;
    vz0 = -10.0;
    vy = vy0;
    vz =vz0;

    Fy_total = 0;
    Fz_total = 0;
    mass = 0.2;//kg
    ForceApply = 0;
    beta = 0;
    //beta = atan(vy0/vz0);
    Fkey_released = 0;

    QTimer *timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()),this, SLOT(updateGL()));
    timer->start(10);



    cursor.setShape(Qt::ArrowCursor);
    setCursor(cursor);

}


EnterThreeDWidget::~EnterThreeDWidget()
{

}

SECTOR EnterThreeDWidget::loadObject(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    //QString line = in.readline();
    SECTOR obj_Sector;
    obj_Sector.numtriangles = 0;

    while (!in.atEnd())
    {
        QString line[3];
        for (int i=0; i<3; i++)
        {
            do
            {
                line[i] = in.readLine();
            }
            while (line[i][0] == '/' || line[i][0] == '\n' );
        }

        obj_Sector.numtriangles++;
        TRIANGLE tempTri;
        for (int i=0; i<3; i++)
        {
            QTextStream inLine(&line[i]);
            inLine  >>tempTri.vertexs[i].x
                    >>tempTri.vertexs[i].y
                    >>tempTri.vertexs[i].z
                    >>tempTri.vertexs[i].u
                    >>tempTri.vertexs[i].v;
 //         qDebug("GrassTriangle %d",Grass_Sector.numtriangles);
 //         qDebug("x=%f,y=%f,z=%f",tempTriGrass.vertexs[i].x,tempTriGrass.vertexs[i].y,tempTriGrass.vertexs[i].z);
 //         qDebug("u=%f,v=%f",tempTriGrass.vertexs[i].u,tempTriGrass.vertexs[i].v);
        }
        obj_Sector.vTriangle.push_back(tempTri);
    }

    file.close();
    return obj_Sector;

}

void EnterThreeDWidget::initializeGL()
{

    Sky_Texture = bindTexture(QPixmap(Sky_Filename));
    Grass_Texture = bindTexture(QPixmap(Grass_Filename));
    //GS_Texture = bindTexture(QPixmap(GS_Filename));
//    Quad_Texture =bindTexture(QPixmap(Quad_Filename));
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    matrix.perspective(90.0f,(GLfloat)mWindowWidth/mWindowHeight,0.1f,100.0f);


    glClearColor(0.92,0.95,0.95,0);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

    m_sphere = new cMeshModel(QString("sphere.obj"));

}

void EnterThreeDWidget::resizeGL(int w, int h)
{
    glViewport(0,0,(GLint)w,(GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0,(GLfloat)w/(GLfloat)h,0.1,100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

//    mWindowWidth = w;
//    mWindowHeight = (h==0)?1:h;

//    glViewport(0,0,mWindowWidth,mWindowHeight);
//    paintGL();
}

void EnterThreeDWidget::DrawSector(SECTOR sec,GLuint a_Texture)
{

    GLfloat x_m, y_m, z_m, u_m, v_m;
    for (int i=0; i<sec.numtriangles; i++)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,a_Texture);
        glBegin(GL_TRIANGLES);
            glNormal3f(0.0f,0.0f,1.0f);
            x_m = sec.vTriangle[i].vertexs[0].x;
            y_m = sec.vTriangle[i].vertexs[0].y;
            z_m = sec.vTriangle[i].vertexs[0].z;
            u_m = sec.vTriangle[i].vertexs[0].u;
            v_m = sec.vTriangle[i].vertexs[0].v;
            glTexCoord2f(u_m,v_m);
            glVertex3f(x_m,y_m,z_m);

            x_m = sec.vTriangle[i].vertexs[1].x;
            y_m = sec.vTriangle[i].vertexs[1].y;
            z_m = sec.vTriangle[i].vertexs[1].z;
            u_m = sec.vTriangle[i].vertexs[1].u;
            v_m = sec.vTriangle[i].vertexs[1].v;
            glTexCoord2f(u_m,v_m);
            glVertex3f(x_m,y_m,z_m);

            x_m = sec.vTriangle[i].vertexs[2].x;
            y_m = sec.vTriangle[i].vertexs[2].y;
            z_m = sec.vTriangle[i].vertexs[2].z;
            u_m = sec.vTriangle[i].vertexs[2].u;
            v_m = sec.vTriangle[i].vertexs[2].v;
            glTexCoord2f(u_m,v_m);
            glVertex3f(x_m,y_m,z_m);

         glEnd();
    }

}

void EnterThreeDWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();


    //GLfloat x_m, y_m, z_m, u_m, v_m;
    GLfloat xTrans = -k_xPos;
    GLfloat zTrans = -k_zPos;
    GLfloat yTrans = -0.25f;
    GLfloat sceneroty = 360.0f - k_yRot;

//    time_count ++;
//    qDebug("time count =%d",time_count);

    glRotatef(k_LookUpDown, 1.0f,0.0f,0.0f);
    glRotatef(sceneroty, 0.0f,1.0f,0.0f);
    glTranslatef(xTrans, yTrans, zTrans);

    //qDebug("xTrans = %f, yTrans = %f, zTrans = %f",xTrans, yTrans, zTrans);


    Sky_Sector = loadObject(m_SkyFile);
    //glBindTexture(GL_TEXTURE_2D, Sky_Texture);

    DrawSector(Sky_Sector,Sky_Texture);

    Grass_Sector = loadObject(m_GrassFile);
    //glBindTexture(GL_TEXTURE_2D, Grass_Texture);
    DrawSector(Grass_Sector,Grass_Texture);

//    GS_Sector = loadObject(m_GSFile);
//    glBindTexture(GL_TEXTURE_2D, GS_Texture);
//    DrawSector(GS_Sector);

    //glLoadIdentity();
/*    glTranslatef(m_xmove, m_ymove, m_zmove);
    glRotatef(m_zRot,0.0f,1.0f,0.0f);
    glRotatef(m_xRot,1.0f,0.0f,0.0f);*/
//    glBindTexture(GL_TEXTURE_2D, Quad_Texture);            //选择纹理
//        glBegin(GL_QUADS);                                  //开始绘制立方体
//            glTexCoord2f(1.0f, 1.0f);
//            glVertex3f(1.0f/2, 1.0f/2, -1.0f/2);                  //右上(顶面)
//            glTexCoord2f(0.0f, 1.0f);
//            glVertex3f(-1.0f/2, 1.0f/2, -1.0f/2);                 //左上(顶面)
//            glTexCoord2f(0.0f, 0.0f);
//            glVertex3f(-1.0f/2, 1.0f/2, 1.0f/2);                  //左下(顶面)
//            glTexCoord2f(1.0f, 0.0f);
//            glVertex3f(1.0f/2, 1.0f/2, 1.0f/2);                   //右下(顶面)

//            glTexCoord2f(0.0f, 0.0f);
//            glVertex3f(1.0f/2, -1.0f/2, 1.0f/2);                  //右上(底面)
//            glTexCoord2f(1.0f, 0.0f);
//            glVertex3f(-1.0f/2, -1.0f/2, 1.0f/2);                 //左上(底面)
//            glTexCoord2f(1.0f, 1.0f);
//            glVertex3f(-1.0f/2, -1.0f/2, -1.0f/2);                //左下(底面)
//            glTexCoord2f(0.0f, 1.0f);
//            glVertex3f(1.0f/2, -1.0f/2, -1.0f/2);                 //右下(底面)

//            glTexCoord2f(1.0f, 1.0f);
//            glVertex3f(1.0f/2, 1.0f/2, 1.0f/2);                   //右上(前面)
//            glTexCoord2f(0.0f, 1.0f);
//            glVertex3f(-1.0f/2, 1.0f/2, 1.0f/2);                  //左上(前面)
//            glTexCoord2f(0.0f, 0.0f);
//            glVertex3f(-1.0f/2, -1.0f/2, 1.0f/2);                 //左下(前面)
//            glTexCoord2f(1.0f, 0.0f);
//            glVertex3f(1.0f/2, -1.0f/2, 1.0f/2);                  //右下(前面)

//            glTexCoord2f(0.0f, 0.0f);
//            glVertex3f(1.0f/2, -1.0f/2, -1.0f/2);                 //右上(后面)
//            glTexCoord2f(1.0f, 0.0f);
//            glVertex3f(-1.0f/2, -1.0f/2, -1.0f/2);                //左上(后面)
//            glTexCoord2f(1.0f, 1.0f);
//            glVertex3f(-1.0f/2, 1.0f/2, -1.0f/2);                 //左下(后面)
//            glTexCoord2f(0.0f, 1.0f);
//            glVertex3f(1.0f/2, 1.0f/2, -1.0f/2);                  //右下(后面)

//            glTexCoord2f(1.0f, 1.0f);
//            glVertex3f(-1.0f/2, 1.0f/2, 1.0f/2);                  //右上(左面)
//            glTexCoord2f(0.0f, 1.0f);
//            glVertex3f(-1.0f/2, 1.0f/2, -1.0f/2);                 //左上(左面)
//            glTexCoord2f(0.0f, 0.0f);
//            glVertex3f(-1.0f/2, -1.0f/2, -1.0f/2);                //左下(左面)
//            glTexCoord2f(1.0f, 0.0f);
//            glVertex3f(-1.0f/2, -1.0f/2, 1.0f/2);                 //右下(左面)

//            glTexCoord2f(1.0f, 1.0f);
//            glVertex3f(1.0f/2, 1.0f/2, -1.0f/2);                  //右上(右面)
//            glTexCoord2f(0.0f, 1.0f);
//            glVertex3f(1.0f/2, 1.0f/2, 1.0f/2);                   //左上(右面)
//            glTexCoord2f(0.0f, 0.0f);
//            glVertex3f(1.0f/2, -1.0f/2, 1.0f/2);                  //左下(右面)
//            glTexCoord2f(1.0f, 0.0f);
//            glVertex3f(1.0f/2, -1.0f/2, -1.0f/2);                 //右下(右面)
//        glEnd();                                            //立方体绘制结束

        //m_rquad -= 0.5f;


    QMatrix4x4 ccMatrix = matrix;
    //qDebug("Fkey_released in paint");
   // qDebug()<<Fkey_released;

    int seed =  QDateTime::currentDateTime().time().second();
    srand(seed);
    int rand_num = rand()%40;
    rand_num = (-1)*rand_num;

    beta = 85 + rand_num;

    qDebug("beta random = %f",beta);

    //qDebug("m_ymove = %f, ",m_ymove);
    if (Fkey_released == 1)
    {
//        int seed =  QDateTime::currentDateTime().time().second();
//        srand(seed);
//        beta = 60 + (-1)*rand()%10;

//        qDebug("beta random = %f",beta);

        Force_y = (float)sin(beta * m_PIOVER180 * 0.1)*ForceApply;
        Force_z = (float)cos(beta * m_PIOVER180 * 0.1)*ForceApply;
        //qDebug("angel = %f, sin = %f",beta*m_PIOVER180*0.1,sin(beta * m_PIOVER180*0.1));

        //qDebug("ForceApply in BatForce = %f",ForceApply);

        vy0 = Force_y * 0.5;
        vz0 = -Force_z * 0.5;
        //time_c = time_count * 0.01;

        //qDebug("2*vy0/9.8 =%f",2*vy0*0.102);
        //qDebug("time_c =%f",time_c);
        if (time_c<= (2*vy0*0.102))
        {
            //BatForce(ForceApply);
            Fy_total = fricy + mass * (-9.8);
            ay = Fy_total/mass;
            m_ymove = vy0* time_c + 0.5 * ay * time_c *time_c; //has to be 0.5, not 1/2
            //qDebug("square part  = %f",0.5 * ay * time_c *time_c);
            qDebug("time_c = %f,ay = %f,m_ymove = %f,vy0 = %f",time_c,ay,m_ymove,vy0);
            vy =vy + time_c * ay;

            fricz = -rou * vz;
            Fz_total =fricz;
            az = Fz_total/mass;
            m_zmove = vz0 * time_c + 0.5 * az * time_c*time_c;

            time_c += 0.01;
            Fkey_released = 1;
        }
        if (time_c > (2*(vy0)*0.102))
        {
            time_c = 0;
            Fkey_released = 0;
            ForceApply = 0;
        }


//        if (time_c >(2*vy0*0.102))
//        {
//                time_count = 0;
//                Fkey_released =0;
//        }
//        else
//        {


//        }
        qDebug("time_c = %f",time_c);



    }


    ccMatrix.rotate(360.0 - k_yRot,QVector3D(0,1,0));

    ccMatrix.translate(QVector3D(0,0,-2.4f));
    ccMatrix.translate(QVector3D(m_xmove + xTrans, m_ymove , m_zmove + zTrans));

    ccMatrix.rotate(m_zRot,QVector3D(0,1,0));
    ccMatrix.rotate(m_xRot,QVector3D(1,0,0));
    //qDebug("m_ymove = %f,y_Rot = %f",m_ymove,k_yRot);
    //qDebug()<<ccMatrix;




    // Render
    m_sphere->Render(ccMatrix);


}



void EnterThreeDWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_F1:
        fullscreen = !fullscreen;
        if (fullscreen)
        {
            showFullScreen();
        }
        else
        {
            showNormal();
        }
        updateGL();
        break;
    case Qt::Key_Escape:
        close();
        break;
    case Qt::Key_PageUp:
        k_LookUpDown -=1.0f;
        if (k_LookUpDown < -90.0f)
        {
            k_LookUpDown = -90.0f;
        }
        break;
    case Qt::Key_PageDown:
        k_LookUpDown += 1.0f;
        if (k_LookUpDown > 90.0f)
        {
            k_LookUpDown = 90.0f;
        }
        break;
    case Qt::Key_Right:
        k_yRot -= 1.0f;
        break;
    case Qt::Key_Left:
        k_yRot += 1.0f;
        break;
    case Qt::Key_Up:
        k_xPos -= (float)sin(k_yRot * m_PIOVER180) * 0.05f;
        k_zPos -= (float)cos(k_yRot * m_PIOVER180) * 0.05f;
        break;
    case Qt::Key_Down:
        k_xPos += (float)sin(k_yRot * m_PIOVER180) * 0.05f;
        k_zPos += (float)cos(k_yRot * m_PIOVER180) * 0.05f;
        break;
    case Qt::Key_D:
        m_zmove += 0.1f;
        break;
    case Qt::Key_E:
        m_zmove += -0.1f;
        break;
    case Qt::Key_Space:
        k_xPos =0.0f;
        m_xRot =0.0f;

        m_yPos =0.0f;
        k_yRot =0.0f;

        k_zPos =0.0f;
        m_zRot =0.0f;

        m_xmove = 0.0f;
        m_ymove = 0.0f;
        m_zmove = 0.0f;
        k_LookUpDown = 0.0f;
        time_count = 0.0f;
//        vy = -5.0;
//        vz = -3.0;
        break;
    case Qt::Key_F:
        time_count =0;


        //qDebug()<< ForceApply;

        ForceApply +=  0.1;
        qDebug("ForceApply = %f", ForceApply);


        break;

    }

}

void EnterThreeDWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F)
    {
        if (event->isAutoRepeat())
        {
            qDebug()<<"auto";
            Fkey_released =0;
            return;
        }
        else
        {
            //BatForce();
            Fkey_released =1;

            qDebug()<<Fkey_released;
        }



    }
}

void EnterThreeDWidget::mouseMoveEvent(QMouseEvent *event)
{
         GLfloat dx = GLfloat(event->x() - lastPos.x())/width();
         GLfloat dy = GLfloat(event->y() - lastPos.y())/height();

         //qDebug("globalX = %f",lastPos.x());

         if (event->buttons() & Qt::LeftButton) {
             cursor.setShape(Qt::ClosedHandCursor);
             setCursor(cursor);
             m_zRot = m_zRot + 180 * dx;
          //   qDebug("m_zRot = %f, dx = %f",m_zRot,dx);
             m_xRot = m_xRot + 180 * dy;
             updateGL();
        }
         else if (event->buttons() & Qt::RightButton) {
            //transVec += QVector3D(dx,-dy,0.0f);
             m_xmove += dx;
             m_ymove += -dy;
           //  qDebug("m_xmove = %f, ",m_xmove);

             updateGL();
        }
         lastPos = event->pos();
}

void EnterThreeDWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void EnterThreeDWidget::mouseReleaseEvent(QMouseEvent *event)
{
//    m_xRot = 0;
//    m_zRot = 0;
    cursor.setShape(Qt::ArrowCursor);
    setCursor(cursor);
}

void EnterThreeDWidget::BatForce(GLfloat ForceApply)
{

    fricy = -rou * vy ;
    //qDebug("fricy = %f,vy = %f",fricy,vy);
//    Fy_total = fricy + mass * (-9.8);
//    ay = Fy_total/mass;
//    m_ymove = vy0* time_c + 0.5 * ay * time_c *time_c; //has to be 0.5, not 1/2
//    //qDebug("square part  = %f",0.5 * ay * time_c *time_c);
//    qDebug("time_c = %f,ay = %f,m_ymove = %f,vy0 = %f",time_c,ay,m_ymove,vy0);
//    vy =vy + time_c * ay;

//    fricz = -rou * vz;
//    Fz_total =fricz;
//    az = Fz_total/mass;
//    m_zmove = vz0 * time_c + 0.5 * az * time_c*time_c;
    vz = vz + time_c * az;
    qDebug("m_zmove = %f",m_zmove);

    beta =  atan(vy/vz);
    //qDebug("beta =  %f",beta);





}


