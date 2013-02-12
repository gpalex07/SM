#include "gnou.h"
#include <cstdlib>
#include <QLabel>
#include <QTimer>
#include <QDebug>


Gnou::Gnou(QMainWindow * inPtr, Gnou ***inVoisinage, bool isLeader) : voisinage(inVoisinage), _isLeader(isLeader)
{
    ptrWin = inPtr;
    dirx = diry = 0;

    posx = rand()%200+100;
    posy=rand()%200+100;
    conteneur = NULL;

    voisinage[(int)posy][(int)posx]=this;
    _gnouQuonSuit =NULL;

    _dansFile =false;
}



void Gnou::bougerLeader()
{
    _gnouQuonSuit=this;

    dirx = 1;
    diry = 0;
    voisinage[(int)posy][(int)posx] =this;

    //posx+=dirx;
}

int Gnou::carreDistance(Gnou* gnou1, Gnou* gnou2)
{
    return pow(gnou1->posx-gnou2->posx,2)+pow(gnou1->posy-gnou2->posy, 2);
}

void Gnou::suivre(Gnou *inGnou)
{
    if(inGnou != NULL)
    {
        // On vérifie que le gnou qu'on suit bouge, sinon c'est probablement qu'il est bloqué par d'autre dans une boule. On doit absolument arrêter de le suivre.
        if(inGnou->dirx!=0 || inGnou->diry!=0)
        {

            dirx=diry=0;    // Par défaut, on ne fait rien (pas de déplacement)

            int Dx=abs(inGnou->posx-posx);
            int Dy=abs(inGnou->posy-posy);

            if(inGnou->posx < posx)
            {
                if(inGnou->posy < posy) {
                    if(Dx<Dy){ diry=-1; if(Dx!=0 && rand()%((int)((float)Dy/(float)Dx))==0)dirx=-1; }
                    else if(Dx>Dy){ dirx=-1; if(Dy!=0 && rand()%((int)((float)Dx/(float)Dy))==0)diry=-1; }
                    else { dirx=diry=-1; }
                } else if(inGnou->posy > posy) {
                    if(Dx<Dy){ diry=1; if(Dx!=0 && rand()%((int)((float)Dy/(float)Dx))==0)dirx=-1; }
                    else if(Dx>Dy){ dirx=-1; if(Dy!=0 && rand()%((int)((float)Dx/(float)Dy))==0)diry=1; }
                    else { dirx=-1; diry=1; }
                } else {
                    dirx=-1;
                }
            } else if(inGnou->posx > posx) {
                if(inGnou->posy < posy) {
                    if(Dx<Dy){ diry=-1; if(Dx!=0 && rand()%((int)((float)Dy/(float)Dx))==0)dirx=1; }
                    else if(Dx>Dy){ dirx=1; if(Dy!=0 && rand()%((int)((float)Dx/(float)Dy))==0)diry=-1; }  // Plus on est proche de la diagonale, plus on a de chance de remonter. Permet de fluidifier les mouvements.
                    else { dirx=1; diry=-1; }
                } else if(inGnou->posy > posy) {
                    if(Dx<Dy){ diry=1; if(Dx!=0 && rand()%((int)((float)Dy/(float)Dx))==0)dirx=1; }
                    else if(Dx>Dy){ dirx=1; if(Dy!=0 && rand()%((int)((float)Dx/(float)Dy))==0)diry=1; }
                    else { dirx=diry=1; }
                } else {
                    dirx=1;
                }
            } else {
                if(inGnou->posy < posy) {
                    diry=-1;
                } else if(inGnou->posy > posy) {
                    diry=1;
                } else {
                    qDebug() << "Gnou::suivre() > Erreur : deux gnous sont sur la même case";
                }
            }

            // On regarde si le gnou qu'on suit était dans ma colonne ou dans ma ligne avant et qu'il vient de la quitter.
            // Si c'est le cas, on essait de ne pas reproduire son déplacement à l'identique, sinon il les gnous se déplacent par colonne verticale et horizontale irréaliste.
            /*if(inGnou->dirx!=0 && inGnou->posx-inGnou->dirx == posx)
                diry=inGnou->diry;
            else if(inGnou->diry && inGnou->posy-inGnou->diry == posy)
                dirx=inGnou->dirx;
            else {

                if(inGnou->posx < posx)
                {
                    if(inGnou->posy < posy) {
                        if(inGnou != voisinage[posy-1][posx-1])
                            dirx=diry=-1;
                    } else if(inGnou->posy > posy) {
                        if(inGnou != voisinage[posy+1][posx-1])
                        {
                            dirx=-1;
                            diry= 1;
                        }
                    } else {
                        if(inGnou != voisinage[posy][posx-1])
                            dirx=-1;
                    }
                } else if(inGnou->posx > posx) {
                    if(inGnou->posy < posy) {
                        if(inGnou != voisinage[posy-1][posx+1]) {
                            dirx=1;
                            diry=-1;
                        }
                    } else if(inGnou->posy > posy) {
                        if(inGnou != voisinage[posy+1][posx+1])
                        {
                            dirx=diry=1;
                        }
                    } else {
                        if(inGnou != voisinage[posy][posx+1])
                            dirx=1;
                    }
                } else {
                    if(inGnou->posy < posy) {
                        if(inGnou != voisinage[posy-1][posx])
                            diry=-1;
                    } else if(inGnou->posy > posy) {
                        if(inGnou != voisinage[posy+1][posx])
                            diry= 1;
                    } else {
                        qDebug() << "Gnou::suivre() > Erreur : deux gnous sont sur la même case";
                    }
                }
            }*/

            // On vérifie qu'on peut se déplacer là où on voudrait
            if(voisinage[(int)(posy+diry)][(int)(posx+dirx)]!=NULL)
                contournerObstacle();   // Recalcule dirx et diry pour contourner l'obstacle (si on n'y arrive pas dirx=diry=0)

            // Si on a trouvé une case où contourner
            if(voisinage[(int)(posy+diry)][(int)(posx+dirx)]==NULL) // Si la case où on veut aller est libre on se déplace, sinon on ne bouge pas
            {
                voisinage[(int)posy][(int)posx]=NULL;
                posx+=dirx;
                posy+=diry;
                voisinage[(int)posy][(int)posx] =this;
            } else { /*dirx=diry=0; On laisse les valeurs de dirx pour savoir vers où on se dirigeait */ /*changerQuiOnSuit(20);*/ dirx=diry=0; } // Si on se retrouve coincé on choisit de suivre un autre gnou
        } else {
            // Le gnou qu'on suit ne bouge pas, il est bloqué par d'autre dans une boule. On doit absolument arrêter de le suivre.
            changerQuiOnSuit(20);
        }
    } else qDebug() << "Gnou::suivre() > Erreur : on nous demande de suivre un gnou* alors que ce pointeur est NULL";
}

