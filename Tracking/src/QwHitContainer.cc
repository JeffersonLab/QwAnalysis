#include "QwHitContainer.h"

void QwHitContainer::Print ()
{
  if (! this) return;
  std::cout << *this << std::endl;
}

std::ostream& operator<< (std::ostream& stream, QwHitContainer& hitlist)
{
  for (QwHitContainer::iterator hit = hitlist.begin(); hit != hitlist.end(); hit++)
    stream << *hit << std::endl;
  return stream;
}
