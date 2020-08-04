#ifndef OBJETOCUADRADO_H
#define OBJETOCUADRADO_H
#include <QGraphicsItem>
#include <QPainter>


class cuadrado:public QGraphicsItem
{
public:
    float r=30;
    float t=300;
    float posx;
    float posy;
    float radio;
    float radio2;
    cuadrado(); //Constructor
    void valores(float x,float y,float r1,float r2);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    ~cuadrado(); //Destructor
};

#endif // OBJETOCUADRADO_H