// Permet de contourner un obstacle, lorsqu'on suit un gnou
void Gnou::contournerObstacle()
{
    dirx=diry=0;

    if(_gnouQuonSuit!=NULL)
    {
        if(_gnouQuonSuit->posx>posx)
        {
            if(_gnouQuonSuit->posy>posy)
            {
                if(rand()%2)
                {
                    if(voisinage[(int)posy][(int)posx+1]==NULL)
                        dirx=1;
                    else if(voisinage[(int)posy+1][(int)posx]==NULL)
                        diry=1;
                } else {
                    if(voisinage[(int)posy+1][(int)posx]==NULL)
                        diry=1;
                    else if(voisinage[(int)posy][(int)posx+1]==NULL)
                        dirx=1;
                }
            } else if(_gnouQuonSuit->posy==posy){
                if(rand()%2)
                {
                    if(voisinage[(int)posy-1][(int)posx+1]==NULL)
                    {   dirx=1; diry=-1;    }
                    else if(voisinage[(int)posy+1][(int)posx+1]==NULL)
                    {   dirx=1; diry=+1;    }
                } else {
                    if(voisinage[(int)posy+1][(int)posx+1]==NULL)
                    {   dirx=1; diry=+1;    }
                    else if(voisinage[(int)posy+1][(int)posx+1]==NULL)
                    {   dirx=1; diry=+1;    }
                }
            } else {
                if(rand()%2)
                {
                    if(voisinage[(int)posy][(int)posx+1]==NULL)
                        dirx=1;
                    else if(voisinage[(int)posy-1][(int)posx]==NULL)
                        diry=-1;
                } else {
                    if(voisinage[(int)posy-1][(int)posx]==NULL)
                        diry=-1;
                    else if(voisinage[(int)posy][(int)posx+1]==NULL)
                        dirx=1;
                }
            }
        } else if(_gnouQuonSuit->posx==posx) {
            if(_gnouQuonSuit->posy>posy)
            {
                if(rand()%2)
                {
                    if(voisinage[(int)posy+1][(int)posx-1]==NULL)
                    { dirx=-1; diry=1; }
                    else if(voisinage[(int)posy+1][(int)posx+1]==NULL)
                    { dirx=1; diry=1; }
                } else {
                    if(voisinage[(int)posy+1][(int)posx+1]==NULL)
                    { dirx=1; diry=1; }
                    else if(voisinage[(int)posy+1][(int)posx-1]==NULL)
                    { dirx=-1; diry=1; }
                }
            } else if(_gnouQuonSuit->posy<posy){
                if(rand()%2)
                {
                    if(voisinage[(int)posy-1][(int)posx-1]==NULL)
                    {   dirx=-1; diry=-1;    }
                    else if(voisinage[(int)posy-1][(int)posx+1]==NULL)
                    {   dirx=1; diry=-1;    }
                } else {
                    if(voisinage[(int)posy-1][(int)posx+1]==NULL)
                    {   dirx=1; diry=-1;    }
                    else if(voisinage[(int)posy-1][(int)posx-1]==NULL)
                    {   dirx=-1; diry=-1;    }
                }
            }
        } else if(_gnouQuonSuit->posx<posx) {
            if(_gnouQuonSuit->posy>posy)
            {
                if(rand()%2)
                {
                    if(voisinage[(int)posy][(int)posx-1]==NULL)
                    { dirx=-1;  }
                    else if(voisinage[(int)posy+1][(int)posx]==NULL)
                    { diry=1; }
                } else {
                    if(voisinage[(int)posy+1][(int)posx]==NULL)
                    { diry=1; }
                    else if(voisinage[(int)posy][(int)posx-1]==NULL)
                    { dirx=-1;  }
                }
            } else if(_gnouQuonSuit->posy==posy){
                if(rand()%2)
                {
                    if(voisinage[(int)posy-1][(int)posx-1]==NULL)
                    {   dirx=-1; diry=-1;    }
                    else if(voisinage[(int)posy+1][(int)posx-1]==NULL)
                    {   dirx=-1; diry=+1;    }
                } else {
                    if(voisinage[(int)posy+1][(int)posx-1]==NULL)
                    {   dirx=-1; diry=+1;    }
                    else if(voisinage[(int)posy-1][(int)posx-1]==NULL)
                    {   dirx=-1; diry=-1;    }
                }
            } else {
                if(rand()%2)
                {
                    if(voisinage[(int)posy][(int)posx-1]==NULL)
                    {   dirx=-1;  }
                    else if(voisinage[(int)posy-1][(int)posx]==NULL)
                    {   diry=-1;    }
                } else {
                    if(voisinage[(int)posy-1][(int)posx]==NULL)
                    {   diry=-1;    }
                    else if(voisinage[(int)posy][(int)posx-1]==NULL)
                    {   dirx=-1;  }
                }
            }
        }
    } else { qDebug() << "Gnou::contounerObstacle(): Erreur: on nous demande de contourner un obstacle alors qu'on ne suit personne"; }
}


