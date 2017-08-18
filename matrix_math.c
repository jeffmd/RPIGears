/**
 * matrix_math.c
 */

#include <stdlib.h>
#include <math.h> 
#include <string.h>

#include "GLES/gl.h"

 
/** 
 * Copies a 4x4 matrix.
 * 
 * 
 * @param md the destination matrix
 * @param ms the matrix to copy
 */
void m4x4_copy(GLfloat *md, const GLfloat *ms)
{
   memcpy (md, ms, sizeof(GLfloat) * 16);
}

/** 
 * Multiplies two 4x4 matrices.
 * 
 * The result is stored in matrix m.
 * 
 * @param m the first matrix to multiply
 * @param n the second matrix to multiply
 */
void m4x4_multiply(GLfloat *m, const GLfloat *n)
{
   GLfloat tmp[16];
   const GLfloat *row, *column;
   div_t d;
   int i, j;

   for (i = 0; i < 16; i++) {
      tmp[i] = 0;
      d = div(i, 4);
      row = n + d.quot * 4;
      column = m + d.rem;
      for (j = 0; j < 4; j++)
         tmp[i] += row[j] * column[j * 4];
   }
   m4x4_copy(m, tmp);
}

/** 
 * Rotates a 4x4 matrix.
 * 
 * @param[in,out] m the matrix to rotate
 * @param angle the angle to rotate in degrees
 * @param x the x component of the direction to rotate to
 * @param y the y component of the direction to rotate to
 * @param z the z component of the direction to rotate to
 */
void m4x4_rotate(GLfloat *m, GLfloat angle, const GLfloat x, const GLfloat y, const GLfloat z)
{
   float s, c;
   
   angle = 2.0f * M_PI * angle / 360.0f;
   s = sinf(angle);
   c = cosf(angle);

   GLfloat r[16] = {
      x * x * (1 - c) + c,     y * x * (1 - c) + z * s, x * z * (1 - c) - y * s, 0,
      x * y * (1 - c) - z * s, y * y * (1 - c) + c,     y * z * (1 - c) + x * s, 0, 
      x * z * (1 - c) + y * s, y * z * (1 - c) - x * s, z * z * (1 - c) + c,     0,
      0, 0, 0, 1
   };

   m4x4_multiply(m, r);
}


/** 
 * Translates a 4x4 matrix.
 * 
 * @param[in,out] m the matrix to translate
 * @param x the x component of the direction to translate to
 * @param y the y component of the direction to translate to
 * @param z the z component of the direction to translate to
 */
void m4x4_translate(GLfloat *m, const GLfloat x, const GLfloat y, const GLfloat z)
{
   GLfloat t[16] = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  x, y, z, 1 };

   m4x4_multiply(m, t);
}

/** 
 * Creates an identity 4x4 matrix.
 * 
 * @param m the matrix make an identity matrix
 */
void m4x4_identity(GLfloat *m)
{
   static const GLfloat t[16] = {
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0,
   };

   m4x4_copy(m, t);
}

/** 
 * Transposes a 4x4 matrix.
 *
 * @param m the matrix to transpose
 */
void m4x4_transpose(GLfloat *m)
{
   const GLfloat t[16] = {
      m[0], m[4], m[8],  m[12],
      m[1], m[5], m[9],  m[13],
      m[2], m[6], m[10], m[14],
      m[3], m[7], m[11], m[15]};

   m4x4_copy(m, t);
}

/**
 * Inverts a 4x4 matrix.
 *
 * This function can currently handle only pure translation-rotation matrices.
 * Read http://www.gamedev.net/community/forums/topic.asp?topic_id=425118
 * for an explanation.
 */
void m4x4_invert(GLfloat *m)
{
   GLfloat t[16];
   m4x4_identity(t);

   // Extract and invert the translation part 't'. The inverse of a
   // translation matrix can be calculated by negating the translation
   // coordinates.
   t[12] = -m[12]; t[13] = -m[13]; t[14] = -m[14];

   // Invert the rotation part 'r'. The inverse of a rotation matrix is
   // equal to its transpose.
   m[12] = m[13] = m[14] = 0;
   m4x4_transpose(m);

   // inv(m) = inv(r) * inv(t)
   m4x4_multiply(m, t);
}

/** 
 * Calculate a perspective projection transformation.
 * 
 * @param m the matrix to save the transformation in
 * @param fovy the field of view in the y direction
 * @param aspect the view aspect ratio
 * @param zNear the near clipping plane
 * @param zFar the far clipping plane
 */
void m4x4_perspective(GLfloat *m, const GLfloat fovy, const GLfloat aspect, const GLfloat zNear, const GLfloat zFar)
{
   GLfloat tmp[16];
   m4x4_identity(tmp);

   float sine, cosine, cotangent, deltaZ;
   GLfloat radians = fovy / 2.0 * M_PI / 180.0;

   deltaZ = zFar - zNear;
   sine = sinf(radians);
   cosine = cosf(radians);

   if ((deltaZ == 0) || (sine == 0) || (aspect == 0))
      return;

   cotangent = cosine / sine;

   tmp[0] = cotangent / aspect;
   tmp[5] = cotangent;
   tmp[10] = -(zFar + zNear) / deltaZ;
   tmp[11] = -1;
   tmp[14] = -2 * zNear * zFar / deltaZ;
   tmp[15] = 0;

   m4x4_copy(m, tmp);
}
