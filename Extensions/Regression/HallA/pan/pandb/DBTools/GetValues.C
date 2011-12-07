{

gROOT->Reset();
gSystem->Load("./libDBTools.so"); 

//TDBTools T("alquds.jlab.org","pandb","dbmanager");
TDBTools T("pandb");
T.PrintInfo();
T.SetDebugMode(0x20);

TFloatBuffer Noise;
T.GetDBFloatBuff("dacnoise","adc6","chan2",1024,&Noise);
cout << "slope: " << Noise.Get(0) << endl;
cout << "intercept: " << Noise.Get(1) << endl;
Noise.Print();

TIntBuffer Ped;
T.GetDBIntBuff("ped","adc5","chan3",1024,&Ped);
cout << "ped: " << Ped.Get(0) << endl;
Ped.Print();

Int_t length;

TString *rtyp = T.GetDBChar("ana","pan","runtype",1024,&length);
cout << "Run Type: " << *rtyp << endl;

TString *Hdr = T.GetDBChar("header","scaler","hdr",1024,&length);
cout << "hdr: " << *Hdr << endl;

TString *DM = T.GetDBChar("ana","pan","datamap",1024,&length);
// cout << "DataMap: " << *DM << endl;

//
//
// Decode the DataMap: 
//
//

cout << "DataMap: " << endl;

char *myc;
myc = new char[strlen(DM->Data())+1];
strcpy(myc,DM->Data());
//   cout << "myc: " <<  myc << endl;

//   TString mys;
//   mys = myc;
//   cout << "mys: " <<  mys << endl;

Int_t len = strlen(myc);
//   cout << "len: " <<  len << endl;

TString str; TString data[2000];
str = myc[0];  
//   cout << "str: " <<  str << endl; 

Int_t k =0;
for (Int_t i=1; i<len; i++){
  if(myc[i]=='+'){
    cout << "DataMap Item: " <<  str << endl;  
    str = "";
    k++;
  }
  else{
    str += myc[i];
    data[k]= str;
  }
}

cout << "DataMap Item: " <<  str << endl; 

////

char *submyc;
submyc = new char[1000];

TString substr; TString subdata[2000];

for (Int_t j=0; j<=k; j++){
  
  strcpy(submyc,data[j].Data());
  Int_t Len = strlen(submyc);
  //  cout << "Len: " <<  Len << endl;
  substr = submyc[0];  
  
  Int_t K =0;
  for (Int_t I=1; I<Len; I++){
    
    if(submyc[I]==','){
      //      cout << "subitem: " <<  substr << endl;  
      substr = "";
      K++;
    }
    else{
      substr += submyc[I];
      subdata[K]= substr;
    }
  }
  
  //  cout << "subitem: " <<  substr << endl; 
}

//

}