// Permet de vérifier que le gnou qu'on suit suit des gnous qui suivent le leader, et qu'il n'y a pas de cycle ni rien
bool Gnou::verifierQuOnSuitLeader()
{
    Gnou * cour = this;
    bool suitLeader =false;

    while(cour!=NULL && cour->_gnouQuonSuit!=cour && cour->flag!=1)     // On met à 1 le flags des gnous qu'on a déjà recontrés pour savoir s'il y a un cycle
    {
        cour->flag =1;
        cour =cour->_gnouQuonSuit;
    }

    if(cour !=NULL && cour->_gnouQuonSuit ==cour)
        suitLeader =true;

    // On remet les flags à 0
    cour =this;
    while(cour !=NULL && cour->flag ==1)
    {
        cour->flag =0;
        cour =cour->_gnouQuonSuit;
    }

    return suitLeader;
}

void Gnou::suivreGnouQuOnSuit()
{
    if(_gnouQuonSuit != NULL)
    {
        // Avant de suivre le gnou, on vérifie que le gnou qu'on suit suit des gnous qui suivent le leader
        if(verifierQuOnSuitLeader() == true)
        {
            dirx=diry=0;

            if(_gnouQuonSuit->posx>posx)    // Détermine dans quelle direction aller pour suivre le _gnouQuOnSuit
                dirx=1;
            else if(_gnouQuonSuit->posx<posx)
                dirx=-1;


            if(_gnouQuonSuit->posy>posy)
                diry=1;
            else if(_gnouQuonSuit->posy<posy)
                diry=-1;

            if(voisinage[(int)(posy+diry)][(int)(posx+dirx)]!=NULL)     // La case est déjà occupée
                contournerObstacle();   // Recalcule dirx et diry pour contourner l'obstacle (si on n'y arrive pas dirx=diry=0)

            // Si on a trouvé une case où contourner
            if(voisinage[(int)(posy+diry)][(int)(posx+dirx)]==NULL) // Si la case où on veut aller est libre on se déplace, sinon on ne bouge pas
            {
                voisinage[(int)posy][(int)posx]=NULL;
                posx+=dirx;
                posy+=diry;
                voisinage[(int)posy][(int)posx] =this;
            }
        } else {
            _gnouQuonSuit =NULL;
            dirx=diry=0;
            _dansFile=false;
            qDebug() << "Gnou::suivreGnouQuOnSuit(): Warning > On suit un gnou qui suit des gnous qui ne suivent pas le leader. On arrête de le suivre";
        }
    } else qDebug() << "Gnou::suivreGnouQuOnSuit(): Erreur > On nous demande de suivre un gnou alors que _gnouQuonSuit=NULL";

}

void Gnou::marcheAleatoire(int frequence, int w, int h)
{
    int x=rand()%w-w/2;
    int y=rand()%h-h/2;

    if(rand()%frequence==0)
    {
        posx+=x;
        posy+=y;
    }
}

