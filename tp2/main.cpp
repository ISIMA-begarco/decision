#include <iostream>
#include <fstream>

#include "data.h"
#include "job.h"
#include "Bierwith.h"


int main(int argc, char** argv) {
	Data * data;
/*
	if(argc>1) {
        data = new Data(argv[1]);
    } else {*/
        data = new Data("INSTANCES/la01.dat");
  //  }

    data->display_all(std::cout);

	Bierwith b(data->nbItems_, data->nbMachines_);
    b.shuffle();


    //data->evaluer(b);
    data->rechercheLocale(b, 10);

    delete data;

	return 0;
}

