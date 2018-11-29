#include "App.h"
#include "Classes/ogldev_util.h"
#include <sys/time.h>

App::App()
{
    frameCount = 0;
    frameTime = 0;
    fps = 0;

    frameTime = startTime = GetCurrentTimeInMillis();
}


void App::CalcFPS()
{
    frameCount++;

    long long time = GetCurrentTimeInMillis();

    if ( (time - frameTime) >= 1000)
    {
        frameTime = time;
        fps = frameCount;
        frameCount = 0;
    }
}


void App::RenderFPS()
{
    char text[32];
    ZERO_MEM(text);
    SNPRINTF(text, sizeof(text), "FPS: %d", fps);
}


float App::GetRunningTime()
{
    float RunningTime = (float)( (double)GetCurrentTimeInMillis() - (double)startTime ) / 1000.0f;
    return RunningTime;
}


long long App::GetCurrentTimeInMillis()
{
    timeval t;
    gettimeofday(&t, NULL);

    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
}
