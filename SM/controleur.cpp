#include "controleur.h"
#include <QDebug>
#include <QTimer>

controleur::controleur(QMainWindow__ *inPtrWin, Gnou *** inVoisinage) : ptrWin(inPtrWin), voisinage(inVoisinage)
{
    // On crée quelques gnous
    vectGnous.push_back(new Gnou(ptrWin, voisinage, true)); // Le leader
    /*vectGnous.push_back(new Gnou(ptrWin, voisinage, false));
    vectGnous.push_back(new Gnou(ptrWin, voisinage, false));
    vectGnous.push_back(new Gnou(ptrWin, voisinage, false));*/


    for(int k=0; k<200; ++k)
        vectGnous.push_back(new Gnou(ptrWin, voisinage, false));

    conteneur = NULL;

    qDebug() << "constructeur done";

    QTimer * timer =  new QTimer();
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(moveGnous()));
    timer->start(50);

    // Prépare l'affichage
    conteneur = new QLabel(ptrWin);
    conteneur->setGeometry(0,0,750*TAILLE_GNOU,500*TAILLE_GNOU);
    conteneur->setStyleSheet("border:1px solid black");
    conteneur->show();




    for(int i=0; i<vectGnous.size(); ++i)
    {
        QString css="border:0px; background-color : ";
        int alea=rand()%5;
        if(alea==0)
            css.append("red");
        else if(alea==1)
            css.append("black");
        else if(alea==2)
            css.append("orange");
        else if(alea==3)
            css.append("green");
        else if(alea==4)
            css.append("blue");

        vectQLabels.push_back(new QLabel(conteneur));
        vectQLabels[i]->setStyleSheet(css);
        vectQLabels[i]->setGeometry(0,0,TAILLE_GNOU,TAILLE_GNOU);
        vectQLabels[i]->show();
    }

    QObject::connect(ptrWin, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(keyPressed(QKeyEvent*)));
}

void controleur::moveGnous()
{

    /*vectGnous[1]->suivre(vectGnous[0]);
    vectGnous[2]->suivre(vectGnous[1]);
    vectGnous[3]->suivre(vectGnous[2]);*/

    vector<Gnou*>::iterator it;
    //bool first = true;

    for (it=vectGnous.begin(); it!= vectGnous.end();++it)
    {
        if (it == vectGnous.begin())
            (*it)->bougerLeader();
        else {(*it)->move();}
    }

    afficherGnous();
}

void controleur::afficherGnous()
{
    vector<Gnou*>::iterator it;

    /*if (conteneur != NULL)
    {
        delete conteneur;
        conteneur = NULL;
    }

    conteneur = new QLabel(ptrWin);
    conteneur->setGeometry(0,0,750*TAILLE_GNOU,500*TAILLE_GNOU);
    conteneur->setStyleSheet("border:1px solid black");
    conteneur->show();*/

    int cpt=0;

    for (it=vectGnous.begin(); it!= vectGnous.end();++it)
    {
        vectQLabels[cpt++]->setGeometry((*it)->getposx()*TAILLE_GNOU,(*it)->getposy()*TAILLE_GNOU,TAILLE_GNOU,TAILLE_GNOU);
        /*QLabel * gnou = new QLabel(conteneur);
        gnou -> setStyleSheet("background-color : red; border:0px");
        gnou ->setGeometry((*it)->getposx()*TAILLE_GNOU,(*it)->getposy()*TAILLE_GNOU,TAILLE_GNOU,TAILLE_GNOU);
        gnou -> show();*/
    }

}

void controleur::keyPressed(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Right){
        vectGnous[0]->setposx(vectGnous[0]->getposx()+10);}
    else if(ev->key() == Qt::Key_Left)
        vectGnous[0]->setposx(vectGnous[0]->getposx()-10);
    else if(ev->key() == Qt::Key_Up)
        vectGnous[0]->setposy(vectGnous[0]->getposy()-10);
    else if(ev->key() == Qt::Key_Down)
        vectGnous[0]->setposy(vectGnous[0]->getposy()+10);
}



