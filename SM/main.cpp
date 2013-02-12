#include <QMainWindow>
#include <QApplication>


#include "gnou.h"
#include "controleur.h"
#include <QDebug>

int main (int argc, char ** argv)
{
    QApplication app(argc, argv);
    QMainWindow__ Window;

    Window.show();
    Window.setGeometry(50,50,1000,700);

    srand(time(0));
    // Initialisation de la matrice de voisinage
    Gnou *** voisinage;
    voisinage = new Gnou** [MAP_H];

    for (int i = 0;i < MAP_H; i++)
    {
        voisinage[i] = new Gnou * [MAP_W];
    }

    for (int i = 0;i < MAP_H; i++)
        for (int j = 0;j < MAP_W; j++)
            voisinage[i][j] = NULL;


    controleur controlor(&Window, voisinage);



    return app.exec();

}
