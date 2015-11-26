#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

#include "job.h"

class Bierwith {
	public:
		std::vector<unsigned int> v_; // Le vecteur de B

    public:
		Bierwith(int, int);
		Bierwith(const Bierwith&); // constructeur par copie
		void shuffle() {
			std::random_shuffle(v_.begin(), v_.end());
		}

		Bierwith& operator=(const Bierwith&);
		unsigned int & at(unsigned int);
		long hash() const;
};

std::ostream & operator<< (std::ostream & os, const Bierwith & b);
