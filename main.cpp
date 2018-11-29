#include <sys/time.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// IrrKlang sound includes
#include <irrKlang.h>

// Project includes
#include "App.h"
#include "ShaderUniformSetters.h"
#include "Skybox.h"
#include "Camera.h"
#include "Model.h"
#include "LightSource.h"
#include "ATB.h"
#include "Particles.h"
#include "Classes/ogldev_math_3d.h"

/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
#define MESH_DRAGON "dragonite7.dae"
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/

/***************** TODO *****************/
/* 2. Get rid of glm::mat4 and use Mat4 */
/* 3. Abstracting skybox does not work  */
/* 4. Clomplete model and light ATB     */
/* 5. Fix rep in Texture & Shader class */
/****************************************/



using namespace std;

struct DirectionalLight
{
    vector<Vector3f> light;
};

const int WIDTH = 2048;
const int HEIGHT = 1000;

// Shader
ShaderFunctions *shader;
Skybox* skybox;
LightSource* light;
Model *dragon;
App *myApp;

// Particle System
Particles* pSystem;
int m_currentTimeMillis = 0;
float prevTime = 0;
float startTime;

// Sound
irrklang::ISoundEngine* engine;
irrklang::ISoundSource* roar;
irrklang::ISoundSource* breath;

// UI
ATB *atb;

//Camera
Camera  camera(glm::vec3( 0.0f, 0.0f, 40.0f ) );
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool firstMouse = true;


bool keys[512];

float deltaTime = 0.0f;
float lastFrame = 0.0f;


