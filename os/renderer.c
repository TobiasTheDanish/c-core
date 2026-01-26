#ifndef OS_RENDERER_C
#define OS_RENDERER_C

#include "../allocator.h"
#include "../os.h"
#include "opengl.c"
#include <GL/gl.h>

GLenum shaderTypeMap[OS_ShaderTypeCount] = {
    [OS_VertexShader] = GL_VERTEX_SHADER,
    [OS_FragmentShader] = GL_FRAGMENT_SHADER,
};

Bool OS_LoadShaderProgram(OS_ShaderProgram *program, OS_ShaderSource *sources,
                          int32_t sourceCount) {
  if (sourceCount == 0) {
    return false;
  }

  uint32_t *glShaders = AllocMemory(sourceCount * sizeof(int32_t));
  for (int32_t i = 0; i < sourceCount; i++) {
    OS_ShaderSource s = sources[i];
    if (!compile_shader_source(s.source, shaderTypeMap[s.type],
                               &(glShaders[i]))) {
      return false;
    }
  }
  *program = glCreateProgram();

  // Attach shaders
  for (int32_t i = 0; i < sourceCount; i++) {
    glAttachShader(*program, glShaders[i]);
  }

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

  // Delete shaders
  for (int32_t i = 0; i < sourceCount; i++) {
    glDeleteShader(glShaders[i]);
  }

  return true;
}

void OS_ActivateShader(OS_ShaderProgram p) { glUseProgram(p); }

int32_t OS_GetUniformLocation(OS_ShaderProgram p, char *name) {
  return glGetUniformLocation(p, name);
}

OS_ShaderAttribute OS_GetShaderAttribute(OS_ShaderProgram p, char *name) {
  return glGetAttribLocation(p, name);
}

void OS_SetShaderAttribute(OS_ShaderAttribute a, int32_t count, GLenum type,
                           int32_t stride, void *data) {
  glVertexAttribPointer(a, count, type, false, stride, data);
  glEnableVertexAttribArray(a);
}

void OS_SetShaderAttribute3f(OS_ShaderAttribute a, int32_t stride, void *data) {
  OS_SetShaderAttribute(a, 3, GL_FLOAT, stride, data);
}

void OS_SetShaderAttribute2f(OS_ShaderAttribute a, int32_t stride, void *data) {
  OS_SetShaderAttribute(a, 2, GL_FLOAT, stride, data);
}

void OS_SetUniformInt(OS_ShaderProgram p, char *name, int32_t value) {
  int32_t pos = OS_GetUniformLocation(p, name);
  glUniform1i(pos, value);
}

void OS_SetUniformVec3(OS_ShaderProgram p, char *name, V3 value) {
  int32_t pos = OS_GetUniformLocation(p, name);
  glUniform3f(pos, value.x, value.y, value.z);
}

void OS_SetUniformMat4f(OS_ShaderProgram p, char *name, M4f value) {
  int32_t pos = OS_GetUniformLocation(p, name);
  glUniformMatrix4fv(pos, 1, false, value.values);
}

void OS_DrawIndices(OS_VertexIndex *indices, uint32_t count) {
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, indices);
}

void OS_RendererInit() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void OS_RenderBegin(OS_Event *events, int32_t count) {
  for (int32_t i = 0; i < count; i++) {
    OS_Event e = events[i];
    if (e.kind == OS_EventKind_WindowResize) {
      glViewport(0, 0, e.resize.width, e.resize.height);
    }
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OS_RenderEnd(OS_Window *w) {
#ifdef RGFW_OPENGL
  RGFW_window_swapBuffers(w->w);
#endif
}

OS_Texture OS_LoadTexture(OS_ShaderProgram p, int32_t width, int32_t height,
                          uint8_t *data) {
  OS_Texture t = {
      .texture = 0,
      .width = width,
      .height = height,
      .data = data,
  };

  glGenTextures(1, &t.texture);
  glBindTexture(GL_TEXTURE_2D, t.texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  return t;
}

void OS_UpdateTexture(OS_Texture *tex, int32_t width, int32_t height,
                      uint8_t *data) {
  if (data) {
    glBindTexture(GL_TEXTURE_2D, tex->texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    tex->width = width;
    tex->height = height;
    tex->data = data;
  }
}

#endif /* ifndef OS_RENDERER_C */
