#ifndef ATB_H
#define ATB_H

#include <AntTweakBar.h>

extern TwType TW_TYPE_VECTOR3F;
extern TwType TW_TYPE_VECTOR3I;

class ATB {

    public:
        ATB();

        ~ATB();

        bool Init(int width, int height);

        bool MouseCB(int button, int state);

        //bool keyboardCB (int key);

        void Render();

        void setDragonInfo();

        void setChickenInfo();

        TwBar* getTwBar();

    private:
        TwBar *myBar;
        typedef enum { DRAGON, CHICKEN } MESH_TYPE;
        MESH_TYPE m_currentMesh;

};



#endif // ATB_H


