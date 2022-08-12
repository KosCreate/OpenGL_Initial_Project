#include "shaderClass.h"
//Comment stuff...
std::string get_file_contents(const char* fileName) {
	//Objects of this class maintain a filebuf object as their internal stream buffer, which performs input/output operations on the file
	//they are associated with (if any).
	std::ifstream in(fileName, std::ios::binary);
	if (in){
		std::string contents;
		//seekg() is a function in the iostream library that allows us to seek an
		//arbitrary position in a file. It is mainly used to set the position of the next character to be
		//extracted from the input stream from a given file in C++ file handling.
		in.seekg(0, std::ios::end);
		//Resizes the string to a length of n characters.
		//in.tellg -> The tellg() function is used with input streams,
		//  and returns the current “get” position of the pointer in the stream. 
		// It has no parameters and returns a value of the member type pos_type, 
		// which is an integer data type representing the current position of the get stream pointer.
		contents.resize(in.tellg());
		//Seek and return the "get" position starting from the 0 elements of the text file...
		in.seekg(0, std::ios::beg);
		//Reads the content starting from the first element of the text file (char array) & to the lenght of it...
		in.read(&contents[0], contents.size());
		//The close() function is used to close the file 
		//currently associated with the object
		in.close();
		return(contents);
	}
	throw(errno);
}
Shader::Shader(const char* vertexFile, const char* fragmentFile) 
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//Building & compiling shader code...
	//----------------------------------//
	//vertex shader...//
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	//Checking for shader compilation errors...
	//---------------------------------------//
	int success;
	//To put in the compilation status
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "COMPILATION ERROR ON THE -- VERTEX SHADER --\n" << infoLog << std::endl;
	}

	//fragment shader...
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "COMPILATION ERROR ON THE -- FRAGMENT SHADER --\n" << infoLog << std::endl;
	}

	//Link shaders into single shader program...
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	//checking for linking errors...
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "LINKING ERROR ON THE -- SHADER PROGRAM --\n" << infoLog << std::endl;
	}
	//Deleting the shaders because they are already in the program itself...
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::Activate() {
	glUseProgram(ID);
}
void Shader::Delete() {
	glDeleteProgram(ID);
}