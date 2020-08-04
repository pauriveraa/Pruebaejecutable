#ifndef CREAR_H
#define CREAR_H

#include <math.h>

class crear
{
private:
    float px;                       //posicion en X
    float py;                       //posicion en Y
    float rad;                      //radio
    double vx;                      //velocidad en X
    double vy;                      //velocidad en Y
    float ax;
    float ay;
    float masa;
    float cRest;
    float cFric;

public:
    crear(float x, float y, float r, float vxi, float vyi, float m, float cR, float cF);
    ~crear();
    void setVel(float x, float y);          //metodo para asignar la velocidad (en X y en Y)
    void setPoint(float x, float y);        //metodo para asignar la posicion (en X y en Y)
    void actualizar(float t);              //metodo que cambia los valores de posicion (realiza el movimiento), recibe el lapso de tiempo transcurrido

    void setVxi(double vxi);
    void setVyi(double vyi);
    void setR(double R);
    void setA(double ax, double ay);

    //metodos para retornar los atributos
    float getX() const;
    float getY() const;
    float getR() const;
    float getVx() const;
    float getVy() const;
    float getAx() const;
    float getAy() const;
    float getcRest() const;

};

#endif // CREAR_H
