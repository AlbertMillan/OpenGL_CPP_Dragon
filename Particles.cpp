#include "Particles.h"
#include "Texture.h"
#include <algorithm>


Particles::Particles()
{
    lastUsedParticle = 0;
}

Particles::~Particles(){}

bool Particles::Init()
{
    if( ! Shader::Init() )
    {
        return false;
    }

    AddShader("Shaders/particles.vs", "Shaders/particles.fs");

    m_CameraRightLoc = GetUniformLocation("CameraRight_worldspace");
    m_CameraUpLoc =  GetUniformLocation("CameraUp_worldspace");
    m_VP = GetUniformLocation("VP");
    m_TextureID = GetUniformLocation("myTextureSampler");

    g_particule_position_size_data =  new GLfloat[MAX_PARTICLES * 4];
    g_particule_color_data = new GLubyte[MAX_PARTICLES * 4];

    for(int i = 0; i < MAX_PARTICLES; i++ )
    {
        ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
    }

    Texture = TextureLoader::loadDDS("Assets/Img/particle.DDS");

	glGenBuffers(1, &billboardVBO);
	glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	glGenBuffers(1, &particlesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, particlesVBO);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	glGenBuffers(1, &particleColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, particleColorVBO);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	return true;
}


void Particles::Render(float delta, glm::mat4 ViewProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 cameraPosition, float animationTime)
{
    // Generate 10 new particles each millisecond.
    int newparticles = (int)(delta * 10000);
    if (newparticles > (int)(0.016f*10000.0))
    {
        newparticles = (int)(0.016f*10000.0);
    }

    for( int i=0; i < newparticles; i++ )
    {
        int particleIndex = FindUnusedParticle();
        ParticlesContainer[particleIndex].life = 5.0f; //This particles lives 5 secons

        ParticlesContainer[particleIndex].pos = glm::vec3(0.0f, 10.3f, 8.2f);

        // Direction (Need to modify for fire particles)
        float spread = 1.5f;
        glm::vec3 mainDirection = glm::vec3(0.0f, 0.0f, 10.0f);
        glm::vec3 randomDirection = glm::vec3(
                                                (rand()%2000 - 1000.0f)/1000.0f,
                                                (rand()%2000 - 1000.0f)/1000.0f,
                                                (rand()%2000 - 1000.0f)/1000.0f
                                             );

        ParticlesContainer[particleIndex].speed = mainDirection + randomDirection*spread;

//        if (ParticlesContainer[particleIndex].speed[2] > 11.0f || ParticlesContainer[particleIndex].speed[2] < 9.0f  ||
//            ParticlesContainer[particleIndex].speed[1] > 1.2f  || ParticlesContainer[particleIndex].speed[1] < -1.2f ||
//            ParticlesContainer[particleIndex].speed[0] > 1.2f  || ParticlesContainer[particleIndex].speed[0] < -1.2f
//            )
//        {
//            ParticlesContainer[particleIndex].r = 255;            // rand() % 256;
//            ParticlesContainer[particleIndex].g = 1;              // rand() % 256;
//            ParticlesContainer[particleIndex].b = 1;              // rand() % 256;
//            ParticlesContainer[particleIndex].a = 255;            // (rand() % 256) / 3;
//        }
//        else if (ParticlesContainer[particleIndex].speed[2] > 10.7f || ParticlesContainer[particleIndex].speed[2] < 9.3f  ||
//                 ParticlesContainer[particleIndex].speed[1] > 0.8f  || ParticlesContainer[particleIndex].speed[1] < -0.8f ||
//                 ParticlesContainer[particleIndex].speed[0] > 0.8f  || ParticlesContainer[particleIndex].speed[0] < -0.8f
//                )
//        {
//            ParticlesContainer[particleIndex].r = 255;            // rand() % 256;
//            ParticlesContainer[particleIndex].g = 255;              // rand() % 256;
//            ParticlesContainer[particleIndex].b = 1;              // rand() % 256;
//            ParticlesContainer[particleIndex].a = 255;            // (rand() % 256) / 3;
//        }
//        else
//        {
//            ParticlesContainer[particleIndex].r = 255;            // rand() % 256;
//            ParticlesContainer[particleIndex].g = 255;              // rand() % 256;
//            ParticlesContainer[particleIndex].b = 255;              // rand() % 256;
//            ParticlesContainer[particleIndex].a = 255;            // (rand() % 256) / 3;
//        }
        if( animationTime >= 6.3f && animationTime <= 9.1f )
        {
            ParticlesContainer[particleIndex].r = 255;            // rand() % 256;
            ParticlesContainer[particleIndex].g = 255;              // rand() % 256;
            ParticlesContainer[particleIndex].b = 150;              // rand() % 256;
            ParticlesContainer[particleIndex].a = 255;            // (rand() % 256) / 3;
        }
        else
        {
            // Very bad way to generate a random color
            ParticlesContainer[particleIndex].r = 255;            // rand() % 256;
            ParticlesContainer[particleIndex].g = 255;              // rand() % 256;
            ParticlesContainer[particleIndex].b = 150;              // rand() % 256;
            ParticlesContainer[particleIndex].a = 256;            // (rand() % 256) / 3;
        }



		ParticlesContainer[particleIndex].size = (rand()%1000)/2000.0f + 0.1f;
    }

    // Simulate all particles
    int ParticlesCount = 0;
    float totalLife = 5.0f;
    for (int i=0; i < MAX_PARTICLES; i++)
    {
        Particle& p = ParticlesContainer[i]; // shortcut

        if(p.life > 0.0f)
        {
            // Decrease life
            p.life -= delta;
            if(p.life > 0.0f)
            {
                // Simulate simple physics : gravity only, no collisions
                p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
                p.pos += p.speed*(float) delta;
                p.cameradistance = glm::length2(p.pos - cameraPosition);

                // Simulate fire
                if( p.life >= 3.0f)
                    p.b = (unsigned char)((int)(150.0f - 150.0f * ((totalLife-p.life) / 2.0f) ) );
                if( p.life >= 2.0f && p.life <= 4.0f )
                    p.g = (unsigned char)((int)(255.0f - 190.0f * ((4.0f-p.life) / 2.0f) ) );

                // Fill the GPU buffer
                g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
                g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
                g_particule_position_size_data[4*ParticlesCount+2] = p.pos.z;
                g_particule_position_size_data[4*ParticlesCount+3] = p.size;

                g_particule_color_data[4*ParticlesCount+0] = p.r;
                g_particule_color_data[4*ParticlesCount+1] = p.g;
                g_particule_color_data[4*ParticlesCount+2] = p.b;
                g_particule_color_data[4*ParticlesCount+3] = p.a;
            }
            else
            {
                // Dead particles placed at the end of the buffer
                p.cameradistance = -1.0f;
            }

            ParticlesCount++;
        }
    }

    SortParticles();
    //printf("%d ",ParticlesCount);

    // Bind particle Position
    glBindBuffer(GL_ARRAY_BUFFER, particlesVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, particleColorVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Use();
    // Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);


	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(m_TextureID, 0);

	glUniform3f(m_CameraRightLoc, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
	glUniform3f(m_CameraUpLoc   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

	glUniformMatrix4fv(m_VP, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);



	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particlesVBO);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 3rd attribute buffer : particles' colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particleColorVBO);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);

	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad

	// Draw Particles
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

    glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

}


int Particles::FindUnusedParticle()
{
    for(int i=lastUsedParticle; i<MAX_PARTICLES; i++){
		if (ParticlesContainer[i].life < 0){
			lastUsedParticle = i;
			return i;
		}
	}

	for(int i=0; i<lastUsedParticle; i++){
		if (ParticlesContainer[i].life < 0){
			lastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}


void Particles::SortParticles()
{
    std::sort(&ParticlesContainer[0], &ParticlesContainer[MAX_PARTICLES]);
}
