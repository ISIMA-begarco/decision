#include "CompareMiddleTW.h"

CompareMiddleTW::CompareMiddleTW()
{}

CompareMiddleTW::~CompareMiddleTW()
{}

bool CompareMiddleTW::operator()(const NodeInfo & n1, const NodeInfo & n2) {
    return ( ((n1.customer->open_ + n1.costumer->close_)/2.0) < ((n2.customer->open_ + n2.customer->close_)/2.0));
}
