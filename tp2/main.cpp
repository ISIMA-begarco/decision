#include <iostream>
#include <fstream>

#include "data.h"
#include "job.h"
#include "Bierwith.h"


int main(int argc, char** argv) {
    int it = 100, pop = 100;
	Data * data;

	if(argc>1) {
        data = new Data(argv[1]);

        if(argc > 2) {
            sscanf(argv[2], "%d", &it);

            if (argc > 3) {
                sscanf(argv[3], "%d", &pop);
            }
        }
    } else {
        data = new Data("INSTANCES/la01.dat");
        //data = new Data("INSTANCES/exemple.dat");
    }

	Bierwith b(data->nbItems_, data->nbMachines_);
    b.shuffle();

    std::cout << "Lancement de l'algorithme genetique avec un maximim de "
              << it << " iterations et une population de " << pop << std::endl;
    std::cout << data->algorithmeGenetique(it, pop);

    delete data;

	return 0;
}

