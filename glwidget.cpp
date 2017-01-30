#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QDebug>

#define check() Q_ASSERT(glGetError() == 0)

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      clearColor(Qt::black),
      program(0),
      texture(0)
{
}

GLWidget::~GLWidget()
{
    makeCurrent();
    vbo.destroy();
    delete texture;
    delete program;
    doneCurrent();
}

static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "uniform vec2 offset;\n"
    "uniform vec2 scale;\n"
    "varying vec2 tcoord;\n"
    "void main(void) {\n"
    "   vec4 pos = vertex;\n"
    "   tcoord.xy = pos.xy;\n"
    "   pos.xy = pos.xy*scale+offset;\n"
    "   gl_Position = pos;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying vec2 tcoord;\n"
    "uniform sampler2D tex;\n"
    "void main(void) {\n"
    "   gl_FragColor = texture2D(tex,tcoord);\n"
    "}\n";


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);

    // Setup our vertex buffer object.
    static const GLfloat quad_vertex_positions[] = {
        0.0f, 0.0f,	1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };
    vbo.create();
    vbo.bind();
    vbo.allocate(quad_vertex_positions, sizeof(quad_vertex_positions));

    texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
    texture->setSize(640, 480);
    texture->setFormat(QOpenGLTexture::RGBAFormat);
    texture->allocateStorage(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);

    #define PROGRAM_VERTEX_ATTRIBUTE 0

    program = new QOpenGLShaderProgram;
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->link();
    program->bind();
    program->setUniformValue("tex", 0);
}

void GLWidget::paintGL()
{
    program->setUniformValue("offset", 1.f, -1.f);
    program->setUniformValue("scale", -2.f, 2.f);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 4, 4 * sizeof(GLfloat));
    texture->bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void GLWidget::SetPixels(const void * data)
{
    texture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, data);
}
