#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <fstream>
#include <QLabel>
#include <QPixmap>
#include <QtWidgets>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <QString>
#include <QtGui>
#include <QImage>
#include <QSound>
#include <QGraphicsPixmapItem>// para item:V pues colocar anuncio o texto
#include <QObject>
using namespace std;

///////////////////////// Creo objetos de la clase QSound ///////////////////////////////////////////////////
QSound *Cancion=new QSound("C:../versión 13/juego/ata.wav");
QSound *Cancion2=new QSound("C:../versión 13/juego/disparador.wav");
QSound *Cancion3=new QSound("C:../versión 13/juego/contrapared.wav");
QSound *Cancion4=new QSound("C:../versión 13/juego/punto.wav");
QSound *Cancion5=new QSound("C:../versión 13/juego/boton.wav");
QSound *Cancion6=new QSound("C:../versión 13/juego/mundo22.wav");
QSound *Cancion7=new QSound("C:../Versión 13/juego/tetriz.wav");


/////////////////////////////////////////////Inicializo parámetros con constructor/////////////////////////////////////////
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    x=y=i=10; //Datos para cambiar posición del objeto con trayectoria circular
    rad=0.01745329252; //angulo en radianes para variar la posición del objeto con trayectoria circular
    potencia=0; //value de potencia
    angulo=0;
    t=0.01; //"tiempo" cada cuánto va dibujar los objetos objetivo  parabolico con la funcion actualizar
    h_limit=900; //Limites del graphicsview para colisiones
    v_limit=300;
    nivel=1; //contadorpara determinar los niveles
    ui->label_14->hide(); //Esconder imagen de arduino
    Cancion->setLoops(10); // Se repite 10 veces la cancion
    Cancion->play();


///////////////////////////////////Implementación con arduino//////////////////////////////////////////////

    serial= new QSerialPort(); //Creo objeto de la clase QSerialPort
    arduino_available = false; //Inicializo bandera para señalar si hay arduino conectado

    foreach (const QSerialPortInfo &serial_Info, QSerialPortInfo::availablePorts()) //Detectar información del arduino
    {
        qDebug()<<"puerto:"<<serial_Info.portName();
        portname = serial_Info.portName();
        qDebug() << "vendor iD"<<serial_Info.vendorIdentifier();
        vendorId=serial_Info.vendorIdentifier();
        qDebug() <<"product id"<<serial_Info.productIdentifier();
        productId=serial_Info.productIdentifier();
        arduino_available=true; //Si hay detección de estos parámetros, indico que hay un arduino conectado
    }
///////////////////////////////// Si se detecta arduino//////////////////////////////////////////////
    if(arduino_available)
    {
        arduino_init(); //Llamo este método si hay un arduino conectado.
        ui->label_14->show();  //Muestro logo de arduino
    }
////////////////////////////Pongo en escena el gif con mi nombre////////////////////////////////////////////

    ui->label_11->setWindowFlag(Qt::FramelessWindowHint);
    ui->label_11->setMask((new QPixmap("C:../versión 13/juego/Giff.gif"))->mask());
    QMovie *movie=new QMovie("C:../versión 13/juego/Giff.gif"); //Creo objeto de la clase QMovie
    ui->label_11->setMovie(movie); //Agrego el giff a este label
    movie->start(); //Play al giff para que no quede como imagen
    ui->label_11->show(); //Pongo en escena el giff

////////////////////////////Tiempos (temporizadores) y escena///////////////////////////////////////////////

    timer=new QTimer(this);  //Nuevo reloj para el objeto parabolico
    timer2=new QTimer(this);  //Nuevo reloj para el objeto con trayectoria circular
    timer3=new QTimer(this);  //Nuevo reloj para el objeto pendulo
    scene=new QGraphicsScene(this);         //crea la escena
    scene->addRect(scene->sceneRect());
    scene->setSceneRect(120,100,250,120);   //Set de la escena con los parametros (x,y,ancho,alto)
    scene->setBackgroundBrush(QImage("C:../versión 13/juego/mundo1.jpg"));
    ui->graphicsView->setScene(scene);
    this->resize(640, 480);
    setFixedSize(650,400);// ancho y altura del mainwindow mas no de la view
    qDebug()<<this->size();

////////////////////////////objetivo//////////////////////////////////////////////////////

    objetivo= new graficar(420,100, 30, 0,0,50,0,0); //Se crea objeto al cual se le debe pegar para ganar puntos
    objetivo->actualizar(t,v_limit);  //Se actualiza posicion del objeto en base al tiempo y el limite del graphicview
    scene->addItem(objetivo); //Se pone el objeto en escena
    msgBox.setWindowTitle("SPACE");

//////////////////Agrego un lcd para mostrar puntaje y contador de disparos////////////////////

     ui->lcdNumber->display(puntaje); //Motrar puntaje en el modo single
     ui->lcdNumber_2->display(contdisparos);  //para mostrar contador de disparos en el modo single
     ui->lcdNumber_2->hide();

/////////////////////////////pendulo//////////////////////////////////////////////////////

     QBrush brush(Qt::black); //pintar el objeto
     //Parámetros de posición y dimensiones
     x1_obstaculo3 = 200.0;
     y1_obstaculo3 = 0.0;
     x2_obstaculo3 = 0.0;
     y2_obstaculo3 = 0.0;
     radio1_obstaculo3 = 15.0;
     radio2_obstaculo3 = 15.0;
     //Creo el objeto con los parámetros y determino valor de magnitud
     obstaculo3 = scene->addEllipse(x1_obstaculo3+200,  y1_obstaculo3,radio1_obstaculo3, radio2_obstaculo3, pen, brush);
     magnitud = sqrt(pow(x1_obstaculo3 - x2_obstaculo3, 2.0)+ pow(y1_obstaculo3 - y2_obstaculo3, 2.0)); //Distancia entre dos puntos
     connect(timer3,SIGNAL(timeout()),this,SLOT(mover())); //Conecto el reloj con el método mover
     timer3->start(20); //Inicio en reloj


//////////////////////////////////Bolita tiro parabolico///////////////////////////////////


    parabolico=new graficar(10,10,10,0,0,30,0.5,0.01); //Creo el objeto parabolico
    parabolico->actualizar(t,v_limit);   //Actualizar posicion de objeto segun el tiempo y el limite del grahicview
    scene->addItem(parabolico);  //Poner en escena el objeto parabolico
    timer->stop();    //Para el timer
    connect(timer,SIGNAL(timeout()),this,SLOT(actualizar()));  //Conecto el tiempo con la funcion actualizar

