#pragma once

// headers
#include <iostream>
#include <vector>
#include <list>


// basic data types
typedef unsigned Id;
typedef float    Location;
typedef float    Distance;


// forward declaration for Node
class Arc;


// Node class: id, location, adjacency lists & methods
class Node
{
  public:
    static const Id NO_NODE;

  protected:
    Id       id_;
    Location x_;
    Location y_;

    Arc * first_in_;
    Arc * first_out_;
    unsigned nb_in_;
    unsigned nb_out_;

    friend class Graph;

  public:
    Node (const Id &, const Location &, const Location &);

    const Id       & id      () const {return id_;}
    const Location & x       () const {return x_;}
    const Location & y       () const {return y_;}

    Arc * first_in  () const {return first_in_;}
    Arc * first_out () const {return first_out_;}
    const unsigned & nb_in  () const {return nb_in_;}
    const unsigned & nb_out () const {return nb_out_;}
};
typedef std::vector<Node> Nvector;

std::ostream & operator<< (std::ostream &, const Node &);


// Arc class: id, orig, dest, cost, adjacency chains & methods
class Arc
{
  public:
    static const Id NO_ARC;

  protected:
    Id       id_;
    Id       orig_;
    Id       dest_;
    Distance cost_;

    unsigned pos_orig_;
    unsigned pos_dest_;
    Arc * next_in_;
    Arc * next_out_;

    static const unsigned NO_POS;
    friend class Graph;

  public:
    Arc (const Id & id, const Id & i, const Id & j, const Distance & c): id_(id), orig_(i), dest_(j), cost_(c), pos_orig_(Arc::NO_POS), pos_dest_(Arc::NO_POS), next_in_(0), next_out_(0) {}

    bool operator< (const Arc & a) const {return cost_ < a.cost_;}

    const Id &       orig () const {return orig_;}
    const Id &       dest () const {return dest_;}
    const Distance & cost () const {return cost_;}

    const Id & id() const {return id_;}
    const unsigned & pos_orig() const {return pos_orig_;}
    const unsigned & pos_dest() const {return pos_dest_;}
    Arc * next_in  () const {return next_in_;}
    Arc * next_out () const {return next_out_;}
};
typedef std::list<Arc> Alist;
typedef std::vector<Arc> Avector;

std::ostream & operator<< (std::ostream &, const Arc &);


// Graph class: lists of nodes and arcs & methods
class Graph
{
  protected:
    std::string name_;
    Nvector  nodes_;
    Avector  arcs_;

    void build_arcs ();
    void build_adjacency ();

  public:
    Graph (const std::string &);

    const std::string & name () const {return name_;}

    unsigned   nb_nodes      () const {return nodes_.size();}
    unsigned   nb_arcs       () const {return arcs_.size();}
    const Nvector  & nodes         () const {return nodes_;}
    const Node     & node          (const Id & i) const {return nodes_[i];}
    const Avector &  arcs          () const {return arcs_;}
};

std::ostream & operator<< (std::ostream &, const Graph &);
