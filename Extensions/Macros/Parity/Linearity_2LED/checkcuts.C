{
  TCut dc[ dcstable->GetEntries() ];
  TCut alldc;
  for (int i =0; i<dcstable->GetEntries(); ++i) {
    dcstable->GetEntry(i);
    dc[i] = TCut(Form("%g<mpsb && mpsb<%g",mpsrange[0],mpsrange[1]));
    alldc = alldc || dc[i] ;
    continue;
    Long64_t bad = t->Draw( "mpsb", dc[i] && acbad, "goff" );
    if (bad)
      cout << "cut " << i << " has " << bad << " bad samples.\n";
  }
}
