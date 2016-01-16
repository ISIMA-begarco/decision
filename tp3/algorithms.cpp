#include "algorithms.h"

#include "workingsolution.h"
#include "CompareMiddleTW.h"

#include <algorithm>
#include <cassert>

using namespace std;

void dummy (WorkingSolution & sol) {
    std::cout << "Debut dummy" << std::endl;
    sol.clear();


    for (auto & node: sol.nodes())   {
        if (node.customer->id() != sol.data().depot()) {
            sol.open_specific_route(node);
        }
    }

  std::cout << "Fin dummy" << std::endl;

for(int i = 0; i < 400; i++) {
std::cout << "Recherche locale a faire" <<std::endl;
RechercheLocale * oropt =  new OrOpt();
oropt->operator()(sol);
std::cout << "Fin recherche locale" << std::endl;

for(RouteInfo* r1 = sol.first(); r1 != NULL; r1 = r1->next_) {
std::cout << "#" << r1->id << " load : " << r1->depot.load <<std::endl;
}
}

}

void insertion (WorkingSolution & sol) {
    std::cout << "Debut insertion" << std::endl;
    vector<NodeInfo> clientsVector;
    sol.clear();

    // recupere les clients dans le vecteur
    for (auto & node : sol.nodes()) {
        if (node.customer->id() != sol.data().depot())
            clientsVector.push_back(node);
    }
    // Trie les clients par leur moyenne de fenetre de temps
    std::sort(clientsVector.begin(), clientsVector.end(), CompareMiddleTW());
    std::random_shuffle(clientsVector.begin(), clientsVector.end());

    list<NodeInfo> clients(clientsVector.begin(), clientsVector.end());

    while(!(clients.empty())) { // boucle principale
        RouteInfo & ri = sol.open_specific_route(clients.front());      // on cree une nouvelle route avec le premier client
        clients.pop_front();
        auto toInsert = clients.begin();
        while((toInsert=rechClientAInserer(clients, toInsert, ri, sol)) != clients.end()) { // on recherche si des clients peuvent etre inseres
            sol.insert(*(ri.depot.prev), *toInsert);     // on insere
            toInsert = clients.erase(toInsert);         // on enleve des clients non desservis
        }
    }

std::cout << "Fin insertion" << std::endl;
while (true) {
std::cout << "Recherche locale a faire" <<std::endl;
RechercheLocale * oropt =  new OrOpt();
oropt->operator()(sol);
std::cout << "Fin recherche locale" << std::endl;
/*
for(RouteInfo* r1 = sol.first(); r1 != NULL; r1 = r1->next_) {
std::cout << "#" << r1->id << " load : " << r1->depot.load <<std::endl;
}*/
}
//    RechLocComplete loc;
//    loc(sol);
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
std::cout << i->customer->id() << std::endl;
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

/**
    heuristique insertion
    heuristique d�bile
    heuristique fusion
**/


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
                            Time incrTime = client2->arrival + s.data().distance(client2->customer->id(),client->next->customer->id()) - client2->arrival,
                                 incrTime2 = client->arrival + s.data().distance(client->customer->id(),client2->next->customer->id()) - client->arrival;

                            if( s.is_feasible(*(client->next), -incrCapa, incrTime) &&
                                s.is_feasible(*(client2->next), incrCapa, incrTime2)
                              ) {       /// l'echange est-il possible ????
                                cout << "echange possible" << endl;
                                cout << "client/load/demand " << client->customer->id()<< "/" << client->load << "/" << client->customer->demand() << endl;
                                cout << "client/load/demand " << client->next->customer->id()<< "/" << client->next->load << "/" << client->next->customer->demand() << endl;
                                cout << "client/load/demand " << client2->customer->id()<< "/" << client2->load << "/" << client2->customer->demand() << endl;
                                cout << "client/load/demand " << client2->next->customer->id()<< "/" << client2->next->load << "/" << client2->next->customer->demand() << endl;
                                /// on fait l'echange
                                NodeInfo * tmp = client->next;
                                client->next = client2->next;
                                client2->next = tmp;
                                client->next->prev = client2;
                                client2->next->prev = client;

                                /// chainage des bons depots d'arrivee
                          /*      tmp = route->depot.prev;
                                route->depot.prev = &(route2->depot);
                                route2->depot.prev = tmp;
                                route->depot.prev->next = route->depot.customer->;
                                route2->depot.prev->next = route2->depot.next->prev;*/
                                //s.update2(*client);
                                //s.update2(*client2->next);

                                optimise = true;
                                cout << "echange effectue" << endl;
                                cout << "client/load/demand " << client->customer->id()<< "/" << client->load << "/" << client->customer->demand() << endl;
                                cout << "client/load/demand " << client->next->customer->id()<< "/" << client->next->load << "/" << client->next->customer->demand() << endl;
                                cout << "client/load/demand " << client2->customer->id()<< "/" << client2->load << "/" << client2->customer->demand() << endl;
                                cout << "client/load/demand " << client2->next->customer->id()<< "/" << client2->next->load << "/" << client2->next->customer->demand() << endl;
                            }
                        }

                        client2 = client2->next;
                    }
                }
                j++;
            }

            client = client->next;
        }
        route = route->next_;
        i++;
    }
}


