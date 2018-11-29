#ifndef APP_H
#define APP_H

class App
{
public:
    App();

    void CalcFPS();

    void RenderFPS();

    float GetRunningTime();

private:
    long long frameTime;
    long long startTime;
    int frameCount;
    int fps;

    long long GetCurrentTimeInMillis();
};
#endif // APP_H
