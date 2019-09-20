#define X11

#include <cstdio>
#include "were_object.h"
#include "were_thread.h"
#include "were_timer.h"
#include "were_backtrace.h"
#ifdef X11
#include "were1_xcb.h"
#endif
#include "were_exception.h"
#include "were_log.h"
#include "were_debug.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
//#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>

#define WIDTH 1280
#define HEIGHT 720
#define BUFFER_SIZE (WIDTH * HEIGHT * 4)
#define FMT GL_RGBA


static const char simpleVS[] =
        "attribute vec4 position;\n"
        "attribute vec2 texCoords;\n"
        "varying vec2 outTexCoords;\n"
        "\nvoid main(void) {\n"
        "    outTexCoords = texCoords;\n"
        "    gl_Position = position;\n"
        "}\n\n";

static const char simpleFS[] =
        "precision mediump float;\n\n"
        "varying vec2 outTexCoords;\n"
        "uniform sampler2D texture;\n"
#ifdef USE_BLENDING
        "uniform float alpha;\n"
#endif
        "\nvoid main(void) {\n"
        "    gl_FragColor = texture2D(texture, outTexCoords);\n"
#ifdef USE_BLENDING
        "    gl_FragColor.a = alpha;\n"
#endif
        "}\n\n";


const GLint FLOAT_SIZE_BYTES = sizeof(float);
const GLint TRIANGLE_VERTICES_DATA_STRIDE_BYTES = 5 * FLOAT_SIZE_BYTES;


static GLuint load_shader(GLenum shaderType, const char *pSource)
{
    GLuint shader = glCreateShader(shaderType);
    if (!shader)
        throw were_exception(WE_SIMPLE);

    glShaderSource(shader, 1, &pSource, NULL);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE)
        throw were_exception(WE_SIMPLE);

    return shader;
}


class test : public were_object
{
public:

    ~test()
    {
        glDeleteTextures(1, &texture_);

        glDeleteProgram(texture_program_);
        glDeleteShader(pixel_shader_);
        glDeleteShader(vertex_shader_);

        eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(egl_display_, egl_context_);
        eglDestroySurface(egl_display_, egl_surface_);
        eglTerminate(egl_display_);
    }

    test()
    {
        MAKE_THIS_WOP

#ifdef X11
        display_ = were1_xcb_display_open();
        window_ = were1_xcb_window_create(display_, WIDTH, HEIGHT);
#endif


        egl_display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (egl_display_ == EGL_NO_DISPLAY)
            throw were_exception(WE_SIMPLE);

        EGLint major_version, minor_version;

        if (eglInitialize(egl_display_, &major_version, &minor_version) != EGL_TRUE)
            throw were_exception(WE_SIMPLE);

        were_log("EGL_VERSION = %s\n",       eglQueryString(egl_display_, EGL_VERSION));
        were_log("EGL_VENDOR = %s\n",        eglQueryString(egl_display_, EGL_VENDOR));
        were_log("EGL_CLIENT_APIS = %s\n",   eglQueryString(egl_display_, EGL_CLIENT_APIS));
        were_log("EGL_EXTENSIONS = %s\n",    eglQueryString(egl_display_, EGL_EXTENSIONS));

        const EGLint config_attribs[] = {
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_NONE};

        EGLint num_configs;

        if (eglChooseConfig(egl_display_, config_attribs, &egl_config_, 1, &num_configs) != EGL_TRUE)
            throw were_exception(WE_SIMPLE);

        eglBindAPI(EGL_OPENGL_ES_API);

        EGLint vid;
        if (eglGetConfigAttrib(egl_display_, egl_config_, EGL_NATIVE_VISUAL_ID, &vid) != EGL_TRUE)
            throw were_exception(WE_SIMPLE);

#ifdef X11
        egl_surface_ = eglCreateWindowSurface(egl_display_, egl_config_, window_->window, NULL);
#else
        EGLint attribs1[5];
        attribs1[0] = EGL_WIDTH;
        attribs1[1] = WIDTH;
        attribs1[2] = EGL_HEIGHT;
        attribs1[3] = HEIGHT;
        attribs1[4] = EGL_NONE;

        egl_surface_ = eglCreatePbufferSurface(egl_display_, egl_config_, attribs1);
#endif
        if (egl_surface_ == EGL_NO_SURFACE)
            throw were_exception(WE_SIMPLE);

        const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE};

