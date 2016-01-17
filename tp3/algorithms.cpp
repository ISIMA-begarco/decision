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
    //std::random_shuffle(clientsVector.begin(), clientsVector.end());
    std::random_shuffle(clientsVector.begin(), clientsVector.end());

    list<NodeInfo> clients(clientsVector.begin(), clientsVector.end());
    //for(auto line : clients)
    //    cout << "#" << line.customer->id() << endl;
    Time localDistance = 0;
    while(!(clients.empty())) { // boucle principale
        RouteInfo & ri = sol.open_specific_route(clients.front());      // on cree une nouvelle route avec le premier client
        clients.pop_front();
        auto toInsert = clients.begin();
        while((toInsert=rechClientAInserer(clients, toInsert, ri, sol)) != clients.end()) { // on recherche si des clients peuvent etre inseres
            toInsert->arrival = std::max(ri.depot.prev->arrival+sol.data().distance(ri.depot.prev->customer->id(),toInsert->customer->id()), toInsert->customer->open());
            sol.insert(*(ri.depot.prev), *toInsert);     // on insere
            toInsert = clients.erase(toInsert);         // on enleve des clients non desservis
        }
        updateDistanceRoute(sol, ri);
        localDistance += ri.distance;
    }
    sol.total_distance() = localDistance;
    RechLocComplete loc;
    loc(sol);
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
                || (i->customer->close() < ri.depot.prev->arrival + sol.data().distance(ri.depot.prev->customer->id(),i->customer->id()) )
                || (i->customer->demand() + ri.depot.load > sol.data().fleetCapacity())
             )
         ) {
        i++;
    }

    return i;
}

///
/// ATTENTION sol.data(i,j).distance contient le service de i + le trajet i->j
///
///     LES DISTANCES SONT LES TRAJETS + SERVICES PAS LES ATTENTES

/**
    heuristique insertion
    heuristique débile
    heuristique fusion
**/



/*****************************************************************/
/**         Utile a la modif                                    **/
/*****************************************************************/

void updateDistanceRoute(WorkingSolution & sol, RouteInfo & route) {
    NodeInfo * cur = route.depot.next;
    route.distance = sol.data().distance(route.depot.customer->id(), cur->customer->id());

    while(cur->customer->id() != route.depot.customer->id()) {
        route.distance += sol.data().distance(cur->customer->id(),cur->next->customer->id());
        cur = cur->next;
    }
}

void updateArrivalRoute(WorkingSolution & sol, RouteInfo & route) {
    NodeInfo * cur = route.depot.next;

    while(cur->customer->id() != route.depot.customer->id()) {
        cur->arrival = std::max(cur->prev->arrival+sol.data().distance(cur->customer->id(),cur->prev->customer->id()), cur->customer->open());
        cur = cur->next;
    }
    route.depot.arrival = route.depot.prev->arrival+sol.data().distance(route.depot.prev->customer->id(),route.depot.customer->id());
}

/*****************************************************************/
/**         Recherche locale                                    **/
/*****************************************************************/

/// recherche locale type cross
void Cross::operator() (WorkingSolution & s) {
    int i =5;
    i + 5;
}

