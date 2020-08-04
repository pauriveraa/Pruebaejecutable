#include "graficar.h"
#include <QDebug>



graficar::graficar(float x, float y, float r, float vxi, float vyi, float m, float cR, float cF) : escala(1)
{
    esf=new crear(x,y,r,vxi,vyi,m,cR,cF);
}

graficar::~graficar()
{
    delete esf;
}

QRectF graficar::boundingRect() const
{
        return QRectF(-1*escala*esf->getR(),-1*escala*esf->getR(),2*escala*esf->getR(),2*escala*esf->getR());
}

void graficar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(Qt::darkMagenta);        //asigna el color
    painter->drawEllipse(boundingRect());    //dibuja una elipse encerrada en la boundingRect
}


void graficar::actualizar(float t, float v_lim)
{
    esf->actualizar(t);
    setPos(esf->getX()*escala,(v_lim-esf->getY())*escala);
    //Muestro la posicón del objeto según la coordenada x con getX y la
    //posición en y de acuerdo al límite menos la posición en y con getY
}

crear *graficar::getEsf()
{
    return esf; //Retorna el objeto esf de la clase crear
}

void graficar::setVxi(double vxi)
{
    esf->setVxi(vxi);
    //Le asigno al atributo vx de la clase crear (objeto esf) el valor de vxi
}

void graficar::setVyi(double vyi)
{
    esf->setVyi(vyi);
}


