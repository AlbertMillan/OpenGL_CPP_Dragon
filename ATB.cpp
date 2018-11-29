#include "ATB.h"
#include "Classes/ogldev_math_3d.h"
#include <GL/freeglut.h>

TwType TW_TYPE_VECTOR3F;
TwType TW_TYPE_VECTOR3I;

ATB::ATB()
{

}


bool ATB::Init(int width, int height)
{
    bool ret = false;

    if( TwInit(TW_OPENGL_CORE, NULL) == 1)
    {
        TwWindowSize( width, height );

        myBar = TwNewBar("ARTHUR INFO");

        TwAddButton(myBar, "Mesh", NULL, NULL, "");

        // A variable for the current selection - will be updated by ATB
        m_currentMesh = DRAGON;

        // Array of drop down items
        TwEnumVal Meshes[] = { {DRAGON, "Dragon"}, {CHICKEN, "Chicken"}, };

        // ATB identifier for the array
        TwType MeshTwType = TwDefineEnum("MeshType", Meshes, 2);

        // Link it to the tweak bar
        TwAddVarRW(myBar, "Type", MeshTwType, &m_currentMesh, NULL);

        // Store Camera Poisitions. Create our own TwType for an array of 3 floats.
        TwStructMember Vector3fMembers[] = {
            { "x", TW_TYPE_FLOAT, offsetof(Vector3f, x), "" },
            { "y", TW_TYPE_FLOAT, offsetof(Vector3f, y), "" },
            { "z", TW_TYPE_FLOAT, offsetof(Vector3f, z), "" }
        };

        TW_TYPE_VECTOR3F = TwDefineStruct("Vector3f", Vector3fMembers, 3, sizeof(Vector3f), NULL, NULL);


    }

    return true;
}

void ATB::Render()
{
    TwDraw();
}

TwBar* ATB::getTwBar()
{
    return myBar;
}


void ATB::setDragonInfo()
{
    m_currentMesh = DRAGON;
}

void ATB::setChickenInfo()
{
    m_currentMesh = CHICKEN;
}




bool ATB::MouseCB(int button, int state)
{
    TwMouseButtonID btn = (button == GLUT_LEFT_BUTTON) ? TW_MOUSE_LEFT : TW_MOUSE_RIGHT;
    TwMouseAction ma = (state == GLUT_DOWN) ? TW_MOUSE_PRESSED : TW_MOUSE_RELEASED;

    return (TwMouseButton(ma, btn) == 1);
}
