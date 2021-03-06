#include "data.h"

#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

Data::Data(int items, int machines) :
    name_ ("Enfant"), nbItems_(items), nbMachines_(machines), last_cp_(nullptr), makespan_(-1) {

    jobs_.reserve(items);

    for(unsigned int i = 0; i < jobs_.size(); i++) {
        jobs_[i].reserve(machines);
    }
}

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

void Data::clear () {
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

void Data::display_all (std::ostream & os) const {
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

std::ostream & operator<< (std::ostream & os, const Data & d) {
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

unsigned Data::evaluer(Bierwith & b) {
    std::vector<Job*>       last_job_on_machine_(this->nbMachines_, NULL);
    std::vector<unsigned>   dispo_machine(this->nbMachines_, 0);
    std::vector<unsigned>   last_op_on_job_(this->nbItems_, -1);
    std::vector<unsigned>   dispo_job(this->nbItems_, 0);
    Job * travail = NULL;

    for(int i = 0 ; i < b.v_.size() ; ++i) {
        unsigned    id_job = b.v_[i];
        travail = &(jobs_[id_job][last_op_on_job_[id_job]+1]);
        unsigned    id_machine = travail->machine_;

        if(dispo_job[id_job] < dispo_machine[id_machine]) {
            travail->starting_ = dispo_machine[id_machine];
            dispo_machine[id_machine] += travail->duration_;
            dispo_job[id_job] = dispo_machine[id_machine];

            travail->father_ = last_job_on_machine_[id_machine];
        } else {
            travail->starting_ = dispo_job[id_job];
            dispo_job[id_job] += travail->duration_;
            dispo_machine[id_machine] = dispo_job[id_job];

            travail->father_ = last_op_on_job_[id_job] == -1 ? NULL : &(this->jobs_[id_job][last_op_on_job_[id_job]]);
        }

        travail->prev_ = last_job_on_machine_[id_machine];
        travail->id_ = i; // Place dans le vecteur

        last_job_on_machine_[id_machine] = travail;
        last_op_on_job_[id_job]++;
    }

    // recherche du makespan
    makespan_ = 0;
    for(int j = 0 ; j < dispo_job.size() ; j++) {
        if(dispo_job[j] > makespan_) {
            makespan_ = dispo_job[j];
            last_cp_ = &(this->jobs_[j][last_op_on_job_[j]]);
        }
    }

    /// affichage du resultat
/**    std::cout << "Le makespan est : " << makespan_ << std::endl;
    travail = last_cp_;
    while(travail != NULL){
        std::cout << *travail << std::endl;
        travail = travail->father_;
    }
**/
    return makespan_;
}

unsigned Data::rechercheLocale(Bierwith& b, int maxIter) {
    unsigned int new_makespan, old_makespan=makespan_;
    int i = 0;
    bool stop = false;

    evaluer(b); // Creation solution initiale

    while(this->amelioration(b) < old_makespan && i < maxIter) {
    	old_makespan = makespan_;
    	i++;
    }

    //std::cout << "Makespan ameliore " << makespan_ /**<< " avec b = " << b**/ << std::endl;

    return makespan_;
}

int Data::amelioration(Bierwith & b) {
	int tmp;

	Data graph_prime = *this; // operator = pour copier
	Bierwith b_prime = b; // Copie sur laquelle on fera les ameliorations
	Job* cour = graph_prime.last_cp_;

	while(cour != NULL && cour->father_ != NULL) {// Parcours chemin critique

		while(cour != NULL && cour->father_ != NULL && cour->father_ != cour->prev_) { //  recherche Disjonctif
            cour = cour->father_;
		}

		if(cour != NULL && cour->father_ != NULL) {
			/// Permuter
			//std::cout << "Inversion : " << *cour << *(cour->father_) << std::endl;
			tmp = b_prime.at(cour->id_);
			b_prime.at(cour->id_) = b_prime.at(cour->prev_->id_);
			b_prime.at(cour->prev_->id_) = tmp;

            /// evaluation apres permutation
            graph_prime.evaluer(b_prime);

            /// verification amelioration
            if(graph_prime.makespan_ < this->makespan_) {
                b = b_prime;
                (*this) = graph_prime;
                cour = NULL;
            } else {
                cour = cour->father_;
            }
		}
	}

	return this->makespan_;
}

Data& Data::operator=(const Data& d) {
    this->name_ = d.name_;
    this->nbItems_ = d.nbItems_;
    this->nbMachines_ = d.nbMachines_;

    this->jobs_.resize(this->nbItems_);
    for (unsigned i = 0; i < this->nbItems_; ++i) {
        this->jobs_[i].resize(this->nbMachines_);
        for(int j = 0 ; j < d.jobs_[i].size() ; j++) {
            this->jobs_[i][j] = d.jobs_[i][j];
        }
    }
    int k = 0;
    while(k < this->nbItems_ && (jobs_[k][nbMachines_-1].starting_ + jobs_[k][nbMachines_-1].duration_) != makespan_) {
        k++;
    }
    if(k < nbItems_)
        last_cp_ = &(jobs_[k][nbMachines_-1]);
    else
        last_cp_ = NULL;

    this->makespan_ = d.makespan_;
    this->rng_engine_ = d.rng_engine_;

    return *this;
}

unsigned long long Data::hash(int modulo) const {
    unsigned long long code = 0;

    for(unsigned int i = 0; i < nbItems_; i++) {
        for(unsigned int j = 0; j < nbMachines_; j++) {
            code = code + (jobs_[i][j].starting_ * jobs_[i][j].starting_);
        }
    }

    return (code % modulo);
}

unsigned Data::algorithmeGenetique(int maxIter, int taillePopulation) {
    int step = 0, noAmelioration = 0, indiv1, indiv2;
    unsigned makespan = -1, makespanOld = -1;
std::cout << "Initialisation de la population..." << std::endl;
    // Creation de lois uniformes pour tirer les individus
    std::uniform_int_distribution<> tenPercent(0, (int)(((double)taillePopulation)*0.1));
    std::uniform_int_distribution<> ntyPercent((int)(((double)taillePopulation)*0.1)+1, taillePopulation-1);

    Population p(taillePopulation, *this); // Initialise la population

std::cout << "Population initialisee" << std::endl;

    while(step < maxIter) {
        /* On double la taille de notre population */
        for(unsigned int j = 0; j < taillePopulation; j++) {
            // Tirage des individus
            indiv1 = tenPercent(this->rng_engine_); // Prend dans les 10%
            indiv2 = ntyPercent(this->rng_engine_); // Prend dans les 90% restants

            // Obtention de l'enfant
            Bierwith lambda = p.croisement(p.m_pop[indiv1]->m_bVector, p.m_pop[indiv2]->m_bVector); // Nouveau vecteur
            makespan = this->rechercheLocale(lambda, maxIter);

//std::cout << "Mix individu " << indiv1 << " et " << indiv2 << " trouve : " << makespan << std::endl;

            // Ajout de l'enfant dans le vecteur
            p.m_pop.push_back(new Individu(lambda, makespan));
        }
        p.select(); // On trie avant de select donc ok

        makespan = p.m_pop[0]->m_makespan; // Le meilleur est toujours en premier

        if(makespan == makespanOld) { // On compte les cas stationnaires
            noAmelioration++;
        } else { // Sinon on a ameliore
            noAmelioration = 0;
            makespanOld = makespan;
        }

        if(noAmelioration == 10) {
            noAmelioration = 0;

            // On regenere les 90% fin de la population
            p.regen(0.9, *this); // On sort apres donc ok
        }
        step++;

        if(step % 100 == 0)
            std::cout << "#" << step << " : " << makespan << std::endl;
    }

    //p.printAll();
    std::cout << "## Makespan final : " << makespan << " ##" << std::endl;

    return makespan;

}
