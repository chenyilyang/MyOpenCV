//
// Created by mac on 12/30/20.
//

#include "../sample_h/TransformFeedbackSample.h"
#include "../include/GLUtils.h"
#define VERTEX_POS_INDEX 0
#define TEXTURE_POS_INDEX 1

TransformFeedbackSample::TransformFeedbackSample() {
    m_VaoId = GL_NONE;
    m_ImageTextureId = GL_NONE;
    m_SamplerLoc = GL_NONE;
    m_TransFeedbackBuffId = GL_NONE;
    m_TransFeedbackObjId = GL_NONE;
}

TransformFeedbackSample::~TransformFeedbackSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void TransformFeedbackSample::LoadImage(NativeImage *pImage) {
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}
void TransformFeedbackSample::Init() {
    GLfloat vVertices [] = {
            -1.0f, -0.5f, 0.0f,
            1.0f, -0.5f, 0.0f,
            -1.0f, 0.5f, 0.0f,

            1.0f, -0.5f, 0.0f,
            1.0f, 0.5f, 0.0f,
            -1.0f, 0.5f, 0.0f
    };
    GLfloat vTexCoords [] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,

            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f
    };
    char vShaderStr [] =
            "#version 300 es                                                                        \n"
            "layout(location = 0) in vec4 a_position;                                               \n"
            "layout(location = 1) in vec2 a_texCoord;                                               \n"
            "out vec2 v_texCoord;                                                                   \n"
            "out vec3 outPos;                                                                       \n"
            "out vec2 outTex;                                                                       \n"
            "void main() {                                                                          \n"
            "   gl_Position = a_position;                                                           \n"
            "   v_texCoord = a_texCoord;                                                            \n"
            "   outPos = vec3(a_position) * 3.0;                                                    \n"
            "   outTex = a_texCoord * 3.0;                                                          \n"
            "}                                                                                      \n";
    char fShaderStr [] =
            "#version 300 es                                                                        \n"
            "precision mediump float;                                                               \n"
            "in vec2 v_texCoord;                                                                    \n"
            "layout(location = 0) out vec4 outColor;                                                \n"
            "uniform sampler2D s_TextureMap;                                                        \n"
            "void main() {                                                                          \n"
            "   outColor = texture(s_TextureMap, v_texCoord);                                       \n"
            "}                                                                                      \n";
    GLchar const * varyings[] = {"outPos", "outTex"};
    m_ProgramObj = GLUtils::CreateProgramWithFeedback(vShaderStr, fShaderStr, m_VertexShader, m_FragmentSahder, varyings, sizeof(varyings) / sizeof(varyings[0]));
    if (m_ProgramObj == GL_NONE) {
        LOGCATE("TransformFeedbackSample::Init m_ProgramObj == GL_NONE");
        return;
    }
    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    //Generate VBO Ids and load the VBOS with Data
    glGenBuffers(2, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vTexCoords), vTexCoords, GL_STATIC_DRAW);
    GO_CHECK_GL_ERROR();

    //Generate VAO Ids
    glGenVertexArrays(1, &m_VaoId);
    //Normal rendering VAO
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_INDEX);
    glVertexAttribPointer(VERTEX_POS_INDEX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(TEXTURE_POS_INDEX);
    glVertexAttribPointer(TEXTURE_POS_INDEX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);

    glGenBuffers(1, &m_TransFeedbackBuffId);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_TransFeedbackBuffId);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, (3 + 2) * 6 * sizeof(GLfloat), NULL, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_NONE);

    glGenTransformFeedbacks(1, &m_TransFeedbackObjId);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TransFeedbackObjId);
    //Specify the index of the binding point within the array specified by target
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_TransFeedbackBuffId);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, GL_NONE);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_NONE);

    //create texture and initialize
    glGenTextures(1, &m_ImageTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    GO_CHECK_GL_ERROR();
}

void TransformFeedbackSample::Draw(int screenW, int screenH) {
    glViewport(0, 0, screenW, screenH);
    glUseProgram(m_ProgramObj);
    glBindVertexArray(m_VaoId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glUniform1i(m_SamplerLoc, 0);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TransFeedbackObjId);
    glBeginTransformFeedback(GL_TRIANGLES);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEndTransformFeedback();

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);

    //read feedback buffer
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_TransFeedbackBuffId);
    void * rawData = glMapBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, (3 + 2) * 6 * sizeof(GLfloat), GL_MAP_READ_BIT);

    float * p = (float *)rawData;
    for (int i = 0; i < 6; ++i) {
        LOGCATE("TransformFeedbackSample::Draw() read feedback buffer outPos[%d] = [%f, %f, %f], outTex[%d] = [%f, %f]", i, p[i * 5], p[i * 5 + 1], p[i * 5 + 2], i, p[i * 5 + 3], p[i * 5 + 4]);
    }
    glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_NONE);
}

void TransformFeedbackSample::Destroy() {
    if (m_ProgramObj) glDeleteProgram(m_ProgramObj);
    if (m_ImageTextureId) glDeleteTextures(1, &m_ImageTextureId);
    if (m_VboIds[0]) glDeleteBuffers(2, m_VboIds);
    if (m_VaoId) glDeleteVertexArrays(1, &m_VaoId);
    if (m_TransFeedbackBuffId) glDeleteBuffers(1, &m_TransFeedbackBuffId);
    if (m_TransFeedbackObjId) glDeleteTransformFeedbacks(1, &m_TransFeedbackObjId);
}