#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

using namespace std;

class Shader {
public:
    // Creates empty shader program
    Shader();

    virtual ~Shader();

    virtual bool Init();

    void Use();

    void LinkProgram( );

    GLuint ID;

protected:

    void AddShader( const char* vertexPath, const char* fragmentPath );

    void AddShader( const char* vertexPath, const char* fragmentPath, const char* geometryPath );

    GLint GetUniformLocation( const char* uniformName );

private:

    //typedef std::list<Gluint> ShaderObjList;
    //ShaderObjList m_shaderObjList;

//    string const& ReadFile( const char* shaderPath  );

    unsigned int CreateShader( const char*pShaderText, GLenum ShaderType );




};
#endif  /* SHADER_H */
