#ifndef POPULATION_H
#define POPULATION_H

#include <iostream>
#include <vector>

#include "data.h"
#include "Bierwith.h"

class Population
{
    public:
        Population();
        Population(int, Data&);
        virtual ~Population();

        int m_taille;               // taille de la population
        std::vector<Data *> m_pop;    // Contient toutes les solutions
        bool solutionDouble(const Data&) const;


    protected:
};

#endif // POPULATION_H
