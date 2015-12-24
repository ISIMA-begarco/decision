#include <vector>
#include <list>
#include "workingsolution.h"


// algorithms on the solution
void dummy        (WorkingSolution &);
std::list<NodeInfo>::iterator rechClientAInserer(const std::list<NodeInfo> & clients, std::list<NodeInfo>::iterator from, const RouteInfo ri, WorkingSolution & sol);
