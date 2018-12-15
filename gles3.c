/* gles3.c */
/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 1999-2008  Brian Paul   All Rights Reserved.
 * Copyright (C) 2009  VMware, Inc.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <assert.h>

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

#define ARRAY_OBJECT_MAX 64


// RPI VC4 GLES 2.0 only has max 8 vertex attributes
#define  VERT_ATTRIB_MAX 8

/**
 * Attributes to describe a vertex array.
 *
 * Contains the size, type, format and normalization flag,
 * along with the index of a vertex buffer binding point.
 *
 * Note that the Stride field corresponds to VERTEX_ATTRIB_ARRAY_STRIDE
 * and is only present for backwards compatibility reasons.
 * Rendering always uses VERTEX_BINDING_STRIDE.
 * The gl*Pointer() functions will set VERTEX_ATTRIB_ARRAY_STRIDE
 * and VERTEX_BINDING_STRIDE to the same value, while
 * glBindVertexBuffer() will only set VERTEX_BINDING_STRIDE.
 */
typedef struct gl_array_attributes
{
   /** Points to client array data. Not used when a VBO is bound */
   const GLubyte *Ptr;      /**< pointer or offset to first element*/
   GLuint BufferObj;        /**< GL_ARB_vertex_buffer_object */
   GLshort Stride;          /**< Stride as specified with gl*Pointer() */
   GLenum Type;             /**< Datatype: GL_FLOAT, GL_INT, etc */
   unsigned Enabled:1;       /**< Whether the array is enabled */
   unsigned Size:3;         /**< Components per element (1,2,3,4) */
   unsigned Normalized:1;   /**< Fixed-point values are normalized when converted to floats */
   unsigned Integer:1;      /**< Fixed-point values are not converted to floats */

} gl_array_attributes;

/**
 * A representation of "Vertex Array Objects" (VAOs) from OpenGL 3.1+ /
 * the GL_ARB_vertex_array_object extension.
 */
typedef struct gl_vao
{
   unsigned char Active;
   /** Vertex attribute arrays */
   gl_array_attributes VertexAttrib[VERT_ATTRIB_MAX];
   /** The index buffer (also known as the element array buffer in OpenGL). */
   GLuint ElementBuffer;
} gl_vao;

/**
 * Vertex array state
 */
typedef struct gl_vao_manager
{
   /** Currently bound array object. */
   GLuint VAO;
   GLuint NextDeletedVAO;
   /** Array objects (GL_ARB_vertex_array_object) */
   gl_vao Objects[ARRAY_OBJECT_MAX];
} gl_vao_manager;


/**
 * gl rendering context.
 *
 * OpenGL ES 3.x state is contained in this structure.
 */
typedef struct gl_context
{

   gl_vao_manager Array;	    /**< Vertex arrays */

   /** bound buffers */
   GLuint Buffer;
   GLuint ElementBuffer;
} gl_context;

static gl_context ctx;

void glBindBufferMod(GLenum target, GLuint buffer)
{
	GLuint *buff = (target == GL_ARRAY_BUFFER) ? &ctx.Buffer : &ctx.ElementBuffer;

	if (*buff != buffer) {
	    *buff = buffer;
	    glBindBuffer(target, buffer);
	    if (target == GL_ELEMENT_ARRAY_BUFFER)
	       ctx.Array.Objects[ctx.Array.VAO].ElementBuffer = buffer;
	}
}

static void delete_vao(GLuint name)
{
   gl_vao *obj = &ctx.Array.Objects[name];
   obj->Active = GL_FALSE;
   if (name < ctx.Array.NextDeletedVAO)
		ctx.Array.NextDeletedVAO = name;
}


/**
 * Initialize attributes of a vertex array within a vertex array object.
 * \param vao  the container vertex array object
 * \param index  which array in the VAO to initialize
 * \param size  number of components (1, 2, 3 or 4) per attribute
 * \param type  datatype of the attribute (GL_FLOAT, GL_INT, etc).
 */
static void init_array_attributes(gl_vao *vao,
           GLuint index)
{
   assert(index < VERT_ATTRIB_MAX);
   gl_array_attributes *array = &vao->VertexAttrib[index];

   array->Size = 4;
   array->Type = GL_FLOAT;
   array->Stride = 0;
   array->Ptr = NULL;
   array->Enabled = GL_FALSE;
   array->Normalized = GL_FALSE;
   array->Integer = GL_FALSE;
   array->BufferObj = 0;
}

/**
 * Initialize a gl_vao's arrays.
 */
