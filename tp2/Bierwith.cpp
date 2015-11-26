#include "Bierwith.h"


Bierwith::Bierwith(int nb_jobs, int nb_tasks): v_(nb_jobs*nb_tasks) {
// Constructeur qui prend le nombre de machines et de taches
	for(int i = 0 ; i < nb_tasks*nb_jobs ; ++i) {
		v_[i] = (int) (i / nb_tasks);
		//std::cout << v_[i] << "\t" ;
	}
	//std::cout << std::endl ;
}

Bierwith& Bierwith::operator=(const Bierwith& b) {
	//v_(b.v_.size()); // TODO regler ca ...
    this->v_.resize(b.v_.size());
	for(unsigned int i = 0 ; i < b.v_.size(); i++) {
		this->v_[i] = b.v_[i];
	}
	return *this;
}

unsigned int & Bierwith::at(unsigned int p_indice) {
    return v_[p_indice];
}

std::ostream & operator<< (std::ostream & os, const Bierwith & b)
{
    for(int i = 0 ; i < b.v_.size() ; ++i) {
        os << b.v_[i] << " ";
    }
    return os;
}
