#include "workingsolution.h"

#include <limits>
#include <algorithm>
#include <cassert>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>


std::ostream & operator<< (std::ostream & os, const NodeInfo & node) {
    os << node.customer->id();

    return os;
}


std::ostream & operator<< (std::ostream & os, const RouteInfo & route) {
    const NodeInfo * depotptr = &(route.depot), * nodeptr;
    os << "route " << route.id << ": [" << *depotptr;
    for (nodeptr = depotptr->next; nodeptr != depotptr; nodeptr = nodeptr->next) {
        os << ' ' << *nodeptr;
    }
    os << ' ' << *nodeptr << "] -> load = " << route.depot.load << " dist = " << route.distance;

    return os;
}


NodeInfo  * WorkingSolution::NO_NODE  = nullptr;
RouteInfo * WorkingSolution::NO_ROUTE = nullptr;
const Time WorkingSolution::REDUCTION_BONUS = -10000.0 * 100.0;
const Time WorkingSolution::BAD_EVAL = std::numeric_limits<Time>::max();


WorkingSolution::WorkingSolution (const Data & d): data_(d) {
    //std::cout << "Solution constructor" << std::endl;
    const unsigned nb_nodes = data_.nb_nodes();
    const unsigned nb_clients = data_.nb_clients();

    // build the vector of customers
    nodes_.resize(nb_nodes); // TODO: use nb_clients() instead, but carefully since id will be != index
    for (unsigned i = 0; i < nb_nodes; ++i) {
        nodes_[i].customer = &(data_.node(i));
    }

    // build the vector of depots
    depots_.resize(nb_clients);
    Customer const * depotptr = &(data_.node(data_.depot()));
    for (unsigned r = 0; r < nb_clients; ++r) {
        depots_[r].customer = depotptr;
    }

    // build the vector of routes
    routes_.resize(nb_clients);
    for (unsigned r = 0; r < nb_clients; ++r) {
        RouteInfo & route = routes_[r];
        route.id = r;
        route.depot.customer = depotptr;
    }

    // set the initial state
    clear();
}


WorkingSolution & WorkingSolution::operator= (const WorkingSolution & sol) {
    //std::cout << "copy operator on WorkingSolution" << std::endl;
    if (this != &sol) {
        // clear the solution
        clear();

        // scan the routes
        for (RouteInfo * solrptr = sol.first_; solrptr != nullptr; solrptr = solrptr->next_) {
            //std::cout << "copying route :" << *solrptr << std::endl;
            RouteInfo & route = open_route();
            //std::cout << "  got " << route.id << std::endl;

            // scan the nodes
            NodeInfo * last = &(route.depot);
            NodeInfo * nodeptr = nullptr;
            for (NodeInfo * solnptr = solrptr->depot.next; solnptr != &(solrptr->depot); last = nodeptr, solnptr = solnptr->next) {
                // get the node
                //std::cout << "  node id = " << solnptr->customer->id() << std::endl;
                nodeptr = &(nodes_[solnptr->customer->id()]);
                //std::cout << "    got " << nodeptr->customer->id() << std::endl;

                // connect the node
                nodeptr->prev = last;
                last->next = nodeptr;

                // update the node information
                nodeptr->arrival = solnptr->arrival;
                nodeptr->load = solnptr->load;
                nodeptr->route = &route;
            }

            // last connection
            route.depot.prev = last;
            last->next = &(route.depot);

            // depot information
            route.depot.arrival = solrptr->depot.arrival;
            route.depot.load = solrptr->depot.load;
            route.depot.route = &route;

            // update the route information
            route.distance = solrptr->distance;
        }

        // update the solution information
        nb_routes_      = sol.nb_routes_;
        total_distance_ = sol.total_distance_;
        cpu_time_       = sol.cpu_time_;
    }

    return *this;
}