void Gnou::sArreterSiOnSuitPasLeLeader()
{
    if(verifierQuOnSuitLeader()==false)
    {
        _gnouQuonSuit=NULL;
        dirx=diry=0;
    }

    /*Gnou * cour = this;
    bool suitLeader =false;

    while(cour!=NULL && cour->_gnouQuonSuit!=cour && cour->flag!=1)     // On met à 1 le flags des gnous qu'on a déjà recontrés pour savoir s'il y a un cycle
    {
        cour->flag =1;
        cour =cour->_gnouQuonSuit;
    }

    if(cour !=NULL && cour->_gnouQuonSuit ==cour)
        suitLeader =true;

    // On remet les flags à 0
    cour =this;
    if(suitLeader==true)
    {
        while(cour !=NULL && cour->flag ==1)    // On remet les flags à 0
        {
            cour->flag =0;
            cour =cour->_gnouQuonSuit;
        }
    } else {
        Gnou * prec=NULL;
        while(cour !=NULL && cour->flag ==1)    // On remet les flags à 0 et on met les pointeurs des gnous qu'on suit à NULL
        {
            prec=cour;
            cour->flag =0;
            cour =cour->_gnouQuonSuit;
            prec->_gnouQuonSuit=NULL;
        }
    }*/
}

void Gnou::move()
{
    int vision =4000;


    /*if(_gnouQuonSuit!=NULL && carreDistance(this, _gnouQuonSuit)>vision)
    {
        _gnouQuonSuit=NULL;
        changerQuiOnSuit(sqrt(4000));  // On s'insère dans la file et on choisit celui qu'on va suivre en fonction de notre direction de déplacement (on prend un gnou devant nous, pas derrière)

        if(_gnouQuonSuit==NULL) // Si jamais on n'a pas trouvé un autre gnou à suivre
            dirx=diry=0;
    }*/

    if(_gnouQuonSuit!=NULL)
    {

        suivre(_gnouQuonSuit);

        if(verifierQuOnSuitLeader()==true) // On vérifie qu'en suivant le gnou qu'on suit, on suit indirectement le leader, sinon on s'arrête
        {
            if(rand()%20==0)
            {
                Gnou * backupGnouQuOnSuit = _gnouQuonSuit;
                changerQuiOnSuit(10);  // On s'insère dans la file

                if(verifierQuOnSuitLeader()==false) // Le fait d'avoir changé le gnou qu'on suit à créé un cycle, on annule et on continue de suivre le même
                {
                    _gnouQuonSuit = backupGnouQuOnSuit;
                }
            }
        } else { _gnouQuonSuit=NULL; dirx=diry=0; }

        /*

        if(verifierQuOnSuitLeader()==false) // Le fait d'avoir changé le gnou qu'on suit à créé un cycle, on annule et on continue de suivre le même
        {
            _gnouQuonSuit = NULL;
        }*/


        //suivreGnouQuOnSuit();

        /*if(_gnouQuonSuit!= NULL && carreDistance(this, _gnouQuonSuit)>vision)
            choisirQuiOnsuit(10);*/

        /*if(_gnouQuonSuit!=NULL && carreDistance(this, _gnouQuonSuit)>vision) // Le gnou qu'on suit est trop loin, on choisit un autre gnou qu'on va suivre
            choisirQuiOnsuit(20);     // On est déjà dans la file, donc on prend un gnou qui est proche de nous*/


        // Vérifie que ceux qu'on suit dans la file suivent le leader, et qu'il n'y a pas de cycle, et qu'on ne suit pas quelqu'un qui ne suit personne
        /*Gnou* gnouCourant =_gnouQuonSuit;
        Gnou* gnouPrec    =_gnouQuonSuit;

        while(gnouCourant!=NULL && gnouCourant!=this && gnouCourant->_gnouQuonSuit!=gnouCourant)
            gnouCourant=gnouCourant->_gnouQuonSuit;

        if(gnouCourant==NULL) // On suit quelqu'un qui ne suit personne
        {
            gnouCourant =_gnouQuonSuit;
            while(gnouCourant!=NULL && gnouCourant!=this && gnouCourant->_gnouQuonSuit!=gnouCourant)
            {
                gnouPrec=gnouCourant;
                gnouCourant=gnouCourant->_gnouQuonSuit;
                gnouPrec->_dansFile=false;
                gnouPrec->dirx=0;
                gnouPrec->diry=0;
                gnouPrec->_gnouQuonSuit=NULL;
            }
            this->_dansFile=false;
            this->_gnouQuonSuit=NULL;
            this->dirx=0;
            this->diry=0;
        } else if(gnouCourant==this){   // Il y a un cycle
            gnouCourant =_gnouQuonSuit;
            while(gnouCourant!=NULL && gnouCourant!=this && gnouCourant->_gnouQuonSuit!=gnouCourant)
            {
                gnouPrec=gnouCourant;
                gnouCourant=gnouCourant->_gnouQuonSuit;
                gnouPrec->_dansFile=false;
                gnouPrec->dirx=0;
                gnouPrec->diry=0;
                gnouPrec->_gnouQuonSuit=NULL;
            }
            this->_dansFile=false;
            this->_gnouQuonSuit=NULL;
            this->dirx=0;
            this->diry=0;
        } else { // On suit le leader
            // Ok
        }*/



    } else {
        //marcheAleatoire(50,3,3);

        choisirQuiOnsuit(10);  // On essaie de s'insérer dans une file (s'il y en existe)


    }

}

