#include "Shader.h"
#include "Util.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;


Shader::Shader()
{
    ID = 0;
}

Shader::~Shader()
{
    if (ID != 0)
    {
        glDeleteProgram(ID);
        ID = 0;
    }
}

bool Shader::Init()
{
    ID = glCreateProgram();

    if (ID == 0) {
        fprintf(stderr, "Error creating shader program\n");
        return false;
    }

    return true;
}

// Use this method to add shaders to the program. When finished - call finalize()
void Shader::AddShader( const char* vertexPath, const char* fragmentPath )
{
    // 1. retrieve the vertex/fragment source code from filePath.
//    const char* vShaderCode = ReadFile( vertexPath );
//    const char* fShaderCode = ReadFile( fragmentPath );
    string vertexCode;
    string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
        // open files
        vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure e)
	{
		cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;

		//cerr << "Error code: " << strerror(errno);
		exit(1);
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

    // 2. create & compile shader objects.
    GLuint vShaderObj = CreateShader( vShaderCode, GL_VERTEX_SHADER );
    GLuint fShaderObj = CreateShader( fShaderCode, GL_FRAGMENT_SHADER );

    // 3. save shader objects - will be deleted in the destructor
    //m_shaderObjList.push_back(vShaderObj);
    //m_shaderObjList.push_back(fShaderObj);

    // 3. attach shader objects to the shader program
    glAttachShader(ID, vShaderObj);
    glAttachShader(ID, fShaderObj);

    // 4. link & validate shader program
    LinkProgram();

    // 5. delete shaders.
    glDeleteShader(vShaderObj);
    glDeleteShader(fShaderObj);

}

void Shader::AddShader( const char* vertexPath, const char* fragmentPath, const char* geometryPath )
{
    string vertexCode;
    string fragmentCode;
    string geometryCode;
	ifstream vShaderFile;
	ifstream fShaderFile;
	ifstream gShaderFile;
	// ensure ifstream objects can throw exceptions
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
        // open files
        vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		gShaderFile.open(geometryPath);
		stringstream vShaderStream, fShaderStream, gShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		gShaderStream << gShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		gShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		geometryCode = gShaderStream.str();
	}
	catch (ifstream::failure e)
	{
		cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;

		//cerr << "Error code: " << strerror(errno);
		exit(1);
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();

    // 2. create & compile shader objects.
    GLuint vShaderObj = CreateShader( vShaderCode, GL_VERTEX_SHADER );
    GLuint fShaderObj = CreateShader( fShaderCode, GL_FRAGMENT_SHADER );
    GLuint gShaderObj = CreateShader( gShaderCode, GL_GEOMETRY_SHADER );

    // 3. save shader objects - will be deleted in the destructor
    //m_shaderObjList.push_back(vShaderObj);
    //m_shaderObjList.push_back(fShaderObj);

    // 3. attach shader objects to the shader program
    glAttachShader(ID, vShaderObj);
    glAttachShader(ID, gShaderObj);
    glAttachShader(ID, fShaderObj);

    // 4. link & validate shader program
    LinkProgram();

    // 5. delete shaders.S
    glDeleteShader(vShaderObj);
    glDeleteShader(fShaderObj);
    glDeleteShader(gShaderObj);
}

GLint Shader::GetUniformLocation( const char* uniformName )
{
    GLuint location = glGetUniformLocation(ID, uniformName);

    if( location == INVALID_UNIFORM_LOCATION )
    {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", uniformName);
    }

    return location;
}

void Shader::Use()
{
    glUseProgram(ID);
}




/****************** PRIVATE FUNCTIONS *******************/
//const char* Shader::ReadFile( const char* shaderPath)
//{
//
//		string shaderCode;
//		ifstream shaderFile;
//		// ensure ifstream objects can throw exceptions
//		shaderFile.exceptions(ifstream::failbit | ifstream::badbit);
//		try
//		{
//			// open files
//			shaderFile.open(shaderPath);
//			stringstream shaderStream;
//			// read file's buffer contents into streams
//			shaderStream << shaderFile.rdbuf();
//			// close file handlers
//			shaderFile.close();
//			// convert stream into string
//			shaderCode = shaderStream.str();
//		}
//		catch (ifstream::failure e)
//		{
//			cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
//
//			//cerr << "Error code: " << strerror(errno);
//			exit(1);
//		}
//
//		return shaderCode.c_str();
//}

unsigned int Shader::CreateShader( const char*pShaderText, GLenum ShaderType )
{
    unsigned int ShaderObj = glCreateShader(ShaderType);
    glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderText, NULL);
	glCompileShader(ShaderObj);
	// print compile errors if any
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar InfoLog[512];
		glGetShaderInfoLog(ShaderObj, 512, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}
	return ShaderObj;
}

void Shader::LinkProgram( )
{
    int success;
    GLchar InfoLog[512];

    // Link Program
    glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", InfoLog);
		exit(1);
	}

	// Validate Program
    glValidateProgram(ID);

    glGetProgramiv(ID, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, sizeof(InfoLog), NULL, InfoLog);
        std::cerr << "Invalid shader program: " << InfoLog << std::endl;
        std::cerr << "Press enter/return to exit..." << std::endl;
        std::cin.get();
        exit(1);
    }
}

