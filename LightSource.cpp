#include "LightSource.h"

#define SKYBOX_VPOS_LOCATION 0

LightSource::LightSource(float r, float g, float b )
{
    color = Vector3f(r, g, b);
    cubeVAO = 0;
    cubeVBO = 0;
}

// Initialize Shader
bool LightSource::Init()
{
    if(!Shader::Init()){
        return false;
    }

    // Add shaders // Perhaps I should convert this into a boolean
    AddShader("Shaders/lightball.vs", "Shaders/lightball.fs");

    m_ProjLoc = GetUniformLocation("projection");
    m_ViewLoc = GetUniformLocation("view");
    m_ModelLoc = GetUniformLocation("model");
    m_ColorLoc = GetUniformLocation("color");

    glGenVertexArrays( 1, &cubeVAO );
    glGenBuffers( 1, &cubeVBO );

    /* BIND VAO */
    glBindVertexArray( cubeVAO );
    glBindBuffer( GL_ARRAY_BUFFER, cubeVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( cubeVertices ), &cubeVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray(SKYBOX_VPOS_LOCATION);
    glVertexAttribPointer( SKYBOX_VPOS_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);

    glBindVertexArray(0);

    SetColor();

    return true;
}


void LightSource::SetColor()
{
    glUniform3f(m_ColorLoc, color.x, color.y, color.z);
}

void LightSource::SetShaderParams(glm::mat4 view, glm::mat4 projection, glm::mat4 model)
{
    glUniformMatrix4fv(m_ProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(m_ViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(m_ModelLoc, 1, GL_FALSE, glm::value_ptr(model));
}


// Render Cube
void LightSource::Render()
{
    glBindVertexArray( cubeVAO );
    glDrawArrays( GL_TRIANGLES, 0, 36 );
    glBindVertexArray(0);
}