bool Gnou::directionProche(Gnou *inGnou)
{
    return inGnou->dirx==dirx || inGnou->diry==diry;    // Si au moins une des 2 directions est identique, alors la direction est proche.
}

// Un gnou est devant nous lorsqu'on se déplace en haut à droite, il est plus en haut à droite que nous
//                         lorsqu'on se déplace à droite, il est plus à droite que nous
//                         lorsqu'on se déplace en bas à droite, il est plus en bas à droite que nous
// Cela revient à regarder si l'écart Dx=gnouQuOnSuit.posx-this.posx et Dy=gnouQuOnSuit.posy-this.posy avec le gnou qu'on suit est tel que
// Dx est du même signe que dirx et Dy du même signe que diry
// Deux nombres A et B sont de même signe si A*B>=0
bool Gnou::estDevantNous(Gnou *inGnou)
{
    return ((inGnou->posx-posx)*dirx>=0 && (inGnou->posy-posy)*diry>=0);
}

// Lorsqu'on change le gnou qu'on suit, on en choisit un autre qui est situé devant nous,
// i.e. dans les 180° autour de notre vecteur de déplacement antérieur
void Gnou::changerQuiOnSuit(int inVision)
{
    // Quand on se déplace dans une direction, on choisit un autre gnou qui se dépalce soit dans la même direction que nous,
    // soit dans une direction adjacente à la notre, ie proche.
    // Si on va en haut, on choisit un gnou qui va en haut à gauche ou en haut à droite
    //             bas                                bas  à gauche ou en bas  à droite
    //          à  gauche,                         en haut à gauche ou en bas à gauche
    //          à  droite                          en haut à droite ou en bas à droite
    //          en haut à droite                   en haut ou à droite
    //

    // On remet _gnouQuonSuit à NULL pour ne suivre personne, si on n'arrive pas à s'insérer
    _gnouQuonSuit =NULL;
    //dirx=diry=0;
    _dansFile=false;

    // Compte le nombre de gnous du voisinage (cercle de vision) qui sont dans une file en mouvement
    // Les gnous trouvés n'appartiennent pas forcément à la même file
    int nbVoisins=0;
    int i,j;
    int min1=(int)posy-inVision;    // Détermine les bornes du voisinage qu'on va explorer
    int max1=posy+inVision;
    int min2=(int)posx-inVision;
    int max2=posx+inVision;

    if(min1<0) min1=0;
    if(max1>MAP_H) max1=MAP_H;
    if(min2<0) min2=0;
    if(max2>MAP_W) max2=MAP_W;

    for(i=min1; i<max1; ++i)    // Parcours le voisinage (du regard) et détermine le nombre de voisins qui appartiennent à une file en mouvement
        for(j=min2; j<max2; ++j)
            if(voisinage[i][j] != NULL && voisinage[i][j] != this && (voisinage[i][j]->dirx!=0 || voisinage[i][j]->diry!=0) && directionProche(voisinage[i][j])==true && estDevantNous(voisinage[i][j])==true) // Il faut aussi que le voisin bouge et ait une direction proche de la nôtre
                ++nbVoisins;

    if(nbVoisins>0) // S'il y a plusieurs voisins qui appartiennent à une file, on va devoir en choisir un au hasard
    {
        // Choisit le gnou qu'on va suivre au hasard
        int voisinChoisit=rand()%nbVoisins;
        int tmp=-1;

        min1=(int)posy-inVision;    // Détermine les bornes du voisinage qu'on va explorer
        max1=posy+inVision;
        min2=(int)posx-inVision;
        max2=posx+inVision;

        if(min1<0) min1=0;
        if(max1>MAP_H) max1=MAP_H;
        if(min2<0) min2=0;
        if(max2>MAP_W) max2=MAP_W;

        for(i=min1; i<max1; ++i)    // Parcours le voisinage (du regard) et trouve quel est le gnou qu'on a choisit
            for(j=min2; j<max2; ++j)
                if(voisinChoisit==tmp)
                {
                    break;
                } else if(voisinage[i][j] != NULL && voisinage[i][j] != this && (voisinage[i][j]->dirx!=0 || voisinage[i][j]->diry!=0) && directionProche(voisinage[i][j])==true && estDevantNous(voisinage[i][j])) // Il faut aussi que le voisin bouge
                {
                    _gnouQuonSuit =voisinage[i][j];
                    ++tmp;
                }

        _dansFile=true; // On est maintenant dans la file
    }

}