void WorkingSolution::read(const std::string & filename) {
    // open the file
    std::ifstream is(filename.c_str());
    if (!is.is_open()) {
        std::cerr << "Error: unable to open file \'" << filename << "\'" << std::endl;
        exit (EXIT_FAILURE);
    }

    // clear the solution
    std::cout << "clear the solution" << std::endl;
    clear();

    // read the number of routes
    unsigned nb_routes = 0;
    std::string str;
    is >> nb_routes >> str >> str;

    // read each route
    for (unsigned r = 0; r < nb_routes; ++r) {
        Load load;
        is >> str >> str >> str >> load;
        //std::cout << "read load = " << load << std::endl;
        char ch;
        do {
            ch = is.peek();
            is.ignore();
        } while (ch != '[');
        RouteInfo * routeptr = nullptr;
        do {
            Id id;
            is >> id >> ch;
            //std::cout << "read node id " << id << std::endl;
            //std::cout << "associated node info = " << nodes_[id] << std::endl;
            if (routeptr == nullptr) {
                routeptr = &(open_specific_route(nodes_[id]));
            } else {
                append(*routeptr, nodes_[id]);
            }
        } while (ch != ']');
        assert((load == routeptr->depot.prev->load) && "error: bad route capacity");
    }

    // read the written distance
    Time dist;
    is >> str >> str >> str >> dist;

    // close the file
    is.close();

    // check the solution
    check();

    // compare with what's computed
    std::cout << "read dist = " << dist << "   computed dist = " << distance()
              << "   computed total distance = " << total_distance_
              << "   same (double) = " << (double(total_distance_ - data_.services()) * 0.01) << std::endl;
    //assert((fabs(dist - total_distance_) <= 1.0e-6) && "error: bad total distance");
}


void WorkingSolution::clear () {
    // clear the information on the clients
    for (auto & info: nodes_) {
        info.arrival = NO_TIME;
        info.load = NO_LOAD;
        info.route = NO_ROUTE;
        info.prev = NO_NODE;
        info.next = NO_NODE;
    }
    // clear the information on the depots
    for (auto & depot: depots_) {
        depot.arrival = NO_TIME;
        depot.load = NO_LOAD;
        depot.route = NO_ROUTE;
        depot.prev = NO_NODE;
        depot.next = NO_NODE;
    }
    // clear the information on the routes
    // TODO: only reset for the routes that are used?
    for (auto & route: routes_) {
        NodeInfo & depot = route.depot;
        depot.arrival = NO_TIME;
        depot.load = NO_LOAD;
        depot.route = &route;
        depot.prev = NO_NODE;
        depot.next = NO_NODE;
    }
    // set the next/prev connections on the routes
    first_ = last_ = NO_ROUTE;
    Rvector::iterator prev = routes_.end();
    for (Rvector::iterator curr = routes_.begin(); curr != routes_.end(); prev = curr, ++curr) {
        if (curr == routes_.begin()) {
            curr->prev_ = NO_ROUTE;
            free_ = &(*curr);
        } else {
            curr->prev_ = &(*prev);
            prev->next_ = &(*curr);
        }
    }
    prev->next_ = NO_ROUTE;

    // reset the basic evaluations
    nb_routes_ = 0;
    total_distance_ = 0.0;
    cpu_time_ = 0.0;
}