////////////////////////////objeto trayectoria circular ///////////////////////////////////

    obstaculo2 = new circulo;  //Creo objeto circular
    //scene->addItem(obstaculo2);
    timer2->start(4);
    timer2->stop();  //Para el timer de este objeto
    //connect(timer2,SIGNAL(timeout()),this,SLOT(posicion()));


////////////////////////////////objeto cuadrado/////////////////////////////////////////////

    obstaculo1 = new cuadrado;   //Creo objeto cuadrado
    //obstaculo1->valores(230,240,20,20);
    //scene->addItem(obstaculo1);

/////////////////////Dejo de mostrar esto en el mainwindow con .hide()/////////////////////////////////

    ui->graphicsView->hide();

    ui->lcdNumber->hide(); //puntaje en modo single
    ui->lcdNumber_2->hide(); //contador disparos en modo single
    ui->lcdNumber_3->hide();  //contador de mundos en el modo single
    ui->lcdNumber_5puntaj2->hide();  //puntaje jugador 2 vs
    ui->lcdNumber_4puntaje1->hide();  //puntaje jugador 1 en vs
    ui->contavs->hide();     //contado disparos en vs
    ui->contanivel->hide(); //Fuera de uso

    ui->pushButton->hide();  //Para generar disparo en modo single
    ui->pushButton_2->hide();  //Para ingresar al modo single después de registrarse
    ui->pushButton_4->hide();  //Para registrar usuario en mdo single
    ui->reiniciar->hide();   //FUERA DE USO
    ui->REGISTRARVS->hide();  //Registrar juadores en el modo vs
    ui->disparovs->hide();    //Generar disparo en el vs
    ui->pushButton_7->hide();    //Guardar partida en modo single
    ui->pushButton_8->hide();    //Cargar partida en modo single
    ui->pushButton_9->show(); //Para mostrar instrucciones
    ui->pushButton_10->hide(); //Para dejar de mostrar instrucciones

    ui->potencia->hide();   //Slider para modificar la variable potencia
    ui->angulo->hide();     //Slider para modificar la variable angulo

    ui->label->hide();    //DISPAROS - en modo single
    ui->label_2->hide();    //PUNTAJE - en modo single
    ui->label_3->hide();   //MUNDO - en modo single
    ui->label_4->hide();   //USUARIO - correspone al modo single para indicar que es el jugador
    ui->label_5->hide();  //Para mostrar el nombre del jugador 1 en vs
    ui->label_6->hide();  //Para mostrar el nombre del jugador 2 en vs
    ui->label_7->hide();   //DISPAROS  - en vs
    ui->label_8->hide();   //PUNTAJES - en vs
    ui->label_9->hide();  //NIVEL - FUERA DE USO
    ui->label_12->hide(); //Mostrar anglo con joystick
    ui->label_13->hide(); //Mostrar potencia con joystick
    ui->label_15->hide();
    ui->label_5JUGADOR1->hide();  //PLAYER 1
    ui->label_6JUGADOR2->hide();  //PLAYER 2
    ui->labelnombre->hide();      //Para mostrar el nombre que se registra en modo single


    ui->lineEdit->hide();  // Para ingresar nombre de usuario en modo single
    ui->lineEdit_2JUGADOR1->hide();  // Para ingresar nombre de usuario 1 en modo vs
    ui->lineEdit_3JUGADOR2->hide();  // Para ingresar nombre de usuario 2 en modo vs
}

MainWindow::~MainWindow()
{
    delete timer;
    delete scene;
    delete ui;
}


bool MainWindow::objColision(crear *b, crear *b2) //Recibo el objeto 1 y 2 para analizar si hay colision
//     Hallo la distancia entre los dos objetos y verfifico si es menor a la suma de sus radios,
//     en ese caso retorno true, colisionaron.
{
    if(sqrt(((b->getX()-b2->getX())*((b->getX())-b2->getX()))+((b->getY()-b2->getY())*(b->getY()-b2->getY())))<= b->getR()+b2->getR())
        return true;
    return false;
}

