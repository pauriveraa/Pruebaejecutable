#include "objetocircular.h"
#include <QPainter>
#include <cmath>

circulo::circulo()
{
  //  setFlags(ItemIsMovable);            //Habilitar movimiento con el mouse
}

QRectF circulo::boundingRect() const
{
    return QRectF(-20, 60, 15, 15);    //rectángulo que demarca los límites del item gráfico
}

QPainterPath circulo::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());    //la forma del objeto gráfico es circular
    return path;
}

void circulo::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::black);         //color del contorno
    painter->setBrush(Qt::green);       //color del relleno
    painter->drawEllipse(boundingRect());
}

void circulo::advance(int phase)
{

}


