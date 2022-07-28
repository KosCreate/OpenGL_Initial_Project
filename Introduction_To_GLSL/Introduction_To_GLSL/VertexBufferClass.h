#ifndef VBO_CLASS_H
#define VBO_CLASS_H
#include<glad/glad.h>
//Vertex Buffer Object...
class VBO {
public :
	unsigned int ID;
	VBO(float* vertices, GLsizeiptr size);
	void Bind();
	void UnBind();
	void Delete();
};
#endif