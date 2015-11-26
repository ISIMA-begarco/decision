#include "population.h"

//Individu::Individu() { std::cerr << "Erreur constructeur par defaut ondividu"; }

Individu::Individu(Bierwith bVector, unsigned makespan) : m_bVector(bVector), m_makespan(makespan) {}

Individu::~Individu() { /* nothing to do */}

/******************************************************************************/

bool compareIndividu::operator()(const Individu& i1, const Individu& i2) const {
    return (i1.m_makespan < i2.m_makespan);
}

bool compareIndividu::operator()(const Individu* i1, const Individu* i2) const {
    return (i1->m_makespan < i2->m_makespan);
}

/******************************************************************************/

//Population::Population() { std::cerr << "Erreur constructeur par defaut population"; } // A ne pas utiliser de toute facon

// Initialisation de la population
Population::Population(int taille, const Data& d) : m_taille(taille) {
    int i = 0, makespan = 0, code = 0;

    m_taillePresence = 10000000;
    // Init tableau de presence
    for(unsigned int i = 0; i < m_taillePresence; i++) {
        m_presence.push_back(0);
    }

    // Initialise le vecteur
    Data copieData = d;

    m_item = copieData.nbItems_;
    m_machine = copieData.nbMachines_;

    m_pop.reserve(m_taille); // Reserve la taille de la population

    Bierwith bVector(m_item, m_machine); /// TODO est dans l'ordre

    while(i < m_taille) {
        makespan = copieData.evaluer(bVector); // Reevalue
std::cout << makespan << " ";
        if(!solutionDouble(copieData)) { // On a pas deja cette solution dans le tableau de presence
            this->m_pop.push_back(new Individu(bVector, makespan));
            i++;
        }
        // Sinon on en fait une autre

        bVector.shuffle(); // Melange pour refaire une solution
    }

    this->sort();
}

// Trie selon le makespan et affihe
void Population::sort() {
    std::sort(m_pop.begin(), m_pop.end(), compareIndividu());

    std::cout << "Vecteur de population (makespan)" << std::endl;
    for(unsigned int j; j < m_pop.size(); j++) {
        std::cout << m_pop[j]->m_makespan << " ";
    }
    std::cout << std::endl;
}

void Population::select() {
    this->sort();
    // On garde le nombre de personnes de base
    while(m_pop.size() > m_taille) {
        m_pop.pop_back();
    }
}

// Teste si cette solutin est deja dans le vecteur ou non
bool Population::solutionDouble(const Data& d) {
    bool ret = true;
    int code;
    code = d.hash(m_taillePresence);

    std::cout << "Hash Data : " << code << std::endl;

    if(m_presence[code] == 0) {
        m_presence[code]++;
        ret = false;
    }

    return ret;
}

// Cree un enfant en croisant 2 individus
Bierwith Population::croisement(Bierwith & bon, Bierwith & mauvais) {
    Bierwith fils(bon.item_, bon.machine_);
    std::vector<unsigned> compteur(bon.item_,0);
    unsigned i = 0;
    for(/* i */; i < bon.v_.size()/2 ; ++i) {   /// on met la moitié du bon
        fils.v_[i] = bon.v_[i];
        compteur[bon.v_[i]]++;
    }
    unsigned j = i;
    for(/* i */; i < bon.v_.size() ; ++i) {   /// on complete avec le mauvais
        if(compteur[mauvais.v_[i]]<fils.machine_) {
            fils.v_[j++] = mauvais.v_[i];
            compteur[mauvais.v_[i]]++;
        }
    }
    unsigned k = 0;
    while(k < compteur.size() && j < fils.v_.size()) {   /// on complete les jobs manquants
        while(compteur[k] < fils.machine_) {
            fils.v_[j++] = k;
            compteur[k]++;
        }
        k++;
    }
    return fils;
}

// Regenere un certain pourcentage de la fin de la population
void Population::regen(double rate, const Data & d) {
    int debut = (int)((double)m_taille*rate);
    unsigned makespan = 0;
    Data copieData = d;
    Bierwith bVector(m_machine, m_item); /// TODO peut etre pas le bon ordre?
    bVector.shuffle(); // Sinon on retombre directement dans le tout premier cas ajoute

    int i = debut;

    while(i < m_taille) {
            //Data copieData = d;
            makespan = copieData.evaluer(bVector); // Reevalue

            if(!solutionDouble(copieData)) { // On a pas deja cette solution dans le vecteur
                this->m_pop.push_back(new Individu(bVector, makespan));
                i++;
            }

            bVector.shuffle(); // Melange pour refaire une solution
    }
    this->sort();
}

Population::~Population() {
    // Vidage du vecteur
    for(unsigned int i = 0; i < m_pop.size(); i++) {
        if(m_pop[i])
            delete(m_pop[i]);
    }
}

