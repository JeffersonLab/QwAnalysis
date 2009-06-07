//
// C++ Implementation: shortnode
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "shortnode.h"
#include <iostream>

using namespace std;

namespace QwTracking {

shortnode::shortnode() {

  debug = 0;

  if( debug )
      cout<<"###### Calling shortnode::shortnode()"<<endl;

  if( debug )
      cout<<"###### Leaving shortnode::shortnode()"<<endl;

}

shortnode::~shortnode()
{
  if( debug )
      cout<<"###### Calling shortnode::~shortnode()"<<endl;

  if (next) delete next;

  if( debug )
      cout<<"###### Leaving shortnode::~shortnode()"<<endl;
}

void shortnode::print() {
  tree->print();
}

} // QwTracking
