#include "VertexArrayClass.h"
// Default Constuctor creates a vertex array of a default size of 
// 1 & array to be a single created shader ID
VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

// Give your own size n
//& array to be generated into the vertex array...
VAO::VAO(unsigned int n, unsigned int arrays)
{
	ID = arrays;
	glGenVertexArrays(n, &ID);
}

void VAO::LinkVBO(VBO VBO, unsigned int layout, unsigned int elements, unsigned int stepOffset, unsigned int startingPosition) {
	VBO.Bind();
	//Main Geometry...
	glVertexAttribPointer(layout, elements, GL_FLOAT, GL_FALSE, stepOffset * sizeof(float), (void*)(startingPosition * sizeof(float)));
	glEnableVertexAttribArray(layout);
	VBO.UnBind();
}

void VAO::Bind() {
	glBindVertexArray(ID);
}

void VAO::UnBind() {
	glBindVertexArray(0);
}

void VAO::DrawTriangleArrays(unsigned int first, unsigned int count) {
	glDrawArrays(GL_TRIANGLES, first, count);
}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}