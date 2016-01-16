#include "bkr.h"
#include "algorithms.h"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>


// inline function to compute the relative gap
inline double relative_gap (const unsigned & val, const unsigned & ref)
{
  return (100.0 * (double(val) - double(ref)) / double(ref));
}


// inline function to compute the relative gap
inline double absolute_gap (const unsigned & val, const unsigned & ref)
{
  return (double(val) - double(ref));
}


// inline function to print out a global report about a method
inline void report (std::ostream & os, const std::string & method, const WorkingSolution & sol, const float & time, const BKR & bkr)
{
  std::streamsize w = os.width();
  int gap_routes   = absolute_gap(sol.nb_routes(), bkr.nb_routes_);
  float gap_distance = relative_gap(sol.distance(), bkr.total_dist_);
  os << bkr.name_ << " [" << std::setw(20) << method << "]   " << std::setw(3) << sol.nb_routes() << std::setw(w) << "," <<  std::setw(5) << sol.distance() << std::setw(w) << "   in " << std::fixed << std::setw(7) << std::setprecision(2) << time << " s   -> " << std::setw(7) << std::showpos << gap_routes << std::noshowpos << ",  " << std::setprecision(2) << gap_distance << " %" << std::setw(w) << std::endl;
}


// function to test the functionalities around WorkingSolution
void test_solution (const Data & data, const BKR & bkr)
{
  WorkingSolution sol(data);
  std::chrono::time_point<std::chrono::system_clock> start, end;
  float duration;

  if (false)
  {
    std::cout << "default solution" << std::endl;
    std::cout << sol << std::endl;
  }

  if (true)
  {
    //std::cout << "dummy solution" << std::endl;
    start = std::chrono::system_clock::now();
    insertion (sol);
    sol.check();
    end = std::chrono::system_clock::now();
    duration = 0.001 * std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();
    report(std::cout, "insertion", sol, duration, bkr);
    std::cout << sol << std::endl;
  }
}


// main function
int main (int argc, char * argv[])
{
  std::string filename("INSTANCES/rc101.txt");

  // check the command line
  if (argc > 2)
  {
    std::cerr << "Error: expecting one single argument (instance name)" << std::endl;
    return 1;
  }
  else if (argc == 2)
  {
    filename = argv[1];
  }

  // load the instance
  Data data(filename);
  //std::cout << data << std::endl;

  // load the BKR
  BKR bkr(data);
  std::cout << "BKR = " << bkr << std::endl;
  std::cout << std::endl;

  // test the solution functionalities
  if (true)
  {
    test_solution (data, bkr);
  }

  return 0;
}
