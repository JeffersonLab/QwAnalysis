{
gROOT->Reset();
gROOT->SetStyle("Plain");

ifstream file;
file.open("inel_files.txt");
if (!file.good()) cout<<"error opening file"<<endl;

int j=0,n=3000;
int run[n],segment[n];
while (file.good())
{
  file>>run[j]>>segment[j];
  //cout <<run[j]<<"  "<<segment[j]<<endl;
  //printf("segment  %03i\n",segment[j]);
  j++;
}
file.close();



gSystem->Setenv("PAN_FILE_PREFIX","Qweak");
gSystem->Setenv("PAN_ROOT_FILE_PATH","./ROOTfiles");
//cout <<gSystem->Getenv("PAN_FILE_PREFIX")<<endl;
//cout <<gSystem->Getenv("PAN_ROOT_FILE_PATH")<<endl;



for (int i=0;i<j-1;i++)
//for (int i=0;i<4;i++)
{
  gSystem->Exec(Form("rm -f ROOTfiles/Qweak_%i.%03i.root",run[i],segment[i]));
  gSystem->Exec(Form("ln -s ~/scratch/rootfiles/Qweak_%i.%03i.root ROOTfiles/.",run[i],segment[i]));
  gSystem->Exec(Form("./redana_00%i -r %i > reg_output/reg_out%i_%03i.log",segment[i],run[i],run[i],segment[i]));
  gSystem->Exec(Form("mv -f ROOTfiles/Qweak_%iregress_Hel_Tree.root ROOTfiles/reg_Qweak_%i.%03i.root",run[i],run[i],segment[i]));
  gSystem->Exec(Form("mv -f Qweak_%iregress.res reg_output/Qweak%i_%03iregress.res",run[i],run[i],segment[i]));
}



}
