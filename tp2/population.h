#ifndef POPULATION_H
#define POPULATION_H

#include <iostream>
#include <vector>
#include <deque>
#include <queue>
#include <algorithm>

#include "data.h"
#include "Bierwith.h"

class Data;

class Population {
    public:
        std::mt19937 m_mt;      // Pour du random
        int m_taille;           // taille de la population
        int m_item;             // Nombre d'items dans nos datas
        int m_machine;          // Nombre de machines des data qu'on manipule
        std::vector<Data *> m_pop;      // Contient toutes les solutions
        std::vector<Data *> m_pop_enf;      // Contient toutes les solutions

        Population();
        Population(int, Data&);
        virtual ~Population();

        bool solutionDouble(const Data&) const;
        void croisement();
        void mutation();
        void garderMeilleurs(Population&);

};

#endif // POPULATION_H
