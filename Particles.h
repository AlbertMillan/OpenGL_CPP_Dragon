#ifndef PARTICLES_H
#define PARTICLES_H

#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "Shader.h"

static GLfloat* g_particule_position_size_data;
static GLubyte* g_particule_color_data;

class Particles : public Shader
{
    public:
        Particles();

        ~Particles();

        bool Init();

        void Render(float delta, glm::mat4 ViewProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 cameraPosition, float animationTime);

        int FindUnusedParticle();

        void SortParticles();

    private:
        #define MAX_PARTICLES 100000
        struct Particle
        {
            glm::vec3 pos, speed;
            unsigned char r, g, b, a;
            float size, angle, weight;
            float life;
            float cameradistance; // *Squared* distance to the camera.

            bool operator <(const Particle& that) const {
                // Sort in reverse order: far particles drawn first.
                return this->cameradistance > that.cameradistance;
            }
        };


        Particle ParticlesContainer[MAX_PARTICLES];
        int lastUsedParticle;

        GLuint billboardVBO;
        GLuint particlesVBO;
        GLuint particleColorVBO;

        GLuint m_CameraRightLoc;
        GLuint m_CameraUpLoc;
        GLuint m_VP;
        GLuint m_TextureID;
        GLuint Texture;



        GLfloat g_vertex_buffer_data[12] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
        };

};
#endif // PARTICLES_H