/// recherche locale type 2 opt
void Opt2::operator() (WorkingSolution & s) {
    RouteInfo * route = s.first();
    unsigned i = 0, j = 0 ;
    bool optimise = false;

    while( i < s.nb_routes() && !optimise ) {                                           ///
        NodeInfo * client = route->depot.next;                                          /// pour tout A
        while( client->customer->id() != route->depot.customer->id() && !optimise ) {   ///

            RouteInfo * route2 = s.first();
            j = 0;
            while( j < s.nb_routes() && !optimise ) {                                               ///
                if(j != i) {                                                                        /// pour tout B
                    NodeInfo * client2 = route2->depot.next;                                        ///
                    while(client2->customer->id() != route2->depot.customer->id() && !optimise) {   ///

                        if(
                            (s.data().distance(client->customer->id(),client->next->customer->id()) +
                             s.data().distance(client2->customer->id(),client2->next->customer->id()))
                           >
                            (s.data().distance(client->customer->id(),client2->next->customer->id()) +
                             s.data().distance(client2->customer->id(),client->next->customer->id()))
                          ) {  /// est-ce que l'on gagne du temps ???
                            Load incrCapa = client->load - client2->load;
                            Time incrTime = client2->arrival + s.data().distance(client2->customer->id(),client->next->customer->id()) - client->next->arrival,
                                 incrTime2 = client->arrival + s.data().distance(client->customer->id(),client2->next->customer->id()) - client2->next->arrival;

                            if( s.is_feasible(*(client->next), -incrCapa, incrTime) &&
                                s.is_feasible(*(client2->next), incrCapa, incrTime2)
                              ) {       /// l'echange est-il possible ????

                                /// on fait l'echange
                                NodeInfo * tmp = client->next;
                                client->next->prev = client2;
                                client2->next->prev = client;
                                client->next = client2->next;
                                client2->next = tmp;

                                /// chainage des bons depots d'arrives
                                route2->depot.prev->next = &(route->depot);
                                tmp = route2->depot.prev;
                                route2->depot.prev = route->depot.prev;
                                route->depot.prev->next = &(route2->depot);
                                route->depot.prev = tmp;

                                s.update2(*client);
                                s.update2(*client2);

                                /// update distances_totales
                                s.total_distance() -= (route->distance + route2->distance);
                                updateDistanceRoute(s, *route);
                                updateDistanceRoute(s, *route2);
                                s.total_distance() += (route->distance + route2->distance);

                                optimise = true;
                            }
                        }

                        client2 = client2->next;
                    }
                }
                route2 = route2->next_;
                j++;
            }

            client = client->next;
        }
        route = route->next_;
        i++;
    }
}


/// recherche locale type or opt
void OrOpt::operator() (WorkingSolution & s) {
    RouteInfo * route = s.first();
    unsigned i = 0, j = 0 ;
    bool optimise = false;

    while( i < s.nb_routes() ) {
        NodeInfo * client = route->depot.next;
        while( client->customer->id() != route->depot.customer->id() ) {
            //cout << client->customer->id() << " ";
            client = client->next;
        }
        i++;
        route = route->next_;
        //cout << endl;
    }
}

/// cas particulier de la recherche locale type 2 opt
void Opt2Etoile::operator() (WorkingSolution & s) {
    RouteInfo * route = s.first();
    unsigned i = 0, j = 0 ;
    bool optimise = false;

    while( i < s.nb_routes() ) {
        NodeInfo * client = route->depot.next;
        while( client->customer->id() != route->depot.customer->id() ) {
            //cout << client->customer->id() << " ";
            client = client->next;
        }
        i++;
        route = route->next_;
        //cout << endl;
    }
}

/// cas particulier de la recherche locale type or opt
void OrOptEtoile::operator() (WorkingSolution & s) {
    int i =5;
    i + 5;
}

/// recherche locale complete
RechLocComplete::RechLocComplete() {
    rl[0] = new Opt2Etoile();
    rl[1] = new OrOptEtoile();
    rl[2] = new Opt2();
    rl[3] = new OrOpt();
    rl[4] = new Cross();
}

void RechLocComplete::operator() (WorkingSolution & s) {
    unsigned    k = 0, max = 1,
                oldNbRoutes = s.nb_routes();
    Time        oldDistance = s.total_distance();

    cout << "coucou on a : " << oldNbRoutes << " routes et " << oldDistance << " km\n";
    while(k < 5) {
        rl[k++]->operator()(s);
        if(oldDistance > s.total_distance() || oldNbRoutes > s.nb_routes()) {
            k = 0;
            oldNbRoutes = s.nb_routes();
            oldDistance = s.total_distance();
            max--;
        }
    }
    cout << "coucou on a : " << oldNbRoutes << " routes et " << oldDistance << " km\n";
}



/*****************************************************************/
/*****************************************************************/
