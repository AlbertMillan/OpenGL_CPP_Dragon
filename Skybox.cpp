#include "Skybox.h"
#include "Texture.h"

#define SKYBOX_VPOS_LOCATION 0

Skybox::Skybox()
{
    skyboxVAO = 0;
    skyboxVBO = 0;
}

// Initialize Shader
bool Skybox::Init()
{
    if(!Shader::Init()){
        return false;
    }

    // Add shaders // Perhaps I should convert this into a boolean
    AddShader("Shaders/skybox.vs", "Shaders/skybox.fs");

    GLuint m_Projection = GetUniformLocation("projection");
    GLuint m_View = GetUniformLocation("view");

    glGenVertexArrays( 1, &skyboxVAO );
    glGenBuffers( 1, &skyboxVBO );

    /* BIND VAO */
    glBindVertexArray( skyboxVAO );
    glBindBuffer( GL_ARRAY_BUFFER, skyboxVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( skyboxVertices ), &skyboxVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray(SKYBOX_VPOS_LOCATION);
    glVertexAttribPointer( SKYBOX_VPOS_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);


    vector<const char *> faces;
    faces.push_back("Assets/siege_bk.tga");
    faces.push_back("Assets/siege_ft.tga");
    faces.push_back("Assets/siege_up.tga");
    faces.push_back("Assets/siege_dn.tga");
    faces.push_back("Assets/siege_lf.tga");
    faces.push_back("Assets/siege_rt.tga");

    cubeTexture = TextureLoader::LoadCubeMap(faces);

    glBindVertexArray(0);

    return true;
}

//void Skybox::SetTextureUnit(unsigned int TextureUnit)
//{
//    glUniform1i(m_textureLocation, TextureUnit);
//}


//void Skybox::Bind(GLenum TextureUnit)
//{
//    glActiveTexture(TextureUnit);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
//}

void Skybox::SetShaderParams(glm::mat4 view, glm::mat4 projection)
{
    glUniformMatrix4fv(m_Projection, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(m_View, 1, GL_FALSE, glm::value_ptr(view));
}


// Render Cube
void Skybox::Render()
{
    glBindVertexArray( skyboxVAO );
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_CUBE_MAP, cubeTexture );
    glDrawArrays( GL_TRIANGLES, 0, 36 );
    glBindVertexArray(0);
}
