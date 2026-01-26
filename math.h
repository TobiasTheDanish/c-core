#ifndef CORE_MATH_H
#define CORE_MATH_H
#include "common.h"

#define M_PI 3.14159265358979323846
#define CORE_IdentityM4f                                                       \
  ((M4f){.values = {                                                           \
             1.0f,                                                             \
             0.0f,                                                             \
             0.0f,                                                             \
             0.0f,                                                             \
             0.0f,                                                             \
             1.0f,                                                             \
             0.0f,                                                             \
             0.0f,                                                             \
             0.0f,                                                             \
             0.0f,                                                             \
             1.0f,                                                             \
             0.0f,                                                             \
             0.0f,                                                             \
             0.0f,                                                             \
             0.0f,                                                             \
             1.0f,                                                             \
         }})

float CORE_DegreesToRadians(float degrees);
float CORE_RadiansToDegree(float radians);

M4f CORE_PerspectiveMatrix(float fov, float aspect, float near, float far);

V3 CORE_V3Mul(V3 v, V3 o);
V3 CORE_V3MulScalar(V3 v, float scalar);

V3 CORE_V3Add(V3 v, V3 o);
V3 CORE_V3AddScalar(V3 v, float scalar);

M4f CORE_MultiplyM4f(M4f mat, M4f other);

M4f CORE_TranslationMatrix(V3 translation);
M4f CORE_RotationMatrixX(float rotation);
M4f CORE_RotationMatrixY(float rotation);
M4f CORE_RotationMatrixZ(float rotation);
M4f CORE_ScalingMatrix(V3 scale);

void CORE_TranslateMatrix(M4f *matrix, V3 translation);
void CORE_TranslateMatrixX(M4f *matrix, float translation);
void CORE_TranslateMatrixY(M4f *matrix, float translation);
void CORE_TranslateMatrixZ(M4f *matrix, float translation);

void CORE_RotateMatrixX(M4f *matrix, float rotation);
void CORE_RotateMatrixY(M4f *matrix, float rotation);
void CORE_RotateMatrixZ(M4f *matrix, float rotation);

void CORE_ScaleMatrix(M4f *matrix, V3 scale);
void CORE_ScaleMatrixX(M4f *matrix, float scale);
void CORE_ScaleMatrixY(M4f *matrix, float scale);
void CORE_ScaleMatrixZ(M4f *matrix, float scale);

#ifdef CORE_MATH_IMPLEMENTATION
#include <math.h>

float CORE_DegreesToRadians(float degrees) { return degrees * (M_PI / 180.f); }
float CORE_RadiansToDegree(float radians) { return radians * (180.f / M_PI); }

V3 CORE_V3Mul(V3 v, V3 o) {
  return (V3){
      .x = v.x * o.x,
      .y = v.y * o.y,
      .z = v.z * o.z,
  };
}
V3 CORE_V3MulScalar(V3 v, float scalar) {
  return CORE_V3Mul(v, (V3){.x = scalar, .y = scalar, .z = scalar});
}

V3 CORE_V3Add(V3 v, V3 o) {
  return (V3){
      .x = v.x + o.x,
      .y = v.y + o.y,
      .z = v.z + o.z,
  };
}
V3 CORE_V3AddScalar(V3 v, float scalar) {
  return CORE_V3Add(v, (V3){.x = scalar, .y = scalar, .z = scalar});
}

M4f CORE_MultiplyM4f(M4f mat, M4f other) {
  M4f result = {0};

  for (int32_t y = 0; y < 4; y++) {
    for (int32_t x = 0; x < 4; x++) {
      result.values[y * 4 + x] = 0;

      for (int32_t i = 0; i < 4; i++) {
        result.values[y * 4 + x] +=
            mat.values[(y * 4) + i] * other.values[(i * 4) + x];
      }
    }
  }

  return result;
}

M4f CORE_TranslationMatrix(V3 translation) {
  M4f res = CORE_IdentityM4f;

  res._03 = translation.x;
  res._13 = translation.y;
  res._23 = translation.z;

  return res;
}

M4f CORE_ScalingMatrix(V3 scale) {
  M4f res = CORE_IdentityM4f;

  res._00 += scale.x;
  res._11 += scale.y;
  res._22 += scale.z;

  return res;
}

void CORE_TranslateMatrixX(M4f *matrix, float translation) {
  CORE_TranslateMatrix(matrix, (V3){.x = translation, .y = 0.f, .z = 0.f});
}

