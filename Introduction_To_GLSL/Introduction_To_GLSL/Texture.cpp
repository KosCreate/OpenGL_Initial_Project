#include "TextureClass.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const char* texturePath, GLenum texType, GLenum slot, GLenum pixelType, bool flipVertically) {
		// Assigns the type of the texture ot the texture object
		Type = texType;
		GLenum format;
		// Generates an OpenGL texture object
		glGenTextures(1, &ID);
		// Stores the width, height, and the number of color channels of the image
		int widthImg, heightImg, numColCh;
		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(true);
		// Reads the image from a file and stores it in bytes
		unsigned char* bytes = stbi_load(texturePath, &widthImg, &heightImg, &numColCh, 0);
		
		if (numColCh == 1)
			format = GL_RED;
		else if (numColCh == 3)
			format = GL_RGB;
		else if (numColCh == 4)
			format = GL_RGBA; 

		glBindTexture(texType, ID);
		// Assigns the image to the OpenGL Texture object
		glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);

		// Generates MipMaps
		glGenerateMipmap(texType);
		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		// Deletes the image data as it is already in the OpenGL Texture object
		stbi_image_free(bytes);

		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(texType, 0);
}

void Texture::Bind(GLenum slot) {
	glActiveTexture(slot);
	glBindTexture(Type, ID);
}
void Texture::UnBind()
{
	glBindTexture(Type, 0);
}
void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}