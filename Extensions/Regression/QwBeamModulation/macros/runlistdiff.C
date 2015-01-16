{
#include<vector>

  ifstream file1("slugOrderedRunlist.dat");
  ifstream file2("slugOrderedRunlist2.dat");
  int slug, run;
  vector<vector<int> >list1, list2;
  list1.resize(2);
  list2.resize(2);
  int n1 = 0, n2 = 0;
  while(!(file1.eof())){
    file1>>slug>>run;
    list1[0].push_back(slug);
    list1[1].push_back(run);
    file1.peek();
    ++n1;
  }
  n1--;
  std::cout<<"Size 1: "<<n1<<" "<<list1[0].size()<<std::endl;
  while(!(file2.eof())){
    file2>>slug>>run;
    list2[0].push_back(slug);
    list2[1].push_back(run);
    file2.peek();
    ++n2;
  }
  n2--;
  std::cout<<"Size 2: "<<n2<<std::endl;

  std::cout<<"BMOD runs not in DB\n";
  for(int i=0;i<n1;++i){
    int j=0;
    while(list1[1][i]!=list2[1][j]&&j<n2){
      if(list1[0][i]<list2[0][j]||(j==n2-1&&list1[1][i]!=list2[1][j])){
	std::cout<<list1[0][i]<<"  "<<list1[1][i]<<std::endl;
	break;
      }
      ++j;
    }
  }

  cout<<"DB runs not in BMOD\n";
  for(int i=0;i<n2;++i){
    int j=0;
    while(list2[1][i]!=list1[1][j]&&j<n1){
      if(list2[0][i]<list1[0][j]||(j==n1-1&&list2[1][i]!=list1[1][j])){
	std::cout<<list2[0][i]<<"  "<<list2[1][i]<<std::endl;
	break;
      }
      ++j;
    }
  }




}
