#ifndef EBO_CLASS_H
#define EBP_CLASS_H
#include <glad/glad.h>
//Comment stuff...
class EBO
{
public : 
	unsigned int ID;
	EBO(int* indices, GLsizeiptr size);
	void Bind();
	void UnBind();
	void Delete();
};
#endif // !EBO_CLASS_H