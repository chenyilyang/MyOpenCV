//
// Created by chenyilyang on 12/17/20.
//

#include "../sample_h/FBOSample.h"
#include "../include/GLUtils.h"
#define VERTEX_POS_INDEX 0
#define TEXTURE_POS_INDEX 1

FBOSample::FBOSample() {
    m_ImageTextureId = GL_NONE;
    m_FboTextureId = GL_NONE;
    m_FboId = GL_NONE;
    m_VaoIds[2] = {GL_NONE};
    m_VboIds[4] = {GL_NONE};
    m_SamplerLoc = GL_NONE;
    m_FboProgramObj = GL_NONE;
    m_FboVertexShader = GL_NONE;
    m_FboFragmentShader = GL_NONE;
    m_FboSamplerLoc = GL_NONE;
}
FBOSample::~FBOSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}
void FBOSample::LoadImage(NativeImage *pImage) {
    LOGCATD("FBOSample::LoadImage pImage = %p", pImage);
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}
void FBOSample::Init() {
    //vertex vertices
    GLfloat vVertices [] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
    };
    //normal texture coors
    GLfloat vTexCoors [] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f,0.0f,
            1.0f, 0.0f
    };
    //fbo texture coors
    GLfloat vFboTexCoors [] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
    };
    GLushort indices [] = {0, 1, 2, 1, 3, 2};
    char vShaderStr [] =
            "#version 300 es                                    \n"
            "layout(location = 0) in vec4 a_position;           \n"
            "layout(location = 1) in vec2 a_texCoord;           \n"
            "out vec2 v_texCoord;                               \n"
            "void main() {                                      \n"
            "   gl_Position = a_position;                       \n"
            "   v_texCoord = a_texCoord;                        \n"
            "}                                                  \n";
    //normal fragment shader
    char fShaderStr [] =
            "#version 300 es                                    \n"
            "precision mediump float;                           \n"
            "in vec2 v_texCoord;                                \n"
            "layout(location = 0) out vec4 outColor;            \n"
            "uniform sampler2D s_TextureMap;                    \n"
            "void main() {                                      \n"
            "   outColor = texture(s_TextureMap, v_texCoord);   \n"
            "}                                                  \n";
    //fbo fragment shader
    char fFboShaderStr [] =
            "#version 300 es                                                                    \n"
            "precision mediump float;                                                           \n"
            "in vec2 v_texCoord;                                                                \n"
            "layout(location = 0) out vec4 outColor;                                            \n"
            "uniform sampler2D s_TextureMap;                                                    \n"
            "void main() {                                                                      \n"
            "   vec4 tmpColor = texture(s_TextureMap, v_texCoord);                              \n"
            "   float luminance = tmpColor.r * 0.299 + tmpColor.g * 0.587 + tmpColor.b * 0.114; \n"
            "   outColor = vec4(vec3(luminance), tmpColor.a);                                   \n"
        "}                                                                                      \n";
    //normal render program
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentSahder);
    //off screen render program
    m_FboProgramObj = GLUtils::CreateProgram(vShaderStr, fFboShaderStr, m_FboVertexShader, m_FboFragmentShader);
    if (m_ProgramObj == GL_NONE || m_FboProgramObj == GL_NONE) {
        LOGCATE("FBOSample::Init could not create programs");
        return;
    }
    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    m_FboSamplerLoc = glGetUniformLocation(m_FboProgramObj, "s_TextureMap");
    //Gen VBO ids and load the VBOs with data
    glGenBuffers(4, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vTexCoors), vTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vFboTexCoors), vFboTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GO_CHECK_GL_ERROR();

    //Gen 2 VAO ids, one is use for normal render, one for off screen render
    glGenVertexArrays(2, m_VaoIds);
    //normal rendering vao
    glBindVertexArray(m_VaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_INDEX);
    glVertexAttribPointer(VERTEX_POS_INDEX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(TEXTURE_POS_INDEX);
    glVertexAttribPointer(TEXTURE_POS_INDEX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);

    //fbo off screen rendering vao
    glBindVertexArray(m_VaoIds[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_INDEX);
    glVertexAttribPointer(VERTEX_POS_INDEX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glEnableVertexAttribArray(TEXTURE_POS_INDEX);
    glVertexAttribPointer(TEXTURE_POS_INDEX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);

    //create and initialize image texture
    glGenTextures(1, &m_ImageTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    GO_CHECK_GL_ERROR();

    if (!CreateFrameBufferObj()) {
        LOGCATE("FBOSample::Init CreateFrameBufferObj fail");
    }
}
void FBOSample::Draw(int screenW, int screenH) {
    //off screen rendering
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glViewport(0, 0, m_RenderImage.width, m_RenderImage.height);
    //do fbo off screen rendering
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    glUseProgram(m_FboProgramObj);
    glBindVertexArray(m_VaoIds[1]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glUniform1i(m_FboSamplerLoc, 0);
    GO_CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //todo save image from glReadPixels
    //
    //
    //
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    //normal rendering
//    glViewport(0, 0, screenW, screenH);
    glViewport(0, 0, m_RenderImage.width, m_RenderImage.height);
    glUseProgram(m_ProgramObj);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(m_VaoIds[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glUniform1i(m_SamplerLoc, 0);
    GO_CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
    GO_CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);
}
void FBOSample::Destroy() {
    if (m_ProgramObj) glDeleteProgram(m_ProgramObj);
    if (m_FboProgramObj) glDeleteProgram(m_FboProgramObj);
    if (m_ImageTextureId) glDeleteTextures(1, &m_ImageTextureId);
    if (m_FboTextureId) glDeleteTextures(1, &m_FboTextureId);
    if (m_VboIds[0]) glDeleteBuffers(4, m_VboIds);
    if (m_VaoIds[0]) glDeleteVertexArrays(2, m_VaoIds);
    if (m_FboId) glDeleteFramebuffers(1, &m_FboId);
}
bool FBOSample::CreateFrameBufferObj() {
    //create and initialize FBO texture
    glGenTextures(1, &m_FboTextureId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    //create and initialize FBO
    glGenFramebuffers(1, &m_FboId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FboTextureId, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGCATE("FBOSample::CraeteFrameBufferObj glCheckFrameBufferStatus status != GL_FRAMEBUFFER_COMPLETE");
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    return true;
}