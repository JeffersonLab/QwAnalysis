{
gROOT->Reset();
gROOT->SetStyle("Plain");

ifstream file;
//file.open("inel_files.txt");
//file.open("test.txt");
file.open("inel_slugs1234.txt");

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
gSystem->Setenv("PAN_ROOT_FILE_PATH","/home/leacock/scratch/rootfiles");
gSystem->Setenv("PAN_LOG_FILE_PATH","/home/leacock/scratch/rootfiles/reg_output");
//cout <<gSystem->Getenv("PAN_FILE_PREFIX")<<endl;
//cout <<gSystem->Getenv("PAN_ROOT_FILE_PATH")<<endl;
//cout <<gSystem->Getenv("PAN_LOG_FILE_PATH")<<endl;

for (int i=0;i<j-1;i++)
//for (int i=0;i<4;i++)
{
  gSystem->Exec(Form("./redana_%03i -r %i > $PAN_LOG_FILE_PATH/reg_out_nocharge%i_%03i.log",segment[i],run[i],run[i],segment[i]));
  gSystem->Exec(Form("mv -f $PAN_ROOT_FILE_PATH/Qweak_%iregress_Hel_Tree.root $PAN_ROOT_FILE_PATH/reg_HallA_nocharge_Qweak_%i.%03i.root",run[i],run[i],segment[i]));
  gSystem->Exec(Form("mv -f Qweak_%iregress.res $PAN_LOG_FILE_PATH/Qweak_nocharge%i_%03iregress.res",run[i],run[i],segment[i]));
}



}
