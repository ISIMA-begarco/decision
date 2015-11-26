#ifndef POPULATION_H
#define POPULATION_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <ostream>

#include "data.h"
#include "Bierwith.h"

class Data;

class Individu {

        public:
                Bierwith m_bVector;
                unsigned m_makespan;

                //Individu();
                Individu(Bierwith, unsigned);
                ~Individu();

};

// Pour faire le tri dans le vecteur
class compareIndividu {
public:
        bool operator()(const Individu&, const Individu&) const;
        bool operator()(const Individu*, const Individu*) const;
};

class Population {
    public:
        std::mt19937 m_mt;      // Pour du random
        int m_taille;           // taille de la population
        int m_item;             // Nombre d'items dans nos datas
        int m_machine;          // Nombre de machines des data qu'on manipule
        std::vector<Individu *> m_pop;      // Contient toutes les solutions

        //Population();
        Population(int, const Data&);
        virtual ~Population();

        void sort();
        void select();
        void regen(double, const Data&);

        bool solutionDouble(const Bierwith&) const;
        Bierwith croisement(Bierwith & bon, Bierwith & mauvais);
        void mutation();
        void garderMeilleurs(Population&);

};

std::ostream & operator<< (std::ostream &, const Population &);

#endif // POPULATION_H
