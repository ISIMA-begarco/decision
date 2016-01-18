#include <vector>
#include <list>
#include "workingsolution.h"

// algorithms on the solution
void dummy        (WorkingSolution &);
void insertion    (WorkingSolution &);
void insertion_sorted(WorkingSolution & sol);
void updateDistanceRoute(WorkingSolution & sol, RouteInfo & route);
std::list<NodeInfo>::iterator rechClientAInserer(const std::list<NodeInfo> & clients, std::list<NodeInfo>::iterator from, const RouteInfo ri, WorkingSolution & sol);


/*****************************************************************/
/**         Recherche locale                                    **/
/*****************************************************************/

/// recherche locale type cross
class RechercheLocale {
public:
    virtual void operator() (WorkingSolution & s) {};
};

/// recherche locale type cross
class Cross : public RechercheLocale {
public:
    void operator() (WorkingSolution & s);
};

/// recherche locale type 2 opt
class Opt2 : public RechercheLocale {
public:
    void operator() (WorkingSolution & s);
};

/// recherche locale type or opt
class OrOpt : public RechercheLocale {
public:
    void operator() (WorkingSolution & s);
};

/// cas particulier de la recherche locale type 2 opt
class Opt2Etoile : public RechercheLocale {
public:
    void operator() (WorkingSolution & s);
};

/// cas particulier de la recherche locale type or opt
class OrOptEtoile : public RechercheLocale {
public:
    void operator() (WorkingSolution & s);
};

/// recherche locale complete
class RechLocComplete {
protected:
    RechercheLocale * rl[5];

public:

    RechLocComplete();
    virtual ~RechLocComplete();
    void operator() (WorkingSolution & s);

};


void MetaHeuristique(int, WorkingSolution &, WorkingSolution &);