        egl_context_ = eglCreateContext(egl_display_, egl_config_, EGL_NO_CONTEXT, context_attribs);
        if (egl_context_ == EGL_NO_CONTEXT)
            throw were_exception(WE_SIMPLE);

        if (eglMakeCurrent(egl_display_, egl_surface_, egl_surface_, egl_context_) != EGL_TRUE)
            throw were_exception(WE_SIMPLE);

        were_log("GL_VERSION = %s\n",    (char *) glGetString(GL_VERSION));
        were_log("GL_VENDOR = %s\n",     (char *) glGetString(GL_VENDOR));
        were_log("GL_RENDERER = %s\n",   (char *) glGetString(GL_RENDERER));
        were_log("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));


        vertex_shader_ = load_shader(GL_VERTEX_SHADER, simpleVS);
        pixel_shader_ = load_shader(GL_FRAGMENT_SHADER, simpleFS);

        texture_program_ = glCreateProgram();
        if (!texture_program_)
            throw were_exception(WE_SIMPLE);

        glAttachShader(texture_program_, vertex_shader_);
        glAttachShader(texture_program_, pixel_shader_);
        glLinkProgram(texture_program_);

        GLint link_status = GL_FALSE;
        glGetProgramiv(texture_program_, GL_LINK_STATUS, &link_status);
        if (link_status != GL_TRUE)
            throw were_exception(WE_SIMPLE);

        texture_position_handle_ = glGetAttribLocation(texture_program_, "position");
        texture_tex_coords_handle_ = glGetAttribLocation(texture_program_, "texCoords");
#ifdef USE_BLENDING
        texture_alpha_handle_ = glGetUniformLocation(texture_program_, "alpha");
#endif
        //texture_sampler_handle_ = glGetUniformLocation(texture_program_, "texture");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        eglQuerySurface(egl_display_, egl_surface_, EGL_WIDTH, &surface_width_);
        eglQuerySurface(egl_display_, egl_surface_, EGL_HEIGHT, &surface_height_);
        glViewport(0, 0, surface_width_, surface_height_);

        eglSwapInterval(egl_display_, 0);


        plane_[0] = -1.0f;
        plane_[1] = -1.0f;
        plane_[2] = 0.0f;
        plane_[3] = 0.0f;
        plane_[4] = 0.0f;

        plane_[5] = 1.0f;
        plane_[6] = -1.0f;
        plane_[7] = 0.0f;
        plane_[8] = 1.0f;
        plane_[9] = 0.0f;

        plane_[10] = -1.0f;
        plane_[11] = 1.0f;
        plane_[12] = 0.0;
        plane_[13] = 0.0f;
        plane_[14] = 1.0f;

        plane_[15] = 1.0f;
        plane_[16] = 1.0f;
        plane_[17] = 0.0f;
        plane_[18] = 1.0f;
        plane_[19] = 1.0f;



        glGenTextures(1, &texture_);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


        glGenBuffers(1, &pbo_);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, BUFFER_SIZE, 0, GL_STREAM_DRAW); // XXX1 dynamic
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        fprintf(stdout, "pbo %d\n", pbo_);


        data_ = malloc(BUFFER_SIZE);
        memset(data_, 255, BUFFER_SIZE);


        were_object_pointer<were_timer> tmr(new were_timer(1000 / 60));
        were_object::connect(tmr, &were_timer::timeout, this_wop, [this_wop]()
        {
            this_wop->timeout();
        });
        tmr->start();
    }

    void timeout()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

