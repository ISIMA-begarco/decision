#include "graph.h"

#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cassert>
#include <map>


const Id Node::NO_NODE = std::numeric_limits<Id>::max();


Node::Node (const Id & i, const Location & x, const Location & y): id_(i), x_(x), y_(y), first_in_(0), first_out_(0), nb_in_(0), nb_out_(0)
{
}


std::ostream & operator<< (std::ostream & os, const Node & n)
{
  os << n.id() << ": (" << n.x() << ',' << n.y() << ")";
  os << " " << n.nb_in() << " in [";
  for (Arc * aptr = n.first_in(); aptr != 0; aptr = aptr->next_in())
  {
    os << " " << aptr->id();
  }
  os << " ] " << n.nb_out() << " out [";
  for (Arc * aptr = n.first_out(); aptr != 0; aptr = aptr->next_out())
  {
    os << " " << aptr->id();
  }
  os << " ]";

  return os;
}


const Id Arc::NO_ARC = std::numeric_limits<Id>::max();
const unsigned Arc::NO_POS = std::numeric_limits<unsigned>::max();


std::ostream & operator<< (std::ostream & os, const Arc & a)
{
  os << a.id() << ": (" << a.orig() << ',' << a.dest() << ") " << a.cost();

  return os;
}


void Graph::build_adjacency()
{
  std::map<Id,unsigned> corresp;
  unsigned pos = 0;
  for (Nvector::const_iterator it = nodes_.begin(); it != nodes_.end(); ++it, ++pos)
  {
    corresp.insert(std::pair<Id,unsigned>(it->id_, pos));
  }

  std::vector<Arc*> last_out(nodes_.size(), 0);
  std::vector<Arc*> last_in(nodes_.size(), 0);
  for (Avector::iterator it = arcs_.begin(); it != arcs_.end(); ++it)
  {
    // get the arc and the two nodes positions
    Arc * aptr = &(*it);
    std::map<Id,unsigned>::iterator it2;
    it2 = corresp.find(it->orig_);
    assert((it2 != corresp.end()) && "unknown orig Id");
    unsigned pos_orig = aptr->pos_orig_ = it2->second;
    it2 = corresp.find(it->dest_);
    assert((it2 != corresp.end()) && "unknown dest Id");
    unsigned pos_dest = aptr->pos_dest_ = it2->second;

    // update the out list (orig)
    if (last_out[pos_orig] == 0)
    {
      nodes_[pos_orig].first_out_ = aptr;
    }
    else
    {
      last_out[pos_orig]->next_out_ = aptr;
    }
    ++nodes_[pos_orig].nb_out_;
    last_out[pos_orig] = aptr;

    // update the in list (dest)
    if (last_in[pos_dest] == 0)
    {
      nodes_[pos_dest].first_in_ = aptr;
    }
    else
    {
      last_in[pos_dest]->next_in_ = aptr;
    }
    ++nodes_[pos_dest].nb_in_;
    last_in[pos_dest] = aptr;
  }
}


Graph::Graph(const std::string & filename)
{
  // open the instance
  std::ifstream is(filename.c_str());
  if (!is)
  {
    std::cerr << "Error: unable to open file \'" << filename << "\'" << std::endl;
    exit (EXIT_FAILURE);
  }

  // get the instance name
  std::string str;
  is >> str >> str >> name_;
  std::cout << "reading name = " << name_ << std::endl;

  // skip the number of nodes
  unsigned nb_nodes;
  is >> str >> str >> nb_nodes;
  std::cout << "reading nb_nodes = " << nb_nodes << std::endl;
  
  // skip the number of arcs
  unsigned nb_arcs;
  is >> str >> str >> nb_arcs;
  std::cout << "reading nb_arcs = " << nb_arcs << std::endl;

  // skip the nodes header
  is >> str;
  std::getline(is, str);
  std::cout << "reading last header = " << str << std::endl;

  // read the nodes
  for (unsigned i = 0; i < nb_nodes; ++i)
  {
    std::getline(is, str);
    std::stringstream ss(str);
    std::string str2;
    Id id;
    Location x, y;
    ss >> str2 >> str2 >> id >> str2 >> str2 >> str2 >> x >> str2 >> str2 >> str2 >> y;

    std::cout << "reading node line = " << str << std::endl;

    nodes_.push_back(Node(id,x,y));
  }

  // skip the arcs header
  is >> str;
  std::getline(is, str);

  // read the arcs
  for (unsigned j = 0; j < nb_arcs; ++j)
  {
    std::getline(is, str);
    std::stringstream ss(str);
    std::string str2;

    Id id, orig, dest;
    Distance cost;
    ss >> str2 >> str2 >> id >> str2 >> str2 >> str2 >> orig >> str2 >> str2 >> str2 >> dest >> str2 >> str2 >> str2 >> cost;

    std::cout << "reading arc line = " << str << std::endl;

    arcs_.push_back(Arc(id, orig, dest, cost));
  }

  // sort the nodes
  std::sort(arcs_.begin(), arcs_.end());

  // build the adjacency lists
  build_adjacency();
}


std::ostream & operator<< (std::ostream & os, const Graph & d)
{
  os << "instance " << d.name() << ": " << d.nb_nodes() << " nodes, " << d.nb_arcs() << " arcs" << std::endl;
  os << "nodes:" << std::endl;
  for (Nvector::const_iterator it = d.nodes().begin(); it != d.nodes().end(); ++it)
  {
    os << "  " << *it << std::endl;
  }
  /**/
  os << "arcs:" << std::endl;
  for (Avector::const_iterator it = d.arcs().begin(); it != d.arcs().end(); ++it)
  {
      os << "  " << *it << std::endl;
  }
  /**/

  return os;
}
