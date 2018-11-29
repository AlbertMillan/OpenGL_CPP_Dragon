#ifndef SHADER_UNIFORM_SETTERS_H
#define SHADER_UNIFORM_SETTERS_H

#include "Shader.h"
#include "Classes/ogldev_math_3d.h"
#include <glm/glm.hpp>
#include <vector>

//Vector3f Direction = Vector3f(0.0f, 0.0f, 0.0f)

class ShaderFunctions : public Shader {
public:
    static const uint MAX_BONES = 100;

    ShaderFunctions();

    virtual bool Init();

    void SetBoneTransform( uint index, const Matrix4f& Transform );

    void SetWVT(glm::mat4 model, glm::mat4 projection, glm::mat4 view);

    void SetDirectionalLight( Vector3f lightColor, float ambientIntensity, Vector3f lightSourcePos, Vector3f viewPos );

    //void SetEyeWorldPos(const Vector3f& EyeWorldPos);

    //void SetWVP(const Matrix4f& WVP);
    //void SetWorldMatrix(const Matrix4f& WVP);



private:
    GLuint m_boneLocation[MAX_BONES];
    GLuint m_matLocation;
    GLuint m_viewMatLocation;
    GLuint m_projMatLocation;

    GLuint m_samplerLocation;
    GLuint m_dirLightColorLocation;
    GLuint m_dirLightAmbientIntensityLocation;
    GLuint m_dirLightSourceLocation;
    GLuint m_dirViewPosLocation;



};
#endif /* SHADER_UNIFORM_SETTERS_H */
