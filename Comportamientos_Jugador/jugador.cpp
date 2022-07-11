#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	//Action accion = actIDLE;

	actual.fila = sensores.posF;
	actual.columna = sensores.posC;
	actual.orientacion = sensores.sentido;

	// Capturo los destinos
	cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
	objetivos.clear();
	for (int i = 0; i < sensores.num_destinos; i++)
	{
		estado aux;
		aux.fila = sensores.destino[2 * i];
		aux.columna = sensores.destino[2 * i + 1];
		objetivos.push_back(aux);
	}

	if (!hayPlan) {
		hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan);
	}

	Action sigAccion;
	if (hayPlan && plan.size() > 0) {
		sigAccion = plan.front();
		plan.erase(plan.begin());
	} else {
		cout << "No se pudo encontrar un plan\n";
	}

	

	return sigAccion;
}

// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan)
{
	vector<estado> tresObjetivos;
	list<estado> destinoAux = destino;
	estado un_objetivo;
	int numDest = destinoAux.size();
	

	switch (level)
	{
	case 0:
		cout << "Demo\n";
		//estado un_objetivo;
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Profundidad(origen, un_objetivo, plan);
		break;

	case 1:
		cout << "Optimo en coste\n";
		//estado un_objetivo;
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFindingAEstrella(origen, un_objetivo, plan);
		break;
	case 2:
		cout << "Optimo en coste para tres objetivos\n";
		
		for (int i = 0; i < numDest; i++) {
			tresObjetivos.push_back(destinoAux.front());
			destinoAux.pop_front();
		}
		
		return pathFindingAEstrellaMasObjetivos(origen, tresObjetivos, plan);
		//cout << "No implementado aun\n";
		break;
	case 3:
		cout << "Reto 1: Descubrir el mapa\n";
		// Incluir aqui la llamada al algoritmo de busqueda para el Reto 1
		cout << "No implementado aun\n";
		break;
	case 4:
		cout << "Reto 2: Maximizar objetivos\n";
		// Incluir aqui la llamada al algoritmo de busqueda para el Reto 2
		cout << "No implementado aun\n";
		break;
	}
	return false;
}

//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla)
{
	if (casilla == 'P' or casilla == 'M')
		return true;
	else
		return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st)
{
	int fil = st.fila, col = st.columna;

	// calculo cual es la casilla de delante del agente
	switch (st.orientacion)
	{
	case 0:
		fil--;
		break;
	case 1:
		fil--;
		col++;
		break;
	case 2:
		col++;
		break;
	case 3:
		fil++;
		col++;
		break;
	case 4:
		fil++;
		break;
	case 5:
		fil++;
		col--;
		break;
	case 6:
		col--;
		break;
	case 7:
		fil--;
		col--;
		break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil < 0 or fil >= mapaResultado.size())
		return true;
	if (col < 0 or col >= mapaResultado[0].size())
		return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col]))
	{
		// No hay obstaculo, actualizo el parametro st poniendo la casilla de delante.
		st.fila = fil;
		st.columna = col;
		return false;
	}
	else
	{
		return true;
	}
}

// Aquí estaba el struct nodo

struct ComparaEstados
{
	bool operator()(const estado &a, const estado &n) const
	{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};

// Implementación de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	stack<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 135 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 3) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actOVERTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de girar a la izquierda 135 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 5) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actOVERTURN_L);
			Abiertos.push(hijoTurnL);
		}


		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

//---------------------- Implementación de A Estrella---------------------------


struct ComparaEstrellaF
{
	bool operator()(const nodoAEstrella &a, const nodoAEstrella &n) const
	{
		if (a.f > n.f)
			return true;
		else
			return false;
	}
};

struct ComparaEstrellaEstados
{
	bool operator()(const nodoAEstrella &a, const nodoAEstrella &n) const
	{
		// Añadir quizá brújula y bikini
		if (
			(a.infoBase.st.fila > n.infoBase.st.fila) ||
			(a.infoBase.st.fila == n.infoBase.st.fila && a.infoBase.st.columna > n.infoBase.st.columna) ||
			(a.infoBase.st.fila == n.infoBase.st.fila && a.infoBase.st.columna == n.infoBase.st.columna && a.infoBase.st.orientacion > n.infoBase.st.orientacion) ||
			(a.infoBase.st.fila == n.infoBase.st.fila && a.infoBase.st.columna == n.infoBase.st.columna && a.infoBase.st.orientacion == n.infoBase.st.orientacion && a.zapatillas > n.zapatillas && a.bikini > n.bikini) ||
			(a.infoBase.st.fila == n.infoBase.st.fila && a.infoBase.st.columna == n.infoBase.st.columna && a.infoBase.st.orientacion == n.infoBase.st.orientacion && a.bikini > n.bikini) ||
			(a.infoBase.st.fila == n.infoBase.st.fila && a.infoBase.st.columna == n.infoBase.st.columna && a.infoBase.st.orientacion == n.infoBase.st.orientacion && a.zapatillas > n.zapatillas)
		)

			return true;
		else
			return false;
	}
};

