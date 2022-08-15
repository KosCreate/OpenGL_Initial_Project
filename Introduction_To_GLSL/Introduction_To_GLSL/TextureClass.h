#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H
#include <glad/glad.h>
#include "shaderClass.h"
class Texture 
{
	public :
	GLuint ID;
	GLenum Type;
	const char* type;
	Texture(const char* texturePath, GLenum textType, const char* charType, GLenum slot, GLenum pixelType, bool flipVertically);
	void Bind(GLenum slot);
	void UnBind();
	void Delete();
};
#endif // !TEXTURE_CLASS_H