// check if the solution is defined the right way
// TODO: check the route ptr for each NodeInfo
bool WorkingSolution::check () const {
    // basic assertions
    assert((nodes_.size() == data_.nb_nodes()) && "wrong size of nodes vector");
    assert((depots_.size() == data_.nb_clients()) && "wrong size of depots vector");
    assert((routes_.size() == data_.nb_clients()) && "wrong size of routes vector");

    // initializations
    std::vector<bool> client_visited(data_.nb_nodes(), false);
    std::vector<bool> route_visited(data_.nb_clients(), false);
    unsigned cpt_clients = 0;
    unsigned cpt_routes = 0;
    Time cpt_distance = 0.0;

    // check the used route list
    RouteInfo * routeptr = first_;
    while (routeptr != NO_ROUTE) {
        assert((routeptr != NO_ROUTE) && "nullptr for route");

        // check the next/prev pointers in the route list
        if (routeptr != first_) {
            assert((routeptr->prev_->next_ == routeptr) && "wrong connection prev_->next_ on a route");
        } else {
            assert((routeptr->prev_ == NO_ROUTE) && "wrong prev_ for first route");
        }

        // check the unicity
        Id id = routeptr->id;
        assert((id < data_.nb_clients()) && "wrong route id");
        assert((route_visited[id] == false) && "route already visited");
        route_visited[id] = true;

        // check the route
        NodeInfo * depotptr = &(routeptr->depot);
        NodeInfo * nodeptr = depotptr;
        assert((nodeptr->next != NO_NODE) && "prev == nullptr for a depot");
        assert((nodeptr->next != depotptr) && "empty route");
        Load load = NO_LOAD;
        Time arrival = depotptr->customer->open();
        Time distance;
        Time cpt_local_distance = NO_TIME;
        nodeptr = nodeptr->next;
        while (nodeptr != depotptr) {
            assert((nodeptr != nullptr) && "null pointer to node");
            id = nodeptr->customer->id();
            //std::cout << "client id = " << id << std::endl;
            assert((id <= data_.nb_clients()) && (id != data_.depot()) && "wrong client id");
            assert((client_visited[id] == false) && "client already visited");
            client_visited[id] = true;

            assert((nodeptr->prev->next == nodeptr) && "wrong prev / next node connection");

            load += nodeptr->customer->demand();
            if (load != nodeptr->load) {
                std::cout << "error at node " << nodeptr->customer->id() << " load = " << nodeptr->load << "  computed = " << load << std::endl;
            }
            assert((load == nodeptr->load) && "wrong load at the client");

            assert((nodeptr->route == routeptr) && "wrong route pointer");

            distance = data_.distance(nodeptr->prev->customer->id(), nodeptr->customer->id());
            arrival = std::max(arrival, nodeptr->prev->customer->open()) + distance;
            if (arrival != nodeptr->arrival) {
                std::cout << "wrong arrival at node " << nodeptr->customer->id() << " arrival = " << nodeptr->arrival << "   computed = " << arrival << std::endl;
            }
            assert((arrival == nodeptr->arrival) && "wrong arrival time at node");
            if (arrival > nodeptr->customer->close()) {
                std::cout << "node " << *nodeptr << " computed arrival = " << arrival << " after closing = " << nodeptr->customer->close() << "   (arrival = " << nodeptr->arrival << ")" << std::endl;
            }
            assert((arrival <= nodeptr->customer->close()) && "arriving after closing time at node");

            // update the evaluation
            cpt_local_distance += distance;
            ++cpt_clients;

            // iterate
            nodeptr = nodeptr->next;
        }
        assert((nodeptr->prev->next == nodeptr) && "wrong prev / next node last connection");
        distance = data_.distance(nodeptr->prev->customer->id(), nodeptr->customer->id());
        arrival = std::max(arrival, nodeptr->prev->customer->open()) + distance;
        assert((arrival == nodeptr->arrival) && "wrong arrival time at depot");
        assert((arrival <= nodeptr->customer->close()) && "arriving after closing time at depot");
        cpt_local_distance += distance;
//    assert(cpt_local_distance == routeptr->distance) && "wrong local distance");
        assert((load == nodeptr->load) && "wrong total route load");
        if (load > data_.fleetCapacity()) {
            std::cout << "route = " << *routeptr << std::endl;
        }
        assert((load <= data_.fleetCapacity()) && "vehicle capacity exceeded");

        // update the evaluations
        ++cpt_routes;
        cpt_distance += cpt_local_distance;

        // iterate
        routeptr = routeptr->next_;
    }
    //assert((routeptr->next_ == NO_ROUTE) && "wrong connection next for the last route");

    // check the evaluations
    assert((cpt_routes == nb_routes_) && "wrong number of routes");
    assert((cpt_clients == data_.nb_clients()) && "some unreferenced clients");
//  std::cout << "total_distance_ = " << total_distance_ << " computed = " << cpt_distance << std::endl;
    assert((cpt_distance == total_distance_) && "wrong total distance");

    // check the free route list
    routeptr = free_;
    while (routeptr != NO_ROUTE) {
        // check the unicity
        Id id = routeptr->id;
        assert((id < data_.nb_clients()) && "wrong route id");
        assert((route_visited[id] == false) && "route already visited");
        route_visited[id] = true;

        // update the evaluation
        ++cpt_routes;

        // iterate
        routeptr = routeptr->next_;
    }

    // check the evaluations
    assert((cpt_routes == data_.nb_clients()) && "some unreferenced routes");

    return true;
}


RouteInfo & WorkingSolution::open_route () {
    assert((free_ != NO_ROUTE) && "no more route available");

    // get a free route
    RouteInfo * routeptr = free_;
    free_ = free_->next_;
    if (free_ != nullptr) {
        free_->prev_ = NO_ROUTE;
    }

    // append it to the set of routes
    routeptr->prev_ = last_;
    routeptr->next_ = NO_ROUTE;
    if (first_ == NO_ROUTE) {
        first_ = routeptr;
    } else {
        last_->next_ = routeptr;
    }
    last_ = routeptr;
    ++nb_routes_;

    // set it empty
    NodeInfo & depot = routeptr->depot;
    depot.prev = depot.next = &depot;
    depot.load = NO_LOAD;
    depot.arrival = depot.customer->open();
    routeptr->distance = 0.0;

    return *routeptr;
}


