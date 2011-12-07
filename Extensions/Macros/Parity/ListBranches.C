//************************************************************************************************************************************************//
//                                                                                                                                                //
//    This macro generates a list of branch names in a standard Qweak Root File, that will be usefull to tree trimming list                       //
//    The results are saved in a text file.                                                                                                       // 
//                                                                                                                                                //
//    Author   : Rakitha Beminiwattha                                                                                                             //
//    contact  : rakithab@jlab.org                                                                                                                //
//                                                                                                                                                //
//************************************************************************************************************************************************//
//                                                                                                                                                //
//                                                                                                                                                //
//     run_number        : Number of the run containing the signal or root file name is required to run this                                      //
//                                                                                                                                                //
//     YOU MUST FIRST SETUP THE QWEAK  ENVIRONMENT VARIABLES                                                                                      //
//                                                                                                                                                //
//************************************************************************************************************************************************// 

#include <iostream>
std::ofstream branchlistfile;

//Pass the run number for standard Qweak_%d.000.root files in std $QW_ROOTFILES directory
//this will generate list of branches into a text file called, Tree_Trim%d.in

void ListBranches(int run_number){
  Char_t textfile[100];
  TFile *file =  new TFile(Form("$QW_ROOTFILES/Qweak_%d.000.root",run_number));
  sprintf(textfile,"Tree_Trim%d.in",run_number);
  branchlistfile.open(textfile);
  branchlistfile <<"# Tree trim file of run  "<<run_number<<std::endl;
  branchlistfile <<"# Mps Tree  "<<std::endl;
  if (file->IsZombie()) {
    printf("Error opening file\n"); 
    delete canvas;canvas = NULL;
    return;
  }
  else{    
    TTree * mpstree = (TTree *) file->Get("Mps_Tree");
    TObjArray * branch_list;
    branch_list=mpstree->GetListOfBranches();
    TObject * branch;
    printf("Tree Mps_Tree  No.of Branches %d \n",branch_list->GetEntries());
    branchlistfile <<"# No. of branches  "<<branch_list->GetEntries()<<std::endl;
    for(Int_t i = 0;i<branch_list->GetEntries();i++ ){
      branch=branch_list->At(i);
      printf("%s \n",branch->GetName());
      branchlistfile <<branch->GetName() <<std::endl;
    }

    branchlistfile <<"# Hel Tree  "<<std::endl;
    mpstree = (TTree *) file->Get("Hel_Tree");
    branch_list=mpstree->GetListOfBranches();
    printf("Tree Hel_Tree  No.of Branches %d \n",branch_list->GetEntries());
    branchlistfile <<"# No. of branches  "<<branch_list->GetEntries()<<std::endl;	
    for(Int_t i = 0;i<branch_list->GetEntries();i++ ){
      branch=branch_list->At(i);
      printf("%s \n",branch->GetName());
      branchlistfile <<branch->GetName() <<std::endl;
    }

    branchlistfile.close(); 
   
  }

}

//Pass the root file name, routine will look for it in the std $QW_ROOTFILES directory
//this will generate list of branches into a text file called, Tree_Trim_%s.in
void ListBranches(TString Rfilename){
  Char_t textfile[100];
  TFile *file =  new TFile(Form("$QW_ROOTFILES/%s",Rfilename.Data()));
  sprintf(textfile,"Tree_Trim_%s.in",Rfilename.Data());
  branchlistfile.open(textfile);
  branchlistfile <<"# Tree trim file of run  "<<Rfilename<<std::endl;
  branchlistfile <<"# Mps Tree  "<<std::endl;
  if (file->IsZombie()) {
    printf("Error opening file\n"); 
    delete canvas;canvas = NULL;
    return;
  }
  else{    
    TTree * mpstree = (TTree *) file->Get("Mps_Tree");
    TObjArray * branch_list;
    branch_list=mpstree->GetListOfBranches();
    TObject * branch;
    printf("Tree Mps_Tree  No.of Branches %d \n",branch_list->GetEntries());
    branchlistfile <<"# No. of branches  "<<branch_list->GetEntries()<<std::endl;
    for(Int_t i = 0;i<branch_list->GetEntries();i++ ){
      branch=branch_list->At(i);
      printf("%s \n",branch->GetName());
      branchlistfile <<branch->GetName() <<std::endl;
    }

    branchlistfile <<"# Hel Tree  "<<std::endl;
    mpstree = (TTree *) file->Get("Hel_Tree");
    branch_list=mpstree->GetListOfBranches();
    printf("Tree Hel_Tree  No.of Branches %d \n",branch_list->GetEntries());
    branchlistfile <<"# No. of branches  "<<branch_list->GetEntries()<<std::endl;	
    for(Int_t i = 0;i<branch_list->GetEntries();i++ ){
      branch=branch_list->At(i);
      printf("%s \n",branch->GetName());
      branchlistfile <<branch->GetName() <<std::endl;
    }

    branchlistfile.close(); 
   
  }

}

