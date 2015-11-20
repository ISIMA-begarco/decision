#pragma once

#include "data.h"


// structure to keep the varying information on the nodes
struct RouteInfo;
struct NodeInfo
{
    Customer const * customer;
    Time        arrival; // arrival time at the node
    Load        load;    // load upon arrival
    RouteInfo * route;   // route tag
    NodeInfo  * prev;    // next node
    NodeInfo  * next;    // previous node
};

typedef std::vector<NodeInfo> Nvector;

std::ostream & operator<< (std::ostream &, const NodeInfo &);


// structure to keep the varying information on the route
struct RouteInfo
{
    Id          id;
    NodeInfo    depot;
    Time        distance;
    RouteInfo * prev_;
    RouteInfo * next_;
};

typedef std::vector<RouteInfo> Rvector;

std::ostream & operator<< (std::ostream &, const RouteInfo &);


// Solution to the VRPTW, used to allow fast modifications
class WorkingSolution
{
  public:
    static NodeInfo  * NO_NODE;
    static RouteInfo * NO_ROUTE;
    static const Time REDUCTION_BONUS;
    static const Time BAD_EVAL;
    enum OutputOption {EPS = 0, PDF, PNG, SVG};

  protected:
    const Data & data_;

    Nvector  nodes_;
    Nvector  depots_;
    Rvector  routes_;
    RouteInfo * first_;
    RouteInfo * last_;
    RouteInfo * free_;
    unsigned nb_routes_;
    Time     total_distance_;
    float    cpu_time_;

  public:
    WorkingSolution (const Data &);          // done

    WorkingSolution & operator= (const WorkingSolution &);

    void read (const std::string &);

    void clear ();                           // done
    bool check () const;                     // done
    RouteInfo & open_route ();               // done
    void close_route (RouteInfo &);          // done
    RouteInfo & open_specific_route (NodeInfo &);   // done
    void append (RouteInfo &, NodeInfo &);   // done
    void insert (NodeInfo &, NodeInfo &);    // done
    void remove (NodeInfo &);
    void do_merge (const Arc &);             // done

    bool is_feasible (NodeInfo &, const Load &, const Time &) const;
    void update      (NodeInfo &, const Load &, const Time &, RouteInfo *);
    void update2     (NodeInfo &);

    const Data &      data      () const {return data_;}
    const unsigned &  nb_routes () const {return nb_routes_;}
          unsigned &  nb_routes ()       {return nb_routes_;}
          double      distance  () const {return (double(total_distance_ - data_.services()) * 0.01);}
          Time & total_distance () {return total_distance_;}
    const float &     cpu_time  () const {return cpu_time_;}
          float &     cpu_time  ()       {return cpu_time_;}

          Nvector &   nodes     ()                 {return nodes_;}
    RouteInfo const * first     ()           const {return first_;}
    RouteInfo       * first     ()                 {return first_;}
    const NodeInfo *  first     (unsigned r_id) const {return &(depots_[r_id]);}
    const NodeInfo *  last      (unsigned r_id) const {return &(depots_[r_id]);}
};

bool operator< (const WorkingSolution &, const WorkingSolution &);
std::ostream & operator<< (std::ostream &, const WorkingSolution &);
