#include <iostream>
#include <fstream>

#include "data.h"
#include "job.h"
#include "Bierwith.h"


int main(int argv, char** argc) {
	Data * data;

	if(argv==2)
        data = new Data(argc[1]);
    else
        data = new Data("INSTANCES/la20.dat");

	Bierwith b(data->nbItems_, data->nbMachines_);
    b.shuffle();


    //data->evaluer(b);
    data->rechercheLocale(b, 10);

    delete data;

	return 0;
}

