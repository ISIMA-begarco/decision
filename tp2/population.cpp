#include "population.h"

//Individu::Individu() { std::cerr << "Erreur constructeur par defaut ondividu"; }

Individu::Individu(Bierwith bVector, unsigned makespan) : m_bVector(bVector), m_makespan(makespan) {}

Individu::~Individu() { /* nothing to do */}

bool compareIndividu::operator()(const Individu& i1, const Individu& i2) const {
    return (i1.m_makespan < i2.m_makespan);
}

//Population::Population() { std::cerr << "Erreur constructeur par defaut population"; } // A ne pas utiliser de toute facon

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
        makespan = copieData.evaluer(bVector); // Reevalue

        if(!solutionDouble(bVector)) { // On a pas deja cette solution dans le vecteur
            this->m_pop.push_back(new Individu(bVector, makespan));
            i++;
        }
        // Sinon on en fait une autre

        bVector.shuffle(); // Melange pour refaire une solution
    }

    this->trier();
}

// Trie selon le makespan et affihe
void Population::trier() {
    std::sort(*m_pop.begin(), *m_pop.end(), compareIndividu());

    std::cout << "Vecteur de population (makespan)" << std::endl;
    for(unsigned int j; j < m_pop.size(); j++) {
        std::cout << m_pop[j]->m_makespan << " ";
    }
    std::cout << std::endl;
}


// Teste si cette solutin est deja dans le vecteur ou non
bool Population::solutionDouble(const Bierwith& b) const{
    bool ret = false;

    /// TODO avec fonction de hash

    return ret;

}

// Cree un enfant en croisant 2 individus
void Population::croisement() {
    int indiv1, indiv2;
    // Cherche le premier individu dans les 10% meilleurs
    indiv1 = m_mt() % (unsigned)((double)m_taille*0.1);
    // Cherche l'autre dans le reste
    indiv2 = m_mt() % (m_taille - (unsigned)((double)m_taille*0.1)) + (unsigned)((double)m_taille*0.1);


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