static gl_vao *init_vao(GLuint name)
{
   gl_vao *vao = &ctx.Array.Objects[name];
   vao->ElementBuffer = 0;

   /* Init the individual arrays */
   for (GLuint i = 0; i < VERT_ATTRIB_MAX; i++) {
     init_array_attributes(vao, i);
   }
   return vao;
}


/**
 * Allocate and initialize a new vertex array object.
 */
static gl_vao *new_vao(GLuint name)
{
   assert(name < ARRAY_OBJECT_MAX);
   gl_vao *vao = init_vao(name);
   vao->Active = GL_TRUE;
   return vao;
}

/**
 * find a deleted vao
 */
static GLint find_deleted_vao()
{
	GLint id;
	for (id = ctx.Array.NextDeletedVAO; id < ARRAY_OBJECT_MAX; id++) {
	  if (ctx.Array.Objects[id].Active == GL_FALSE) {
		ctx.Array.NextDeletedVAO = id + 1;
	    return id;
	  }
	}
	return id;
}

/**
 * Initialize vertex array state for given context.
 */
static void init_vao_manager(void)
{
   new_vao(0);
   ctx.Array.VAO = 0;
   ctx.Array.NextDeletedVAO = 1;
}

void gles3_init()
{
  init_vao_manager();
  ctx.Buffer = 0;
  ctx.ElementBuffer = 0;
}


/**
 * Generate a set of unique array object IDs and store them in \c arrays.
 * Helper for _mesa_GenVertexArrays() and _mesa_CreateVertexArrays()
 * below.
 *
 * \param n       Number of IDs to generate.
 * \param arrays  Array of \c n locations to store the IDs.
 */
static inline void gen_vaos(GLsizei n, GLuint *arrays)
{
   GLint i;
   if (n < 0) {
      printf("GL_INVALID_VALUE: glGenVertexArrays (n < 0)\n");
      return;
   }

   if (!arrays)
      return;

   /* For the sake of simplicity we create the array objects in both
    * the Gen* and Create* cases.
    */
   for (i = 0; i < n; i++) {
      GLuint name = find_deleted_vao();
      printf("new vao: %i\n", name);
      new_vao(name);
      arrays[i] = name;
   }
}

/**
 * ARB version of glGenVertexArrays()
 * All arrays will be required to live in VBOs.
 */
void glGenVertexArrays(GLsizei n, GLuint *arrays)
{
   gen_vaos(n, arrays);
}

/**
 * Determine if ID is the name of an array object.
 *
 * \param id  ID of the potential array object.
 * \return  \c GL_TRUE if \c id is the name of a array object,
 *          \c GL_FALSE otherwise.
 */
GLboolean glIsVertexArray( GLuint id )
{
   return ((0 < id) && (id < ARRAY_OBJECT_MAX)) ;
}

/**
 * ARB version of glBindVertexArray()
 */
void glBindVertexArray(GLuint id)
{
   if (ctx.Array.VAO == id)
      return;   /* rebinding the same array object- no change */

   assert(id < ARRAY_OBJECT_MAX);

   gl_vao *oldVao =  &ctx.Array.Objects[ctx.Array.VAO];
   gl_vao *vao =  &ctx.Array.Objects[id];
   ctx.Array.VAO = id;
   
   glBindBufferMod(GL_ELEMENT_ARRAY_BUFFER, vao->ElementBuffer);

   for (GLuint i = 0; i < VERT_ATTRIB_MAX; i++) {
	  gl_array_attributes *oldVertexAttrib = &oldVao->VertexAttrib[i];
	  gl_array_attributes *newVertexAttrib = &vao->VertexAttrib[i];
	  
	  if (newVertexAttrib->Enabled) {
		 glBindBufferMod(GL_ARRAY_BUFFER, newVertexAttrib->BufferObj);
		 glEnableVertexAttribArray(i);
		 glVertexAttribPointer(i, newVertexAttrib->Size, newVertexAttrib->Type,
			newVertexAttrib->Normalized, newVertexAttrib->Stride, newVertexAttrib->Ptr);

	  }
	  else if (oldVertexAttrib->Enabled) {

		 glDisableVertexAttribArray(i);
	  }
   }
}

/**
 * Delete a set of vertex array objects.
 *
 * \param n      Number of array objects to delete.
 * \param ids    Array of \c n array object IDs.
 */