void MainWindow::nivelN()
{
    /*
Esta funcion verifica inicialmente si bandera verificador es 0 (para saber si está en modo single o vs)
y si hay colision entre e objeto parabolico y el objetivo, de esta forma incrementa nivel que inica en 1,
modifica el puntaje, borra el objetivo y vuelve a ubicar el objeto parabolico en su posicion inicial.
Según el valor de nivel cambiarán los parámetros con los que se crea el objetivo y actualiza su posicion y se muestra puntaje.
    */
    if(banderaverificadorvs==0 &&objColision(parabolico->getEsf(), objetivo->getEsf())==true)
    {
        //scene->setBackgroundBrush(QImage("C:../versión 13/juego/mundo1.jpg"));
        Cancion4->play();
        contdisparos++; //Lleva conteo de disparos en modo single
        puntaje=puntaje+100; //Incrementa la variavle puntaje en modo single
        nivel++; //Varible para determinar el cambio de nivel
        delete objetivo; //Borrar objetivo
        parabolico->getEsf()->setPoint(10,10); //Ubicar en la posicion inicial
        parabolico->setVxi(0);
        parabolico->setVyi(0);
        if(nivel==2)
        {
             objetivo=new graficar(400,50, 30, 0,0,50,0.5,0); //Graficar el objetico con nuevos parámetros
             objetivo->actualizar(t,v_limit);
             scene->addItem(objetivo);
             ui->lcdNumber->display(puntaje);
        }

        if(nivel==3)
        {
            objetivo=new graficar(400,150, 30, 0,0,50,0.5,0); //Graficar el objetico con nuevos parámetros
            objetivo->actualizar(t,v_limit);
            scene->addItem(objetivo);
            ui->lcdNumber->display(puntaje);
        }
        if(nivel==4)
        {
            objetivo=new graficar(350,100, 25, 0,0,50,1,0); //Graficar el objetico con nuevos parámetros
            objetivo->getEsf()->setVel(0, 0);
            objetivo->actualizar(t,v_limit);
            scene->addItem(objetivo);
            ui->lcdNumber->display(puntaje);
        }

/*
 Nivel igual a 5 lo uso como un avisador de que terminarmos ese mundo, incremento la variable controldemundos
 para cambiar al mundo 2 ó 3.
*/
        if(nivel==5)
        {
           ui->lcdNumber->display(puntaje);
           controldemundos++;
           if(nivel==5&&controldemundos==4) //Para determinar si el jugador gana.
           {
               msgBox.setText("YOU WIN TERMINASTE");
               msgBox.setInformativeText("");
               msgBox.exec();
               close();
           }

           //Grafica objetivo con carcteristicas de nivel =1
           objetivo=new graficar(420,100, 30, 0,0,50,0,0);
           objetivo->getEsf()->setVel(0, 0);
           objetivo->actualizar(t,v_limit);
           scene->addItem(objetivo);

           //Para mostrar un mensaje cuando cambiamos de nivel
           if(controldemundos!=4)
           {
               msgBox.setText("PASASTE AL SIGUIENTE MUNDO");
               msgBox.setInformativeText("");
               msgBox.exec();
               ui->lcdNumber_3->display(controldemundos); //Mostramos el cambio de mundo
               scene->setBackgroundBrush(QImage("C:../versión 13/juego/mundo2sustituto.jpg")); //Pongo en escena el mundo 2
           }

           //Se adiciona al escenario el objeto cuadrado desde el mundo 2
           obstaculo1->valores(230,240,20,20);
           scene->addItem(obstaculo1);
           nivel=1; //nivel vuelve a ser 1 para repatir las características anteriores de los niveles.
           Cancion->stop();
           Cancion6->setLoops(10); // Se repite 10 veces la cancion
           Cancion6->play();

           if(controldemundos==3)
           {
               //Se adiciona el objeto con trayectoria circular para completar caracteristicas del mundo 3
               scene->setBackgroundBrush(QImage("C:../versión 13/juego/mundo3.jpg")); //Pongo en escena el fondo mundo 3
               scene->addItem(obstaculo2);
               timer2->start(12);
               connect(timer2,SIGNAL(timeout()),this,SLOT(posicion()));
               Cancion6->stop();
               Cancion7->setLoops(10); // Se repite 10 veces la cancion
               Cancion7->play();
           }
        }
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Si banderaverificadorvs es 2, es porque estamos en modo vs.
Si e cumple que banderaverificadorvs es 2 y hay colision entre el objetivo y parabolico, entonces se
incrementa la variable puntaje
*/
    if(banderaverificadorvs==1 &&objColision(parabolico->getEsf(), objetivo->getEsf())==true)
    {
        //scene->setBackgroundBrush(QImage("C:../versión 13/juego/mundo2sustituto.jpg"));
        Cancion4->play();
        delete objetivo;

//DETERMINO QUIEN GANA si se hacen los 4 niveles
        if(terminacion==1) //Entramos cuando va iniciar jugador 2
        {
            terminacion=0;
            contdisparos1=3; //Porque ya habría tenido su primer disparo
            banderajugador2=1; //Señal de inicio jugador 2
            ui->contavs->display(contdisparos1);
            objetivo= new graficar(420,100, 30, 0,0,50,0,0); //Graficamos el objetivo
            objetivo->actualizar(t,v_limit);
            scene->addItem(objetivo);
            puntaje1=puntaje1+100; //Sumo los últimos 100 del jugador 1
            puntaje2=puntaje2-100; //Resto 100 que se van a sumar más abajo de esta vuelta, pero que aún corresponde al último lanzamiento del jugador 1
            msgBox.setText("TURNO JUGADOR 2");
            msgBox.setInformativeText("");
            msgBox.exec();
        }
        if(terminacion==2) //A partir de tres if, definimo cuál jugador es el ganador según el puntaje acumulado.
        {
            contdisparos1++;
            puntaje2=puntaje2+100;
            objetivo= new graficar(420,100, 30, 0,0,50,0,0);
            objetivo->actualizar(t,v_limit);
            scene->addItem(objetivo);
            if(puntaje1<puntaje2)
            {
                msgBox.setText("GANADOR JUGADOR 2");
                msgBox.setInformativeText("");
                msgBox.exec();
                close();
            }
            if(puntaje1>puntaje2)
            {
                msgBox.setText("GANADOR JUGADOR 1");
                msgBox.setInformativeText("");
                msgBox.exec();
                close();
            }
            if(puntaje1==puntaje2)
            {
                msgBox.setText("EMPATADOS");
                msgBox.setInformativeText("");
                msgBox.exec();
                close();
            }

        }

        if(banderajugador2==0) //Esta bandera inicia en cero, se suma 100 al puntaje de del jugador 1 por atinar al objetivo
        {
        contdisparos1++;
        puntaje1=puntaje1+100;
        }

        if(banderajugador2==1)//Si es uno, se suma 100 al puntaje de del jugador 2 por atinar al objetivo
        {
            contdisparos1++;
            puntaje2=puntaje2+100;
        }

        ui->lcdNumber_5puntaj2->display(puntaje2); //Muestro puntaje jugador 2
        ui->lcdNumber_4puntaje1->display(puntaje1); //Muestro puntaje jugador 1
        nivel1++; //Cambia de nivel
        ui->contavs->display(contdisparos1); //Muestro puntaje
        ui->contanivel->display(nivel1);  //Para mostrar el nivel (FUERA DE USO POR AHORA)
        parabolico->getEsf()->setPoint(10,10); //Devuelvo el objeto a lanzar a sus condiciones iniciales.
        parabolico->setVxi(0);
        parabolico->setVyi(0);

        // ESTOS SON LOS 4 NIVELES
        //Cambiar poición y tamaño del obetivo según el nivel
        if(nivel1==2)
        {
            objetivo=new graficar(400,50, 30, 0,0,50,0.5,0);
            objetivo->actualizar(t,v_limit);
            scene->addItem(objetivo);
            ui->contavs->display(contdisparos1);
            ui->contanivel->display(nivel1);
        }
        if(nivel1==3)
        {
            objetivo=new graficar(400,150, 30, 0,0,50,0.5,0);
            objetivo->actualizar(t,v_limit);
            scene->addItem(objetivo);
            ui->contavs->display(contdisparos1);
            ui->contanivel->display(nivel1);
        }

        if(nivel1==4)
        {
            //Aparte de definir características de nivel 4, se reinicia la variable nivel1=0
            objetivo=new graficar(350,100, 25, 0,0,50,1,0);
            objetivo->getEsf()->setVel(0, 0);
            objetivo->actualizar(t,v_limit);
            scene->addItem(objetivo);
            ui->contanivel->display(nivel1);
            nivel1=0;
            terminacion=1; //Para indicar que va empezar jugador 2
            if(banderajugador2==1)//No entramos aquí cuando termina el jugador 1, pero si cuando termine el jugador 2
            {
                terminacion=2; //Bandera para indicar que va terminar el jugador 2
                qDebug()<<terminacion;
            }
            if(contdisparos1==0) //Recargar para jugador 2
            {
                contdisparos1=4;
            }
            ui->contavs->display(contdisparos1); //Mostrar en el lcd la cantida de disparos.
        }
    }

/////////////////////////////////////////////////////////////////////////////////

    ui->lcdNumber_2->display(contdisparos);
//PARAMETROS DE COLISION PARA MODO SINGLE Y MULTIJUGADOR
// Si el el objeto parabolio colisiona con alguno de los obstaculos
 //////////////////////Colision con cuadrado///////////////////////////

    collide=parabolico->collidesWithItem(obstaculo1); //Colision entre el objeto parabolico y el cuadrado
    if(collide==true)
    {
        Cancion3->play();
        parabolico->getEsf()->setPoint(10,10); //Ubico el objeto parabolico en posicion inicial
        parabolico->setVxi(0);
        parabolico->setVyi(0);
        if(contdisparos==0||contdisparos1==0)//Entramos cuando los jugadores no tengan disparos
        {
            auxiliar= auxiliar+1;
            if(contdisparos==0 && auxiliar ==2)
            {
                controladorarduino=1; //Detener el arduino
                timer->stop(); //Detengo el reloj del objeto parabolico
                msgBox.setText("YOU LOSE murito");
                msgBox.setInformativeText("");
                msgBox.exec();
                close();
            }
            if(contdisparos1==0&&auxiliar==2)
            {
                if(banderajugador2==0) //REINICIO PARA JUGADOR2
                {
                    msgBox.setText("TURNO JUGADOR 2");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    contdisparos1=4;//recargar
                    ui->contavs->display(contdisparos1);
                    banderajugador2=1; //Indica que va empezar el jugador 2
                    nivel1=1; //Reinicio para jugador 2
                    terminacion=0;
                    delete objetivo;
                    objetivo= new graficar(420,100, 30, 0,0,50,0,0);
                    objetivo->actualizar(t,v_limit);
                    scene->addItem(objetivo);
                }

                // JUGADOR 2 ESTABA ACTIVADO(banderajugador2==1)
                //Determino cuál de los dos jugadores es el ganador
                if(banderajugador2==1 && puntaje1<puntaje2&&contdisparos1==0)
                //Si bandera jugador es 1 y contdisparos1 es cero habrá finalizado el jugador 2
                //y verificamos si hay un ganador o empate.
                {
                    msgBox.setText("GANADOR JUGADOR 2");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();
                }
                if(banderajugador2==1 && puntaje1>puntaje2&&contdisparos1==0)
                {
                    msgBox.setText("GANADOR JUGADOR 1");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();
                }
                if(banderajugador2==1 && puntaje1==puntaje2&&contdisparos1==0)
                {
                    msgBox.setText("EMPATADOS");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();
                }
            }
        }
    }

 ///////////////////////////////Colision con pendulo//////////////////
//Determino si hubo colision entre objeto parabolico y obstaculo 3 (MAS)
    collide=parabolico->collidesWithItem(obstaculo3);
    if(collide==true)
    {
        Cancion3->play();
        //Pongo al objeto tiro parabolico en su posición inicial
        parabolico->getEsf()->setPoint(10,10);
        parabolico->setVxi(0);
        parabolico->setVyi(0);
        if(contdisparos==0||contdisparos1==0) //Determino si han finalizado partida en alguno de los modos
        {
            auxiliar= auxiliar+1; //visador de que el juego está por terminar
            if(contdisparos==0 && auxiliar ==2) //Finalización en el modo single
            {
                controladorarduino=1; //Detengo el interruptor para no recibir señal después de iniciada la partida
                timer->stop(); //Detengo reloj de objeto parabolico
                msgBox.setText("YOU LOSE pendulo");
                msgBox.setInformativeText("");
                msgBox.exec();
                close();
            }
            if(contdisparos1==0&&auxiliar==2) //Reviso si ya finalizó alguno de lo dos jugadores
            {
                if(banderajugador2==0) //Si finaliza el jugador 1, preparo todo para el jugador 2
                {
                    msgBox.setText("TURNO JUGADOR 2");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    contdisparos1=4;//recargar
                    ui->contavs->display(contdisparos1);
                    banderajugador2=1; //Avisador de que inicia el jugador 2
                    nivel1=1; //Reinicio nivel
                    terminacion=0;
                    delete objetivo; //Borro el objetivo que ya existía
                    objetivo= new graficar(420,100, 30, 0,0,50,0,0); //Creo objetivo con parámetros de nivel 1
                    objetivo->actualizar(t,v_limit); //Actualizo valores
                    scene->addItem(objetivo); //Pongo en escena el objetivo
                }
                //Si ambos jugadores finalizaron, decido si hubo ganador o empate
                //Con estos tres if que comparan sus puntajes
                if(banderajugador2==1 && puntaje1<puntaje2&&contdisparos1==0)
                {
                    msgBox.setText("GANADOR JUGADOR 2");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();
                }
                if(banderajugador2==1 && puntaje1>puntaje2&&contdisparos1==0)
                {
                    msgBox.setText("GANADOR JUGADOR 1");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();
                }
                if(banderajugador2==1 && puntaje1==puntaje2&&contdisparos1==0)
                {
                    msgBox.setText("EMPATADOS");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();
                }
            }
        }
    }

////////////////////////Colison con objeto circular ///////////////////////////////

//Se toman y modifican los mismos parámetros que en la colisión con los dos obstaculos anteriores
    collide=parabolico->collidesWithItem(obstaculo2); //Si hay colisión con el obstáculo 2, retorna true
    if(collide==true)
    {
        Cancion3->play();
        parabolico->getEsf()->setPoint(10,10);
        parabolico->setVxi(0);
        parabolico->setVyi(0);
        if(contdisparos==0||contdisparos1==0)
        {
            auxiliar= auxiliar+1;
            if(contdisparos==0 && auxiliar ==2)
            {
                controladorarduino=1;
                timer->stop();
                msgBox.setText("YOU LOSE orbita");
                msgBox.setInformativeText("");
                msgBox.exec();
                close();

            }
            if(contdisparos1==0&&auxiliar==2)
            {
                if(banderajugador2==0)
                    //Inicializo variables para jugador 2
                {
                    msgBox.setText("TURNO JUGADOR 2");
                    msgBox.exec();
                    contdisparos1=4;
                    ui->contavs->display(contdisparos1);
                    banderajugador2=1;
                    nivel1=1;
                    terminacion=0;
                    delete objetivo;
                    objetivo= new graficar(420,100, 30, 0,0,50,0,0);
                    objetivo->actualizar(t,v_limit);
                    scene->addItem(objetivo);
                }
                //Defino un ganador del vs
                if(banderajugador2==1 && puntaje1<puntaje2&&contdisparos1==0)
                {
                    msgBox.setText("GANADOR JUGADOR 2");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();
                }
                if(banderajugador2==1 && puntaje1>puntaje2&&contdisparos1==0)
                {
                    msgBox.setText("GANADOR JUGADOR 1");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();
                }
                if(banderajugador2==1 && puntaje1==puntaje2&&contdisparos1==0)
                {
                    msgBox.setText("EMPATADOS");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();
                }

            }



        }
    }
}

/////////////////////////////Colisión con los bordes///////////////////////////////////

void MainWindow::bordercollision(crear *b)
//Funcion para detectar colision entre el objeto parabolico y los limites.
{
    if(b->getX()<= b->getR())
    {
        b->setVel(-b->getcRest()*b->getVx(),b->getVy()); //Para los choques con los bordes y que afecte la velocidad y posicion
        b->setPoint(b->getR(),b->getY());
    }
    else if(b->getX()>=h_limit-b->getR())
    //PARA DETECTAR SI HAY COLISION ENTRE EL OBJETO PARABOLICO Y EL LIMITE DERECHO
    {
        parabolico->getEsf()->setPoint(10,10); //UBICO EN LA POSICION INICIAL
        parabolico->setVxi(0);
        parabolico->setVyi(0);
        if(contdisparos==0||contdisparos1==0) //Verifico que hayan finalizado en los dos modos
        {
            auxiliar= auxiliar+1; //Avisador de que está por terminar el juego
            if(contdisparos==0 && auxiliar ==2) //Finaliza el modo single
            {
                controladorarduino=1; //Finalizo la intervención del arduino
                timer->stop();
                msgBox.setText("YOU LOSE muroderecha");
                msgBox.setInformativeText("");
                msgBox.exec();
                close();
            }
            if(contdisparos1==0&&auxiliar==2) //Verifico si los jugadores del vs terminaron su turno
            {
                if(banderajugador2==0) //Si finalizó el jugador 1, reinicio variables para el jugador 2
                {
                    msgBox.setText("TURNO JUGADOR 2");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    contdisparos1=4;
                    ui->contavs->display(contdisparos1);
                    banderajugador2=1; //Bandera para indicar turno del jugador 2
                    nivel1=1;
                    terminacion=0;
                    delete objetivo; //Elimino objetivo que ya existe con ciertos parámetros.
                    objetivo= new graficar(420,100, 30, 0,0,50,0,0); //Creo objetivo con parámetros de nivel 1
                    objetivo->actualizar(t,v_limit);
                    scene->addItem(objetivo);
                }
                //Determino quién gana el juego con tres if que comparan puntajes
                if(banderajugador2==1 && puntaje1<puntaje2&&contdisparos1==0)
                {
                    msgBox.setText("GANADOR JUGADOR 2");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();                                                              }
                if(banderajugador2==1 && puntaje1>puntaje2&&contdisparos1==0)
                {
                    msgBox.setText("GANADOR JUGADOR 1");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();
                }
                if(banderajugador2==1 && puntaje1==puntaje2&&contdisparos1==0)
                {
                    msgBox.setText("EMPATADOS");
                    msgBox.setInformativeText("");
                    msgBox.exec();
                    close();
                }

             }
      }
}

    if(b->getY()<=b->getR()) // limite inferior
        //PARA DETERMINAR SI HAY COLISION ENTRE EL OBJETO PARABOLICO Y EL  LIMITE INFERIOR
        //Se tienen en cuenta los mismos parámetros de la colision con el limite derecho
    {
        parabolico->getEsf()->setPoint(10,10);
        parabolico->setVxi(0);
        parabolico->setVyi(0);
        if(contdisparos==0||contdisparos1==0)
        {
            auxiliar= auxiliar+1;
            if(contdisparos==0 && auxiliar ==2) //Finaliza el modo single
            {
                controladorarduino=1;
                timer->stop();
                msgBox.setText("YOU LOSE muroabajo");
                msgBox.setInformativeText("");
                msgBox.exec();
                close();
            }
            if(contdisparos1==0&&auxiliar==2)
            {
                if(banderajugador2==0) //Reinicio parámetros ara jugador 2 orque ya terminó el jugadorr 1
                {
                     msgBox.setText("TURNO JUGAADOR 2");
                     msgBox.setInformativeText("");
                     msgBox.exec();
                     contdisparos1=4;
                     ui->contavs->display(contdisparos1);
                     banderajugador2=1;
                     nivel1=1;
                     terminacion=0;
                     delete objetivo;
                     objetivo= new graficar(420,100, 30, 0,0,50,0,0);
                     objetivo->actualizar(t,v_limit);
                     scene->addItem(objetivo);
                }
                //Determino quién fue el ganador
                if(banderajugador2==1 && puntaje1<puntaje2&&contdisparos1==0)
                {
                     msgBox.setText("GANADOR JUGADOR 2");
                     msgBox.setInformativeText("");
                     msgBox.exec();
                     close();
                }
                if(banderajugador2==1 && puntaje1>puntaje2&&contdisparos1==0)


                {
                     msgBox.setText("GANADOR JUGADOR 1");
                     msgBox.setInformativeText("");
                     msgBox.exec();
                     close();
                }
                if(banderajugador2==1 && puntaje1==puntaje2&&contdisparos1==0)
                {
                     msgBox.setText("EMPATADOS");
                     msgBox.setInformativeText("");
                     msgBox.exec();
                     close();
                }
            }
        }
    }
    else if(b->getY()>=v_limit-b->getR())
    { //Colisiona con el limite
        b->setVel(b->getVx(),-b->getcRest()*b->getVy());
        b->setPoint(b->getX(),v_limit-b->getR());
    }
}

void MainWindow::actualizar()
//Función que actualiza valores de mi objeto parabólico y si hay colisión con los bordes o obstáculos
{
    bordercollision(parabolico->getEsf());
    parabolico->actualizar(t,v_limit); //Actualizo
    nivelN();
}

void MainWindow::arduino_init()
{
    //Método que verifica nombre del puerto, que el serial.begin coincida, que haya lectrura y escritura
    //en el puerto y realiza la conección entre lo que hay en el serial y el método serial read que toma
    //decisiones con a informacion que llega

    serial->setPortName(portname);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadWrite);
    connect(serial,SIGNAL(readyRead()),this,SLOT(serial_read()));
}