// Appelé pour s'insérer dans la file
void Gnou::choisirQuiOnsuit(int inVision)
{
    //qDebug() << "debut choisir";
    // On remet _gnouQuonSuit à NULL pour ne suivre personne, si on n'arrive pas à s'insérer
    _gnouQuonSuit =NULL;
    dirx=diry=0;
    _dansFile=false;

    // Compte le nombre de gnous du voisinage (cercle de vision) qui sont dans une file en mouvement
    // Les gnous trouvés n'appartiennent pas forcément à la même file
    int nbVoisins=0;
    int i,j;
    int min1=(int)posy-inVision;    // Détermine les bornes du voisinage qu'on va explorer
    int max1=posy+inVision;
    int min2=(int)posx-inVision;
    int max2=posx+inVision;

    if(min1<0) min1=0;
    if(max1>MAP_H) max1=MAP_H;
    if(min2<0) min2=0;
    if(max2>MAP_W) max2=MAP_W;

    for(i=min1; i<max1; ++i)    // Parcours le voisinage (du regard) et détermine le nombre de voisins qui appartiennent à une file en mouvement
        for(j=min2; j<max2; ++j)
            if(voisinage[i][j] != NULL && voisinage[i][j] != this && (voisinage[i][j]->dirx!=0 || voisinage[i][j]->diry!=0)) // Il faut aussi que le voisin bouge
                ++nbVoisins;

    if(nbVoisins>0) // S'il y a plusieurs voisins qui appartiennent à une file, on va devoir en choisir un au hasard
    {
        // Choisit le gnou qu'on va suivre au hasard
        int voisinChoisit=rand()%nbVoisins;
        int tmp=-1;

        min1=(int)posy-inVision;    // Détermine les bornes du voisinage qu'on va explorer
        max1=posy+inVision;
        min2=(int)posx-inVision;
        max2=posx+inVision;

        if(min1<0) min1=0;
        if(max1>MAP_H) max1=MAP_H;
        if(min2<0) min2=0;
        if(max2>MAP_W) max2=MAP_W;

        for(i=min1; i<max1; ++i)    // Parcours le voisinage (du regard) et trouve quel est le gnou qu'on a choisit
            for(j=min2; j<max2; ++j)
                if(voisinChoisit==tmp)
                {
                    break;
                } else if(voisinage[i][j] != NULL && voisinage[i][j] != this && (voisinage[i][j]->dirx!=0 || voisinage[i][j]->diry!=0)) // Il faut aussi que le voisin bouge
                {
                    _gnouQuonSuit =voisinage[i][j];
                    ++tmp;
                }

        _dansFile=true; // On est maintenant dans la file
    }
    //qDebug() << "fin choisir";
}

// Une amélioration consiste à ne changer le gnou qu'on suit qu'en prenant un autre qui est devant nous (et pas derrière)
void Gnou::moveGnou()
{
    int vision =10000;
    if(_dansFile){
        if(rand()%50==0)    // Il y a une chance sur 50 pour changer le gnou qu'on suit
            sInsererDansFile(10);   // On est dans la file, et soudainement, on décide de changer le gnou qu'on suit, on en choisit donc un proche
        if(_gnouQuonSuit!=NULL && carreDistance(this, _gnouQuonSuit)>vision) // Le gnou qu'on suit est trop loin, on choisit un autre gnou qu'on va suivre
        {
            sInsererDansFile(20);     // On est déjà dans la file, donc on prend un gnou qui est proche de nous
        }
        //qDebug() << "Va suivre\n";
        suivreFile();
    }
    else { /*qDebug() << "Va s'inserer\n";*/ sInsererDansFile(30); }    // On s'insère pour la première fois, donc on regarde loin




    // Vérifie que ceux qu'on suit dans la file suivent le leader, et qu'il n'y a pas de cycle, et qu'on ne suit pas quelqu'un qui ne suit personne
    Gnou* gnouCourant =_gnouQuonSuit;
    Gnou* gnouPrec    =_gnouQuonSuit;

    while(gnouCourant!=NULL && gnouCourant!=this && gnouCourant->_gnouQuonSuit!=gnouCourant)
    {
        gnouCourant=gnouCourant->_gnouQuonSuit;
    }

    if(gnouCourant==NULL) // On suit quelqu'un qui ne suit personne
    {
        gnouCourant =_gnouQuonSuit;
        while(gnouCourant!=NULL && gnouCourant!=this && gnouCourant->_gnouQuonSuit!=gnouCourant)
        {
            gnouPrec=gnouCourant;
            gnouCourant=gnouCourant->_gnouQuonSuit;
            gnouPrec->_dansFile=false;
            gnouPrec->dirx=0;
            gnouPrec->diry=0;
            gnouPrec->_gnouQuonSuit=NULL;
        }
    } else if(gnouCourant==this){   // Il y a un cycle
        gnouCourant =_gnouQuonSuit;
        while(gnouCourant!=NULL && gnouCourant!=this && gnouCourant->_gnouQuonSuit!=gnouCourant)
        {
            gnouPrec=gnouCourant;
            gnouCourant=gnouCourant->_gnouQuonSuit;
            gnouPrec->_dansFile=false;
            gnouPrec->dirx=0;
            gnouPrec->diry=0;
            gnouPrec->_gnouQuonSuit=NULL;
        }
    } else { // On suit le leader
        // Ok
    }



    // regarde si la tile est disponible
    //if(voisinage[(int)(posy+diry)][(int)(posx+dirx)] == NULL)

    if(_dansFile)
    {
        if(voisinage[(int)(posy+diry)][(int)(posx+dirx)]==NULL) // Si la case où on veut aller est libre on se déplace, sinon on ne bouge pas
        {
            voisinage[(int)posy][(int)posx]=NULL;
            posy+=diry;
            posx+=dirx;
            voisinage[(int)posy][(int)posx] =this;
        } else { // La case où n veut aller est prise, on fait une procédure d'évitement
            contournerObstacle();
            posx+=dirx;
            posy+=diry;
        }
    }
}

