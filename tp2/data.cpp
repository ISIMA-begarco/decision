#include "data.h"

#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>


Data::Data (const std::string & filename) : name_(filename), jobs_(0), last_cp_(nullptr) {
    makespan_ = -1; // Plus grand entier on signe

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

int Data::evaluer(const Bierwith & b) {
    unsigned int id, start, machine, makespan = 0;
    Job travail;

    std::vector<unsigned int> j_; // Vecteur reperant quelle machine a traiter par job
    std::vector<unsigned int> j_dispo_; // Date de dispo

    std::vector<Job*> m_; // Vecteur reperant le dernier job machine
    std::vector<unsigned int> m_dispo_; // Date de dispo

    // Initialisation des vector
    for(register unsigned int i = 0; i < nbItems_; i++){
        j_.push_back(0); // TODO si ca f*** mettre des 1
        j_dispo_.push_back(0);
    }

    for(register unsigned int i = 0; i < nbMachines_; i++) {
        m_.push_back(0);
        m_dispo_.push_back(0);
    }
    // end initialization

    // Parcours du vecteur
    for(register unsigned int i = 0; i < b.v_.size(); i++) {
        id = b.v_.at(i);                // Quel numero de job on traite
        start = j_dispo_.at(id);        // Date de debut du job
        machine = j_.at(id);            // Machine du job
        travail = jobs_[id][machine];   // Recuperation du job dans tableau principal

        /* Change les attributs du job en cours de traitement */
        travail.starting_ = (start >= m_dispo_.at(machine))? start : m_dispo_.at(machine); // Change date de start
        travail.location_ = machine; // Change la machine du job

        travail.prev_ = m_.at(machine); // Change le pecedent de ce job
        //travail.prev_->next_ =  &travail; // Je suis le suivant du job precedent

        // TODO Modif father marche ptete pas
        //makespan = 0;
        for(register unsigned int j = 0 ; j < m_dispo_.size() ; ++j) {
            if(m_dispo_[j] > makespan) {
                makespan = m_dispo_[j];
                travail.father_ = m_.at(j); // On cree le chemin critique // TODO ce truc foire
//std::cout << "Modif father " << i << std::endl;
            }
        } std::cout << travail.father_ << " -- " << std::endl;
        // end father

        // Modification des vecteurs pour continuer
        j_.at(id)++; // La prochaine fois le job se fait sur la machine suivante
        j_dispo_.at(id) =  travail.starting_ + travail.duration_; // change la date de dispo pour le job

        m_.at(machine) = &travail; // Pointeur sur la derniere operation de la machine
        m_dispo_.at(machine) = travail.starting_ + travail.duration_; // Change dispo pour les machines
    } // end parcours

    // Recherche du makespan
    makespan = 0;
    for(register unsigned int i = 0 ; i < m_dispo_.size() ; ++i) {
        //std::cout << m_dispo_[i] << std::endl; // Affiche temps de fin
        if(m_dispo_[i] > makespan) {
            makespan = m_dispo_[i];
            last_cp_ = m_.at(i); // Pour recup le chemin critique apres
//std::cout << "last_cp_ : " << last_cp_->father_ << std::endl;
        }
    }

    std::cout << "makespan : " << makespan << std::endl;

    return makespan; // Et on le retourne
}

void Data::rechercheLocale(const Bierwith& b, int maxIter) {
    unsigned int makespan, new_makespan;
    bool stop = true;

    Bierwith b_prime = b; // Copie sur laquelle on fera les ameliorations
    std::vector<Job*> crit; // Chemin citique
    Job* cour;
    cour = last_cp_; // Pour le chemin critique

    makespan = evaluer(b); // Creation de la solution initiale

    // Creation du chemin critique de la solution initiale TODO ca marche pas, c'est le = qui foire
    while(last_cp_ != nullptr) {
        std::cout << "Je m'ajoute " << last_cp_->item_ << std::endl;
        crit.push_back(last_cp_); // Ajout dans le chemin critique
        last_cp_ = last_cp_->father_; // Passe au father
    }
    std::reverse(crit.begin(), crit.end()); // Pour l'avoir dans le sens de la marche

std::cout << "Affichage du chemin critique : size " << crit.size() << std::endl;
for(unsigned int i = 0; i < crit.size(); i++) {
    std::cout << crit[i]->item_ << " - ";
}

    do {
        // TODO Recherche d'une amélioration

        if(new_makespan < makespan) { // On a une amelioration
            makespan = new_makespan;
        } else {
            stop = true;
        }

    }while (!stop);
}
