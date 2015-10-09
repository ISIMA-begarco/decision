#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

class Bierwith {
	public:
		std::vector<unsigned int> v_;
		
		Bierwith(int nb_jobs, int nb_tasks): v_(nb_jobs*nb_tasks) {
			for(int i = 0 ; i < nb_tasks*nb_jobs ; ++i) {
				v_[i] = (int) (i / nb_jobs);
			}
		}
		
		void shuffle() {
			std::random_shuffle(v_.begin(), v_.end());
		}
};