void display()
{
    Vector3f LightSourcePos = Vector3f(-25.0, 200.0, 0.0);
    myApp->CalcFPS();
    long long TimeNowMillis = GetCurrentTimeMillis();
    assert(TimeNowMillis >= m_currentTimeMillis);
    unsigned int DeltaTimeMillis = (unsigned int)(TimeNowMillis - m_currentTimeMillis);
    m_currentTimeMillis = TimeNowMillis;
    float RunningTime = myApp->GetRunningTime();
    float delta = RunningTime - prevTime;
    prevTime = RunningTime;


    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Calculate all the objects matrices for model, view & projection
    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)WIDTH / (float)HEIGHT, 0.1f, 100000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 VP = projection * view;
    glm::mat4 skyboxView = glm::mat4( glm::mat3( camera.GetViewMatrix( ) ) );
	glm::mat4 dragonModel = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), glm::vec3(LightSourcePos.x, LightSourcePos.y, LightSourcePos.z));

    /********************** SKYBOX **********************/
	glDepthFunc( GL_LEQUAL );
    skybox->Use();
    glUniformMatrix4fv( glGetUniformLocation( skybox->ID, "view"), 1, GL_FALSE, glm::value_ptr(skyboxView));
    glUniformMatrix4fv( glGetUniformLocation( skybox->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    skybox->Render();
    glDepthFunc( GL_LESS );     // depth-pSysteming interprets a smaller value as "closer"
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    /******************** END SKYBOX ********************/

    /********************** DRAGON **********************/
	shader->Use();
    vector<Matrix4f> Transforms;
    dragon->BoneTransform(RunningTime, Transforms);

    for( uint i = 0; i < Transforms.size(); i++ )
    {
        shader->SetBoneTransform( i, Transforms[i]);
    }

	// update uniforms for dragon
	shader->SetWVT(dragonModel, projection, view);
    shader->SetDirectionalLight(light->color, 0.1f, LightSourcePos, Vector3f(camera.position.x, camera.position.y, camera.position.z));
	dragon->Render( shader );
    /******************** END DRAGON ********************/

    /********************** LIGHT ***********************/
    light->Use();
    light->SetShaderParams(view, projection, lightModel);
    light->SetColor();
	light->Render();
	/******************** END LIGHT ********************/

	/***************** PARTICLE SYSTEM *****************/
	float animationTime = fmod(RunningTime, 12.7f);
    pSystem->Render(delta, VP, view, camera.position, animationTime);
    /*************** END PARTICLE SYSTEM ***************/

    /***************** SOUND EFFECTS *******************/
    if( animationTime >= 6.3f && animationTime <= 9.1f)
    {
        engine->play2D(breath);
    }
    if( animationTime >= 0.2f && animationTime <= 0.6f )
    {
        engine->play2D(roar);
    }



    /**** MENU ****/
    atb->Render();

	glutSwapBuffers();
}


void updateScene()
{
    // Update deltaTime
    float currentFrame = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if( keys['w'] || keys[38] )
    {
        camera.ProcessKeyboard( FORWARD, deltaTime );
    }

    if( keys['s'] || keys[39] )
    {
        camera.ProcessKeyboard( BACKWARD, deltaTime );
    }

    if( keys['d'] || keys[40] )
    {
        camera.ProcessKeyboard( RIGHT, deltaTime );
    }

    if( keys['a'] || keys[37] )
    {
        camera.ProcessKeyboard( LEFT, deltaTime );
    }

    //cout << deltaTime << endl;

	glutPostRedisplay();
}


void init()
{
    light = new LightSource(1.0f, 1.0f, 1.0f);
    if(!light->Init()){
        printf("Error initializing the light shader functions\n");
        exit(0);
    }

    shader = new ShaderFunctions();
    if(!shader->Init()){
        printf("Error initializing the dragon shader functions\n");
        exit(0);
    }

	dragon = new Model();
	if( !dragon->LoadMesh( MESH_DRAGON ) )
	{
        printf("Dragon Mesh load failed\n");
        exit(0);
	}

    skybox = new Skybox();
    if(!skybox->Init()){
        printf("Error initializing the skybox shader functions\n");
        exit(0);
    }


    engine = irrklang::createIrrKlangDevice();
    if (!engine)
	{
		printf("Could not initialized sound engine\n");
		exit(0); // error starting up the engine
	}
	engine->play2D("Assets/Sound/FarHorizon.mp3", true);
	roar = engine->addSoundSourceFromFile("Assets/Sound/dragon_roar.mp3");
    breath = engine->addSoundSourceFromFile("Assets/Sound/dragon_breath.wav");

	pSystem = new Particles();
	if( !pSystem->Init() )
	{
        printf("Could not initialize particle system\n");
		exit(0);
	}

	myApp = new App();

    atb = new ATB();
    if( !atb->Init(WIDTH, HEIGHT) )
    {
        printf("Could not initialize TweakBar menu\n");
        exit(0);
    }
    dragon->AddToATB(atb->getTwBar());
    camera.AddToATB(atb->getTwBar());


    m_currentTimeMillis = GetCurrentTimeMillis();

    startTime = myApp->GetRunningTime();

    //TwAddVarRW(myBar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation, " axisz=-z ");
}

void close()
{
    delete(shader);
    delete(skybox);
    delete(light);
    delete(dragon);
    delete(myApp);
    delete(pSystem);
    engine->drop();

    exit(0);
}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y)
{
    // ESC pressed => exit
    if( key == 27 )
    {
        exit(0);
    }

    if( key > 0 && key <= 512 )
    {
        switch(key)
        {
            case 'j': atb->setDragonInfo(); break;
            case 'k': atb->setChickenInfo(); break;
            case 'q': close(); break;
            default: break;
        }
        keys[key] = true;
    }
}

void keyup (unsigned char key, int x, int y)
{
    keys[key] = false;
}

void MouseCallback( int xPos, int yPos )
{
    if( firstMouse )
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement( xOffset, yOffset );
}

void ScrollCallback (int button, int state, int xOffset, int yOffset)
{
    if ( button == GLUT_LEFT_BUTTON )
    {
        printf("Left Button Clicked!");
        atb->MouseCB(button, state);
    }
    if ( button == 3 )
    {
        camera.ProcessMouseScroll( yOffset );
    }
    if ( button == 4 )
    {
        camera.ProcessMouseScroll( -yOffset );
    }

}




int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Final Project");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(keyup);
	glutIdleFunc(updateScene);
	glutPassiveMotionFunc((MouseCallback));
	//glutMouseFunc((ScrollCallback)TwEventMouseButtonGLUT);
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}
