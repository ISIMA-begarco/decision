#include "job.h"


const unsigned Job::NO_TIME     = std::numeric_limits<unsigned>::max();
const unsigned Job::NO_LOCATION = std::numeric_limits<unsigned>::max();


void Job::display_all (std::ostream & os) const
{
  os << '(' << item_ << " - " << machine_ << " - " << duration_
     << " -> " << starting_ << " / " << ((prev_ == nullptr)? 100: prev_->item_) << " / " << ((next_ == nullptr)? 100: next_->item_)
     << " / " << ((father_ == nullptr)? 100 : father_->item_) <<  ')';
}


std::ostream & operator<< (std::ostream & os, const Job & j)
{
  os << '(' << j.item_ << ' ' << j.machine_ << ' ' << j.duration_  << ')';
  return os;
}