int ComportamientoJugador::distanciaDiagonal(const estado &origen, const estado &destino) {
	int dx = abs(origen.fila - destino.fila);
	int dy = abs(origen.columna - destino.columna);

	return max(dx, dy);
}

int ComportamientoJugador::distanciaManhattan(const estado &origen, const estado &destino) {
	return (abs(origen.fila - destino.fila) + abs(origen.columna - destino.columna));
}

double ComportamientoJugador::distanciaEuclidea(const estado &origen, const estado &destino) {
	int dx = pow((origen.fila - destino.fila), 2);
	int dy = pow((origen.columna - destino.columna), 2);

	return sqrt((dx + dy));
}

void ComportamientoJugador::checkPowerUp(nodoAEstrella &a) {
	if (mapaResultado[a.infoBase.st.fila][a.infoBase.st.columna]=='K')
			a.bikini=true;
		if (mapaResultado[a.infoBase.st.fila][a.infoBase.st.columna]=='D')
			a.zapatillas=true;
}

int ComportamientoJugador::calculaPeso(const nodoAEstrella & n, const Action & a) {
	char tipoTerreno = mapaResultado[n.infoBase.st.fila][n.infoBase.st.columna];
	int peso = -1;

	if (tipoTerreno == 'A') {
		if (a == actFORWARD) {
			if (n.bikini)
				peso = 15;
			else
				peso = 100;
		} else if (a == actOVERTURN_L || a == actOVERTURN_R) {
			if (n.bikini)
				peso = 10;
			else
				peso = 200;
		} else if (a == actSEMITURN_L || a == actSEMITURN_R) {
			if (n.bikini)
				peso = 5;
			else
				peso = 75;
		} else if (a == actWHEREIS)
			peso = 500;
	} else if (tipoTerreno == 'B') {
		if (a == actFORWARD) {
			if (n.zapatillas)
				peso = 10;
			else
				peso = 200;
		} else if (a == actOVERTURN_L || a == actOVERTURN_R) {
			if (n.zapatillas)
				peso = 6;
			else
				peso = 350;
		} else if (a == actSEMITURN_L || a == actSEMITURN_R) {
			if (n.zapatillas)
				peso = 3;
			else
				peso = 150;
		} else if (a == actWHEREIS)
			peso = 500;
	} else if (tipoTerreno == 'T') {
		if (a == actFORWARD || a == actOVERTURN_L || a == actOVERTURN_R || a == actSEMITURN_L || a == actSEMITURN_R) {
			if (n.zapatillas)
				peso = 1;
			else
				peso = 3;
		} else if (a == actWHEREIS)
			peso = 10;
	} else if (a == actWHEREIS)
		peso = 500;
	else
		peso = 1;
	
	assert(peso != -1);

	return peso;
}

void ComportamientoJugador::calculaHeuristica(nodoAEstrella & n, const Action & a, const estado &destino) {
	n.g += calculaPeso(n, a);
	n.h = distanciaDiagonal(n.infoBase.st, destino);
	//n.h = distanciaManhattan(n.infoBase.st, destino);
	//n.h = distanciaEuclidea(n.infoBase.st, destino);
	n.f = n.g + n.h;
}

