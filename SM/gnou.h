#ifndef GNOU_H
#define GNOU_H

#include <vector>
#include <math.h>
#include <QObject>
#include <QLabel>
#include <QMainWindow>
#include <cstdlib>
#include <time.h>

#define MAP_W 1000
#define MAP_H 700

using namespace std;



class Gnou : public QObject
{
    Q_OBJECT

 private:
    bool _dansFile;
    bool _isLeader;
    float dirx;
    float diry;
    int posx;
    int posy;
    int flag;   // Flags qui sert à savoir s'il y a un cycle dans les files
    QLabel * conteneur;
    QMainWindow * ptrWin;
    Gnou *** voisinage;
    Gnou * _gnouQuonSuit;

public:
    Gnou(QMainWindow * ptrWin, Gnou *** inVoisinage, bool isLeader);
    void bougerLeader();
    void afficher();
    void moveGnou();
    void choisirQuiOnsuit(int inVision);
    void changerQuiOnSuit(int inVision);
    bool verifierQuOnSuitLeader();
    void suivreGnouQuOnSuit();
    void move();
    void suivre(Gnou * inGnou);
    bool directionProche(Gnou * inGnou);
    bool estDevantNous(Gnou * inGnou);
    void marcheAleatoire(int frequence, int w, int h);
    void sArreterSiOnSuitPasLeLeader();

    void sInsererDansFile(int vision);
    void suivreFile();
    void contournerObstacle();

    int getposx(){ return posx; }
    int getposy(){ return posy; }

    void setposx(int inx){ posx=inx; }
    void setposy(int iny){ posy=iny; }

    static int carreDistance(Gnou* gnou1, Gnou* gnou2);
};

#endif // GNOU_H
