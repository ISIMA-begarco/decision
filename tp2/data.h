#pragma once

#include "job.h"

#include <vector>
#include <random>


struct Data
{
  std::string name_;
  unsigned nbItems_;
  unsigned nbMachines_;
  std::vector< std::vector<Job> > jobs_;
  std::vector<Job*> first_;
  std::vector<Job*> last_;
  Job *  last_cp_; // TODO: mettre en in-class initialization
  //unsigned makespan_;

  std::mt19937 rng_engine_;

  Data (const std::string &);
  void clear ();

  void display_all (std::ostream &) const;
};

std::ostream & operator<< (std::ostream &, const Data &);