void MainWindow::on_pushButton_clicked()
// PARA GENERAR EL DISPARO EN MODO SINGLE
{
    if(contdisparos!=0)  //Podrá generarse un disparo si contdisparos es distinto de cero.
    {
        Cancion2->play();
        timer->start(500*t); //Cada cuanto se grafica segun el reloj
        parabolico->getEsf()->setPoint(10,10); //Ubico en posicion inicial
        int vxi=potencia*cos(angulo*M_PI/180); // Vario la veocidad en x segun el valor del slider angulo y paso a radianes
        int vyi=potencia*sin(angulo*M_PI/180); // Vario la veocidad en y segun el valor del slider angulo y paso a radianes
        parabolico->setVxi(vxi*2); //Para duplicar la velocidad en el eje x
        parabolico->setVyi(vyi*2); //Para duplicar la velocidad en el eje y
        parabolico->actualizar(t,v_limit); //Actualizo posicion y otros parametros
        scene->addItem(parabolico); //Pongo en escena el objeto

        if(contdisparos > 0)
        {   //Descontar disparos
            contdisparos=contdisparos-1; //Descuento los disparos cada vez que lance.
        }
        ui->lcdNumber_2->display(contdisparos);
        if(contdisparos==1) // Solo queda un disparo y está a punto de finalizar
        {
            auxiliar=1; //Es un avisador para que a través de auxiliar acabe la simulación
        }
    }
}

