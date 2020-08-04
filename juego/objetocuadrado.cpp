#include "objetocuadrado.h"

cuadrado::cuadrado()
{

}

void cuadrado::valores(float x, float y, float r1, float r2)
{
    posx = x;
    posy = y;
    radio = r1;
    radio2 = r2;
    setPos(posx,posy);

}

cuadrado::~cuadrado()
{

}

QRectF cuadrado::boundingRect() const
{
    return QRectF(-radio,-radio2,2*radio,2*radio2);
}

void cuadrado::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(Qt::blue);        //asigna el color
    painter->drawRect(boundingRect());
}
