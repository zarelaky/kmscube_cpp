/*
 * Copyright (c) 2012 Arvin Schnell <arvin.schnell@gmail.com>
 * Copyright (c) 2012 Rob Clark <rob@ti.com>
 * Copyright (c) 2013 Anand Balagopalakrishnan <anandb@ti.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/* Based on a egl cube test app originally written by Arvin Schnell */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "kms.h"
#include "esUtil.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define MAX_DISPLAYS 	(4)
uint8_t DISP_ID = 0;
uint8_t all_display = 0;
int8_t connector_id = -1;
KMS* kms = NULL;
static struct {
	EGLDisplay display;
	EGLConfig config;
	EGLContext context;
	EGLSurface surface;
	GLuint program;
	GLint modelviewmatrix, modelviewprojectionmatrix, normalmatrix;
	GLuint vbo;
	GLuint positionsoffset, colorsoffset, normalsoffset;
	GLuint vertex_shader, fragment_shader;
} gl;

static int init_gl(KMS* kms)
{
	EGLint major, minor, n;
	GLint ret;

	static const GLfloat vVertices[] = {
			// front
			-1.0f, -1.0f, +1.0f, // point blue
			+1.0f, -1.0f, +1.0f, // point magenta
			-1.0f, +1.0f, +1.0f, // point cyan
			+1.0f, +1.0f, +1.0f, // point white
			// back
			+1.0f, -1.0f, -1.0f, // point red
			-1.0f, -1.0f, -1.0f, // point black
			+1.0f, +1.0f, -1.0f, // point yellow
			-1.0f, +1.0f, -1.0f, // point green
			// right
			+1.0f, -1.0f, +1.0f, // point magenta
			+1.0f, -1.0f, -1.0f, // point red
			+1.0f, +1.0f, +1.0f, // point white
			+1.0f, +1.0f, -1.0f, // point yellow
			// left
			-1.0f, -1.0f, -1.0f, // point black
			-1.0f, -1.0f, +1.0f, // point blue
			-1.0f, +1.0f, -1.0f, // point green
			-1.0f, +1.0f, +1.0f, // point cyan
			// top
			-1.0f, +1.0f, +1.0f, // point cyan
			+1.0f, +1.0f, +1.0f, // point white
			-1.0f, +1.0f, -1.0f, // point green
			+1.0f, +1.0f, -1.0f, // point yellow
			// bottom
			-1.0f, -1.0f, -1.0f, // point black
			+1.0f, -1.0f, -1.0f, // point red
			-1.0f, -1.0f, +1.0f, // point blue
			+1.0f, -1.0f, +1.0f  // point magenta
	};

	static const GLfloat vColors[] = {
			// front
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  0.0f,  1.0f, // magenta
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  1.0f,  1.0f, // white
			// back
			1.0f,  0.0f,  0.0f, // red
			0.0f,  0.0f,  0.0f, // black
			1.0f,  1.0f,  0.0f, // yellow
			0.0f,  1.0f,  0.0f, // green
			// right
			1.0f,  0.0f,  1.0f, // magenta
			1.0f,  0.0f,  0.0f, // red
			1.0f,  1.0f,  1.0f, // white
			1.0f,  1.0f,  0.0f, // yellow
			// left
			0.0f,  0.0f,  0.0f, // black
			0.0f,  0.0f,  1.0f, // blue
			0.0f,  1.0f,  0.0f, // green
			0.0f,  1.0f,  1.0f, // cyan
			// top
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  1.0f,  1.0f, // white
			0.0f,  1.0f,  0.0f, // green
			1.0f,  1.0f,  0.0f, // yellow
			// bottom
			0.0f,  0.0f,  0.0f, // black
			1.0f,  0.0f,  0.0f, // red
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  0.0f,  1.0f  // magenta
	};

	static const GLfloat vNormals[] = {
			// front
			+0.0f, +0.0f, +1.0f, // forward
			+0.0f, +0.0f, +1.0f, // forward
			+0.0f, +0.0f, +1.0f, // forward
			+0.0f, +0.0f, +1.0f, // forward
			// back
			+0.0f, +0.0f, -1.0f, // backbard
			+0.0f, +0.0f, -1.0f, // backbard
			+0.0f, +0.0f, -1.0f, // backbard
			+0.0f, +0.0f, -1.0f, // backbard
			// right
			+1.0f, +0.0f, +0.0f, // right
			+1.0f, +0.0f, +0.0f, // right
			+1.0f, +0.0f, +0.0f, // right
			+1.0f, +0.0f, +0.0f, // right
			// left
			-1.0f, +0.0f, +0.0f, // left
			-1.0f, +0.0f, +0.0f, // left
			-1.0f, +0.0f, +0.0f, // left
			-1.0f, +0.0f, +0.0f, // left
			// top
			+0.0f, +1.0f, +0.0f, // up
			+0.0f, +1.0f, +0.0f, // up
			+0.0f, +1.0f, +0.0f, // up
			+0.0f, +1.0f, +0.0f, // up
			// bottom
			+0.0f, -1.0f, +0.0f, // down
			+0.0f, -1.0f, +0.0f, // down
			+0.0f, -1.0f, +0.0f, // down
			+0.0f, -1.0f, +0.0f  // down
	};

	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	static const EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 0,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	static const char *vertex_shader_source =
			"uniform mat4 modelviewMatrix;      \n"
			"uniform mat4 modelviewprojectionMatrix;\n"
			"uniform mat3 normalMatrix;         \n"
			"                                   \n"
			"attribute vec4 in_position;        \n"
			"attribute vec3 in_normal;          \n"
			"attribute vec4 in_color;           \n"
			"\n"
			"vec4 lightSource = vec4(2.0, 2.0, 20.0, 0.0);\n"
			"                                   \n"
			"varying vec4 vVaryingColor;        \n"
			"                                   \n"
			"void main()                        \n"
			"{                                  \n"
			"    gl_Position = modelviewprojectionMatrix * in_position;\n"
			"    vec3 vEyeNormal = normalMatrix * in_normal;\n"
			"    vec4 vPosition4 = modelviewMatrix * in_position;\n"
			"    vec3 vPosition3 = vPosition4.xyz / vPosition4.w;\n"
			"    vec3 vLightDir = normalize(lightSource.xyz - vPosition3);\n"
			"    float diff = max(0.0, dot(vEyeNormal, vLightDir));\n"
			"    vVaryingColor = vec4(diff * in_color.rgb, 1.0);\n"
			"}                                  \n";

	static const char *fragment_shader_source =
			"precision mediump float;           \n"
			"                                   \n"
			"varying vec4 vVaryingColor;        \n"
			"                                   \n"
			"void main()                        \n"
			"{                                  \n"
			"    gl_FragColor = vVaryingColor;  \n"
			"}                                  \n";

	gl.display = eglGetDisplay(kms->getNativeDisplay());

	if (!eglInitialize(gl.display, &major, &minor)) {
		printf("failed to initialize\n");
		return -1;
	}

	printf("Using display %p with EGL version %d.%d\n",
			gl.display, major, minor);

	printf("EGL Version \"%s\"\n", eglQueryString(gl.display, EGL_VERSION));
	printf("EGL Vendor \"%s\"\n", eglQueryString(gl.display, EGL_VENDOR));
	printf("EGL Extensions \"%s\"\n", eglQueryString(gl.display, EGL_EXTENSIONS));

	if (!eglBindAPI(EGL_OPENGL_ES_API)) {
		printf("failed to bind api EGL_OPENGL_ES_API\n");
		return -1;
	}

	if (!eglChooseConfig(gl.display, config_attribs, &gl.config, 1, &n) || n != 1) {
		printf("failed to choose config: %d\n", n);
		return -1;
	}

	gl.context = eglCreateContext(gl.display, gl.config,
			EGL_NO_CONTEXT, context_attribs);
	if (gl.context == NULL) {
		printf("failed to create context\n");
		return -1;
	}

	gl.surface = eglCreateWindowSurface(gl.display, gl.config, kms->getNativeWindow(), NULL);
	if (gl.surface == EGL_NO_SURFACE) {
		printf("failed to create egl surface\n");
		return -1;
	}

	/* connect the context to the surface */
	eglMakeCurrent(gl.display, gl.surface, gl.surface, gl.context);


	gl.vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(gl.vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(gl.vertex_shader);

	glGetShaderiv(gl.vertex_shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char *log = NULL;

		printf("vertex shader compilation failed!:\n");
		glGetShaderiv(gl.vertex_shader, GL_INFO_LOG_LENGTH, &ret);
		if (ret > 1) {
			log = (char*)malloc(ret);
			glGetShaderInfoLog(gl.vertex_shader, ret, NULL, log);
			printf("%s", log);
		}
        free(log);

		return -1;
	}

	gl.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(gl.fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(gl.fragment_shader);

	glGetShaderiv(gl.fragment_shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char *log = NULL;

		printf("fragment shader compilation failed!:\n");
		glGetShaderiv(gl.fragment_shader, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = (char*)malloc(ret);
			glGetShaderInfoLog(gl.fragment_shader, ret, NULL, log);
			printf("%s", log);
		}
        free(log);
		return -1;
	}

	gl.program = glCreateProgram();

	glAttachShader(gl.program, gl.vertex_shader);
	glAttachShader(gl.program, gl.fragment_shader);

	glBindAttribLocation(gl.program, 0, "in_position");
	glBindAttribLocation(gl.program, 1, "in_normal");
	glBindAttribLocation(gl.program, 2, "in_color");

	glLinkProgram(gl.program);

	glGetProgramiv(gl.program, GL_LINK_STATUS, &ret);
	if (!ret) {
		char *log = NULL;

		printf("program linking failed!:\n");
		glGetProgramiv(gl.program, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = (char*)malloc(ret);
			glGetProgramInfoLog(gl.program, ret, NULL, log);
			printf("%s", log);
		}
        free(log);
		return -1;
	}

	glUseProgram(gl.program);

	gl.modelviewmatrix = glGetUniformLocation(gl.program, "modelviewMatrix");
	gl.modelviewprojectionmatrix = glGetUniformLocation(gl.program, "modelviewprojectionMatrix");
	gl.normalmatrix = glGetUniformLocation(gl.program, "normalMatrix");

	glViewport(0, 0, kms->getDisplayWidth(), kms->getDisplayHeight());
	glEnable(GL_CULL_FACE);

	gl.positionsoffset = 0;
	gl.colorsoffset = sizeof(vVertices);
	gl.normalsoffset = sizeof(vVertices) + sizeof(vColors);
	glGenBuffers(1, &gl.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices) + sizeof(vColors) + sizeof(vNormals), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, gl.positionsoffset, sizeof(vVertices), &vVertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, gl.colorsoffset, sizeof(vColors), &vColors[0]);
	glBufferSubData(GL_ARRAY_BUFFER, gl.normalsoffset, sizeof(vNormals), &vNormals[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)gl.positionsoffset);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)gl.normalsoffset);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)gl.colorsoffset);
	glEnableVertexAttribArray(2);

	return 0;
}


