#ifndef OBJETOCIRCULAR_H
#define OBJETOCIRCULAR_H

#include <QGraphicsItem>

class circulo : public QGraphicsItem
{

public:
    circulo();
    QRectF boundingRect() const;  //Forma
    QPainterPath shape() const;   //Color
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void advance(int phase);
};

#endif // OBJETOCIRCULAR_H



