#ifndef CONTROLEUR_H
#define CONTROLEUR_H

#include <vector>
#include "gnou.h"

#include <QKeyEvent>
#include <QDebug>


#define TAILLE_GNOU 2

class QMainWindow__ : public QMainWindow
{
    Q_OBJECT
public:
    QMainWindow__(){ }
    void keyPressEvent(QKeyEvent *ev)
    {
        emit keyPressed(ev);
    }

signals:
    void keyPressed(QKeyEvent * event);
};


class controleur : public QObject
{
    Q_OBJECT
private:
    std::vector<Gnou*> vectGnous;
    std::vector<QLabel*> vectQLabels;
    QMainWindow__ * ptrWin;
    QLabel * conteneur;
    Gnou *** voisinage;

public:
    controleur(QMainWindow__ * inPtrWin, Gnou *** inVoisinage);
    void afficherGnous();

public slots:
    void moveGnous();
    void keyPressed(QKeyEvent * ev);
};






#endif // CONTROLEUR_H