static void exit_gl(void)
{
        glDeleteProgram(gl.program);
        glDeleteBuffers(1, &gl.vbo);
        glDeleteShader(gl.fragment_shader);
        glDeleteShader(gl.vertex_shader);
        eglDestroySurface(gl.display, gl.surface);
        eglDestroyContext(gl.display, gl.context);
        eglTerminate(gl.display);
        return;
}


void cleanup_kmscube(void)
{
	exit_gl();
	printf("Cleanup of GL, GBM and DRM completed\n");
    if (kms) {
        kms->deinit();
        delete kms;

    }
}

static void draw(uint32_t i)
{
	ESMatrix modelview;

	/* clear the color buffer */
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	esMatrixLoadIdentity(&modelview);
	esTranslate(&modelview, 0.0f, 0.0f, -8.0f);
	esRotate(&modelview, 45.0f + (0.25f * i), 1.0f, 0.0f, 0.0f);
	esRotate(&modelview, 45.0f - (0.5f * i), 0.0f, 1.0f, 0.0f);
	esRotate(&modelview, 10.0f + (0.15f * i), 0.0f, 0.0f, 1.0f);
    GLfloat width   = kms->getDisplayWidth();
    GLfloat height  = kms->getDisplayHeight();

	GLfloat aspect = height/width;

	ESMatrix projection;
	esMatrixLoadIdentity(&projection);
	esFrustum(&projection, -2.8f, +2.8f, -2.8f * aspect, +2.8f * aspect, 6.0f, 10.0f);

	ESMatrix modelviewprojection;
	esMatrixLoadIdentity(&modelviewprojection);
	esMatrixMultiply(&modelviewprojection, &modelview, &projection);

	float normal[9];
	normal[0] = modelview.m[0][0];
	normal[1] = modelview.m[0][1];
	normal[2] = modelview.m[0][2];
	normal[3] = modelview.m[1][0];
	normal[4] = modelview.m[1][1];
	normal[5] = modelview.m[1][2];
	normal[6] = modelview.m[2][0];
	normal[7] = modelview.m[2][1];
	normal[8] = modelview.m[2][2];

	glUniformMatrix4fv(gl.modelviewmatrix, 1, GL_FALSE, &modelview.m[0][0]);
	glUniformMatrix4fv(gl.modelviewprojectionmatrix, 1, GL_FALSE, &modelviewprojection.m[0][0]);
	glUniformMatrix3fv(gl.normalmatrix, 1, GL_FALSE, normal);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
}


