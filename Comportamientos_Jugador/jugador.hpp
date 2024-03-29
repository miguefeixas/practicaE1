#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <queue>
#include <assert.h>

struct estado {
  int fila;
  int columna;
  int orientacion;
};

struct nodo
{
	estado st;
	list<Action> secuencia;
};

struct nodoAEstrella
{
	nodo infoBase;
	int f;
	int g;
	int h;
	bool zapatillas, bikini;
  int numObj;
  vector<bool> visitados;
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      hayPlan = bikiniPersonaje = zapatillasPersonaje = false;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      hayPlan = bikiniPersonaje = zapatillasPersonaje = false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    estado actual;
    list<estado> objetivos;
    list<Action> plan;
    bool hayPlan, bikiniPersonaje, zapatillasPersonaje;

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFindingAEstrella(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFindingAEstrellaMasObjetivos(const estado &origen, const vector<estado> &destinos, list<Action> &plan);
    

    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);
    int distanciaDiagonal(const estado &origen, const estado &destino);
    int distanciaManhattan(const estado &origen, const estado &destino);
    double distanciaEuclidea(const estado &origen, const estado &destino);
    void checkPowerUp(nodoAEstrella &a);
    int calculaPeso(const nodoAEstrella & n, const Action & a);
    void calculaHeuristica(nodoAEstrella & n, const Action & a, const estado &destino);
    int distanciaDiagonalMasObjetivos(const nodoAEstrella & n, const vector<estado> &destinos);
    void calculaHeuristicaMasObjetivos(nodoAEstrella & n, const Action & a, const vector<estado> &destinos);


};

#endif
