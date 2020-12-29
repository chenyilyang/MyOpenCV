//
// Created by mac on 12/29/20.
//

#include "../sample_h/VAOVBOSample.h"
#include "../include/GLUtils.h"
#define VERTEX_POS_SIZE         3//x, y and z
#define VERTEX_COLOR_SIZE       4//r,g,b and a
#define VERTEX_POS_INDEX        0//shader layout loc
#define VERTEX_COLOR_INDEX      1//shader layout loc

#define VERTEX_STRIDE           (sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE))

VAOVBOSample::VAOVBOSample() {
    m_VaoId = GL_NONE;
}
VAOVBOSample::~VAOVBOSample() {

}
void VAOVBOSample::LoadImage(NativeImage *pImage) {
    //null implements
}
void VAOVBOSample::Init() {
    const char vShaderStr [] =
            "#version 300 es                                                                        \n"
            "layout(location = 0) in vec4 a_position;                                               \n"
            "layout(location = 1) in vec4 a_color;                                                  \n"
            "out vec4 v_color;                                                                      \n"
            "out vec4 v_position;                                                                   \n"
            "void main(){                                                                           \n"
            "   v_color = a_color;                                                                  \n"
            "   gl_Position = a_position;                                                           \n"
            "   v_position = a_position;                                                            \n"
            "}                                                                                      \n";
    const char fShaderStr [] =
            "#version 300 es                                                                        \n"
            "precision mediump float;                                                               \n"
            "in vec4 v_color;                                                                       \n"
            "in vec4 v_position;                                                                    \n"
            "out vec4 o_fragColor;                                                                  \n"
            "void main(){                                                                           \n"
            "   float n = 10.0;                                                                     \n"
            "   float span = 1.0 / n;                                                               \n"
            "   int i = int((v_position.x +0.5)/span);                                              \n"
            "   int j = int((v_position.y + 0.8)/span);                                             \n"
            "                                                                                       \n"
            "   int grayColor = int(mod(float(i+j), 2.0));                                          \n"
            "   if(grayColor == 1){                                                                 \n"
            "       float luminance = v_color.r * 0.299 + v_color.g *0.587 + v_color.b*0.114;       \n"
            "       o_fragColor = vec4(vec3(luminance), v_color.a);                                 \n"
            "   } else {                                                                            \n"
            "       o_fragColor = v_color;                                                          \n"
            "   }                                                                                   \n"
            "}                                                                                      \n";
    GLfloat vertices [4 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] = {
            -0.5f, 0.8f, 0.0f,//v0
            1.0f, 0.0f, 0.0f, 1.0f,//c0
            -0.5f, -0.8f, 0.0f,//v1
            0.0f, 1.0f, 0.0f, 1.0f,//c1
            0.5f, -0.8f, 0.0f, //v2
            0.0f, 0.0f, 1.0f, 1.0f,//c2
            0.5f, 0.8f, 0.0f,//v3
            1.0f, 0.0f, 0.0f, 1.0f//c3
    };
    GLshort indices[6] = {0, 1, 2, 0, 2, 3};
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentSahder);
    //Generate VBO Ids and load the VBOS with data
    glGenBuffers(2, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[1]);

    glEnableVertexAttribArray(VERTEX_POS_INDEX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDEX);

    glVertexAttribPointer(VERTEX_POS_INDEX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (const void *)0);
    glVertexAttribPointer(VERTEX_COLOR_INDEX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (const void *)(VERTEX_POS_SIZE * sizeof(GLfloat)));

    glBindVertexArray(GL_NONE);
}

void VAOVBOSample::Draw(int screenW, int screenH) {
    if (m_ProgramObj == GL_NONE) return;
    glUseProgram(m_ProgramObj);
    glBindVertexArray(m_VaoId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
    glBindVertexArray(GL_NONE);
}
void VAOVBOSample::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(2, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}