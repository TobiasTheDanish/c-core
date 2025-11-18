#include "../types.h"
#include <assert.h>
#include <stdio.h>

#define RGL_LOAD_IMPLEMENTATION
#include "rglLoad.h"

#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

const char *vert_shader_source =
    "#version 330\n"
    "precision mediump float;\n"
    "out vec2 uv;\n"
    "void main(void)\n"
    "{\n"
    "    uv.x = (gl_VertexID & 1);\n"
    "    uv.y = ((gl_VertexID >> 1) & 1);\n"
    "    vec2 pos = uv * 2.0 - 1.0;\n"
    "    gl_Position = vec4(pos.x, -pos.y, 0.0, 1.0);\n"
    "}\n";

const char *frag_shader_source = "#version 330\n"
                                 "precision mediump float;\n"
                                 "uniform sampler2D tex;\n"
                                 "in vec2 uv;\n"
                                 "out vec4 out_color;\n"
                                 "void main(void) {\n"
                                 "    out_color = texture(tex, uv);\n"
                                 "}\n";

const char *shader_type_as_cstr(GLuint shader) {
  switch (shader) {
  case GL_VERTEX_SHADER:
    return "GL_VERTEX_SHADER";
  case GL_FRAGMENT_SHADER:
    return "GL_FRAGMENT_SHADER";
  default:
    return "(Unknown)";
  }
}

Bool compile_shader_source(const GLchar *source, GLenum shader_type,
                           GLuint *shader) {
  *shader = glCreateShader(shader_type);
  glShaderSource(*shader, 1, &source, NULL);
  glCompileShader(*shader);

  GLint compiled = 0;
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);

  if (!compiled) {
    GLchar message[1024];
    GLsizei message_size = 0;
    glGetShaderInfoLog(*shader, sizeof(message), &message_size, message);
    fprintf(stderr, "ERROR: could not compile %s\n",
            shader_type_as_cstr(shader_type));
    fprintf(stderr, "%.*s\n", message_size, message);
    return false;
  }

  return true;
}

Bool link_program(GLuint vert_shader, GLuint frag_shader, GLuint *program) {
  *program = glCreateProgram();

  glAttachShader(*program, vert_shader);
  glAttachShader(*program, frag_shader);
  glLinkProgram(*program);

  GLint linked = 0;
  glGetProgramiv(*program, GL_LINK_STATUS, &linked);
  if (!linked) {
    GLsizei message_size = 0;
    GLchar message[1024];

    glGetProgramInfoLog(*program, sizeof(message), &message_size, message);
    fprintf(stderr, "Program Linking: %.*s\n", message_size, message);
    return false;
  }

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  return true;
}

GLint tex_uni;
GLint textureUnit = GL_TEXTURE0;
GLuint vao;

void texture_copy(GLint texture_unit) {
  glUniform1i(tex_uni, texture_unit);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void init_texture() {
  GLuint texture;
  glActiveTexture(textureUnit);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

void OPENGL_init() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GLuint vert_shader;
  assert(compile_shader_source(vert_shader_source, GL_VERTEX_SHADER,
                               &vert_shader));
  GLuint frag_shader;
  assert(compile_shader_source(frag_shader_source, GL_FRAGMENT_SHADER,
                               &frag_shader));
  GLuint program;
  assert(link_program(vert_shader, frag_shader, &program));
  glUseProgram(program);

  tex_uni = glGetUniformLocation(program, "tex");

  init_texture();

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
}

void OPENGL_RenderBitmap(Bitmap b) {
  glActiveTexture(textureUnit);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, b.width, b.height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, b.data);

  texture_copy(textureUnit);
}