RouteInfo & WorkingSolution::open_specific_route (NodeInfo & node) {
    assert((free_ != NO_ROUTE) && "no more route available");

    // get a free route
    RouteInfo * routeptr = free_;
    free_ = free_->next_;
    if (free_ != NO_ROUTE) {
        free_->prev_ = NO_ROUTE;
    }

    // append it to the set of routes
    routeptr->prev_ = last_;
    routeptr->next_ = NO_ROUTE;
    if (first_ == NO_ROUTE) {
        first_ = routeptr;
    } else {
        last_->next_ = routeptr;
    }
    last_ = routeptr;
    ++nb_routes_;

    // fill it with the node
    NodeInfo & depot = routeptr->depot;
    depot.prev = depot.next = &node;
    node.prev = node.next = &depot;
    node.route = routeptr;
    node.load = depot.load = node.customer->demand();
    Id id = node.customer->id();
    Time dist1 = data_.distance(data_.depot(),id);
    Time time = dist1;
    node.arrival = time;
    if (time < node.customer->open()) {
        time = node.customer->open();
    }
    Time dist2 = data_.distance(id,data_.depot());
    depot.arrival = time + dist2;
    routeptr->distance = dist1 + dist2;
    total_distance_ += routeptr->distance;

    return *routeptr;
}


bool WorkingSolution::is_feasible (NodeInfo & node, const Load & incr_capa, const Time & incr_time) const {
    // capacity check: O(1)
    if (node.route->depot.load + incr_capa > data_.fleetCapacity())
        return false;

    // time window check: O(k)
    // TODO: reduce to O(1) check
    if (incr_time <= 0)
        return true;

    NodeInfo * nodeptr = &node;
    Time time = nodeptr->arrival + incr_time;
    do {
        if (time < nodeptr->customer->open())
            return true;
        if (time > nodeptr->customer->close())
            return false;
        if (nodeptr->customer->id() == data_.depot())
            break;
        time = std::max(time, nodeptr->customer->open()) + data_.distance(nodeptr->customer->id(), nodeptr->next->customer->id());
        nodeptr = nodeptr->next;
    } while (true);

    return true;
}


// update function, starts from the first node to be updated (load & arrival to update)
void WorkingSolution::update (NodeInfo & node, const Load & incr_load, const Time & incr_time, RouteInfo * routeptr) {
    NodeInfo * nodeptr = &node;
    Time time = nodeptr->arrival + incr_time;
    do {
        nodeptr->load += incr_load;
        nodeptr->arrival = time;
        nodeptr->route = routeptr;
        time = std::max(time, nodeptr->customer->open()) + data_.distance(nodeptr->customer->id(), nodeptr->next->customer->id());
        nodeptr = nodeptr->next;
    } while (nodeptr != &(routeptr->depot));
    std::cout << "update function: before = " << nodeptr->load << " incr = " << incr_load << " -> now = " << (nodeptr->load + incr_load) << std::endl;
    nodeptr->load += incr_load;
    nodeptr->arrival = time;
}


// same update function, but starts from the last right node
void WorkingSolution::update2 (NodeInfo & node) {
    RouteInfo * routeptr = node.route;
    NodeInfo * nodeptr = &node;
    NodeInfo * depotptr = &(routeptr->depot);

    // safety check: update from the initial depot
    if (&node == depotptr) {
        node.arrival = NO_TIME;
        node.load = NO_LOAD;
    }

    // propagate the information: arrival, load
    Time time = nodeptr->arrival;
    Load load = nodeptr->load;
    do {
        time = std::max(time, nodeptr->customer->open()) + data_.distance(nodeptr->customer->id(), nodeptr->next->customer->id());
        nodeptr = nodeptr->next;
        nodeptr->arrival = time;
        load += nodeptr->customer->demand();
        nodeptr->load = load;
        nodeptr->route = routeptr;
    } while (nodeptr != depotptr);
}


void WorkingSolution::do_merge (const Arc & arc) {
    // no safety checks, supposed already done
    // perform the connections
    NodeInfo & orig = nodes_[arc.orig()];
    NodeInfo & dest = nodes_[arc.dest()];
    NodeInfo * odepotptr = orig.next;
    NodeInfo * ddepotptr = dest.prev;
    orig.next = &dest;
    dest.prev = &orig;
    odepotptr->prev = ddepotptr->prev;
    odepotptr->prev->next = odepotptr;
    RouteInfo * routeptr = odepotptr->route;
    routeptr->distance += ddepotptr->route->distance + arc.saving();
    total_distance_ += arc.saving();

    //update(dest, orig.load, dest.arrival - std::max(orig.arrival, orig.customer->open()) + data_.distance(orig.customer->id(), dest.customer->id()), orig.route);
    update2(orig);

    // tour to be removed
    ddepotptr->prev = ddepotptr->next = ddepotptr;
    close_route(*(ddepotptr->route));
}


