#ifndef POPULATION_H
#define POPULATION_H

#include <iostream>
#include <vector>

#include "data.h"
#include "Bierwith.h"

class Data;

class Population {
    public:
        int m_taille;               // taille de la population
        std::vector<Data *> m_pop;    // Contient toutes les solutions

        Population();
        Population(int, Data&);
        virtual ~Population();

        bool solutionDouble(const Data&) const;
        void croisement();
        void mutation();
        void garderMeilleurs(Population&);

};

#endif // POPULATION_H
