#include <TTree.h>
#include <set>

std::set<Double_t> all_scandata1(TTree* t) {
  std::set<Double_t> s;
  t->SetEstimate( t->GetEntries() );
  t->Draw("scandata1","","goff");
  Double_t *raw = t->GetV1();
  for(int i = 0; i < t->GetEntries(); ++i) {
    s.insert( raw[i] );
  }
  return s;
}

#include <iostream>
//template <class T> // ugh, can't get CINT to handle this
void print_set(std::set<Double_t> s) {
  std::set<Double_t>::iterator it = s.begin();
  cout << "The set's members are:"; 
  if (s.size () > 40) {
    for (int i=0 ; i<=10 && it != s.end(); ++i, ++it) {
      cout << " " << *it ;
    }
    cout << " ... " << *it << "\n";
    cout << "(" << s.size() << " entries in total)\n";
  } else {
    while (it != s.end()) {
      cout << " " << *it++;
    }
    cout << "\n" ;
  }
}
