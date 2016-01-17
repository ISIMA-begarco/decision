#include "algorithms.h"

#include "workingsolution.h"
#include "CompareMiddleTW.h"

#include <algorithm>
#include <cassert>

#include <cstdlib>

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
    // std::sort(clientsVector.begin(), clientsVector.end(), CompareMiddleTW());
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

void insertion_sorted(WorkingSolution & sol) {
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

void aff(WorkingSolution & s) {
    RouteInfo * route = s.first();
    unsigned i = 0;

    while( i < s.nb_routes() ) {
        NodeInfo * client = route->depot.next;
        while( client->customer->id() != route->depot.customer->id() ) {
            cout << client->customer->id() << " ";
            client = client->next;
        }
        i++;
        route = route->next_;
        cout << endl;
    }
}

void swapClients(NodeInfo * c1, NodeInfo * c2) {
    //cout << "On peut SWAPPER le SWAP" << endl;
    NodeInfo * tmp;

    /// suivant des actuelles
    tmp = c1->next;
    c1->next = c2->next;
    c2->next = tmp;
    /// precedant des actuelles
    tmp = c1->prev;
    c1->prev = c2->prev;
    c2->prev = tmp;

    /// suivant des precedant
    tmp = c1->prev->next;
    c1->prev->next = c2->prev->next;
    c2->prev->next = tmp;
    /// precedant des suivants
    tmp = c1->next->prev;
    c1->next->prev = c2->next->prev;
    c2->next->prev = tmp;

}

/*****************************************************************/
/**         Recherche locale                                    **/
/*****************************************************************/

/// recherche locale type cross
void Cross::operator() (WorkingSolution & s) {
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

 /********************************************************************************************/
                        bool    seul1 = (client->next == client->prev),
                                seul2 = (client2->next == client2->prev),
                                faisable1 = true,
                                faisable2 = true;
                        Id      id1 = client->customer->id(),
                                id2 = client2->customer->id();
                        if(!seul1) {    /// si il y a plusieurs éléments dans route 1
                            Load capa = - client->customer->demand() + client2->customer->demand();
                            Time variable = std::max(s.data().distance(client->prev->customer->id(), id2), (client2->customer->open() - client->prev->arrival));
                            Time temps =    variable
                                          + std::max(s.data().distance(id2, client->next->customer->id()), (client->next->customer->open() - client->prev->arrival + variable))
                                          - (client->next->arrival - client->prev->arrival);
//cout << "temps de décalage de la fin de route 1 " << temps << endl;
                            faisable1 =     ( (route->depot.load - client->customer->demand() + client2->customer->demand()) <= s.data().fleetCapacity() )
                                        &&  ( s.is_feasible(*(client->next), capa, temps) )
                                        &&  ( client2->customer->close() >= (client->prev->arrival+s.data().distance(client->prev->customer->id(), id2)) );
                        }
                        if(faisable1) {
                            if(!seul2) {    /// si il y a plusieurs éléments dans route 2
                                Load capa = - client2->customer->demand() + client->customer->demand();
                                Time variable = std::max(s.data().distance(client2->prev->customer->id(), id1), (client->customer->open() - client2->prev->arrival));
                                Time temps =    variable
                                              + std::max(s.data().distance(id1, client2->next->customer->id()), (client2->next->customer->open() - client2->prev->arrival + variable))
                                              - (client2->next->arrival - client2->prev->arrival);
//cout << "temps de décalage de la fin de route 2 " << temps << endl;
                                faisable2 =     ( (route2->depot.load - client2->customer->demand() + client->customer->demand()) <= s.data().fleetCapacity() )
                                            &&  ( s.is_feasible(*(client2->next), capa, temps) )
                                            &&  ( client->customer->close() >= (client2->prev->arrival+s.data().distance(client2->prev->customer->id(), id1)) );
                            }
                        }
                        if(faisable1 && faisable2) {
                            Time oldDistance = route->distance + route2->distance;
                            swapClients(client, client2);

                            s.update2(*(client->prev));
                            s.update2(*(client2->prev));
                            updateDistanceRoute(s, *route);
                            updateDistanceRoute(s, *route2);
                            //cout << " Old value = " << oldDistance << " - New value = " << route->distance+route2->distance << endl;
                            if(route->distance+route2->distance < oldDistance) {
                                optimise = true;
                                s.total_distance() += (-oldDistance+route->distance+route2->distance);
                                //cout << "OPTIMISATION swap " << client->customer->id() << " " << client2->customer->id() << endl;
                            } else {
                                swapClients(client, client2);
                                s.update2(*(client->prev));
                                s.update2(*(client2->prev));
                                updateDistanceRoute(s, *route);
                                updateDistanceRoute(s, *route2);
                            }

                        }
/****************************************************************************************************/

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
    //std::cout << "On commence a faire le OR-OPT" << std::endl;
    bool optimise = false;
    unsigned i = 0, j = 0;
    RouteInfo* r1 = s.first();
    RouteInfo* r2 = NULL;
    while( i < s.nb_routes() && !optimise ) { /// Pour chaque tournee 1
        r2 = r1->next_;
        j = i+1;
        while( j < s.nb_routes() && !optimise ) { /// Pour chaque tournee 2, ou r2 = s.first et tester si pas meme tournee
           /// std::cout << "Couple route " << r1->id << " ; " << r2->id <<std::endl;
           /// std::cout << "On va tester les clients de tournees differentes"  << std::endl;
            NodeInfo* c1 = r1->depot.next;
            while( c1->customer->id() != 0 && !optimise ) { /// Pour chaque client de la tournee 1, sauf le depot
                NodeInfo* c2 = r2->depot.next;
                while( c2->customer->id() != 0 && !optimise ) { // Pour chaque client de la tournee 2
                    ///std::cout << "Test ajout client " << c1->customer->id() << " (route " << r1->id << ") apres client " << c2->customer->id() << " (route " << r2->id << ")"  << std::endl;
                    if(c1->route->id != c2->route->id && s.data().is_valid(c1->customer->id(), c2->customer->id())  /// si pas la meme route et chemin existe
                       && c2->next != c2->prev) { /// On est pas dans la meme tournee et le chemin existe
                        /// Alors on peut deplacer
                        Time incr = std::max(c1->arrival + s.data().distance(c1->customer->id(), c2->customer->id()), c2->customer->open()) + s.data().distance(c2->customer->id(), c1->next->customer->id()) - c1->next->arrival;
                        //cout << "l'incr vaut " << incr << endl;
                        if( c1->arrival + s.data().distance(c1->customer->id(), c2->customer->id()) < c2->customer->close() && /// si c2 est ok a cette place
                            s.is_feasible(*(c1->next), c2->customer->demand(), incr)) { /// et que la fin de r1 aussi
                            Time oldDistance = r1->distance + r2->distance;
                            NodeInfo * precC2 = c2->prev;
                            s.remove(*c2); // Enleve c1 de sa route, la close si necessaire
                            s.insert(*c1, *c2); // Ajoute c1 apres c2 dans sa route
                            //std::cout << "Ajout du client " << c2->customer->id() << " apres le client " << c1->customer->id() << " dans la route " << r1->id  << std::endl;
                            if(r1->distance+r2->distance < oldDistance) {
                                optimise = true;
                            } else {
                                s.remove(*c2);
                                s.insert(*precC2, *c2);
                            }
                        }
                    }
                    c2 = c2->next;
                }
                c1 = c1->next;
            }
            j++;
            r2 = r2->next_;
        }
        i++;
        r1 = r1->next_;
    }
    //std::cout << "On a fini de faire le OR-OPT" << std::endl;
}

/// cas particulier de la recherche locale type 2 opt
void Opt2Etoile::operator() (WorkingSolution & s) {
    RouteInfo * route1 = s.first();
    RouteInfo * route2 = NULL;
    unsigned i = 0, j = 0 ;
    bool optimise = false;

    while( i < s.nb_routes() && !optimise ) {           /// pour chaque route
        route2 = s.first();
        while( j < s.nb_routes() && !optimise ) {       /// on teste toutes les autres routes
            if(route1->id != route2->id) {      /// sauf la meme
                Time incrTemps = route1->depot.prev->arrival + s.data().distance(route1->depot.prev->customer->id(),route2->depot.next->customer->id()) - s.data().distance(route2->depot.customer->id(),route2->depot.next->customer->id());
                if( s.is_feasible(*(route2->depot.next), route1->depot.load, incrTemps ) ) {
                    //cout << "Optimisation possible entre " << route1->id << " et " << route2->id << endl;

                    Time saving = - s.data().distance(route1->depot.prev->customer->id(),route1->depot.customer->id()) - s.data().distance(route2->depot.customer->id(),route2->depot.next->customer->id()) + std::max(s.data().distance(route1->depot.prev->customer->id(),route2->depot.next->customer->id()), route2->depot.next->customer->open() - route1->depot.prev->arrival );
                    NodeInfo * orig = route1->depot.prev;


                    ///chainage inter tournees
                    route1->depot.prev->next = route2->depot.next;
                    route2->depot.next->prev = route1->depot.prev;

                    ///chainage au depot 1
                    route1->depot.prev = route2->depot.prev;
                    route1->depot.prev->next = &(route1->depot);

                    /// chainage du depot 2 a lui meme
                    route2->depot.next = &(route2->depot);
                    route2->depot.prev = &(route2->depot);

                    /// maj
                    s.update2(*orig);
                    route1->distance += (route2->distance + saving);
                    s.total_distance() += saving;

                    /// suppression
                    s.close_route(*(route2));

                    optimise = true;
                }
            }

            j++;
            route2 = route2->next_;
        }
        i++;
        route1 = route1->next_;
    }
                   // cout << "Fin du Opt2Etoile" << endl;
}

/// cas particulier de la recherche locale type or opt
void OrOptEtoile::operator() (WorkingSolution & s) {
    bool optimise = false;
    unsigned i = 0, j = 0;
    RouteInfo* r1 = s.first();
    RouteInfo* r2 = NULL;
    while( i < s.nb_routes() && !optimise ) { /// Pour chaque tournee 1
        r2 = r1->next_;
        j = i+1;
        while( j < s.nb_routes() && !optimise ) { /// Pour chaque tournee 2, ou r2 = s.first et tester si pas meme tournee
           /// std::cout << "Couple route " << r1->id << " ; " << r2->id <<std::endl;
           /// std::cout << "On va tester les clients de tournees differentes"  << std::endl;
            NodeInfo* c1 = r1->depot.next;
            while( c1->customer->id() != 0 && !optimise ) { /// Pour chaque client de la tournee 1, sauf le depot
                NodeInfo* c2 = r2->depot.next;
/******************************************************************************/
                ///std::cout << "Test ajout client " << c1->customer->id() << " (route " << r1->id << ") apres client " << c2->customer->id() << " (route " << r2->id << ")"  << std::endl;
                if(c2->next == c2->prev && c1->route->id != c2->route->id && s.data().is_valid(c1->customer->id(), c2->customer->id()) ) {  /// si pas la meme route et chemin existe
                    /// Alors on peut deplacer
                    Time incr = std::max(c1->arrival + s.data().distance(c1->customer->id(), c2->customer->id()), c2->customer->open()) + s.data().distance(c2->customer->id(), c1->next->customer->id()) - c1->next->arrival;
                    //cout << "l'incr vaut " << incr << endl;
                    if( c1->arrival + s.data().distance(c1->customer->id(), c2->customer->id()) < c2->customer->close() && /// si c2 est ok a cette place
                        s.is_feasible(*(c1->next), c2->customer->demand(), incr)) { /// et que la fin de r1 aussi

                        s.remove(*c2); // Enleve c1 de sa route, la close si necessaire
                        s.insert(*c1, *c2); // Ajoute c1 apres c2 dans sa route
                        //std::cout << "Ajout du client " << c2->customer->id() << " apres le client " << c1->customer->id() << " dans la route " << r1->id  << std::endl;

                        optimise = true;
                    }
                }
/******************************************************************************/
                c1 = c1->next;
            }
            j++;
            r2 = r2->next_;
        }
        i++;
        r1 = r1->next_;
    }
}

/// recherche locale complete
RechLocComplete::RechLocComplete() {
    rl[0] = new Opt2Etoile();
    rl[1] = new OrOptEtoile();
    rl[2] = new Opt2();
    rl[3] = new OrOpt();
    rl[4] = new Cross();
}

RechLocComplete::~RechLocComplete() {
    for(unsigned i = 0 ; i < 5 ; i++)
        delete rl[i];
}

void RechLocComplete::operator() (WorkingSolution & s) {
    unsigned    k = 0,
                oldNbRoutes = s.nb_routes();
    Time        oldDistance = s.total_distance();

    //cout << "coucou on a : " << oldNbRoutes << " routes et " << oldDistance << " km\n";
    while(k < 5) {
        rl[k++]->operator()(s);
        if(oldDistance > s.total_distance() || oldNbRoutes > s.nb_routes()) {
            k = 0;
            oldNbRoutes = s.nb_routes();
            oldDistance = s.total_distance();
    //cout << "coucou on a : " << oldNbRoutes << " routes et " << oldDistance << " km\n";
        }
    }
    //cout << "coucou on a : " << oldNbRoutes << " routes et " << oldDistance << " km\n";
}

void MetaHeuristique(int maxIter, WorkingSolution & s1, WorkingSolution & s2) {
// Base sur le multistart
    insertion_sorted(s1);
    s2 = s1;
    WorkingSolution actualSol = s1; // On conserve une copie de la meilleure actuelle
    WorkingSolution bestSolD = s1; // Meilleur solution en distance
    WorkingSolution bestSolN = s2; // Meilleur solution en nb de tournees

    Time actualDistance = actualSol.total_distance();

    actualSol.clear();

    for(int i = 0; i < maxIter; i++) { // On fait l'algo jusqu'a maxIter

        // Heuristique d'insertion, les clients sont en shuffle puis on applique la recherche locale
        insertion(actualSol);

        if(actualSol.total_distance() < bestSolD.total_distance() || bestSolD.total_distance() == 0) {
            // Alors j'ai mieux
            bestSolD = actualSol;
        }

        if(actualSol.nb_routes() < bestSolN.nb_routes() || bestSolN.total_distance() == 0) {
            bestSolN = actualSol;
        }

        actualSol.clear();
    }
    s1 = bestSolN;
    s2 = bestSolD;
    std::cout << "Meilleur solution en distance : " << bestSolD.distance() << std::endl;
    std::cout << "Meilleur solution en nombre de routes : " << bestSolN.nb_routes() << " routes " << std::endl;
    std::cout << "Ces solutions " << ((bestSolD.total_distance() == bestSolN.total_distance())?"sont egales":"ne sont pas egales") << std::endl;
}

/*****************************************************************/
/*****************************************************************/
