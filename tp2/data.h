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

		Data(int, int);
		Data (const std::string &);
		void clear ();

		void display_all (std::ostream &) const;

		unsigned evaluer(Bierwith &); // retourne le makespan
		unsigned rechercheLocale(Bierwith&, int);
		int amelioration(Bierwith&);
		unsigned algorithmeGenetique(int = 100, int = 100); // L'entier est la taille de la population
		Data& operator=(const Data&);
		unsigned long long hash(int = 1000000) const;
};

std::ostream & operator<< (std::ostream &, const Data &);