void CORE_TranslateMatrixY(M4f *matrix, float translation) {
  CORE_TranslateMatrix(matrix, (V3){.x = 0.f, .y = translation, .z = 0.f});
}

void CORE_TranslateMatrixZ(M4f *matrix, float translation) {
  CORE_TranslateMatrix(matrix, (V3){.x = 0.f, .y = 0.f, .z = translation});
}

void CORE_TranslateMatrix(M4f *matrix, V3 translation) {
  M4f translationMatrix = CORE_TranslationMatrix(translation);
  *matrix = CORE_MultiplyM4f(translationMatrix, *matrix);
}

void CORE_ScaleMatrix(M4f *matrix, V3 scale) {
  M4f scaleMatrix = CORE_ScalingMatrix(scale);
  *matrix = CORE_MultiplyM4f(scaleMatrix, *matrix);
}
void CORE_ScaleMatrixX(M4f *matrix, float scale) {
  CORE_ScaleMatrix(matrix, (V3){.x = scale, .y = 0.f, .z = 0.f});
}

void CORE_ScaleMatrixY(M4f *matrix, float scale) {
  CORE_ScaleMatrix(matrix, (V3){.x = 0.f, .y = scale, .z = 0.f});
}

void CORE_ScaleMatrixZ(M4f *matrix, float scale) {
  CORE_ScaleMatrix(matrix, (V3){.x = scale, .y = 0.f, .z = scale});
}

M4f CORE_RotationMatrixX(float rotation) {
  M4f res = CORE_IdentityM4f;

  float cosine = cosf(rotation);
  float sine = sinf(rotation);

  res._11 = cosine;
  res._12 = -sine;
  res._21 = sine;
  res._22 = cosine;

  return res;
}

M4f CORE_RotationMatrixY(float rotation) {
  M4f res = CORE_IdentityM4f;

  float cosine = cosf(rotation);
  float sine = sinf(rotation);

  res._00 = cosine;
  res._02 = sine;
  res._20 = -sine;
  res._22 = cosine;

  return res;
}

M4f CORE_RotationMatrixZ(float rotation) {
  M4f res = CORE_IdentityM4f;

  float cosine = cosf(rotation);
  float sine = sinf(rotation);

  res._00 = cosine;
  res._01 = -sine;
  res._10 = sine;
  res._11 = cosine;

  return res;
}

void CORE_RotateMatrixX(M4f *matrix, float rotation) {
  M4f rotMat = CORE_RotationMatrixX(rotation);
  *matrix = CORE_MultiplyM4f(rotMat, *matrix);
}

void CORE_RotateMatrixY(M4f *matrix, float rotation) {
  M4f rotMat = CORE_RotationMatrixY(rotation);
  *matrix = CORE_MultiplyM4f(rotMat, *matrix);
}

void CORE_RotateMatrixZ(M4f *matrix, float rotation) {
  M4f rotMat = CORE_RotationMatrixZ(rotation);
  *matrix = CORE_MultiplyM4f(rotMat, *matrix);
}

M4f CORE_PerspectiveMatrix(float fov, float aspect, float near, float far) {
  M4f outMatrix = {0};
  float top, bottom, right, left;
  float scale = tanf(fov * 0.5f * M_PI / 180.f) * near;
  top = scale, bottom = -top;
  right = aspect * scale, left = -right;

  // column 1
  outMatrix.values[0] = (2 * near) / (right - left);
  outMatrix.values[1] = 0.f;
  outMatrix.values[2] = 0.f;
  outMatrix.values[3] = 0.f;

  // column 2
  outMatrix.values[4] = 0.f;
  outMatrix.values[5] = (2 * near) / (top - bottom);
  outMatrix.values[6] = 0.f;
  outMatrix.values[7] = 0.f;

  // column 3
  outMatrix.values[8] = (right + left) / (right - left);
  outMatrix.values[9] = (top + bottom) / (top - bottom);
  outMatrix.values[10] = -((far + near) / (far - near));
  outMatrix.values[11] = -1.0f;

  // column 4
  outMatrix.values[12] = 0.f;
  outMatrix.values[13] = 0.f;
  outMatrix.values[14] = -((2 * far * near) / (far - near));
  outMatrix.values[15] = 0.f;

  return outMatrix;
}

#endif // CORE_MATH_IMPLEMENTATION
#endif // !CORE_MATH_H
