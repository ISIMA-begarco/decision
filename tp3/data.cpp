#include "data.h"

#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>
#include <algorithm>
#include <cassert>


const Id Customer::NO_NODE = std::numeric_limits<Id>::max();


Customer::Customer (const Id & i, const Location & x, const Location & y, const Load & d, const Time & o, const Time & c, const Time & s): id_(i), x_(x), y_(y), demand_(d), open_(o), close_(c), service_(s)
{
}


std::ostream & operator<< (std::ostream & os, const Customer & c)
{
  os << c.id() << ": (" << c.x() << ',' << c.y() << ") ask " << c.demand() << " in [" << c.open() << ',' << c.close() << "] dur " << c.service();

  return os;
}


const Time Arc::USELESS = std::numeric_limits<Time>::max();
const Time Arc::INFINITE = std::numeric_limits<Time>::max();


std::ostream & operator<< (std::ostream & os, const Arc & a)
{
  os << '(' << a.orig() << ',' << a.dest() << ' ' << a.cost() << ' ' << a.saving() << ')';

  return os;
}


void Data::build_arcs()
{
  //std::cout << "entering build_arcs" << std::endl;

  // build the list of arcs
  arcs_.clear();
  for (auto & orig: nodes_)
  {
    for (auto & dest: nodes_)
    {
      if (&orig == &dest)
      {
        continue;
      }
      if (orig.demand() + dest.demand() > capacity_)
      {
        continue;
      }
      const Time & dist = distance_[orig.id()][dest.id()];
      if (orig.open() + dist > dest.close())
      {
        continue;
      }
      Arc arc(orig.id(), dest.id(), dist);
      Time saving = Arc::USELESS;
      if ((orig.id() != depot_) && (dest.id() != depot_))
      {
        saving = dist - distance_[orig.id()][depot_] - distance_[depot_][dest.id()];
      }
      arc.saving() = saving;
      arcs_.push_back(arc);
    }
  }
  //arcs_.sort([](const Arc & a, const Arc & b){return (a.saving() < b.saving());});
  std::sort(arcs_.begin(), arcs_.end(), [](const Arc & a, const Arc & b){return (a.saving() < b.saving());});
  std::cout << arcs_.size() << " arcs sorted (" << nb_nodes()*(nb_nodes() - 1) << " possible)" << std::endl;

  // set the valid matrix
  is_valid_.resize(nb_nodes());
  for (unsigned i = 0; i < nb_nodes(); ++i)
  {
    std::vector<bool> & line = is_valid_[i];
    line.resize(nb_nodes());
    std::fill(line.begin(), line.end(), false);
  }

  // fill the valid matrix
  for (auto & arc: arcs_)
  {
    is_valid_[arc.orig()][arc.dest()] = true;
  }
  is_valid_[depot_][depot_] = true; // should help as well

  //std::cout << "leaving build_arcs" << std::endl;
}


void Data::build_adjacency()
{
  build_arcs();
}


Data::Data(const std::string & filename)
{
  // open the instance
  std::ifstream is(filename.c_str());
  if (!is)
  {
    std::cerr << "Error: unable to open file \'" << filename << "\'" << std::endl;
    exit (EXIT_FAILURE);
  }

  // get the instance name
  is >> name_;

  // skip the next three tokens
  std::string str;
  is >> str >> str >> str;

  // get the fleet information
  is >> nb_vehicles_ >> capacity_;

  // skip the customers header
  is >> str >> str >> str >> str >> str >> str >> str >> str >> str >> str >> str >> str;

  // read the customers information
  total_services_ = 0;
  depot_ = Customer::NO_NODE;
  do
  {
    do
    {
      getline(is, str);
    } while ((!is.eof()) && (str.find_first_not_of(" \t\n\v\f\r") == std::string::npos));
    if (is.eof())
      break;
    std::stringstream ss(str);
    Id i;
    Location x, y;
    Load d;
    Time o, c, s;
    ss >> i >> x >> y >> d >> o >> c >> s;

    if ((d == 0) && (s == 0))
    {
      if (depot_ != Customer::NO_NODE)
      {
        std::cerr << "Error: the depot has already been set (read " << i << " was " << depot_ << ")" << std::endl;
        exit(1);
      }
      depot_ = nodes_.size();
    }

    o *= 100.0;
    c *= 100.0;
    s *= 100.0;
    nodes_.push_back(Customer(i,x,y,d,o,c,s));
    total_services_ += s;
  } while (true);

  // close the instance
  is.close();

  // compute the distance matrix
  // TODO: compute the distances only once per node pair
  const unsigned size = nodes_.size();
  distance_.resize(size);
  for (unsigned i = 0; i < size; ++i)
  {
    distance_[i].resize(size);
  }
  for (auto i: nodes_)
  {
    for (auto j: nodes_)
    {
      Time dist = 0.0;
      if (i.id() != j.id())
      {
        Location delta_x = i.x() - j.x();
        Location delta_y = i.y() - j.y();
        dist = std::round(sqrt(delta_x * delta_x + delta_y * delta_y) * 100.0);
      }
      distance_[i.id()][j.id()] = i.service() + dist;
    }
  }
  assert(distance_[depot_][depot_] == 0.0);

  // build the adjecency lists
  build_adjacency();

  has_coordinates_ = true;
}


std::ostream & operator<< (std::ostream & os, const Data & d)
{
  os << "instance " << d.name() << ": " << d.nb_nodes() << " nodes, " << d.fleetSize() << " agents of capacity " << d.fleetCapacity() << std::endl;
  os << "depot " << d.depot() << std::endl;
  os << "nodes:" << std::endl;
  for (auto c: d.nodes())
  {
    os << "  " << c << std::endl;
  }
  /*
  os << "distances (service included):" << std::endl;
  for (auto i: d.distance_)
  {
    for (auto j: i)
    {
      os << ' ' << j;
    }
    os << std::endl;
  }
  */

  return os;
}