bool ComportamientoJugador::pathFindingAEstrella(const estado &origen, const estado &destino, list<Action> &plan) {
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<nodoAEstrella, ComparaEstrellaEstados> Cerrados; 								// Lista de Cerrados
	priority_queue<nodoAEstrella, vector<nodoAEstrella>, ComparaEstrellaF> Abiertos;	// Lista de Abiertos


	nodoAEstrella current;
	current.infoBase.st = origen;
	current.infoBase.secuencia.empty();
	current.g = 0;
	current.f = distanciaDiagonal(current.infoBase.st, destino);
	current.f = current.g + current.f;
	current.bikini = bikiniPersonaje;
	current.zapatillas = zapatillasPersonaje;

	cout << endl << "Bikini = " << current.bikini << endl;
	cout << "Zapatillas = " << current.zapatillas << endl;

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.infoBase.st.fila != destino.fila or current.infoBase.st.columna != destino.columna))
	{

		//cout << "En abiertos hay " << Abiertos.size() << " nodos." << endl;

		Abiertos.pop();
		Cerrados.insert(current);
		checkPowerUp(current);

		// Generar descendiente de girar a la derecha 135 grados
		nodoAEstrella hijoTurnR = current;
		hijoTurnR.infoBase.st.orientacion = (hijoTurnR.infoBase.st.orientacion + 3) % 8;
		calculaHeuristica(hijoTurnR, actOVERTURN_R, destino);
		
		if (Cerrados.find(hijoTurnR) == Cerrados.end())
		{
			hijoTurnR.infoBase.secuencia.push_back(actOVERTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodoAEstrella hijoSEMITurnR = current;
		hijoSEMITurnR.infoBase.st.orientacion = (hijoSEMITurnR.infoBase.st.orientacion + 1) % 8;
		calculaHeuristica(hijoSEMITurnR, actSEMITURN_R, destino);

		if (Cerrados.find(hijoSEMITurnR) == Cerrados.end())
		{
			hijoSEMITurnR.infoBase.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodoAEstrella hijoSEMITurnL = current;
		hijoSEMITurnL.infoBase.st.orientacion = (hijoSEMITurnL.infoBase.st.orientacion + 7) % 8;
		calculaHeuristica(hijoSEMITurnL, actSEMITURN_L, destino);

		if (Cerrados.find(hijoSEMITurnL) == Cerrados.end())
		{
			hijoSEMITurnL.infoBase.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de girar a la izquierda 135 grados
		nodoAEstrella hijoTurnL = current;
		hijoTurnL.infoBase.st.orientacion = (hijoTurnL.infoBase.st.orientacion + 5) % 8;
		calculaHeuristica(hijoTurnL, actOVERTURN_L, destino);

		if (Cerrados.find(hijoTurnL) == Cerrados.end())
		{
			hijoTurnL.infoBase.secuencia.push_back(actOVERTURN_L);
			Abiertos.push(hijoTurnL);
		}


		// Generar descendiente de avanzar
		nodoAEstrella hijoForward = current;
		calculaHeuristica(hijoForward, actFORWARD, destino);

		if (!HayObstaculoDelante(hijoForward.infoBase.st))
		{
			if (Cerrados.find(hijoForward) == Cerrados.end())
			{
				hijoForward.infoBase.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.infoBase.st.fila == destino.fila and current.infoBase.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.infoBase.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

//---------------------- Implementación de A Estrella para varios objetivos ---------------------------

struct ComparaEstrellaEstadosTresObj
{
	bool operator()(const nodoAEstrella &a, const nodoAEstrella &n) const
	{
		// Añadir quizá brújula y bikini
		if (
			(a.infoBase.st.fila > n.infoBase.st.fila) ||
			(a.infoBase.st.fila == n.infoBase.st.fila && a.infoBase.st.columna > n.infoBase.st.columna) ||
			(a.infoBase.st.fila == n.infoBase.st.fila && a.infoBase.st.columna == n.infoBase.st.columna && a.infoBase.st.orientacion > n.infoBase.st.orientacion) ||
			(a.infoBase.st.fila == n.infoBase.st.fila && a.infoBase.st.columna == n.infoBase.st.columna && a.infoBase.st.orientacion == n.infoBase.st.orientacion && a.zapatillas > n.zapatillas && a.bikini > n.bikini) ||
			(a.infoBase.st.fila == n.infoBase.st.fila && a.infoBase.st.columna == n.infoBase.st.columna && a.infoBase.st.orientacion == n.infoBase.st.orientacion && a.bikini > n.bikini) ||
			(a.infoBase.st.fila == n.infoBase.st.fila && a.infoBase.st.columna == n.infoBase.st.columna && a.infoBase.st.orientacion == n.infoBase.st.orientacion && a.zapatillas > n.zapatillas) ||
			(a.infoBase.st.fila == n.infoBase.st.fila && a.infoBase.st.columna == n.infoBase.st.columna && a.infoBase.st.orientacion == n.infoBase.st.orientacion && a.zapatillas == n.zapatillas && a.bikini == n.bikini && a.numObj < n.numObj) 
		)

			return true;
		else
			return false;
	}
};

/*struct ComparaEstrellaFVarios
{
	bool operator()(const nodoAEstrella &a, const nodoAEstrella &n) const
	{
		if (a.numObj > n.numObj && a.f > n.f)
			return true;
		else
			return false;
	}
};*/

int ComportamientoJugador::distanciaDiagonalMasObjetivos(const nodoAEstrella & n, const vector<estado> &destinos) {
	int menorH = 3001;
	int auxH = 0;

	for (int i = 0; i < destinos.size(); i++) {
		if (!n.visitados[i]) {
			//cout << "Aqui entra" << endl;
			auxH = distanciaDiagonal(n.infoBase.st, destinos[i]);
			if (menorH > auxH)
				menorH = auxH;
			//auxH += distanciaDiagonal(n.infoBase.st, destinos[i]);
		}
	}

	return menorH;
}

void ComportamientoJugador::calculaHeuristicaMasObjetivos(nodoAEstrella & n, const Action & a, const vector<estado> &destinos) {
	n.g += calculaPeso(n, a);

	n.h = distanciaDiagonalMasObjetivos(n, destinos);
	n.f = n.g + n.h;
}

bool ComportamientoJugador::pathFindingAEstrellaMasObjetivos(const estado &origen, const vector<estado> &destinos, list<Action> &plan) {
	cout << "Tamaño de destinos: " << destinos.size() << endl;

	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<nodoAEstrella, ComparaEstrellaEstadosTresObj> Cerrados; 								// Lista de Cerrados
	priority_queue<nodoAEstrella, vector<nodoAEstrella>, ComparaEstrellaF> Abiertos;	// Lista de Abiertos

	//int maxObjConseguidos = 0;

	nodoAEstrella current;
	current.infoBase.st = origen;
	current.infoBase.secuencia.empty();
	for (int i = 0; i < destinos.size(); i++)
		current.visitados.push_back(false);
	current.g = 0;
	current.h = distanciaDiagonalMasObjetivos(current, destinos);
	current.f = current.g + current.f;
	current.bikini = bikiniPersonaje;
	current.zapatillas = zapatillasPersonaje;
	current.numObj = 0;

	cout << endl << "Bikini = " << current.bikini << endl;
	cout << "Zapatillas = " << current.zapatillas << endl;

	Abiertos.push(current);

	while (!Abiertos.empty() && current.numObj < 3)
	{
		cout << "El num de objetivos ya visitados es de: " << current.numObj << endl;
		//cout << "En abiertos hay " << Abiertos.size() << " nodos." << endl;

		Abiertos.pop();
		Cerrados.insert(current);
		checkPowerUp(current);

		/*for (int i = 0; i < destinos.size(); i++) {
			cout << "El destino " << i << "es: (" << destinos[i].fila << ", " << destinos[i].columna << ")" << endl;
		}*/

		for (int i = 0; i < destinos.size(); i++) {
			if (current.infoBase.st.fila == destinos[i].fila && current.infoBase.st.columna == destinos[i].columna and !current.visitados[i]) {
				//cout << "El destino que se ha alcanzado es el " << i << endl;
				current.visitados[i] = true;
				current.numObj++;

				//cout << "Un objetivo alcanzado, ahora mismo hay encontrados " << current.numObj << endl;
			}
		}

		if (current.numObj == 3) {
			break;
		}

		// Generar descendiente de girar a la derecha 135 grados
		nodoAEstrella hijoTurnR = current;
		hijoTurnR.infoBase.st.orientacion = (hijoTurnR.infoBase.st.orientacion + 3) % 8;
		calculaHeuristicaMasObjetivos(hijoTurnR, actOVERTURN_R, destinos);
		
		if (Cerrados.find(hijoTurnR) == Cerrados.end())
		{
			hijoTurnR.infoBase.secuencia.push_back(actOVERTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodoAEstrella hijoSEMITurnR = current;
		hijoSEMITurnR.infoBase.st.orientacion = (hijoSEMITurnR.infoBase.st.orientacion + 1) % 8;
		calculaHeuristicaMasObjetivos(hijoSEMITurnR, actSEMITURN_R, destinos);

		if (Cerrados.find(hijoSEMITurnR) == Cerrados.end())
		{
			hijoSEMITurnR.infoBase.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodoAEstrella hijoSEMITurnL = current;
		hijoSEMITurnL.infoBase.st.orientacion = (hijoSEMITurnL.infoBase.st.orientacion + 7) % 8;
		calculaHeuristicaMasObjetivos(hijoSEMITurnL, actSEMITURN_L, destinos);

		if (Cerrados.find(hijoSEMITurnL) == Cerrados.end())
		{
			hijoSEMITurnL.infoBase.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de girar a la izquierda 135 grados
		nodoAEstrella hijoTurnL = current;
		hijoTurnL.infoBase.st.orientacion = (hijoTurnL.infoBase.st.orientacion + 5) % 8;
		calculaHeuristicaMasObjetivos(hijoTurnL, actOVERTURN_L, destinos);

		if (Cerrados.find(hijoTurnL) == Cerrados.end())
		{
			hijoTurnL.infoBase.secuencia.push_back(actOVERTURN_L);
			Abiertos.push(hijoTurnL);
		}


		// Generar descendiente de avanzar
		nodoAEstrella hijoForward = current;
		calculaHeuristicaMasObjetivos(hijoForward, actFORWARD, destinos);

		if (!HayObstaculoDelante(hijoForward.infoBase.st))
		{
			if (Cerrados.find(hijoForward) == Cerrados.end())
			{
				hijoForward.infoBase.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}

		/*if (current.numObj > maxObjConseguidos) {
			maxObjConseguidos = current.numObj;

			//cout << "Actualiza maxObjConseguidos, ahora vale " << maxObjConseguidos << endl;
			//cout << "En abiertos hay " << Abiertos.size() << " nodos." << endl;
			
		}*/
		
		
	}

	cout << "Terminada la busqueda\n";

	if (current.numObj == 3)
	{
		cout << "Cargando el plan\n";
		plan = current.infoBase.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;

}

// Sacar por la consola la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan)
{
	bool error = false;
	auto it = plan.begin();
	while (it != plan.end() and !error)
	{
		if (*it == actTURN_R or *it == actTURN_L){
			cout << "Las acciones actTURN_R y actTURN_L no son acciones válidas para el agente para esta práctica: REVISA TU CÓDIGO\n";
			error = true;
		}
		else if (*it == actFORWARD)
		{
			cout << "A ";
		}
		else if (*it == actOVERTURN_R)
		{
			cout << "D ";
		}
		else if (*it == actSEMITURN_R)
		{
			cout << "d ";
		}
		else if (*it == actOVERTURN_L)
		{
			cout << "I ";
		}
		else if (*it == actSEMITURN_L)
		{
			cout << "i ";
		}
		else
		{
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

// Funcion auxiliar para poner a 0 todas las casillas de una matriz
void AnularMatriz(vector<vector<unsigned char>> &m)
{
	for (int i = 0; i < m[0].size(); i++)
	{
		for (int j = 0; j < m.size(); j++)
		{
			m[i][j] = 0;
		}
	}
}

// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	estado cst = st;
	bool error = false;

	auto it = plan.begin();
	while (it != plan.end() and !error)
	{
		if (*it == actTURN_R or *it == actTURN_L){
			error = true;
		}
		else if (*it == actFORWARD)
		{
			switch (cst.orientacion)
			{
			case 0:
				cst.fila--;
				break;
			case 1:
				cst.fila--;
				cst.columna++;
				break;
			case 2:
				cst.columna++;
				break;
			case 3:
				cst.fila++;
				cst.columna++;
				break;
			case 4:
				cst.fila++;
				break;
			case 5:
				cst.fila++;
				cst.columna--;
				break;
			case 6:
				cst.columna--;
				break;
			case 7:
				cst.fila--;
				cst.columna--;
				break;
			}
			mapaConPlan[cst.fila][cst.columna] = 1;
		}
		else if (*it == actOVERTURN_R)
		{
			cst.orientacion = (cst.orientacion + 3) % 8;
		}
		else if (*it == actSEMITURN_R)
		{
			cst.orientacion = (cst.orientacion + 1) % 8;
		}
		else if (*it == actOVERTURN_L)
		{
			cst.orientacion = (cst.orientacion + 5) % 8;
		}
		else if (*it == actSEMITURN_L)
		{
			cst.orientacion = (cst.orientacion + 7) % 8;
		}
		it++;
	}
	if (error){
		AnularMatriz(mapaConPlan);
	}
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
