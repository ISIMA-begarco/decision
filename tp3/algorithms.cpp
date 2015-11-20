#include "algorithms.h"

#include "workingsolution.h"
#include "CompareMiddleTW.h"

#include <algorithm>
#include <cassert>

using namespace std;

void dummy0 (WorkingSolution & sol) {
  sol.clear();


  for (auto & node: sol.nodes())   {
    if (node.customer->id() != sol.data().depot()) {
      sol.open_specific_route(node);
    }
  }
}

void dummy (WorkingSolution & sol) {
    RouteInfo & ri = sol.open_route();
    vector<NodeInfo> clients;
    sol.clear();

    // recupere les clients dans le vecteur
    for (auto & node: sol.nodes()) {
        if (node.customer->id() != sol.data().depot())
            clients.push_back(node);
    }
    // Trie les clients par leur moyenne de fenetre de temps
    std::sort(clients.begin(), clients.end(), CompareMiddleTW);

    while(clients.size()) { // boucle principale
        for(auto & node : clients) {
            if(sol.data().is_valid(ri. ))
        }
    }
}

NodeInfo & rechPrec(RouteInfo & tournee, NodeInfo & clients) {
    NodeInfo & prec = tournee.depot;
    NodeInfo * cour =
    while()
}

/** \brief Recherche quel client inserer a la fin d'une tournee
 *
 * \param Vecteur contenant les clients tries par moyenne de fenetre de temps
 * \param Place a partir de laquelle rechercher le client
 * \param La tournee en cours
 * \param La solution courante
 * \return L'indice du client a inserer en fin de tournee, -1 si on en a pas
 *
 */
int rechClientAInserer(const vector<NodeInfo> clients, unsigned int from, const RouteInfo ri, WorkingSolution & sol) {
    unsigned int i = from;

    // Date de fermeture de ce client > que date
    while( i < clients.size()
          && ( (client[i].customer->close_ < (ri.depot.prev->arrival + ri.depot.prev->customer->service_) )
          || (clients[i].customer->demand_ + ri.depot.load <  sol.data().fleetCapacity()) ){
        i++;
    }

    return ((i > clients.size())? -1; i);
}

/**
    heuristique insertion
    heuristique débile
    heuristique fusion
**/
