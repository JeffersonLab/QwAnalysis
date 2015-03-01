#ifndef _MASK_C_
#define _MASK_C_

#include "rootClass.h"
#include "comptonRunConstants.h"

Int_t stripMask()
{
  //cout<<blue<<"\nStarting into stripMask.C **************\n"<<normal<<endl;
    skipStrip.push_back(2);
    skipStrip.push_back(6);
    skipStrip.push_back(10);
    skipStrip.push_back(20);
    skipStrip.push_back(39);
    skipStrip.push_back(60);
    cout<<green<<"masked strips # 2,6,10,20,39,60 accross all planes"<<normal<<endl;//!update this with above list
  return 1;
}
#endif