static void delete_vaos(GLsizei n, const GLuint *ids)
{
   GLsizei i;

   for (i = 0; i < n; i++) {
      GLuint obj = ids[i];

      if (obj) {
         assert(obj < ARRAY_OBJECT_MAX);

         /* If the array object is currently bound, the spec says "the binding
          * for that object reverts to zero and the default vertex array
          * becomes current."
          */
         if (obj == ctx.Array.VAO)
            glBindVertexArray(0);

         delete_vao(obj);
      }
   }
}

void glDeleteVertexArrays(GLsizei n, const GLuint *ids)
{
   if (n < 0) {
      printf("GL_INVALID_VALUE: glDeleteVertexArray(n)\n");
      return;
   }

   delete_vaos(n, ids);
}

/**
 * Update state for glVertexArrayPointer functions for bound vertex aray object.
 *
 * \param attrib  the attribute array index to update
 * \param size  components per element (1, 2, 3 or 4)
 * \param type  datatype of each component (GL_FLOAT, GL_INT, etc)
 * \param stride  stride between elements, in elements
 * \param normalized  are integer types converted to floats in [-1, 1]?
 * \param integer  integer-valued values (will not be normalized to [-1,1])
 * \param ptr  the address (or offset inside VBO) of the array data
 */
static void update_array(GLuint attrib, GLint size, GLenum type,
             GLsizei stride, GLboolean normalized, GLboolean integer,
             const GLvoid *ptr)
{
   gl_array_attributes *const array = &ctx.Array.Objects[ctx.Array.VAO].VertexAttrib[attrib];

   array->Size = size;
   array->Type = type;
   array->Normalized = normalized;
   array->Integer = integer;
   array->Stride = stride;
   array->Ptr = ptr;
   array->BufferObj = ctx.Buffer;
}

/**
 * Set a generic vertex attribute array.
 * Note that these arrays DO NOT alias the conventional GL vertex arrays
 * (position, normal, color, fog, texcoord, etc).
 */
void glVertexAttribPointerMod(GLuint index, GLint size, GLenum type,
                             GLboolean normalized,
                             GLsizei stride, const GLvoid *ptr)
{
   glVertexAttribPointer(index, size, type, normalized, stride, ptr);

   update_array(index, size, type, stride, normalized, GL_FALSE, ptr);
}

void glVertexAttribIPointer(GLuint index, GLint size, GLenum type,
                           GLsizei stride, const GLvoid *ptr)
{

   glVertexAttribPointer(index, size, type, GL_FALSE, stride, ptr);

   update_array(index, size, type, stride, GL_FALSE, GL_TRUE, ptr);
}

static inline void enable_vertex_array_attrib(GLuint id, GLuint attrib)
{
   assert(id < ARRAY_OBJECT_MAX);
   assert(attrib < VERT_ATTRIB_MAX);
   gl_vao *vao =  &ctx.Array.Objects[id];
   vao->VertexAttrib[attrib].Enabled = GL_TRUE;
}

void glEnableVertexAttribArrayMod(GLuint index)
{
  enable_vertex_array_attrib(ctx.Array.VAO, index);
  glEnableVertexAttribArray(index);
}

void glEnableVertexArrayAttrib(GLuint vaobj, GLuint index)
{

   /* The ARB_direct_state_access specification says:
    *
    *   "An INVALID_OPERATION error is generated by EnableVertexArrayAttrib
    *    and DisableVertexArrayAttrib if <vaobj> is not
    *    [compatibility profile: zero or] the name of an existing vertex
    *    array object."
    */
   if (!vaobj)
      return;

   enable_vertex_array_attrib(vaobj, index);
}

static inline void disable_vertex_array_attrib(GLuint id, GLuint attrib)
{
   assert(id < ARRAY_OBJECT_MAX);
   assert(attrib < VERT_ATTRIB_MAX);

   ctx.Array.Objects[id].VertexAttrib[attrib].Enabled = GL_FALSE;

}

void glDisableVertexAttribArrayMod(GLuint index)
{
  disable_vertex_array_attrib(ctx.Array.VAO, index);
  glDisableVertexAttribArray(index);
}

void glDisableVertexArrayAttrib(GLuint vaobj, GLuint index)
{
   /* The ARB_direct_state_access specification says:
    *
    *   "An INVALID_OPERATION error is generated by EnableVertexArrayAttrib
    *    and DisableVertexArrayAttrib if <vaobj> is not
    *    [compatibility profile: zero or] the name of an existing vertex
    *    array object."
    */
   if (!vaobj)
       return;

   disable_vertex_array_attrib(vaobj, index);
}

