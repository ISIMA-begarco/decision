#include "algorithms.h"

#include "workingsolution.h"
#include "CompareMiddleTW.h"

#include <algorithm>
#include <cassert>

using namespace std;

void dummy (WorkingSolution & sol) {
  sol.clear();


  for (auto & node: sol.nodes())   {
    if (node.customer->id() != sol.data().depot()) {
      sol.open_specific_route(node);
    }
  }
}

void insertion (WorkingSolution & sol) {
    vector<NodeInfo> clientsVector;
    sol.clear();

    // recupere les clients dans le vecteur
    for (auto & node : sol.nodes()) {
        if (node.customer->id() != sol.data().depot())
            clientsVector.push_back(node);
    }
    // Trie les clients par leur moyenne de fenetre de temps
    std::sort(clientsVector.begin(), clientsVector.end(), CompareMiddleTW());

    list<NodeInfo> clients(clientsVector.begin(), clientsVector.end());
    //for(auto line : clients)
    //    cout << "#" << line.customer->id() << endl;

    while(!(clients.empty())) { // boucle principale
        RouteInfo & ri = sol.open_specific_route(clients.front());      // on cree une nouvelle route avec le premier client
        clients.pop_front();
        auto toInsert = clients.begin();
        while((toInsert=rechClientAInserer(clients, toInsert, ri, sol)) != clients.end()) { // on recherche si des clients peuvent etre inseres
            sol.insert(*(ri.depot.prev), *toInsert);     // on insere
            toInsert = clients.erase(toInsert);         // on enleve des clients non desservis
        }
    }
}
/*
NodeInfo & rechPrec(RouteInfo & tournee, NodeInfo & clients) {
    NodeInfo & prec = tournee.depot;
    NodeInfo * cour =
    while()
}
*/
/** \brief Recherche quel client inserer a la fin d'une tournee
 *
 * \param Vecteur contenant les clients tries par moyenne de fenetre de temps
 * \param Place a partir de laquelle rechercher le client
 * \param La tournee en cours
 * \param La solution courante
 * \return L'indice du client a inserer en fin de tournee, -1 si on en a pas
 *
 *//**
int rechClientAInserer(const forward_list<NodeInfo> clients, unsigned int from, const RouteInfo ri, WorkingSolution & sol) {
    unsigned int i = from;

    // Date de fermeture de ce client > que date
    while( i < clients.size()
          && (     !(sol.data().is_valid(ri.depot.prev->customer->id(),clients[i].customer->id()))
                || (clients[i].customer->close() < (ri.depot.prev->arrival + ri.depot.prev->customer->service()) )
                || (clients[i].customer->demand() + ri.depot.load <  sol.data().fleetCapacity())
             )
         ){
        i++;
    }

    return ((i > clients.size())? -1 : i);
}**/

list<NodeInfo>::iterator rechClientAInserer(const list<NodeInfo> & clients, list<NodeInfo>::iterator from, const RouteInfo ri, WorkingSolution & sol) {
    list<NodeInfo>::iterator i = from;

///TODO is_feasible ne traite pas l'existence des chemins
    // Date de fermeture de ce client > que date

    while( (i != clients.end())
          && (     !(sol.data().is_valid(ri.depot.prev->customer->id(),i->customer->id()))
                || (i->customer->close() < (ri.depot.prev->arrival + ri.depot.prev->customer->service()) + sol.data().distance(ri.depot.prev->customer->id(),i->customer->id()) )
                || (i->customer->demand() + ri.depot.load > sol.data().fleetCapacity())
             )
         ) {
        i++;
    }

    return i;
}

/**
    heuristique insertion
    heuristique débile
    heuristique fusion
**/


/*****************************************************************/
/**         Recherche locale                                    **/
/*****************************************************************/

/// recherche locale type cross
class RechercheLocale {
    public: virtual void operator() (WorkingSolution & s);
};

/// recherche locale type cross
class Cross : public RechercheLocale {
    public: void operator() (WorkingSolution & s) {

    }
};

/// recherche locale type 2 opt
class Opt2 : public RechercheLocale {
    public: void operator() (WorkingSolution & s) {

    }
};

/// recherche locale type or opt
class OrOpt : public RechercheLocale {
    public: void operator() (WorkingSolution & s) {

    }
};

/// cas particulier de la recherche locale type 2 opt
class Opt2Etoile : public RechercheLocale {
    public: void operator() (WorkingSolution & s) {

    }
};

/// cas particulier de la recherche locale type or opt
class OrOptEtoile : public RechercheLocale {
    public: void operator() (WorkingSolution & s) {

    }
};

/// recherche locale complete
class RechLocComplete {
    protected:
        vector<RechercheLocale> rl;

    public:

        RechLocComplete() {
            rl.push_back(Opt2Etoile());
            rl.push_back(OrOptEtoile());
            rl.push_back(Opt2());
            rl.push_back(OrOpt());
            rl.push_back(Cross());
        }

        void operator() (WorkingSolution & s) {
            unsigned k = 1;
            while(k < 6) {
            }
        }

};

/*****************************************************************/
/*****************************************************************/
