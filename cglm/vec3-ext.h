/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*!
 * @brief SIMD like functions
 */

/*
 Functions:
   CGLM_INLINE void  glm_vec_mulv(vec3 a, vec3 b, vec3 d);
   CGLM_INLINE void  glm_vec_broadcast(float val, vec3 d);
   CGLM_INLINE bool  glm_vec_eq(vec3 v, float val);
   CGLM_INLINE bool  glm_vec_eq_eps(vec3 v, float val);
   CGLM_INLINE bool  glm_vec_eq_all(vec3 v);
   CGLM_INLINE bool  glm_vec_eqv(vec3 v1, vec3 v2);
   CGLM_INLINE bool  glm_vec_eqv_eps(vec3 v1, vec3 v2);
   CGLM_INLINE float glm_vec_max(vec3 v);
   CGLM_INLINE float glm_vec_min(vec3 v);
 */

#ifndef cglm_vec3_ext_h
#define cglm_vec3_ext_h

#include "common.h"
#include "util.h"
#include <stdbool.h>
#include <math.h>
#include <float.h>

/*!
 * @brief DEPRECATED! use glm_vec_mul
 *
 * @param[in]  a vec1
 * @param[in]  b vec2
 * @param[out] d vec3 = (v1[0] * v2[0],  v1[1] * v2[1], v1[2] * v2[2])
 */
CGLM_INLINE
void
glm_vec_mulv(vec3 a, vec3 b, vec3 d) {
  d[0] = a[0] * b[0];
  d[1] = a[1] * b[1];
  d[2] = a[2] * b[2];
}

/*!
 * @brief fill a vector with specified value
 *
 * @param[in]  val value
 * @param[out] d   dest
 */
CGLM_INLINE
void
glm_vec_broadcast(float val, vec3 d) {
  d[0] = d[1] = d[2] = val;
}

/*!
 * @brief check if vector is equal to value (without epsilon)
 *
 * @param[in] v   vector
 * @param[in] val value
 */
CGLM_INLINE
bool
glm_vec_eq(vec3 v, float val) {
  return v[0] == val && v[0] == v[1] && v[0] == v[2];
}

/*!
 * @brief check if vector is equal to value (with epsilon)
 *
 * @param[in] v   vector
 * @param[in] val value
 */
CGLM_INLINE
bool
glm_vec_eq_eps(vec3 v, float val) {
  return fabsf(v[0] - val) <= FLT_EPSILON
         && fabsf(v[1] - val) <= FLT_EPSILON
         && fabsf(v[2] - val) <= FLT_EPSILON;
}

/*!
 * @brief check if vectors members are equal (without epsilon)
 *
 * @param[in] v   vector
 */
CGLM_INLINE
bool
glm_vec_eq_all(vec3 v) {
  return v[0] == v[1] && v[0] == v[2];
}

/*!
 * @brief check if vector is equal to another (without epsilon)
 *
 * @param[in] v1 vector
 * @param[in] v2 vector
 */
CGLM_INLINE
bool
glm_vec_eqv(vec3 v1, vec3 v2) {
  return v1[0] == v2[0]
        && v1[1] == v2[1]
        && v1[2] == v2[2];
}

/*!
 * @brief check if vector is equal to another (with epsilon)
 *
 * @param[in] v1 vector
 * @param[in] v2 vector
 */
CGLM_INLINE
bool
glm_vec_eqv_eps(vec3 v1, vec3 v2) {
  return fabsf(v1[0] - v2[0]) <= FLT_EPSILON
         && fabsf(v1[1] - v2[1]) <= FLT_EPSILON
         && fabsf(v1[2] - v2[2]) <= FLT_EPSILON;
}

/*!
 * @brief max value of vector
 *
 * @param[in] v vector
 */
CGLM_INLINE
float
glm_vec_max(vec3 v) {
  float max;

  max = v[0];
  if (v[1] > max)
    max = v[1];
  if (v[2] > max)
    max = v[2];

  return max;
}

/*!
 * @brief min value of vector
 *
 * @param[in] v vector
 */
CGLM_INLINE
float
glm_vec_min(vec3 v) {
  float min;

  min = v[0];
  if (v[1] < min)
    min = v[1];
  if (v[2] < min)
    min = v[2];

  return min;
}

/*!
 * @brief check if all items are NaN (not a number)
 *        you should only use this in DEBUG mode or very critical asserts
 *
 * @param[in] v vector
 */
CGLM_INLINE
bool
glm_vec_isnan(vec3 v) {
  return isnan(v[0]) || isnan(v[1]) || isnan(v[2]);
}

/*!
 * @brief check if all items are INFINITY
 *        you should only use this in DEBUG mode or very critical asserts
 *
 * @param[in] v vector
 */
CGLM_INLINE
bool
glm_vec_isinf(vec3 v) {
  return isinf(v[0]) || isinf(v[1]) || isinf(v[2]);
}

/*!
 * @brief check if all items are valid number
 *        you should only use this in DEBUG mode or very critical asserts
 *
 * @param[in] v vector
 */
CGLM_INLINE
bool
glm_vec_isvalid(vec3 v) {
  return !glm_vec_isnan(v) && !glm_vec_isinf(v);
}

/*!
 * @brief get sign of 32 bit float as +1, -1, 0
 *
 * Important: It returns 0 for zero/NaN input
 *
 * @param v vector
 */
CGLM_INLINE
void
glm_vec_sign(vec3 v, vec3 dest) {
  dest[0] = glm_signf(v[0]);
  dest[1] = glm_signf(v[1]);
  dest[2] = glm_signf(v[2]);
}

/*!
 * @brief square root of each vector item
 *
 * @param[in]  v    vector
 * @param[out] dest destination vector
 */
CGLM_INLINE
void
glm_vec_sqrt(vec3 v, vec3 dest) {
  dest[0] = sqrtf(v[0]);
  dest[1] = sqrtf(v[1]);
  dest[2] = sqrtf(v[2]);
}

#endif /* cglm_vec3_ext_h */
