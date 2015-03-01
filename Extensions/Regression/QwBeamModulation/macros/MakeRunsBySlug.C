{
  TChain *ch = new TChain("tree");
  ch->Add("/net/data2/paschkelab2/DB_rootfiles/run1/HYDROGEN-CELL_off_tree.root");
  ch->Add("/net/data2/paschkelab2/DB_rootfiles/run2/HYDROGEN-CELL_off_tree.root");
  int run, slug;
  FILE *file;
  file = fopen("runsBySlug.dat","w");
  if(file==NULL)
    exit(0);
  ch->SetBranchAddress("run_number",&run);
  ch->SetBranchAddress("slug",&slug);
  cout<<ch->GetEntries()<<endl;
  int prev_run = 0;
  for(int i=0;i<ch->GetEntries();++i){
    ch->GetEntry(i);
    if(run!=prev_run && slug>41 && slug<322){
      printf("%i \t%i\n", slug, run);
      fprintf(file,"%i \t%i\n", slug, run);
    }
    prev_run = run;
  }
  fclose(file);
}