void print_usage()
{
	printf("Usage : kmscube <options>\n");
	printf("\t-h : Help\n");
	printf("\t-a : Enable all displays\n");
	printf("\t-c <id> : Display using connector_id [if not specified, use the first connected connector]\n");
	printf("\t-n <number> (optional): Number of frames to render\n");
}

int kms_signalhandler(int signum)
{
	switch(signum) {
	case SIGINT:
        case SIGTERM:
                /* Allow the pending page flip requests to be completed before
                 * the teardown sequence */
                sleep(1);
                printf("Handling signal number = %d\n", signum);
		cleanup_kmscube();
		break;
	default:
		printf("Unknown signal\n");
		break;
	}
	exit(1);
}

int main(int argc, char *argv[])
{
	uint32_t i = 0;
	int ret;
	int opt;
	int frame_count = -1;


	while ((opt = getopt(argc, argv, "ahc:n:")) != -1) {
		switch(opt) {
		case 'a':
			all_display = 1;
			break;

		case 'h':
			print_usage();
			return 0;

		case 'c':
			connector_id = atoi(optarg);
			break;
		case 'n':
			frame_count = atoi(optarg);
			break;


		default:
			printf("Undefined option %s\n", argv[optind]);
			print_usage();
			return -1;
		}
	}

	if (all_display) {
		printf("### Enabling all displays\n");
		connector_id = -1;
	}

    kms = new KMS("/dev/dri/card0");
    if(!kms->init()) {
        return -3;
    }

	ret = init_gl(kms);
	if (ret) {
		printf("failed to initialize EGL\n");
		return ret;
	}

	/* clear the color buffer */
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(gl.display, gl.surface);
    if (!kms->applyMode()) {
        printf("applyMode failed\n");
        return -1;
    }

	while (frame_count != 0) {

		draw(i++);

		eglSwapBuffers(gl.display, gl.surface);
        kms->flip();

        if(frame_count >= 0)
		frame_count--;
	}

	cleanup_kmscube();
	printf("\n Exiting kmscube \n");

	return ret;
}
