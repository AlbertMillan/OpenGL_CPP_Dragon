#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "math.h"
#include "ATB.h"

#define GLEW_STATIC

// https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/7.4.camera_class/camera_class.cpp
// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const GLfloat YAW         = -90.0f;
const GLfloat PITCH       = 0.0f;
const GLfloat SPEED       = 0.05f;
const GLfloat SENSITIVITY = 0.1f;
const GLfloat ZOOM        = 45.0f;

class Camera
{
    public:

        glm::vec3 position;
        glm::vec3 worldUp;
        glm::vec3 Front;

        Camera( glm::vec3 position = glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH ) : Front( glm::vec3( 0.0f, 0.0f, -1.0f) ), mouseSensitivity( SENSITIVITY ), zoom( ZOOM )
        {
            this->position = position;
            this->worldUp = up;
            this->yaw = yaw;
            this->pitch = pitch;
            this->movementSpeed = SPEED;
            this->updateCameraVectors();
        }

        Camera( GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ,
                GLfloat yaw, GLfloat pitch): Front( glm::vec3(0.0f, 0.0f, -1.0f) ), movementSpeed( SPEED ), mouseSensitivity( SENSITIVITY ), zoom( ZOOM )
        {
            this->position = glm::vec3( posX, posY, posZ );
            this->worldUp = glm::vec3( upX, upY, upZ );
            this->yaw = yaw;
            this->pitch = pitch;
            this->updateCameraVectors();
        }


        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt( this->position, this->position + this->Front, this->up );
        }

        void ProcessKeyboard( CameraMovement direction, GLfloat deltaTime )
        {
            GLfloat velocity = this->movementSpeed * deltaTime;

            // If statement used to enable motion when
            //  two or more keys are pressed.
            if( FORWARD == direction )
            {
                this->position += this->Front * velocity;
            }

            if( BACKWARD == direction )
            {
                this->position -= this->Front * velocity;
            }

            if( LEFT == direction )
            {
                this->position -= this->right * velocity;
            }

            if( RIGHT == direction )
            {
                this->position += this->right * velocity;
            }
        }

        // Processes input received from a mouse input system.
        // Expects the offset value in both the x and y direction.
        void ProcessMouseMovement( GLfloat xOffset, GLfloat yOffset,
                                    GLboolean constrainPitch = true )
        {
            xOffset *= this->mouseSensitivity;
            yOffset *= this->mouseSensitivity;

            this->yaw += xOffset;
            this->pitch += yOffset;

            if( constrainPitch )
            {
                if( this->pitch > 89.0f )
                {
                    this->pitch = 89.0f;
                }

                if( this->pitch < -89.0f )
                {
                    this->pitch = -89.0f;
                }
            }
            this->updateCameraVectors();

        }

        void ProcessMouseScroll( GLfloat yOffset )
        {
            if( this->zoom >= 1.0f && this->zoom <= 500.0f )
            {
                this->zoom -= yOffset;
            }

            if( this->zoom <= 1.0f )
            {
                this->zoom = 1.0f;
            }

            if( this->zoom >= 500.0f )
            {
                this->zoom = 500.0f;
            }
        }


        GLfloat GetZoom()
        {
            return this->zoom;
        }

        void AddToATB(TwBar* myBar)
        {
            TwAddButton(myBar, "Camera", NULL, NULL, "");
            TwAddVarRO(myBar, "Position", TW_TYPE_VECTOR3F, (void*)&position, NULL);
            TwAddVarRO(myBar, "Direction", TW_TYPE_DIR3F, &this->Front, " axisz=-z ");
            TwAddSeparator(myBar, "", NULL);
        }

    private:


        glm::vec3 up;
        glm::vec3 right;


        GLfloat yaw;
        GLfloat pitch;

        GLfloat movementSpeed;
        GLfloat mouseSensitivity;
        GLfloat zoom;

        void updateCameraVectors()
        {
            glm::vec3 front;
            front.x = cos( glm::radians( this->yaw ) ) * cos( glm::radians( this->pitch ) );
            front.y = sin( glm::radians( this->pitch ) );
            front.z = sin( glm::radians( this->yaw ) ) * cos( glm::radians( this->pitch ) );

            this->Front = glm::normalize( front );
            this->right = glm::normalize( glm::cross( this->Front, this->worldUp ) );
            this->up    = glm::normalize( glm::cross( this->right, this->Front ) );
        }

};




#endif
