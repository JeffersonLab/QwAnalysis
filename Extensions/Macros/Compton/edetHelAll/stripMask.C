#ifndef _MASK_C_
#define _MASK_C_

#include "rootClass.h"
#include "comptonRunConstants.h"

Int_t stripMask()
{
  cout<<blue<<"\nStarting into stripMask.C **************\n"<<normal<<endl;
  Bool_t additionalStripMask=1;///this will be my primary tool to skip masked strips in asymFit.C
  
  if(additionalStripMask) {//notice that the strip number pushed is in human counts 
    skipStrip.push_back(2);
    skipStrip.push_back(6);
    skipStrip.push_back(10);
    skipStrip.push_back(20);
    skipStrip.push_back(39);
    cout<<green<<"masked strips # 2,6,10,20,39 accross all planes"<<normal<<endl;//!update this with above list
  }
  return 1;
}
#endif
