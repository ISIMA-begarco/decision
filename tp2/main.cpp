#include <iostream>
#include <fstream>

#include "data.h"
#include "job.h"
#include "Bierwith.h"

int main(int, char**) {
	Data * data = new Data("INSTANCES/exemple.dat");
	
	//data->display_all(std::cout);
	Bierwith b(3,3);
	
	//while(1) {
		data->evaluer(b);
//		b.shuffle();
	//}
	
	

	return 0;
}