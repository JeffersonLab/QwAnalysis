
#include "TFloatBuffer.hh"

ClassImp(TFloatBuffer)
  
  
  TFloatBuffer::TFloatBuffer(){
  lBuffer = NULL;
  lBuffLength = 0;
}

TFloatBuffer::~TFloatBuffer(){
  ResetBuffer();
}

void TFloatBuffer::AllocateBuffer(int gLength){
  lBuffer = new Float_t[gLength];
  lBuffLength =  gLength;
}

void TFloatBuffer::ResetBuffer(){
  // Delete allocated Buffer
  //
  if(lBuffer != NULL){
    delete lBuffer;
    lBuffer = NULL;
    lBuffLength = 0;
  }
}

int TFloatBuffer::BoundCheck(int gPos){
  // Chacks if gPos in whithin range 0..lBuffLength
  //
  if((gPos<0)||(gPos>=lBuffLength)) return -1;
  return 0;
}

void TFloatBuffer::SetSize(int gSize){
  // Deletes existing buffer and allocates new one with size gSize
  //
  ResetBuffer();
  if(gSize>0)
    AllocateBuffer(gSize);  else  cout << "TFloatBuffer::SetSize : Cannot Allocate Buffer with size " << gSize << endl; 
}

void   TFloatBuffer::Set(int gPos, float gValue){
  if(BoundCheck(gPos)!=0){
    cout << "TFloatBuffer::Add : Position " << gPos << " is out of Range." << " Specify number in range [ 0 : " << lBuffLength-1 << " ]" << endl; 
    return;
  }
  lBuffer[gPos] = gValue;
}

Float_t  TFloatBuffer::Get(int gPos){
  if(BoundCheck(gPos)!=0){
    cout << "TFloatBuffer::Add : Position " << gPos << " is out of Range." << " Specify number in range [ 0 : " << lBuffLength-1 << " ]" << endl; 
    return 0.0;
  }
  return lBuffer[gPos];
}

void TFloatBuffer::Print(){
  cout << "TFloatBuffer: with " << lBuffLength << "  elements " << endl;
  for(int j =0; j < lBuffLength; j++){
    cout << "Element[" << j << "] = " << lBuffer[j] << endl; 
  }
}

Int_t TFloatBuffer::GetSize(){
  return lBuffLength;
}

