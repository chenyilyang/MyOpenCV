//
// Created by chenyilyang on 12/28/20.
//

#include "../sample_h/YUVTextureSample.h"
#include "../include/GLUtils.h"

void YUVTextureSample::LoadImage(NativeImage *pImage) {
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}
void YUVTextureSample::Init() {
    char vShaderStr [] =
            "#version 300 es                                                                        \n"
            "layout(location = 0) in vec4 a_position;                                               \n"
            "layout(location = 1) in vec2 a_texCoord;                                               \n"
            "out vec2 v_texCoord;                                                                   \n"
            "void main() {                                                                          \n"
            "   gl_Position = a_position;                                                           \n"
            "   v_texCoord = a_texCoord;                                                            \n"
            "}                                                                                      \n";
    char fShaderStr [] =
            "#version 300 es                                                                        \n"
            "precision mediump float;                                                               \n"
            "in vec2 v_texCoord;                                                                    \n"
            "layout(location = 0) out vec4 outColor;                                                \n"
            "uniform sampler2D y_texture;                                                           \n"
            "uniform sampler2D uv_texture;                                                          \n"
            "void main() {                                                                          \n"
            "   vec3 yuv;                                                                           \n"
            "   yuv.x = texture(y_texture, v_texCoord).r;                                           \n"
            "   yuv.y = texture(uv_texture, v_texCoord).a - 0.5;                                    \n"
            "   yuv.z = texture(uv_texture, v_texCoord).r - 0.4;                                    \n"
            "   highp vec3 rgb = mat3(1, 1, 1, 0, -0.344, 1.770, 1.403, -0.714, 0) * yuv;           \n"
            "   outColor = vec4(rgb, 1);                                                            \n"
            "}                                                                                      \n";
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentSahder);

    m_ySamplerLoc = glGetUniformLocation(m_ProgramObj, "y_texture");
    m_uvSamplerLoc = glGetUniformLocation(m_ProgramObj, "uv_texture");

    GLuint textureIds[2] = {0};
    glGenTextures(2, textureIds);
    m_yTextureId = textureIds[0];
    m_uvTextureId = textureIds[1];
}
void YUVTextureSample::Draw(int screenW, int screenH) {
    LOGCATD("NV21TextureSample::Draw");
    if (m_ProgramObj == GL_NONE || m_yTextureId == GL_NONE || m_uvTextureId == GL_NONE) return;
    //upload y plane data
    glBindTexture(GL_TEXTURE_2D, m_yTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width, m_RenderImage.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //upload uv plane data
    glBindTexture(GL_TEXTURE_2D, m_uvTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, m_RenderImage.width >> 1, m_RenderImage.height >> 1, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[1]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    float ratio = ((float)screenW) / m_RenderImage.width;
    float ratioH = ratio * m_RenderImage.height / screenH;
    GLfloat verticesCoords [] = {
            -1.0f, ratioH, 0.0f,
            -1.0f, -ratioH, 0.0f,
            1.0f, -ratioH, 0.0f,
            1.0f, ratioH, 0.0f
    };
    GLfloat textureCoords [] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
    };
    GLushort indices [] = {0, 1, 2, 0, 2, 3};
    glUseProgram(m_ProgramObj);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), verticesCoords);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), textureCoords);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    //Bind the y plane map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_yTextureId);
    //set the y plane sampler to texture unit to 0
    glUniform1i(m_ySamplerLoc, 0);
    //Bind the uv plane map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_uvTextureId);
    //set the uv plane sampler to texture unit to 1
    glUniform1i(m_uvSamplerLoc, 1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

void YUVTextureSample::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteTextures(1, &m_yTextureId);
        glDeleteTextures(1, &m_uvTextureId);
    }
}