void MainWindow::on_potencia_valueChanged(int value)
//Recibe la variacion del slider potencia y lo asigna a la variable
{
    potencia=value;
}

void MainWindow::on_angulo_valueChanged(int value)
//Recibe la variacion del slider angulo y lo asigna a la variable
{
    angulo=value;
}

void MainWindow::on_reiniciar_clicked()   //FUERA DE USO
{
    nivel=1;
    contdisparos=12;
    ui->lcdNumber_2->display(contdisparos);
    parabolico->getEsf()->setPoint(10,10);
    parabolico->setVxi(0);
    parabolico->setVyi(0);
    delete objetivo;
    objetivo= new graficar(420,100, 30, 0,0,50,0,0);
    objetivo->actualizar(t,v_limit);
    scene->addItem(objetivo);
}

void MainWindow::posicion(void)
{
    int r=65; //RADIO DE LA TRAYECTORIA

    //Para cambiar giro del objeto 2 (trayectoria circular) si colisiona con los otros dos obstaculos
    collide=obstaculo2->collidesWithItem(obstaculo1);
    collide2=obstaculo2->collidesWithItem(obstaculo3);
    if(collide==true || collide2==true)
    {
         bandera1=!bandera1; //Si hubo colision, bandera1 lo cambio a false
    }
//si no hubo colision, a i le llevo el angulo en radianes e incremento en sentido contrario a las manecillas
    if(bandera1==true)
    {
        i+=rad;
    }
//si no hubo colision, a i le llevo el angulo en radianes e incremento en sentido de las manecillas
    if(bandera1==false)
    {
        i+=-rad;
    }
    x=-1*r*cos(i*2); //Modifico posicion en x
    y=-1*r*sin(i*2);  //Modifico posicion en y
    obstaculo2->setPos(x+300,y+100);//Cambia la posición del circulo 1 con x y y
}

