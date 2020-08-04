#ifndef GRAFICAR_H
#define GRAFICAR_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include "crear.h"

class graficar: public QGraphicsItem          //Clase para graficar las esferas
{
public:
    graficar(float x, float y, float r, float vxi, float vyi, float m, float cR, float cF);
    ~graficar();
    QRectF boundingRect() const;    //necesario definirla, devuelve el rectangulo que encierra el objeto
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); //define como se pintara el objeto
    void setEscala(float s);
    void actualizar(float dt, float v_lim);
    crear* getEsf();
    void setVxi(double vxi);
    void setVyi(double vyi);
private:
    crear* esf;
    float escala;
};

#endif // GRAFICAR_H
