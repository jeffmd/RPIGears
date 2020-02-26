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
   memcpy(md, ms, sizeof(GLfloat) * 16);
}

/** 
 * Multiplies two 4x4 matrices.
 * 
 * The result is stored in matrix m.
 * 
 * @param m the first matrix to multiply
 * @param n the second matrix to multiply
 */
void m4x4_multiply(GLfloat d[16], const GLfloat m0[16], const GLfloat m1[16])
{
  /*
  d[0] = m0[0]*m1[0] + m0[4]*m1[1] + m0[8]*m1[2] + m0[12]*m1[3];
  d[1] = m0[1]*m1[0] + m0[5]*m1[1] + m0[9]*m1[2] + m0[13]*m1[3];
  d[2] = m0[2]*m1[0] + m0[6]*m1[1] + m0[10]*m1[2] + m0[14]*m1[3];
  d[3] = m0[3]*m1[0] + m0[7]*m1[1] + m0[11]*m1[2] + m0[15]*m1[3];

  d[4] = m0[0]*m1[4] + m0[4]*m1[5] + m0[8]*m1[6] + m0[12]*m1[7];
  d[5] = m0[1]*m1[4] + m0[5]*m1[5] + m0[9]*m1[6] + m0[13]*m1[7];
  d[6] = m0[2]*m1[4] + m0[6]*m1[5] + m0[10]*m1[6] + m0[14]*m1[7];
  d[7] = m0[3]*m1[4] + m0[7]*m1[5] + m0[11]*m1[6] + m0[15]*m1[7];

  d[8] = m0[0]*m1[8] + m0[4]*m1[9] + m0[8]*m1[10] + m0[12]*m1[11];
  d[9] = m0[1]*m1[8] + m0[5]*m1[9] + m0[9]*m1[10] + m0[13]*m1[11];
  d[10] = m0[2]*m1[8] + m0[6]*m1[9] + m0[10]*m1[10] + m0[14]*m1[11];
  d[11] = m0[3]*m1[8] + m0[7]*m1[9] + m0[11]*m1[10] + m0[15]*m1[11];

  d[12] = m0[0]*m1[12] + m0[4]*m1[13] + m0[8]*m1[14] + m0[12]*m1[15];
  d[13] = m0[1]*m1[12] + m0[5]*m1[13] + m0[9]*m1[14] + m0[13]*m1[15];
  d[14] = m0[2]*m1[12] + m0[6]*m1[13] + m0[10]*m1[14] + m0[14]*m1[15];
  d[15] = m0[3]*m1[12] + m0[7]*m1[13] + m0[11]*m1[14] + m0[15]*m1[15];
  */

  for (int i=0; i<16; i+=4) {
    const float ma = m1[i];
    const float mb = m1[i+1];
    const float mc = m1[i+2];
    const float md = m1[i+3];

    for (int j=0; j<4; j++) {
      d[i+j] = m0[j]*ma + m0[j+4]*mb + m0[j+8]*mc + m0[j+12]*md;
    }
  }
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
void m4x4_rotate(GLfloat *d, const GLfloat *m, GLfloat angle, const GLfloat x, const GLfloat y, const GLfloat z)
{
   angle = 2.0f * M_PI * angle / 360.0f;
   const float s = sinf(angle);
   const float c = cosf(angle);
   const float mc = 1.0f - c;

   GLfloat r[16] = {
      x * x * mc + c,     y * x * mc + z * s, x * z * mc - y * s, 0.0f,
      x * y * mc - z * s, y * y * mc + c,     y * z * mc + x * s, 0.0f, 
      x * z * mc + y * s, y * z * mc - x * s, z * z * mc + c,     0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
   };

   m4x4_multiply(d, m, r);
}


/** 
 * Translates a 4x4 matrix.
 * 
 * @param[in,out] m the matrix to translate
 * @param x the x component of the direction to translate to
 * @param y the y component of the direction to translate to
 * @param z the z component of the direction to translate to
 */
void m4x4_translate(GLfloat *d, const GLfloat *m, const GLfloat x, const GLfloat y, const GLfloat z)
{
  //   0  1  2  3   4  5  6  7   8  9 10 11  12 13 14 15
  // { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  x, y, z, 1 };

  m4x4_copy(d, m);  
  for (int i=0; i<4; i++) {
    d[i+12] += m[i]*x + m[i+4]*y + m[i+8]*z;
  }
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
#define SWAP(i, j) t = m[i]; m[i] = m[j]; m[j] = t
  GLfloat t;
  
  SWAP(1, 4);
  SWAP(2, 8);
  SWAP(3, 12);
  SWAP(6, 9);
  SWAP(7, 13);
  SWAP(11, 14);
}

/**
 * Inverts a 4x4 matrix.
 *
 * This function can currently handle only pure translation-rotation matrices.
 * Read http://www.gamedev.net/community/forums/topic.asp?topic_id=425118
 * for an explanation.
 */
void m4x4_invert(GLfloat *d, GLfloat *m)
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

   m4x4_multiply(d, m, t);
}

void m4xv3(float r[3], const float mat[16], const float vec[3])
{
  const float v0 = vec[0];
  const float v1 = vec[1];
  const float v2 = vec[2];
  for (int i=0; i<3; i++ ) {
    r[i] = v0 * mat[i] + v1 * mat[i+4] + mat[i+8] * v2 + mat[i+12];
  }
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
   m4x4_identity(m);

   float sine, cosine, cotangent, deltaZ;
   GLfloat radians = fovy / 2.0 * M_PI / 180.0;

   deltaZ = zFar - zNear;
   sine = sinf(radians);
   cosine = cosf(radians);

   if ((deltaZ == 0) || (sine == 0) || (aspect == 0))
      return;

   cotangent = cosine / sine;

   m[0] = cotangent / aspect;
   m[5] = cotangent;
   m[10] = -(zFar + zNear) / deltaZ;
   m[11] = -1.0;
   m[14] = -2.0 * zNear * zFar / deltaZ;
}
