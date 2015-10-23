#include "data.h"

#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>


Data::Data (const std::string & filename) : name_(filename), jobs_(0), last_cp_(nullptr)
{
  std::ifstream is(filename.c_str());

  if (!is)
  {
    std::cerr << "Error: unable to open file \'" << filename << "\'" << std::endl;
    exit (EXIT_FAILURE);
  }

  // step over the first line
  std::string line;
  getline(is, line);

  // get the instance size
  is >> nbItems_ >> nbMachines_;
  
  // memory allocation
  jobs_.resize(nbItems_);
  for (unsigned i = 0; i < nbItems_; ++i)
  {
    jobs_[i].resize(nbMachines_);
  }
  first_.resize(nbMachines_);
  last_.resize(nbMachines_);

  // get the information
  for (unsigned i = 0; i < nbItems_; ++i)
  {
    for (unsigned m = 0; m < nbMachines_; ++m)
    {
      Job & job = jobs_[i][m];
      job.item_ = i;
      is >> job.machine_ >> job.duration_;
    }
  }
}

void Data::clear ()
{
  for (auto & line : jobs_)
  {
    for (auto & elt : line)
    {
      elt.clear();
    }
  }
  for (auto & jobptr : first_)
  {
    jobptr = nullptr;
  }
  for (auto & jobptr : last_)
  {
    jobptr = nullptr;
  }
  last_cp_ = nullptr;
  //makespan_ = Job::NO_TIME;
}


void Data::display_all (std::ostream & os) const
{
  os << "instance " << name_ << ": " << nbItems_ << " items " << nbMachines_ << " machines" << std::endl;
  for (auto & line : jobs_)
  {
    for (auto & job : line)
    {
      job.display_all(os);
    }
    os << std::endl;
  }
  os << "last_cp = ";
  if (last_cp_ == nullptr)
  {
    os << "nullptr";
  }
  else
  {
    last_cp_->display_all(os);
  }
  os << std::endl;
}


std::ostream & operator<< (std::ostream & os, const Data & d)
{
  os << "instance " << d.name_ << ": " << d.nbItems_ << " items " << d.nbMachines_ << " machines" << std::endl;
  for (auto & line : d.jobs_)
  {
    for (auto & job : line)
    {
      os << ' ' << job.machine_ << ' ' << job.duration_;
    }
    os << std::endl;
  }
  return os;
}
void Data::evaluer(const Bierwith & b) {
	int id, start, machine;
	Job travail;
	
	std::vector<unsigned int> j_; // Vecteur reperant quelle machine a traiter par job
	std::vector<unsigned int> j_dispo_; // Date de dispo
	
	std::vector<Job*> m_; // Vecteur reperant le dernier job machine
	std::vector<unsigned int> m_dispo_; // Date de dispo
	
	// Initialisation des vector
	for(unsigned int i = 0; i < nbItems_; i++){
		j_.push_back(0); // TODO si ca f*** mettre des 1
		j_dispo_.push_back(0);
	}
	
	for(unsigned int i = 0; i < nbMachines_; i++) {
		m_.push_back(0);
		m_dispo_.push_back(0);
	}
	// end initialization
	
	// Parcours du vecteur
	for(unsigned int i = 0; i < b.v_.size(); i++) { 
		id = b.v_.at(i); // Quel numero de job on traite
		start = j_dispo_.at(id); // Date de debut du job
		machine = j_.at(id); // Machine du job
		travail = jobs_[id][machine]; // Recuperation du job dans le tableau principal
		
		
		travail.starting_ = (start >= m_dispo_.at(machine))? start : m_dispo_.at(machine);
		travail.prev_ = m_.at(machine);
		travail.location_ = machine;
		
		j_dispo_.at(id) =  travail.starting_ + travail.duration_; // change la date de dispo pour le job
		
		m_.at(machine) = &travail;
		m_dispo_.at(machine) = travail.starting_ + travail.duration_;
		
		j_.at(id)++;
	}
	
	int makespan = 0;
	for(unsigned int i = 0 ; i < m_dispo_.size() ; ++i ) {
		if(m_dispo_[i] > makespan) {
			makespan = m_dispo_[i];
		}
	}
	std::cout << makespan << std::endl;
}
