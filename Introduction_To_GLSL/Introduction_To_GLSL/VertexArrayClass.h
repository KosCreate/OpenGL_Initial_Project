#ifndef VERTEX_ARRAY_CLASS_H
#define VERTEX_ARRAY_CLASS_H
#include <glad/glad.h>
#include "vertexBufferClass.h"
//Comment stuff...
class VAO
{
public:
	unsigned int ID;
	VAO();
	VAO(unsigned int n, unsigned int arrays);
	void LinkVBO(VBO VBO, unsigned int layout, unsigned int elements, unsigned int stepOffset, unsigned int startingPosition);
	void Bind();
	void UnBind();
	void DrawTriangleArrays(unsigned int first, unsigned int count);
	void Delete();
};
#endif // !VERTEX_ARRAY_CLASS_H