// Choisit un gnou (appartenant à une file en mouvement) qu'on va suivre
//
// Le gnou est supposé faire une activité quelconque (manger, boire ...)
// Cette méthode lui permet de s'insérer dans une file de gnous qui lui passent à côté
// Il peut y avoir plusieurs files différentes qui lui passent à côté
// Il choisira de suivre un gnou au hasard (enfin..), donc la file est choisie au hasard
void Gnou::sInsererDansFile(int vision)
{

    Gnou* gnouCourant = NULL;
    Gnou* gnouPrec    =NULL;


    // On remet _gnouQuonSuit à NULL pour ne suivre personne, si on n'arrive pas à s'insérer
    _gnouQuonSuit =NULL;


    //int vision=20;
    int nbVoisins=0;
    int i,j;
    // // // // //
    // Liste tous les gnous du voisinage (cercle de vision) qui sont dans une file en mouvement
    // Les gnous trouvés n'appartiennent pas forcément à la même file

    // Compte le nombre de voisins
    int min1=(int)posy-vision;
    if(min1<0) min1=0;
    int max1=posy+vision;
    if(max1>MAP_H) max1=MAP_H;

    int min2=(int)posx-vision;
    if(min2<0) min2=0;
    int max2=posx+vision;
    if(max2>MAP_W) max2=MAP_W;

    //qDebug() << i << max1 << "_" << j << max2;

    for(i=min1; i<max1; ++i)
        for(j=min2; j<max2; ++j)
            if(voisinage[i][j] != NULL && voisinage[i][j] != this && (voisinage[i][j]->dirx!=0 || voisinage[i][j]->diry!=0)) // Il faut aussi que le voisin bouge
                //_gnouQuonSuit =voisinage[i][j];
                ++nbVoisins;

    /*if(_gnouQuonSuit!=NULL)
    {
        qDebug() << "on suit quelqu'un";
        _dansFile=true;
    }*/

    /*int nonNull=0;
    for (int i = 0;i < MAP_H; i++)
        for (int j = 0;j < MAP_W; j++)
            if(voisinage[i][j]!=NULL)
                nonNull++;

    qDebug() << nonNull;*/

    //qDebug() << nbVoisins << " voisins trouves";

    if(nbVoisins>0)
    {

        // // // // //
        // Choisit le gnou qu'on va suivre au hasard
        int voisinChoisit=rand()%nbVoisins;

        int tmp=-1;

         min1=(int)posy-vision;
        if(min1<0) min1=0;
         max1=posy+vision;
        if(max1>MAP_H) max1=MAP_H;

         min2=(int)posx-vision;
        if(min2<0) min2=0;
         max2=posx+vision;
        if(max2>MAP_W) max2=MAP_W;

        for(i=min1; i<max1; ++i)
            for(j=min2; j<max2; ++j)
                if(voisinChoisit==tmp)
                {
                    break;
                } else if(voisinage[i][j] != NULL && voisinage[i][j] != this && (voisinage[i][j]->dirx!=0 || voisinage[i][j]->diry!=0)) // Il faut aussi que le voisin bouge
                {
                    _gnouQuonSuit =voisinage[i][j];
                    ++tmp;
                }// else qDebug() << "rien";



        //qDebug() << voisinage[i][j] <<i <<j<<tmp;

        if(_gnouQuonSuit!=NULL){
            // On vérifie qu'on ne forme pas un cycle, sinon on aura A suit B et B suit A et ça ne marchera pas
            gnouCourant = this;
            gnouPrec    =NULL;
            while(gnouCourant!=NULL && gnouCourant!=gnouPrec && gnouCourant->_gnouQuonSuit!=this)
            {
                gnouPrec   =gnouCourant;
                gnouCourant=gnouCourant->_gnouQuonSuit;
            }


            // S'il n'y a pas de cycle et on suit des gnous qui suivent le leader
            if(gnouCourant!=NULL&&gnouCourant->_gnouQuonSuit!=this)
                _dansFile=true;
            else { // Soit il y a un cycle, soit on suit des gnous qui suivent un gnou qui vient de s'arreter (car ne voit plus le leader par ex.)
                   // En tout cas, personne ne suit le leader dans ce groupe, ils se suivent les uns les autres

                _gnouQuonSuit=NULL;
                _dansFile=false;

                /*if(gnouCourant==NULL)  // On suivait des gnous qui suivaient un gnou qui n'avance pas.
                {
                    gnouCourant =this;

                    while(gnouCourant->_gnouQuonSuit!=NULL)    // On arrête tout le monde qui appartient à ce groupe
                    {
                        gnouPrec=gnouCourant;
                        gnouCourant=gnouCourant->_gnouQuonSuit;
                        gnouPrec->_gnouQuonSuit =NULL;
                        gnouPrec->_dansFile =false;
                        gnouPrec->dirx=0;
                        gnouPrec->diry=0;
                    }
                } else { // Il y a un cycle
                    gnouCourant =_gnouQuonSuit;

                    while(gnouCourant !=this)    // On arrête tout le monde qui appartient à ce groupe
                    {
                        gnouPrec=gnouCourant;
                        gnouCourant=gnouCourant->_gnouQuonSuit;
                        gnouPrec->_gnouQuonSuit =NULL;
                        gnouPrec->_dansFile =false;
                        gnouPrec->dirx=0;
                        gnouPrec->diry=0;
                    }
                }*/

                // Pour tous les gnous du cycle (y compris celui ci donc), on indique qu'ils ne suivent personne
                /*gnouCourant =_gnouQuonSuit;
                gnouPrec=_gnouQuonSuit;
                qDebug() << "Cycle va etre calme";
                while(gnouCourant!=this)
                {
                    gnouPrec=gnouCourant;
                    gnouCourant=gnouCourant->_gnouQuonSuit;
                    gnouPrec->_gnouQuonSuit =NULL;
                    gnouPrec->_dansFile =false;
                }

                qDebug() << "Cycle detecte\n";*/
            }
            //qDebug() << "insertion reussie";
        } else qDebug() << "echec de l'insertion";

    }
    /* Inutile
    // Pour chaque file trouvée, on calcule le vecteur directeur de la file
    // Pour cela on fait la moyenne des vecteurs directeurs des gnous de la file
    // Cela nous permet de connaître le sens de déplacement de la file

    // Maintenant qu'on connaît le sens de déplacement de la file, on peut exclure un gnou qui
    // se déplacerait dans le sens inverse de la file (pour ne pas le suivre)
    */
}

