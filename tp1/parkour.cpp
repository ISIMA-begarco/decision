#include "parkour.h"

const float INFINI = 1000000.;

// Relachement
void relachement(const Graph & g, float & distance[], Arc* pere[], const Arc & a) {
	if (distance[a.dest()] < (distance[a.orig()] + a.cost()) ) {
		distance[a.dest()] = distance[a.orig()] + a.cost();
		pere[a.dest()] = &a; //TODO Il est possible que ce ne soit pas bon
	}
}

// Initialise les donnees avant les algos
void init(const Graph & g, const Id origine, float & distance[], Arc* pere[]) {
	Node courant;
	int sommet_max = g.nodes().size();
	
	
	// Initialise les tableaux de distance et pere
	for (int i = 0; i < sommet_max ; i++)	{
		distance[i] = INFINI;
		pere[i] = 0;
	}
	distance[origine] = 0;
	
	
}

void dijkstra(const Graph & g, Id origine, float & distance[], Arc* pere[]) {
	Nvector noeuds = g.nodes(); //copie pour faire les modifications TODO mettre un tas (le prof a dit map)
	
	//Initialisation des parametres
	init(g, origine, distance, pere);
	
	while (!noeuds.empty()) {
		//TODO faire la suite
	}
}

//TODO finir le tp
