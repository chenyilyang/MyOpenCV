//
// Created by chenyilyang on 12/16/20.
//

#include "../include/EGLRender.h"
#include "../include/LogUtil.h"
#include "../include/GLUtils.h"
#include "../include/SampleBase.h"

#define VERTEX_POS_LOC 0
#define TEXTURE_POS_LOC 1

EGLRender * EGLRender::m_Instance = nullptr;

const char vShaderStr[] =
        "#version 300 es                                \n"
        "layout(location = 0) in vec4 a_position;       \n"
        "layout(location = 1) in vec2 a_texCoord;       \n"
        "out vec2 v_texCoord;                           \n"
        "void main()                                    \n"
        "{                                              \n"
        "   gl_Position = a_position;                   \n"
        "   v_texCoord = a_texCoord;                    \n"
        "}                                              \n";
const char fShaderStr0[] =
        "#version 300 es                                \n"
        "precision medium float;                        \n"
        "in vec2 v_texCoord;                            \n"
        "layout(location = 0) out vec4 outColor;        \n"
        "uniform sampler2D s_TextureMap;                \n"
        "void main()                                    \n"
        "{                                              \n"
        "   outColor = texture(s_TextureMap, v_texCoord);\n"
        "}                                              \n";

//vertex array
const GLfloat vVertices [] = {
        -1.0f, -1.0f, 0.0f,//bottom left
        1.0f, -1.0f, 0.0f,//bottom right
        -1.0f, 1.0f, 0.0f,//top left
        1.0f, 1.0f, 0.0f,//top right
};
//texture coors
const GLfloat vTexCoors [] = {
        0.0f, 1.0f,//bottom left
        1.0f, 1.0f,//bottom right
        0.0f, 0.0f,//top left
        1.0f, 0.0f,//top right
};
//fbo texture coors, reverse of texture coors(mirror in vertical)
const GLfloat vFboTexCoors [] = {
        0.0f, 0.0f,//bottom left
        1.0f, 0.0f,//bottom right
        0.0f, 1.0f,//top left
        1.0f, 1.0f//top right
};

const GLushort indices [] = {0, 1, 2, 1, 3, 2};

EGLRender::EGLRender() {
    m_VaoIds[1]                 = {GL_NONE};
    m_VaoIds[3]                 = {GL_NONE};
    m_ImageTextureId            = GL_NONE;
    m_FboTextureId              = GL_NONE;
    m_SamplerLoc                = GL_NONE;
    m_TexSizeLoc                = GL_NONE;
    m_FboId                     = GL_NONE;
    m_ProgramObj                = GL_NONE;
    m_VertextShader             = GL_NONE;
    m_FragmentShader            = GL_NONE;

    m_IsGLContextReady          = false;
    m_ShaderIndex               = 0;
}