#if 0
        glClearColor(0.15f, 0.35f, 0.55f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
#endif

        glUseProgram(texture_program_);

        glVertexAttribPointer(texture_position_handle_, 3, GL_FLOAT, GL_FALSE, TRIANGLE_VERTICES_DATA_STRIDE_BYTES, &plane_[0]);
        glVertexAttribPointer(texture_tex_coords_handle_, 2, GL_FLOAT, GL_FALSE, TRIANGLE_VERTICES_DATA_STRIDE_BYTES, &plane_[3]);

        glEnableVertexAttribArray(texture_position_handle_);
        glEnableVertexAttribArray(texture_tex_coords_handle_);


#ifdef USE_BLENDING
            if (surface->alpha() != 1.0f)
            {
                glUniform1f(_gl->_textureAlphaHandle, surface->alpha());

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
#endif

            glBindTexture(GL_TEXTURE_2D, texture_); // XXX1 unbind


#if 1
            if (0)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, FMT, WIDTH, HEIGHT, 0, FMT, GL_UNSIGNED_BYTE, data_);
            }
            else
            {
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
                glBufferData(GL_PIXEL_UNPACK_BUFFER, BUFFER_SIZE, 0, GL_STREAM_DRAW);
                void *ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, BUFFER_SIZE, GL_MAP_WRITE_BIT);
                memcpy(ptr, data_, BUFFER_SIZE);
                glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
                glTexImage2D(GL_TEXTURE_2D, 0, FMT, WIDTH, HEIGHT, 0, FMT, GL_UNSIGNED_BYTE, NULL);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
            }
#endif




            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#ifdef USE_BLENDING
            if (surface->alpha() != 1.0f)
            {
                glDisable(GL_BLEND);
            }
#endif

        glDisableVertexAttribArray(texture_position_handle_);
        glDisableVertexAttribArray(texture_tex_coords_handle_);

        glFinish();

        eglSwapBuffers(egl_display_, egl_surface_);

        were_debug::instance().frame();
    }

private:
#ifdef X11
    struct were1_xcb_display *display_;
    struct were1_xcb_window *window_;
#endif

    EGLDisplay egl_display_;
    EGLConfig egl_config_;

    EGLSurface egl_surface_;
    EGLContext egl_context_;

    GLuint vertex_shader_;
    GLuint pixel_shader_;
    GLuint texture_program_;
    GLuint texture_position_handle_;
    GLuint texture_tex_coords_handle_;
#ifdef USE_BLENDING
    GLuint texture_alpha_handle_;
#endif
    //GLuint texture_sampler_handle_;

    int surface_width_;
    int surface_height_;

    float plane_[20];
    GLuint texture_;

    GLuint pbo_;

    void *data_;

};


#if 0
{
            static int gen = 0;
            if (gen == 0)
            {
                gen = 1;
                glGenBuffers(1, &pbo_id);
                glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
                glBufferData(GL_PIXEL_PACK_BUFFER, _eglut->window_width*_eglut->window_height*4, 0, GL_DYNAMIC_READ);
                glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
                fprintf(stdout, "pbo %d %d\n", pbo_id[0], pbo_id[1]);
            }


            //glBindFramebuffer(GL_FRAMEBUFFER, 0); // Needed?



            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
            glReadPixels(0, 0, _eglut->window_width, _eglut->window_height, GL_RGBA, GL_UNSIGNED_BYTE, 0);


            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
            GLubyte *ptr = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, _eglut->window_width*_eglut->window_height*4, GL_MAP_READ_BIT);
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}
#endif


int main(int argc, char *argv[])
{
    were_debug::instance().start();

    were_backtrace::instance().enable();

    {
        were_object_pointer<were_thread> t(new were_thread());
    }


    were_object_pointer<test> test1(new test());

    were_thread::current_thread()->run();

    return 0;
}

