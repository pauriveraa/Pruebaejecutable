#include "crear.h"
#include <QDebug>
#define pi 3.14

crear::crear(float x, float y, float r, float vxi, float vyi, float m,
                     float cR, float cF):px(x),py(y),rad(r),vx(vxi),vy(vyi), masa(m), cRest(cR), cFric(cF)
{

}

crear::~crear()
{

}

void crear::setVel(float x, float y)
{
    vx=x;
    vy=y;
}

void crear::setPoint(float x, float y)
{
    px=x;
    py=y;
}

void crear::actualizar(float t)
{
    ax=-sqrt(pow(vx,2)+pow(vy,2))*cFric*pi*pow(rad,2)*cos(atan2(vy,vx))/(2*masa);
    ay=(-sqrt(pow(vx,2)+pow(vy,2))*cFric*pi*pow(rad,2)*sin(atan2(vy,vx))/(2*masa))-20;
    vx+=ax*t;
    vy+=ay*t;
    px+=vx*t+ax*pow(t,2)/2;
    py+=vy*t+ay*pow(t,2)/2;
}

void crear::setVxi(double vxi)
{
    vx=vxi;
}

void crear::setVyi(double vyi)
{
    vy=vyi;
}

void crear::setR(double R)
{
    rad=R;
}


float crear::getX() const
{
    return px;
}

float crear::getY() const
{
    return py;
}

float crear::getR() const
{
    return rad;
}

float crear::getVx() const
{
    return vx;
}

float crear::getVy() const
{
    return vy;
}
float crear::getAx() const
{
    return ax;
}

float crear::getAy() const
{
    return ay;
}

float crear::getcRest() const
{
    return cRest;
}