void WorkingSolution::close_route (RouteInfo & route) {
    NodeInfo * depotptr = &(route.depot);
    assert((depotptr->prev == depotptr) && (depotptr->next == depotptr) && "non-empty route");

    if (route.prev_ == NO_ROUTE) {
        first_ = route.next_;
    } else {
        route.prev_->next_ = route.next_;
    }
    if (route.next_ == NO_ROUTE) {
        last_ = route.prev_;
    } else {
        route.next_->prev_ = route.prev_;
    }
    route.prev_ = NO_ROUTE;
    route.next_ = free_;
    free_ = &route;

    --nb_routes_;
}


void WorkingSolution::append (RouteInfo & route, NodeInfo & node) {
    NodeInfo & depot = route.depot;
    NodeInfo * last = depot.prev;
    const Id id = node.customer->id();
    assert((id != data_.depot()) && "inserting the depo");
    assert ((last != &depot) && "empty route");

    node.prev = last;
    last->next = &node;
    node.next = &depot;
    depot.prev = &node;

    Time dist_del = data_.distance(last->customer->id(), data_.depot());
    Time dist_add1 = data_.distance(last->customer->id(), id);
    Time dist_add2 = data_.distance(id, data_.depot());
    Time time = std::max(last->arrival, last->customer->open()) + dist_add1;
    node.arrival = time;
    node.load = last->load + node.customer->demand();
    node.route = &route;
    time = std::max(time, node.customer->open()) + dist_add2;
    depot.arrival = time;
    depot.load = node.load;

    time = dist_add1 + dist_add2 - dist_del;
    route.distance += time;
    total_distance_ += time;
}


// TODO: only insert a node, does not remove it from its route
void WorkingSolution::insert (NodeInfo & prev, NodeInfo & node) {
    const Id id = node.customer->id();
    assert((id != data_.depot()) && "inserting the depot");

    node.prev = &prev;
    node.next = prev.next;
    prev.next = &node;
    node.next->prev = &node;

    node.route = prev.route;
    update2(prev);

    Time delta_dist = data_.distance(prev.customer->id(), id) + data_.distance(id, node.next->customer->id()) - data_.distance(prev.customer->id(), node.next->customer->id());
    node.route->distance += delta_dist;
    total_distance_ += delta_dist;
}


void WorkingSolution::remove (NodeInfo & node) {
    //std::cout << "entering remove" << std::endl;
    const Id id = node.customer->id();
    assert((id != data_.depot()) && "removing the depot");
    //std::cout << "initialization done" << std::endl;

    // disconnect the node from its neighbors
    std::cout << "removing node " << node << std::endl;
    //std::cout << *(node.route) << std::endl;
    node.prev->next = node.next;
    node.next->prev = node.prev;
    //std::cout << "reconnection done" << std::endl;

    // update the distance
    Time delta_dist = data_.distance(node.prev->customer->id(), node.next->customer->id()) - data_.distance(node.prev->customer->id(), id) - data_.distance(id, node.next->customer->id());
    node.route->distance += delta_dist;
    total_distance_ += delta_dist;
    //std::cout << "distances updated" << std::endl;

    // update the route information
    if (node.prev != node.next) {
        /*
        if (node.prev == &(node.route->depot))
        {
          node.prev->arrival = NO_TIME;
          node.prev->load = NO_LOAD;
        }
        */
        update2(*(node.prev));
        //std::cout << "information updated" << std::endl;
    } else {
        //std::cout << "route to close" << std::endl;
        close_route(*(node.route));
        //std::cout << "route closed" << std::endl;
    }

    // reset the node information
    node.arrival = NO_TIME;
    node.load = NO_LOAD;
    node.route = NO_ROUTE;
    node.prev = NO_NODE;
    node.next = NO_NODE;
}


bool operator< (const WorkingSolution & s1, const WorkingSolution & s2) {
    return ((s1.nb_routes() < s2.nb_routes()) || ((s1.nb_routes() == s2.nb_routes()) && (s1.distance() < s2.distance())));
}


std::ostream & operator<< (std::ostream & os, const WorkingSolution & sol) {
    os << "solution: " << sol.nb_routes() << " routes, total distance " << sol.distance() << " cpu = " << sol.cpu_time() << " s\n";
//  for (RouteInfo const * routeptr = sol.first(); routeptr != WorkingSolution::NO_ROUTE; routeptr = routeptr->next_)
//  {
//    os << *routeptr << "\n";
//  }

    return os;
}