void MainWindow::serial_read()
{
    QByteArray data= serial->readAll(); //Leo los datos del arduino y almaceno en data
    qDebug()<<"dato"<<data.toInt();

    if(serial->isWritable()||arduino_available) //Si arduiono está activo y el serial detecta algo.
    {
        if(controladorarduino==0) //inicia en cero
        {
            if(data.toInt()==100) //si en data hay un numero 100, activar boton disparo
            {
                on_pushButton_clicked(); //boton disparo modo single
            }
            if(data.toInt()==200)  //si en data hay un numero 200, cambiar valor de la variable angulo
            {
                angulo=angulo+10; //icremento en 10
                qDebug()<<"angulo"<<angulo;
                ui->label_12->show(); //mostrar el label que contiene el valor de angulo
                ui->label_12->setNum(angulo); //llevar el valor de angulo al label
            }
            if(data.toInt()==400) //si en data hay un numero 400, disminuir el valor de la variable angulo
            {
                angulo=angulo-10;
                qDebug()<<"angulo"<<angulo;
                ui->label_12->show(); //mostrar el label que contiene el valor de angulo
                ui->label_12->setNum(angulo); //llevar el valor de angulo al label
            }
            if(data.toInt()==300) //si en data hay un numero 300, cambiar valor de la variable potencia
            {
                potencia=potencia+10; //incremento la potencia en 10
                qDebug()<<"potencia"<<potencia;
                ui->label_13->show(); //mostrar el label que tendrá el valor de potencia
                ui->label_13->setNum(potencia);  //llevar el valor de potencia al label
            }
            if(data.toInt()==500) //si en data hay un numero 500, disminuir valor de la variable potencia
            {
                potencia=potencia-10; //
                qDebug()<<"potencia"<<potencia;
                ui->label_13->show(); //mostrar el label que tendrá el valor de potencia
                ui->label_13->setNum(potencia);  //llevar el valor de potencia al label
            }
        }
        if(data.toInt()==600&&controladorarduino==1) //si en data hay un numero 600, cambiar la variable controldemundos a 3
        {
            controldemundos=3; //variable contro de mundos
        }
    }
}

void MainWindow::mover()
{
    //Funcion que se encarga del movimiento del obstaculo 3 (pendulo)

    double angulo = asin((x1_obstaculo3 ) / magnitud); //hallo el valor del teta (posicion) inicial en coordenadas polares
    double a_x = -GRAV * sin(angulo); //Determino valor de la aceleracion en el eje x
    static double vx = 0;// velocidad incial en x es cero
    vx += a_x * TIME;// velocidad en x
    static double w = 0; //Velocidad angular inicia en cero
    w = vx / magnitud; //Hallo la velocidad angular
    double ace_angulo = w / TIME; //aceleracion en coordenadas polares
    angulo = angulo + w * TIME  + (ace_angulo / 2 ) * TIME * TIME; //posicion en coordenadas polares

    x1_obstaculo3 = magnitud * sin(angulo); //posicion en x segun la magnitud de la velocidad y el cuadrante en el que estemos
    y1_obstaculo3 = magnitud * cos(angulo); //posicion en y segun la magnitud de la velocidad y el cuadrante en el que estemos
    obstaculo3->setPos(x1_obstaculo3 - 200, y1_obstaculo3); //Mostramos la posicion del objeto

}