EGLRender::~EGLRender() {
    if (m_RenderImage.ppPlane[0]) {
        NativeImageUtil::FreeNativeImage(&m_RenderImage);
        m_RenderImage.ppPlane[0] = nullptr;
    }
}
void EGLRender::Init() {
    LOGCATD("EGLRender::Init");
    if (CreateGlesEnv() == 0) {
        m_IsGLContextReady = true;
    }
    if (!m_IsGLContextReady) return;
    //todo add sample fragment shader source here with index
    m_fShaderStrs[SAMPLE_SHADER_TYPE_NORMAL] = fShaderStr0;
    m_fShaderStrs[SAMPLE_SHADER_TYPE_FBO] = fShaderStr0;

    glGenTextures(1, &m_ImageTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glGenTextures(1, &m_FboTextureId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, m_fShaderStrs[m_ShaderIndex], m_VertextShader, m_FragmentShader);
    if (!m_ProgramObj) {
        GLUtils::CheckGLError("Create Program");
        LOGCATE("EGLRender::Init Could not create program");
        return;
    }
    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    m_TexSizeLoc = glGetUniformLocation(m_ProgramObj, "u_texSize");

    //Generate VBO Ids and load the VBOS with data
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vFboTexCoors), vTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GO_CHECK_GL_ERROR();

    //Generate VAO Ids
    glGenVertexArrays(1, m_VaoIds);
    //FBO off screen rendering VAO
    glBindVertexArray(m_VaoIds[0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_LOC);
    glVertexAttribPointer(VERTEX_POS_LOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(TEXTURE_POS_LOC);
    glVertexAttribPointer(TEXTURE_POS_LOC, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);
}

int EGLRender::CreateGlesEnv() {
    //EGL config attributes
    const EGLint confAttr [] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,//EGL_WINDOW_BIT EGL_PBUFFER_BIT we will create a pixel buffer surface
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    //EGL Context attributes
    const EGLint ctxAttr [] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    //surface attributes
    //the surface size is set to the input frame size
    const EGLint surfaceAttr [] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE
    };
    EGLint eglMajVers, eglMinVers;
    EGLint numConfigs;
    int resultCode = 0;
    do{
        //obtain EGLDisplay object, build connection with local window system
        m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (m_eglDisplay == EGL_NO_DISPLAY) {
            LOGCATE("EGLRender::CreateGlesEnv unable to open connection to local windowing system");
            resultCode = -1;
            break;
        }
        //initialize egl context
        if (!eglInitialize(m_eglDisplay, &eglMajVers, &eglMinVers)) {
            LOGCATE("EGLRender::CreateGlesEnv unable to initialize EGL");
            resultCode = -1;
            break;
        }
        LOGCATD("EGLRender::CreateGlesEnv init with version %d.%d", eglMajVers, eglMinVers);
        //obtain EGLConfig object, make sure render surface configuration
        if (!eglChooseConfig(m_eglDisplay, confAttr, &m_eglConf, 1, &numConfigs)) {
            LOGCATE("EGLRender::CreateGlesEnv some config is wrong");
            resultCode = -1;
            break;
        }
        //create eglsurface, use eglCreatePbufferSurface create an offscreen area
        m_eglSurface = eglCreatePbufferSurface(m_eglDisplay, m_eglConf, surfaceAttr);
        if (m_eglSurface == EGL_NO_SURFACE) {
            switch(eglGetError()) {
                case EGL_BAD_ALLOC:
                    //Not enough resources available, handle and recover
                    LOGCATE("EGLRender::CreateGlesEnv Not enough resources available");
                    break;
                case EGL_BAD_CONFIG:
                    //Verify that provided EGLConfig is valid
                    LOGCATE("EGLRender::CreateGlesEnv Provided EGLConfig is invalid");
                    break;
                case EGL_BAD_PARAMETER:
                    //verify that the EGL_WIDTH and EGL_HEIGHT are non-negative values
                    LOGCATE("EGLRender::CreateGlesEnv provided EGL_WIDTH and EGL_HEIGHT is invalid");
                    break;
                case EGL_BAD_MATCH:
                    //Check window and EGLConfig attributes
                    //compatibility and pbuffer-texture parameters
                    LOGCATE("EGLRender::CreateGlesEnv Check window and EGLConfig attributes");
                    break;
            }
        }
        //create EGLContext
        m_eglCtx = eglCreateContext(m_eglDisplay, m_eglConf, EGL_NO_CONTEXT, ctxAttr);
        if (m_eglCtx == EGL_NO_CONTEXT) {
            EGLint error = eglGetError();
            if (error == EGL_BAD_CONFIG) {
                //handle error and recover
                LOGCATE("EGLRender::CreateGlesEnv EGL_BAD_CONFIG");
                resultCode = -1;
                break;
            }
        }
        //bind context
        if (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglCtx)) {
            LOGCATE("EGLRender::CreateGlesEnv MakeCurrent failed");
            resultCode = -1;
            break;
        }
        LOGCATD("EGLRender::CreateGlesEnv initialize success!");
    }while (false);
    if (resultCode != 0) LOGCATE("EGLRender::CreateGlesEnv fail!");
    return resultCode;
}

