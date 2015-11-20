#pragma once


#include "data.h"


struct BKR
{
    static const std::string BKR_FILE;

    std::string name_;
    unsigned    nb_routes_;
    double      total_dist_;

    BKR (const Data &);
};


std::ostream & operator<< (std::ostream &, const BKR &);
