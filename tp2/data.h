#pragma once

#include "job.h"
#include "Bierwith.h"
#include "population.h"

#include <vector>
#include <random>


class Data {
	public:
		std::string name_;
	  	unsigned nbItems_;
	  	unsigned nbMachines_;

	  	std::vector< std::vector<Job> > jobs_;
	  	std::vector<Job*> first_;
	  	std::vector<Job*> last_;

		Job *  last_cp_; // TODO: mettre en in-class initialization
		unsigned makespan_;

		std::mt19937 rng_engine_;

		Data (const std::string &);
		void clear ();

		void display_all (std::ostream &) const;

		int evaluer(Bierwith &); // retourne le makespan
		void rechercheLocale(Bierwith&, int);
		int amelioration(Bierwith&);
		void algorithmeGenetique(int); // L'entier est la taille de la population
};

std::ostream & operator<< (std::ostream &, const Data &);
