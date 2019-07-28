void DrawPrimitiveWithVBOs ( ESContext *esContext,
                             GLint numVertices, GLfloat *vtxBuf,
                             GLint vtxStride, GLint numIndices,
                             GLushort *indices )
{
   UserData *userData = esContext->userData;
   GLuint   offset = 0;

   // vboIds[0] - used to store vertex attribute data
   // vboIds[l] - used to store element indices
   if ( userData->vboIds[0] == 0 && userData->vboIds[1] == 0 )
   {
      // Only allocate on the first draw
      glGenBuffers ( 2, userData->vboIds );

      glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[0] );//绑定到0号vbo，顶点属性
      glBufferData ( GL_ARRAY_BUFFER, vtxStride * numVertices,
                     vtxBuf, GL_STATIC_DRAW );
      glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1] );//绑定到1号vbo，索引值
      glBufferData ( GL_ELEMENT_ARRAY_BUFFER,
                     sizeof ( GLushort ) * numIndices,
                     indices, GL_STATIC_DRAW );//static表示设置一次，后面不会修改这个值
   }

   glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[0] );
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1] );

   glEnableVertexAttribArray ( VERTEX_POS_INDX );
   glEnableVertexAttribArray ( VERTEX_COLOR_INDX );

   glVertexAttribPointer ( VERTEX_POS_INDX, VERTEX_POS_SIZE,//设置位置参数
                           GL_FLOAT, GL_FALSE, vtxStride,
                           ( const void * ) offset );//偏置0开始

   offset += VERTEX_POS_SIZE * sizeof ( GLfloat );//设置颜色参数
   glVertexAttribPointer ( VERTEX_COLOR_INDX,
                           VERTEX_COLOR_SIZE,
                           GL_FLOAT, GL_FALSE, vtxStride,
                           ( const void * ) offset );//偏置3开始

   glDrawElements ( GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT,
                    0 );

   glDisableVertexAttribArray ( VERTEX_POS_INDX );
   glDisableVertexAttribArray ( VERTEX_COLOR_INDX );

   glBindBuffer ( GL_ARRAY_BUFFER, 0 );//解绑
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void DrawPrimitiveWithoutVBOs ( GLfloat *vertices,
                                GLint vtxStride,
                                GLint numIndices,
                                GLushort *indices )
{
   GLfloat   *vtxBuf = vertices;

   glBindBuffer ( GL_ARRAY_BUFFER, 0 );//绑在0上面就是解绑的意思，不使用VBO
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );

   glEnableVertexAttribArray ( VERTEX_POS_INDX );//这个index是在shader中定义的layout location的编号
   glEnableVertexAttribArray ( VERTEX_COLOR_INDX );

   glVertexAttribPointer ( VERTEX_POS_INDX, VERTEX_POS_SIZE,
                           GL_FLOAT, GL_FALSE, vtxStride,
                           vtxBuf );
   vtxBuf += VERTEX_POS_SIZE;

   glVertexAttribPointer ( VERTEX_COLOR_INDX,
                           VERTEX_COLOR_SIZE, GL_FLOAT,
                           GL_FALSE, vtxStride, vtxBuf );

   glDrawElements ( GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT,
                    indices );

   glDisableVertexAttribArray ( VERTEX_POS_INDX );
   glDisableVertexAttribArray ( VERTEX_COLOR_INDX );

}

   const char vShaderStr[] =
      "#version 300 es                            \n"
      "layout(location = 0) in vec4 a_position;   \n"
      "layout(location = 1) in vec4 a_color;      \n"
      "uniform float u_offset;                    \n"
      "out vec4 v_color;                          \n"
      "void main()                                \n"
      "{                                          \n"
      "    v_color = a_color;                     \n"
      "    gl_Position = a_position;              \n"
      "    gl_Position.x += u_offset;             \n"
      "}";


   const char fShaderStr[] =
      "#version 300 es            \n"
      "precision mediump float;   \n"
      "in vec4 v_color;           \n"
      "out vec4 o_fragColor;      \n"
      "void main()                \n"
      "{                          \n"
      "    o_fragColor = v_color; \n"
      "}" ;


  void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
   GLfloat vertices[3 * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE )] =
   {
      -0.5f,  0.5f, 0.0f,        // v0
      1.0f,  0.0f, 0.0f, 1.0f,  // c0
      -1.0f, -0.5f, 0.0f,        // v1
      0.0f,  1.0f, 0.0f, 1.0f,  // c1
      0.0f, -0.5f, 0.0f,        // v2
      0.0f,  0.0f, 1.0f, 1.0f,  // c2
   };
   // Index buffer data
   GLushort indices[3] = { 0, 1, 2 };

   glViewport ( 0, 0, esContext->width, esContext->height );
   glClear ( GL_COLOR_BUFFER_BIT );
   glUseProgram ( userData->programObject );
   glUniform1f ( userData->offsetLoc, 0.0f );

   DrawPrimitiveWithoutVBOs ( vertices,
                              sizeof ( GLfloat ) * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ),
                              3, indices );

   // Offset the vertex positions so both can be seen
   glUniform1f ( userData->offsetLoc, 1.0f );

   DrawPrimitiveWithVBOs ( esContext, 3, vertices,
                           sizeof ( GLfloat ) * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ),
                           3, indices );
}