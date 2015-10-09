#include <iostream>
#include <fstream>

#include "data.h"
#include "job.h"
#include "Bierwith.h"

int main(int, char**) {
	Data * data = new Data("INSTANCES/la01.dat");
	
	data->display_all(std::cout);

	return 0;
}
