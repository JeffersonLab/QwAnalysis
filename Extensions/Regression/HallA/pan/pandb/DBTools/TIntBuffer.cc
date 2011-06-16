
#include "TIntBuffer.hh"


ClassImp(TIntBuffer)
  
  
  //
  // Class TIntBuffer 
  //
  
  TIntBuffer::TIntBuffer(){
  lBuffer = NULL;
  lBuffLength = 0;
}

TIntBuffer::~TIntBuffer(){
  ResetBuffer();
}

void TIntBuffer::AllocateBuffer(int gLength){
  lBuffer = new Int_t[gLength];
  lBuffLength =  gLength;
}

void TIntBuffer::ResetBuffer(){
  // Delete allocated Buffer
  //
  if(lBuffer != NULL){
    delete lBuffer;
    lBuffer = NULL;
    lBuffLength = 0;
  }
}

int TIntBuffer::BoundCheck(int gPos){
  // Chacks if gPos in whithin range 0..lBuffLength
  //
  if((gPos<0)||(gPos>=lBuffLength)) return -1;
  return 0;
}

void TIntBuffer::SetSize(int gSize){
  // Deletes existing buffer and allocates new one with size gSize
  //
  ResetBuffer();
  if(gSize>0)
    AllocateBuffer(gSize);  else  cout << "TIntBuffer::SetSize : Cannot Allocate Buffer with size " << gSize << endl; 
}

void   TIntBuffer::Set(int gPos, int gValue){
  if(BoundCheck(gPos)!=0){
    cout << "TIntBuffer::Add : Position " << gPos << " is out of Range." << " Specify number in range [ 0 : " << lBuffLength-1 << " ]" << endl; 
    return;
  }
  lBuffer[gPos] = gValue;
}

Int_t  TIntBuffer::Get(int gPos){
  if(BoundCheck(gPos)!=0){
    cout << "TIntBuffer::Add : Position " << gPos << " is out of Range." << " Specify number in range [ 0 : " << lBuffLength-1 << " ]" << endl; 
    return 0;
  }
  return lBuffer[gPos];
}

void TIntBuffer::Print(){
  cout << "TIntBuffer: with " << lBuffLength << "  elements " << endl;
  for(int j =0; j < lBuffLength; j++){
    cout << "Element[" << j << "] = " << lBuffer[j] << endl; 
  }
}

Int_t TIntBuffer::GetSize(){
  return lBuffLength;
}

