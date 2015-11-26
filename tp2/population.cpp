#include "population.h"

Individu::Individu() { std::cerr << "Erreur constructeur par defaut ondividu"; }

Individu::Individu(Bierwith bVector, unsigned makespan) : m_bVector(b), m_makespan(makespan) {}

Individu::~Individu() { /* nothing to do */}

bool compareIndividu::operator()(const Individu& i1, const Individu& i2) const {
    return (i1.m_makespan < i2.m_makespan);
}

Population::Population() { std::cerr << "Erreur constructeur par defaut population"; } // A ne pas utiliser de toute facon

// Initialisation de la population
Population::Population(int taille, const Data& d) : m_taille(taille) {
    // Initialise le vecteur
    int i = 0, makespan = 0;
    Data copieData = d;

    m_item = copieData.nbItems_;
    m_machine = copieData.nbMachines_;

    m_pop.reserve(m_taille); // Reserve la taille de la population

    Bierwith bVector(m_machine, m_item); /// TODO peut etre pas le bon ordre?

    while(i < m_taille) {
        makespan = copie.evaluer(bVector); // Reevalue

        if(!solutionDouble(bVector)) { // On a pas deja cette solution dans le vecteur
            this->m_pop.push_back(new Individu(bVector, makespan)); /// TODO constructeur par copie de Data
            i++;
        } // Sinon on en fait une autre

        bVector.shuffle(); // Melange pour refaire une solution
    }

    std::sort(m_pop.begin(), m_pop.end(), compareIndividu()); // Pour trier selon le makespan

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

// Cree un enfant en croisant 2 individus
void Population::croisement() {
    int indiv1, indiv2, k;
    Data *enf = new Data(m_item, m_machine);

    // Selection aleatoire des parents dans la population
    indiv1 = m_mt() % m_pop.size();
    while((indiv2 = m_mt() % m_pop.size()) == indiv1); // On veut des individus differents
    // Selection aleatoire de la machine
    k = m_mt() % m_machine;

    for(unsigned int i = 0; i < m_item; i++) {
        for(unsigned int j = 0; j < m_machine; i++) {
            if(j != k){
                enf->jobs_[i][j] = m_pop[indiv1]->jobs_[i][j];
            } else {
                enf->jobs_[i][j] = m_pop[indiv2]->jobs_[i][j];
            }
        }
    }
    /// Tester s'il n'a pas de cycles ?
}

void Population::mutation() {
/**
Debut
 Selectionner aleatoirement un individu i.
 Selectionner aleatoirement une machine k parmi m.
 Pour « h = 1 jusqu’a m » Faire
 Si « h != k » Alors
 Indiv mute recoit les memes affectations que indiv i.
 Sinon
 Selectionner aleatoirement deux operations Oi,j et Oi’,j
 Permuter l’ordre des operations Oi,j et Oi’,j’.
 Fin si ;
 Fin de pour ;

 Tester la realisabilite :
 Si « solution sans cycle » Alors
 Indiv mute acceptes (solution realisable)
 Sinon
 Indiv mute refuses (solutions non realisable)
 Fin si ;
Fin.
*/

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
