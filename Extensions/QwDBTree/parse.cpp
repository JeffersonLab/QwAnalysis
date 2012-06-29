#include <TFile.h>
#include <Riostream.h>
#include "parse.h"

using namespace std;

void QwParse::parse(void)
{
    ifstream f (mapfile);

    string buffer;

    if(f.is_open())
    {
        while(f.good())
        {
            // there is probably a more ROOTy way of doing this, but for now
            // this is okay
            getline(f,buffer);
            if(buffer == "") continue;
            if(buffer.find(" ") == string::npos) 
            {
                detectors.push_back(buffer);
                type.push_back("a");
            }
            else
            {
                detectors.push_back(buffer.substr(0, buffer.find(" ")));
                type.push_back(buffer.substr(buffer.find_last_of(" ")+1));
            }
        }
    }
    else cout << "Error opening lumi.map" << endl;
    f.close();
}

Int_t QwParse::num_detectors(void)
{
    return detectors.size();
}

TString QwParse::detector(Int_t num)
{
    return detectors[num];
}

TString QwParse::id_type(Int_t num)
{
    return type[num];
}
