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

  // make vfp vector friendly
  for (int i=0; i<16; i+=4) {
    float s0 = 0.0f;
    float s1 = 0.0f;
    float s2 = 0.0f;
    float s3 = 0.0f;

    for (int j=0; j<4; j++) {
      const float s4 = m1[j+i];

      s0 += s4 * m0[0 + j*4];
      s1 += s4 * m0[1 + j*4];
      s2 += s4 * m0[2 + j*4];
      s3 += s4 * m0[3 + j*4];
    }
    
    d[i] = s0;
    d[i+1] = s1;
    d[i+2] = s2;
    d[i+3] = s3;
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
   const float c = cosf(angle);
   const float mc = 1.0f - c;
   const float s = sinf(angle);

   GLfloat r[16];
   const float zmc = z * mc;
   const float xmc = x * mc;
   const float ymc = y * mc;
   const float ys = y * s;
   const float zs = z * s;
   const float xs = x * s;
   const float yzmc = y * zmc;
   const float yxmc = y * xmc;
   const float xzmc = x * zmc;

   float s0 = c;
   float s1 = yxmc + zs;
   float s2 = xzmc - ys;

   s0 += x * xmc;

   r[3] = 0.0f;
   r[0] = s0;
   r[1] = s1;
   r[2] = s2;

   float s5 = c;

   float s4 = yxmc - zs;
   float s6 = yzmc + xs;
   s5 += y * ymc;

   r[7] = 0.0f;
   r[4] = s4;
   r[5] = s5;
   r[6] = s6;
   
   float s10 = c;
   
   float s8 = xzmc + ys;
   float s9 = yzmc - xs;
   s10 += z * zmc;
   
   r[11] = 0.0f;
   r[8] = s8;
   r[9] = s9;
   r[10] = s10;

   r[12] = 0.0f;
   r[13] = 0.0f;
   r[14] = 0.0f;
   r[15] = 1.0f;

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
  float vec[3] = {x, y, z};  

  float s0 = m[12];
  float s1 = m[13];
  float s2 = m[14];
  float s3 = m[15];

  for (int j=0; j<3; j++) {
    float vs = vec[j];
    s0 += vs * m[0 + j*4];
    s1 += vs * m[1 + j*4];
    s2 += vs * m[2 + j*4];
    s3 += vs * m[3 + j*4];
  }

  d[12] = s0;
  d[13] = s1;
  d[14] = s2;
  d[15] = s3;
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
  float s0 = mat[12];
  float s1 = mat[13];
  float s2 = mat[14];

  for (int j=0; j<3; j++) {
    float vs = vec[j];
    s0 += vs * mat[j*4];
    s1 += vs * mat[j*4+1];
    s2 += vs * mat[j*4+2];
  }

  r[0] = s0;
  r[1] = s1;
  r[2] = s2;
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
