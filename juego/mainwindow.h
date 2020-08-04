#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define GRAV 10
#define TIME 0.1
#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QFile>
#include <QImage>
#include "crear.h"
#include "graficar.h"
#include<stdlib.h>
#include<time.h>
#include "objetocircular.h"
#include "objetocuadrado.h"
#include <QMessageBox>
#include <QString>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString nombre;
    QString jugador1;
    QString jugador2;
private slots:
    void actualizar();
    bool objColision(crear *b, crear *b2);
    void nivelN();
    void mover();
    void posicion();
    void serial_read();

    void on_pushButton_clicked();

    void on_potencia_valueChanged(int value);

    void on_angulo_valueChanged(int value);

    void on_reiniciar_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_REGISTRARVS_clicked();

    void on_disparovs_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_mutear_clicked();

private:
    int a;
    Ui::MainWindow *ui;
    QTimer *timer;              //timer para los intervalos de tiempo entre cada frame
    QTimer *timer2;
    QTimer *timer3;
    QTimer *timer4;
    QGraphicsScene *scene;      //scene que muestra los objetos animados
    float puntaje=0;
    float t;                   //intervalo de tiempo entre frames
    int h_limit;                //longitud en X del mundo
    int v_limit;                //longitud en Y del mundo
    int nivel=1;
    int controldemundos=1;
    int prueba=0;
    int angulo=0; //value
    int potencia; //value
    void bordercollision(crear *b);   //metodo para las colisiones con los bordes del mundo
    graficar*parabolico; //circulo a lanzar con tiro parabolico
    graficar*objetivo;  //circulo objetivo
    circulo *obstaculo2; //obstaculo con trayectoria circular
    cuadrado *obstaculo1; //obstaculo cuadrado
    float rad;//Ángulo(radianes)
    float x,y,i;
    //Datos del circulo con trayectoria de un pendulo
    double x1_obstaculo3;
    double x2_obstaculo3;
    double y1_obstaculo3;
    double y2_obstaculo3;
    double radio1_obstaculo3;
    double radio2_obstaculo3;
    double magnitud;
    QGraphicsEllipseItem *obstaculo3;
    QPen pen;
    bool colision;
    bool collide;
    bool collide2;
    QMessageBox msgBox;
    int contdisparos=12;
    bool bandera1=true;
    int auxiliar=0;
    int banderaverificador=0;
    int banderaverificadorvs=0;
    int puedoguardar=0;

//variables para jugador
    float puntaje1=0;
    int nivel1=1;
    int  contdisparos1=4;
    ////// variables jugador2
    int banderajugador2=0;
    int contdisparos2=0;
     int puntaje2=0;
     int terminacion=0;

     //arduino
     QSerialPort *serial;
     QString portname;
     quint16 vendorId;
     quint16 productId;
     bool arduino_available=false;
     void arduino_init();
     int pulsadortrampa=0;
     int controladorarduino=1;

};

#endif // MAINWINDOW_H