void MainWindow::on_pushButton_2_clicked()
{   controladorarduino=0;
    //Funcion para ingresar al modo single, esconden los botones, label que sean necesarios y se muestran otros
    ui->label_14->hide(); //imagen arduino
    Cancion5->play();
    if(banderaverificador>=1) //para determinar si hubo un registro
    {
        if(controldemundos==1) //si estamos en el mundo 1 se muestran y esconden esos botones, label, lcd, etc...
        {
            ui->label_4->show();
            ui->lcdNumber_3->show();
            ui->lcdNumber_3->display(controldemundos);
            ui->label_3->show();
            ui->lineEdit->hide();
            ui->pushButton_4->hide();
            ui->pushButton_2->hide();
            ui->pushButton_3->hide();
            ui->lcdNumber_2->show();
            ui->graphicsView->show();
            ui->lcdNumber->show();
            ui->pushButton->show();
            ui->potencia->show();
            ui->angulo->show();
            ui->labelnombre->show();
            ui->label->show();
            ui->label_2->show();
            ui->labelnombre->setText(nombre);
            ui->pushButton_7->show();
            ui->pushButton_8->show();
            ui->label_11->hide();
            ui->pushButton_9->show();
            ui->mutear->show();

        }
        if(controldemundos==3) //si contol de mundo es 3se cambia el fondo del graphicview y otros detalles
        {
            Cancion5->stop();
            puntaje=800;
            scene->setBackgroundBrush(QImage("C:../versión 13/juego/mundo3.jpg")); //Imagen del mundo 3
            scene->addItem(obstaculo2); //pongo en escena objeto trayectoria circular
            obstaculo1->valores(230,240,20,20); //determino valores del objeto cuadrado
            scene->addItem(obstaculo1); //pongo en escena objeto cuadrado
            timer2->start(8); //activo reloj del objeto circular
            connect(timer2,SIGNAL(timeout()),this,SLOT(posicion())); //conecto reloj con la funcion posicion
            Cancion->stop();
            Cancion6->stop();
            Cancion7->setLoops(10); // Se repite 10 veces la cancion
            Cancion7->play();
            //Muestro estos lcd´s, label, botones...
            ui->label_4->show();
            ui->lcdNumber_3->show();
            ui->lcdNumber_3->display(controldemundos);
            ui->label_3->show();
            ui->lineEdit->hide();
            ui->pushButton_4->hide();
            ui->pushButton_2->hide();
            ui->pushButton_3->hide();
            ui->lcdNumber_2->show();
            ui->graphicsView->show();
            ui->lcdNumber->show();
            ui->pushButton->show();
            ui->potencia->show();
            ui->angulo->show();
            ui->labelnombre->show();
            ui->label->show();
            ui->label_2->show();
            ui->labelnombre->setText(nombre);
            ui->pushButton_7->show();
            ui->pushButton_8->show();
            ui->label_11->hide();

         }
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    //Boton para salir del juego
    Cancion5->play();
    close();
}


void MainWindow::on_pushButton_4_clicked()
{
    Cancion5->play();
    nombre=ui->lineEdit->text(); //le llevo lo ingresado en el linedit a la variable nombre
    if(nombre=="") //si el usuario no ingresa nada, dejo bandera verificador en cero
    {
        banderaverificador=0; //no podrá ingresar al juego
    }
    else
    {
        banderaverificador++; //si ingresó algo, podrá ingresar al juego
        if(banderaverificador>0)
        {
            ui->pushButton_2->show(); //si ingresó algo, muestro el boton star para que ingrese al juego
        }
    }
}


void MainWindow::on_pushButton_5_clicked()
{
    //boton inicial para acceder al modo single, me activa los botones de registro y salir.
    Cancion5->play();
    ui->pushButton_3->show();
    ui->pushButton_4->show();
    ui->pushButton_5->hide();
    ui->pushButton_6->hide();
    ui->lineEdit->show();
    //ui->label_fondo->show();

}

void MainWindow::on_pushButton_6_clicked()
{
    //Boton inicial para acceder al modo vs y muestra los linedit y botones para hacer registro
    Cancion5->play();
    ui->pushButton_3->hide();
    ui->pushButton_5->hide();
    ui->pushButton_6->hide();
    ui->REGISTRARVS->show();
    ui->label_5JUGADOR1->show();
    ui->label_6JUGADOR2->show();
    ui->lineEdit_2JUGADOR1->show();
    ui->lineEdit_3JUGADOR2->show();
    ui->label_14->hide();
}

void MainWindow::on_REGISTRARVS_clicked()
{
    //Boton para hacer registro en modo vs
    Cancion5->play();
    ui->label_11->hide();
    jugador1= ui->lineEdit_2JUGADOR1->text(); //Lo que hay en line edit lo almaceno el la variable jugador 1
    jugador2= ui->lineEdit_3JUGADOR2->text(); //Lo que hay en line edit lo almaceno el la variable jugador 2
    if(jugador1==""||jugador2=="")
        //Debe haber algo escrito en ambos, sino, no podrá ingresar
    {
        banderaverificadorvs=0; //No incializa el vs
        msgBox.setText("DEBES REGISTRAR 2 USUARIOS");
        msgBox.setInformativeText("");
        msgBox.exec();
    }
    else
    {
        //si ya tienen acceso al vs despues de haber registrado correctamente
        banderaverificadorvs=1; //cambio a 1, señal de que estamos en vs y no single
        //Se muestran estos label, lcd´s, pushbutton...
        ui->label_5JUGADOR1->hide();
        ui->label_6JUGADOR2->hide();
        ui->REGISTRARVS->hide();
        ui->lineEdit_2JUGADOR1->hide();
        ui->lineEdit_3JUGADOR2->hide();
        ui->REGISTRARVS->hide();
        ui->angulo->show();
        ui->potencia->show();
        ui->graphicsView->show();
        obstaculo1->valores(230,240,20,20);
        scene->addItem(obstaculo1); //agrego obstaculo cuadrado a la escena para definir como un mundo 2
        ui->disparovs->show();
        ui->label_5->show();
        ui->label_6->show();
        ui->contavs->show();
        ui->contanivel->hide();
        ui->lcdNumber_5puntaj2->show();
        ui->lcdNumber_4puntaje1->show();
        ui->label_5->setText(jugador1);
        ui->label_6->setText(jugador2);
        ui->contavs->display(contdisparos1);
        ui->label_7->show();
        ui->label_8->show();
       }
}

void MainWindow::on_disparovs_clicked()
{
    if(contdisparos1!=0)
//siempre que hayan disparos hago lo mismo que en el modo single
    {
        Cancion2->play();
        ui->contavs->display(contdisparos1);
        timer->start(500*t);//Inicializo el reloj
        //Defino parámetros iniciales para el objeto tiro parabolico
        parabolico->getEsf()->setPoint(10,10);
        int vxi=potencia*cos(angulo*M_PI/180); // Vario la veocidad en x segun el valor del slider angulo y paso a radianes
        int vyi=potencia*sin(angulo*M_PI/180); // Vario la veocidad en y segun el valor del slider angulo y paso a radianes
        parabolico->setVxi(vxi*2); //Para duplicar la velocidad en el eje x
        parabolico->setVyi(vyi*2); //Para duplicar la velocidad en el eje y
        parabolico->actualizar(t,v_limit); //Actualizo posicion y otros parametros
        scene->addItem(parabolico); //Pongo en escena el objeto

        if(contdisparos1 > 0) //Descontar disparos
        {
            contdisparos1=contdisparos1-1; //Descuento los disparos cada vez que lance.
        }
        ui->contavs->display(contdisparos1);
        if(contdisparos1==1) // Solo queda un disparo y está a punto de finalizar
        {
            auxiliar=1; //Es un avisador para que a través de auxiliar acabe la simulación
        }
    }
}

void MainWindow::on_pushButton_7_clicked()
{
    Cancion5->play();
    string hola;
    string nivelasdf;
    string tiros;
    ifstream archivo;
    archivo.open("guardar.txt",ios::out| ios::app); //abro el archivo, creo el archivo guardar si no existe
    archivo.close(); //cierro el archivo
    archivo.open("guardar.txt");
    while(!archivo.eof()) //si aun no termina de ller el archivo
    {
        archivo>>hola; //guardo en el archivo
        archivo>>nivelasdf;
        archivo>>tiros;
        if(hola==nombre.toStdString()) //verifico si el nombre exite en el archivo
        {
            puedoguardar++;
            qDebug()<<puedoguardar;
            if(puedoguardar==1) //si el nombre existe, imprimo que ya no pde guardar partida con ese nombre
            {
                msgBox.setText(nombre);
                msgBox.setInformativeText("Este usuario ya existe,si desea guardar datos hazlo con otro usuario");
                msgBox.exec();
            }
        }
    }
    archivo.close();
    if(puedoguardar==0) //si no existe el nombre en el archivo
    {
        ofstream Fichero;
        Fichero.open("guardar.txt",ios::out| ios::app);
        Fichero<<nombre.toStdString()<<" "<<controldemundos<<" "<<contdisparos<<endl; //guardo nombre, mundo y contador de disparos
        Fichero.close();
        puedoguardar=0;
    }
    puedoguardar=0;
}

void MainWindow::on_pushButton_8_clicked()
{
    //Función para cargar partida sgún el nombre registrado
    Cancion5->play();
    QString prueba;
    int contadordelregistro=0; //
    string hola;
    string nivelasdf;
    string tiros;
    ifstream archivo;
    archivo.open("guardar.txt", ios::in);
    while(!archivo.eof()) //recorro el archivo hasta que finalice
    {
        archivo>>hola;
        archivo>>nivelasdf;
        archivo>>tiros;
        contadordelregistro++;
        if(hola==nombre.toStdString()) //verifico si el nombre ingresado existe
        {
            //verifico en qué mundo guardó
            if(nivelasdf=="1"&&controldemundos==1)
            {
                //Si es el mundo 1, reinicio las variables relacinadas
                ui->lcdNumber_2->display(stoi(tiros));
                ui->lcdNumber_3->display(stoi(nivelasdf));
                ui->lcdNumber->display(0);
                puntaje=0;
                contdisparos=stoi(tiros);
                controldemundos=stoi(nivelasdf);
                nivel=1;
                delete objetivo; //Elimino el objetivo que se mostró al iniciar partida
                objetivo= new graficar(420,100, 30, 0,0,50,0,0); //Creo objetivo con parámteros del mundo 1
                objetivo->actualizar(t,v_limit); //Actualizo parámetros de posición y otros
                scene->setBackgroundBrush(QImage("C:../versión 13/juego/mundo1.jpg")); //Fondo del mundo 1
                scene->addItem(objetivo);  //Pongo en escena el objetivo
            }
            if(nivelasdf=="2"&&controldemundos<=2)
            {
                //Parámtros correspondientes al mundo 2
                ui->lcdNumber_2->display(stoi(tiros));
                ui->lcdNumber_3->display(stoi(nivelasdf));
                ui->lcdNumber->display(400);
                puntaje=400;
                contdisparos=stoi(tiros);
                controldemundos=stoi(nivelasdf);
                obstaculo1->valores(230,240,20,20);
                scene->addItem(obstaculo1);
                nivel=1;
                delete objetivo;
                objetivo= new graficar(420,100, 30, 0,0,50,0,0);
                objetivo->actualizar(t,v_limit);
                scene->addItem(objetivo);
                Cancion->stop();
                Cancion6->setLoops(10);
                Cancion6->play();
                Cancion7->stop();
                scene->setBackgroundBrush(QImage("C:../versión 13/juego/mundo2sustituto.jpg"));
            }
            if(nivelasdf=="3"&&controldemundos<=3)
            {
                //Parametros correspondientes al mundo 3
                ui->lcdNumber_2->display(stoi(tiros));
                ui->lcdNumber_3->display(stoi(nivelasdf));
                ui->lcdNumber->display(800);
                puntaje=800;
                contdisparos=stoi(tiros);
                controldemundos=stoi(nivelasdf);
                scene->addItem(obstaculo2);
                delete timer2;
                timer2=new QTimer(this);
                timer2->start(15);
                connect(timer2,SIGNAL(timeout()),this,SLOT(posicion()));
                obstaculo1->valores(230,240,20,20);
                scene->addItem(obstaculo1);
                nivel=1;
                delete objetivo;
                objetivo= new graficar(420,100, 30, 0,0,50,0,0);
                objetivo->actualizar(t,v_limit);
                scene->addItem(objetivo);
                Cancion6->stop();
                Cancion->stop();
                Cancion7->setLoops(10);
                Cancion7->play();
                scene->setBackgroundBrush(QImage("C:../versión 13/juego/mundo3.jpg"));
            }
        }
  }
  archivo.close();
}

void MainWindow::on_pushButton_9_clicked()
{
    //Método-Botón para mostrar las instrucciones del juego
    Cancion5->play();
    ui->label_15->show();
    ui->pushButton_10->show(); //Activo botón atrás
}

void MainWindow::on_pushButton_10_clicked()
{
    //Método-Botón para dejar de mostrar las instrucciones del juego
    Cancion5->play();
    ui->label_15->hide(); //Oculto las instrucciones
    ui->pushButton_10->hide(); //Oculto el botón atrás
}

void MainWindow::on_mutear_clicked()
{
    //Detengo las canciones, en cada mundo se reroducen y se deben volver a pausar si se desea
    Cancion->stop();
    //Cancion1->play();
    Cancion2->stop();
    Cancion3->stop();
    Cancion4->stop();
    //Cancion5->play();
    Cancion6->stop();
    Cancion7->stop();
}
