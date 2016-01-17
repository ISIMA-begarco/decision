#include "algorithms.h"

#include "workingsolution.h"
#include "CompareMiddleTW.h"

#include <algorithm>
#include <cassert>

#include <cstdlib>

using namespace std;

void dummy (WorkingSolution & sol) {
    sol.clear();

    for (auto & node: sol.nodes()) {
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
    //std::sort(clientsVector.begin(), clientsVector.end(), CompareMiddleTW());
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

/** \brief Recherche quel client inserer a la fin d'une tournee
 *
 * \param Vecteur contenant les clients tries par moyenne de fenetre de temps
 * \param Place a partir de laquelle rechercher le client
 * \param La tournee en cours
 * \param La solution courante
 * \return L'indice du client a inserer en fin de tournee, -1 si on en a pas
 *
 */

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
    //std::cout << "On commence a faire le OR-OPT" << std::endl;
    //Opt2Etoile()(s);
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
    RouteInfo * route = s.first();
    unsigned i = 0, j = 0 ;
    bool optimise = false;

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
            k = 2;
            oldNbRoutes = s.nb_routes();
            oldDistance = s.total_distance();
            max--;
        }
    }
    cout << "coucou on a : " << oldNbRoutes << " routes et " << oldDistance << " km\n";
}

void RechLocComplete::MetaHeuristique(int maxIter, WorkingSolution & s) {
// Base sur le multistart

    WorkingSolution actualSol = s; // On conserve une copie de la meilleure actuelle
    WorkingSolution bestSolD = s; // Meilleur solution en distance
    WorkingSolution bestSolN = s; // Meilleur solution en nb de tournees

    RechercheLocale loc;
    int maxLocale = 0;

    unsigned int oldDistance = std::numeric_limits<int>::max();
    Time actualDistance = s.total_distance();

    for(int i = 0; i < maxIter; i++) { // On fait l'algo jusqu'a maxIter
        std::cout << "Etape " << i << std::endl;
        oldDistance = std::numeric_limits<int>::max();

        // Heuristique d'insertion, les clients sont en shuffle
        insertion(s);
        actualDistance = s.total_distance();

        maxLocale = 0;
        // Tant qu'on ameliore la distance
        while(actualDistance < oldDistance && maxLocale < 500) {
            // Sauvegarde anciennes valeurs
            oldDistance = actualDistance;
            actualSol = s; // En sortant de la boucle c'est elle qu'on a

            // Recherche locale
            loc(s);

            //Mise a jour valeurs
            actualDistance = s.total_distance();

            maxLocale++;
        }

        if(actualSol.total_distance() < bestSolD.total_distance() || bestSolD.total_distance() == 0) {
            // Alors j'ai mieux
            bestSolD = actualSol;
        }

        if(actualSol.nb_routes() < bestSolN.nb_routes() || bestSolN.total_distance() == 0) {
            bestSolN = actualSol;
        }

        actualSol.clear();
    }

    std::cout << "Meilleur solution en distance : " << bestSolD.total_distance() << " kms" << std::endl;
    std::cout << "Meilleur solution en nombre de routes : " << bestSolN.nb_routes() << " routes " << std::endl;
    std::cout << "Ces solutions " << ((bestSolD.total_distance() == bestSolN.total_distance())?"sont egales":"ne sont pas egales") << std::endl;
}

/*****************************************************************/
/*****************************************************************/
