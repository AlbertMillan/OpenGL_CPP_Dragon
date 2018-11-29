#include "ShaderUniformSetters.h"
#include "Util.h"
#include <glm/gtc/type_ptr.hpp>

ShaderFunctions::ShaderFunctions() {}

// Initialize shaders. Get Location of shader variables and
// store them in class variables.
bool ShaderFunctions::Init()
{
    if(!Shader::Init()){
        return false;
    }

    // Add shaders // Perhaps I should convert this into a boolean
    AddShader("Shaders/dragon.vs", "Shaders/dragon.fs");

    // Get Location of shader variables and store them in class variables
    //m_samplerLocation = GetUniformLocation("gSampler");
    m_matLocation = GetUniformLocation("model");
    m_viewMatLocation = GetUniformLocation("view");
    m_projMatLocation = GetUniformLocation("proj");
    m_dirLightColorLocation = GetUniformLocation("gDirectionalLight.lightColor");
    m_dirLightAmbientIntensityLocation = GetUniformLocation("gDirectionalLight.ambientIntensity");
    m_dirLightSourceLocation = GetUniformLocation("gDirectionalLight.lightSourcePos");
    m_dirViewPosLocation = GetUniformLocation("gDirectionalLight.viewPos");



        // Bone Variables
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_boneLocation) ; i++) {
        char Name[128];
        memset(Name, 0, sizeof(Name));
        SNPRINTF(Name, sizeof(Name), "gBones[%d]", i);
        m_boneLocation[i] = GetUniformLocation(Name);
    }

    return true;
}


void ShaderFunctions::SetBoneTransform( uint index, const Matrix4f& Transform )
{
    assert( index < MAX_BONES );
    //Transform.Print();
    glUniformMatrix4fv(m_boneLocation[index], 1, GL_TRUE, (const GLfloat*)Transform);
}

void ShaderFunctions::SetWVT(glm::mat4 model, glm::mat4 projection, glm::mat4 view)
{
     glUniformMatrix4fv(m_matLocation, 1, GL_FALSE, glm::value_ptr( model ));
     glUniformMatrix4fv(m_viewMatLocation, 1, GL_FALSE, glm::value_ptr( view ));
     glUniformMatrix4fv(m_projMatLocation, 1, GL_FALSE, glm::value_ptr( projection ));
}



void ShaderFunctions::SetDirectionalLight(Vector3f color, float ambientIntensity, Vector3f lightSourcePos, Vector3f viewPos)
{
    glUniform3f(m_dirLightColorLocation, color.x, color.y, color.z);
    glUniform3f(m_dirLightSourceLocation, lightSourcePos.x, lightSourcePos.y, lightSourcePos.z);
    glUniform3f(m_dirViewPosLocation, viewPos.x, viewPos.y, viewPos.z);
    glUniform1f(m_dirLightAmbientIntensityLocation, ambientIntensity);
}




