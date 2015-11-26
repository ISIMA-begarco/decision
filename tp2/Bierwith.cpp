#include "Bierwith.h"


Bierwith::Bierwith(int nb_jobs, int nb_tasks): v_(nb_jobs*nb_tasks) {
// Constructeur qui prend le nombre de machines et de taches
    this->item_ = nb_jobs;
    this->machine_ = nb_tasks;
	for(int i = 0 ; i < nb_tasks*nb_jobs ; ++i) {
		v_[i] = (int) (i / nb_tasks);
	}
}

Bierwith::Bierwith(const Bierwith& b) {
    this->v_.resize(b.v_.size());
    this->item_ = b.item_;
    this->machine_ = b.machine_;
    for(unsigned int i = 0; i < b.v_.size(); i++) {
        this->v_[i] = b.v_[i];
    }
}


Bierwith& Bierwith::operator=(const Bierwith& b) {
	//v_(b.v_.size()); // TODO regler ca ...
    this->v_.resize(b.v_.size());
    this->item_ = b.item_;
    this->machine_ = b.machine_;
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

