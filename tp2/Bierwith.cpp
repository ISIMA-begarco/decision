#include "Bierwith.h"


Bierwith::Bierwith(int nb_jobs, int nb_tasks): v_(nb_jobs*nb_tasks) {
// Constructeur qui prend le nombre de machines et de taches
	for(int i = 0 ; i < nb_tasks*nb_jobs ; ++i) {
		v_[i] = (int) (i / nb_jobs);
		std::cout << v_[i] << "\t" ;
	}
	std::cout << std::endl ;
}


