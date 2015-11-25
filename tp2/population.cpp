#include "population.h"

Population::Population() {std::cerr << "Erreur de population";} // A ne pas utiliser de toute facon

// Initialisation de la population
Population::Population(int taille, Data& d) {
    // Initialise le vecteur
    int i = 0;
    m_taille = taille;
    m_pop.reserve(taille);
    Data copie = d;

    Bierwith bVector(d.nbMachines_, d.nbItems_); /// TODO peut etre pas le bon ordre?

    while(i < m_taille) {
        copie.evaluer(bVector); // Reevalue

        if(!solutionDouble(copie)) { // On a pas deja cette solution dans le vecteur
            this->m_pop.push_back(new Data(copie)); /// TODO constructeur par copie de Data
            i++;
        } // Sinon on en fait une autre

        bVector.shuffle(); // Melange pour refaire une solution
    }

}


// Teste si cette solutin est deja dans le vecteur ou non
bool Population::solutionDouble(const Data& d) const{
    bool ret = false;

    for(unsigned int i = 0; i < m_pop.size() && !ret; i++) {
        ret = true;
        for(unsigned int j = 0; j < d.nbItems_ && ret; j++) {
            for(unsigned int k = 0; k < d.nbMachines_ && ret; k++) { // On teste toutes les dates de debut
            ret = ret && (d.jobs_[j][k].starting_ == m_pop[i]->jobs_[j][k].starting_);
            // Si tout est vrai, on doit forcement avoir vrai en sortie, faux sinon
            }
        }
    }

    return ret;

}

void Population::croisement() {
    /// TODO
}

void Population::mutation() {
    /// TODO
}

void Population::garderMeilleurs(Population& p_prime) {
    /// TODO
}

Population::~Population() {
    // Vidage du vecteur
    for(unsigned int i = 0; i < m_pop.size(); i++) {
        if(m_pop[i])
            delete(m_pop[i]);
    }
}
