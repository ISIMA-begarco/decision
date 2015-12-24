#include "CompareMiddleTW.h"

CompareMiddleTW::CompareMiddleTW()
{}

CompareMiddleTW::~CompareMiddleTW()
{}

bool CompareMiddleTW::operator()(const NodeInfo & n1, const NodeInfo & n2) {
    return ( ((n1.customer->open() + n1.customer->close())/2.0) < ((n2.customer->open() + n2.customer->close())/2.0));
}