void Gnou::suivreFile()
{

    // On suit le gnou qu'on a choisit de suivre dans la file


    dirx = diry = 0;

    if(_gnouQuonSuit!=NULL)
    {
        if(_gnouQuonSuit->posx>posx)
            dirx=1;
        else if(_gnouQuonSuit->posx<posx)
            dirx=-1;

        if(_gnouQuonSuit->posy>posy)
            diry=1;
        else if(_gnouQuonSuit->posy<posy)
            diry=-1;

        if(voisinage[(int)(posy+diry)][(int)(posx+dirx)]!=NULL)
        {
            _dansFile=false;    // On va donc rechercher un gnou à suivre à la prochaine iter
            //qDebug() << "case deja prise";
        }
    }
    /*if(_gnouQuonSuit->posx>posx &&_gnouQuonSuit->posy<posy)
    {
        if(c)
        {
            dirx=1; diry=-1;
        } else {
            int choice =rand()%2;   // On peut pas prendre la diagonale, on choisit une alternative au hasard
            if(choice ==1)
            {
                if(b)
                {
                    dirx=0; diry=-1;
                } else if(e){
                    dirx=1; diry=0;
                } else { dirx=0; diry=0; }
            } else {
                if(e)
                {
                    dirx=1; diry=0;
                } else if(b){
                    dirx=0; diry=-1;
                } else { dirx=0; diry=0; }
            }
        }
   }*/






        /*if(_gnouQuonSuit->posx>posx)
            dirx=1;
        else if(_gnouQuonSuit->posx<posx)
            dirx=-1;

        if(_gnouQuonSuit->posy>posy)
            diry=1;
        else if(_gnouQuonSuit->posy<posy)
            diry=-1;*/
    //} //else qDebug() << "Erreur: _gnouQuonSuit=NULL, on devrait appeler sInsererDansFile";
    /*posx+=dirx;
    posy+=diry;*/

    /*float sumdirx = 0, sumdiry = 0;
    float k = 0;
    qDebug() << "suivre file";

    const int vision = 10;

    int debutY=posy-vision;
    if(debutY<0) debutY=0;

    int debutX=posx-vision;
    if(debutX<0) debutX=0;

    for (int i = debutY; i < posy+vision && i<MAP_H; i++)
    {
        for (int j = debutX;j < posy+vision && j< MAP_W; j++)
        {
            if (voisinage[i][j] != NULL)
            {
                if (voisinage[i][j]->posx*posx + voisinage[i][j]->posy*posy > 0)
                {
                    sumdirx +=voisinage[i][j]->dirx;
                    sumdiry +=voisinage[i][j]->diry;
                    k++;
                }
            }
        }
    }

    if(k != 0)
    {
        sumdirx /= k;
        sumdiry /= k;
        qDebug() << sumdirx;

        dirx = sumdirx / sqrt ( pow (sumdirx,2) + pow (sumdiry,2));
        diry = sumdiry / sqrt ( pow (sumdirx,2) + pow (sumdiry,2));
        qDebug() << dirx << diry;
    }*/
}


