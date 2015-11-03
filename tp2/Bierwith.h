#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

#include "job.h"

class Bierwith {
	public:
		std::vector<unsigned int> v_; // Le vecteur de B


		Bierwith(int, int);

		void shuffle() {
			std::random_shuffle(v_.begin(), v_.end());
		}

		Bierwith& operator=(const Bierwith&);
};