/** recherche locale type or opt
 * \brief Extrait un sommet d'une tournee et va tenter de l'inserer dans une autre
 *
 * \param La solution dans la quelle executer la recherche locale
*/
void OrOpt::operator() (WorkingSolution & s) {
///    std::cout << "On commence a faire le OR-OPT" << std::endl;

    for(RouteInfo* r1 = s.first(); r1 != NULL; r1 = r1->next_) { // Pour chaque tournee 1
        for(RouteInfo* r2 = r1->next_; r2 != NULL; r2 = r2->next_) { // Pour chaque tournee 2, ou r2 = s.first et tester si pas meme tournee
///            std::cout << "Couple route " << r1->id << " ; " << r2->id <<std::endl;
///            std::cout << "On va tester les clients de tournees differentes"  << std::endl;
            for(NodeInfo* c1 = r1->depot.next; c1->customer->id() != 0; c1 = c1->next) { // Pour chaque client de la tournee 1, sauf le depot
                for(NodeInfo* c2 = r2->depot.next; c2->customer->id() != 0; c2 = c2->next) { // Pour chaque client de la tournee 2
///                    std::cout << "Test ajout client " << c1->customer->id() << " (route " << r1->id << ") apres client " << c2->customer->id() << " (route " << r2->id << ")"  << std::endl;
                    if(c1->route->id != c2->route->id && s.data().is_valid(c1->customer->id(), c2->customer->id())) { // On est pas dans la meme tournee et le chemin existe
                        // Alors on peut deplacer
                        Time incr = s.data().distance(c2->customer->id(), c1->customer->id()) + s.data().distance(c1->customer->id(), c2->next->customer->id()) - s.data().distance(c2->customer->id(), c2->next->customer->id());
                        if(s.is_feasible(*c2, c1->customer->demand(), incr)) {
                            s.remove(*c1); // Enleve c1 de sa route, la close si necessaire
                            s.insert(*c2, *c1); // Ajoute c1 apres c2 dans sa route
///                            std::cout << "Ajout du client " << c1->customer->id() << " apres le client " << c2->customer->id() << " dans la route " << r2->id  << std::endl;
                        }
                    }
                }
            }
        }
    }
///    std::cout << "On sort du OR-OPT" << std::endl;
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
    unsigned    k = 0,
                oldNbRoutes = s.nb_routes();
    Time        oldDistance = s.total_distance();

    cout << "coucou on a : " << oldNbRoutes << " routes et " << oldDistance << " km\n";
    while(k < 5) {
        rl[k++]->operator()(s);
        if(oldDistance > s.total_distance() || oldNbRoutes > s.nb_routes()) {
            k = 0;
            oldNbRoutes = s.nb_routes();
            oldDistance = s.total_distance();
        }
    }
    cout << "coucou on a : " << oldNbRoutes << " routes et " << oldDistance << " km\n";
}



/*****************************************************************/
/*****************************************************************/