void EGLRender::SetImageData(uint8_t *pData, int width, int height) {
    LOGCATD("EGLRender::SetImageData pData = %p, [w,h] = [%d,%d]", pData, width, height);
    if (pData && m_IsGLContextReady) {
        if (m_RenderImage.ppPlane[0]) {
            NativeImageUtil::FreeNativeImage(&m_RenderImage);
            m_RenderImage.ppPlane[0] = nullptr;
        }
        m_RenderImage.width = width;
        m_RenderImage.height = height;
        m_RenderImage.format = IMAGE_FORMAT_RGBA;
        NativeImageUtil::AllocNativeImage(&m_RenderImage);
        memcpy(m_RenderImage.ppPlane[0], pData, static_cast<size_t>(width * height * 4));

        glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        if (m_FboId ==GL_NONE) {
            //Create FBO
            glGenFramebuffers(1, &m_FboId);
            glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
            glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FboTextureId, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                LOGCATE("EGLRender::SetImageData glCheckFrameBufferStatus != GL_FRAMEBUFFER_COMPLETE");
            }
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
        }
    }
}

void EGLRender::SetIntParams(int paramType, int param) {
    LOGCATD("EGLRender::SetIntParams paramType = %d, param = %d", paramType, param);
    switch(paramType) {
        case SAMPLE_SHADER_TYPE_NORMAL:{
            if (param > 0) {
                m_ShaderIndex = param;
                if (m_ProgramObj) {
                    glDeleteProgram(m_ProgramObj);
                    m_ProgramObj = GL_NONE;
                }
                m_ProgramObj = GLUtils::CreateProgram(vShaderStr, m_fShaderStrs[m_ShaderIndex], m_VertextShader, m_FragmentShader);
                if (!m_ProgramObj) {
                    GLUtils::CheckGLError("Create Program");
                    LOGCATE("EGLRender::SetIntParams Could not create program");
                    return;
                }
                m_SamplerLoc = glGetUniformLocation(m_ProgramObj,"s_TextureMap");
                GO_CHECK_GL_ERROR();
                m_TexSizeLoc = glGetUniformLocation(m_ProgramObj, "u_texSize");
                GO_CHECK_GL_ERROR();
            }
            break;
        }
        default:
            break;
    }
}

void EGLRender::Draw(){
    LOGCATD("EGLRender::Draw");
    if (m_ProgramObj == GL_NONE) return;
    glViewport(0, 0, m_RenderImage.width, m_RenderImage.height);
    //do FBO of Screen Rendering
    glUseProgram(m_ProgramObj);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);

    glBindVertexArray(m_VaoIds[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glUniform1i(m_SamplerLoc, 0);

    if (m_TexSizeLoc > -1) {
        GLfloat size[2];
        size[0] = m_RenderImage.width;
        size[1] = m_RenderImage.height;
        glUniform2f(m_TexSizeLoc, size[0], size[1]);
    }

    //render
    GO_CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    //if unbind FBO, may not be able use readPixels API
//    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void EGLRender::UnInit() {
    LOGCATD("EGLRender::UnInit");
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        m_ProgramObj = GL_NONE;
    }
    if (m_ImageTextureId) {
        glDeleteTextures(1, &m_ImageTextureId);
        m_ImageTextureId = GL_NONE;
    }
    if (m_FboTextureId) {
        glDeleteTextures(1, &m_FboTextureId);
        m_FboTextureId = GL_NONE;
    }
    if (m_VboIds[0]) {
        glDeleteBuffers(3, m_VboIds);
        m_VboIds[0] = GL_NONE;
        m_VboIds[1] = GL_NONE;
        m_VboIds[2] = GL_NONE;
    }
    if (m_VaoIds[0]) {
        glDeleteVertexArrays(1, m_VaoIds);
        m_VaoIds[0] = GL_NONE;
    }
    if (m_FboId) {
        glDeleteFramebuffers(1, &m_FboId);
        m_FboId = GL_NONE;
    }
    if (m_IsGLContextReady) {
        DestroyGlesEnv();
        m_IsGLContextReady = false;
    }
}

void EGLRender::DestroyGlesEnv() {
    //release EGLenv
    if (m_eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(m_eglDisplay, m_eglCtx);
        eglDestroySurface(m_eglDisplay, m_eglSurface);
        eglReleaseThread();
        eglTerminate(m_eglDisplay);
    }
    m_eglDisplay = EGL_NO_DISPLAY;
    m_eglSurface = EGL_NO_SURFACE;
    m_eglCtx = EGL_NO_CONTEXT;
}