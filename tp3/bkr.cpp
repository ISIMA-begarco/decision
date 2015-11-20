#include "bkr.h"


#include <fstream>
#include <algorithm>


const std::string BKR::BKR_FILE = "BKR.txt";


BKR::BKR (const Data & d)
{
  // initialization
  nb_routes_ = std::numeric_limits<unsigned>::max();
  total_dist_ = std::numeric_limits<Time>::max();

  // get the instance name
  size_t first = d.name().rfind('/');
  if (first == std::string::npos) first = 0; else ++first;
  size_t length = d.name().rfind('.');
  if (length != std::string::npos) length -= first;
  std::string instance(d.name().substr(first, length));
  std::transform(instance.begin(), instance.end(), instance.begin(), ::toupper);
  //std::cout << "instance = " << instance << std::endl;

  // open the BKR file
  std::ifstream is(BKR_FILE);
  if (!is.is_open())
  {
    std::cerr << "Error: unable to open \"" << instance << "\"" << std::endl;
  }

  // loop over the records
  do
  {
    std::string record_name;
    unsigned    record_routes;
    double      record_distance;

    // read the first two information
    is >> record_name >> record_routes >> record_distance;
    //std::cout << "read " << record_name << ' ' << record_routes << ' ' << record_distance << std::endl;

    if (record_name == instance)
    {
      name_ = record_name;
      nb_routes_ = record_routes;
      total_dist_ = record_distance;
      break;
    }

    // skip the end of the line
    std::string line;
    getline(is, line);
  } while (!is.eof());

  // close the BKR file
  is.close();
}


std::ostream & operator<< (std::ostream & os, const BKR & bkr)
{
  os << bkr.name_ << '(' << bkr.nb_routes_ << ", " << bkr.total_dist_ << ')';
  return os;
}
