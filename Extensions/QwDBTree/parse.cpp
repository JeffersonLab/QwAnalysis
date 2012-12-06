#include <TFile.h>
#include <Riostream.h>
#include "parse.h"

using namespace std;

/* Constructors. */
QwParse::QwParse(TString filename) {
    mapfile = filename;
}

/*
 * Method to parse simple mapfiles. It first gets a line, and skips newlines.
 *
 * FIXME: Skip comments.
 *
 * Then it check if there are one or two columns. If only one column, it
 * assumes that they are asymmetries and sets the detector name. If there are
 * two, it sets the detector name and type.
 */
void QwParse::parse(void) {
    /* Open the mapfile. */
    ifstream f(mapfile);
    /* Just a buffer for getline. */
    string buffer;

    /*
     * First check if the file is actually open. Then, check if there are one
     * column or two. If there is one, set the type to "a" and set the detector
     * name.
     */
    if(f.is_open()) {
        while(f.good()) {
            getline(f,buffer);
            if(buffer == "") continue;
            if(buffer.find(" ") == string::npos) {
                detectors.push_back(buffer);
                type.push_back("a");
            }
            /* If there are two, get the detector name and type. */
            else {
                detectors.push_back(buffer.substr(0, buffer.find(" ")));
                type.push_back(buffer.substr(buffer.find_last_of(" ")+1));
            }
        }
    }
    /* If its not open, print an error. */
    else cout << "Error opening lumi.map" << endl;
    f.close();
}

/* Method to return number of detectors in a mapfile. */
Int_t QwParse::num_detectors(void) {
    return detectors.size();
}

/* Method to return detector for given index. */
TString QwParse::detector(Int_t num) {
    return detectors[num];
}

/* Method to return detector type for a given index. */
TString QwParse::id_type(Int_t num) {
    return type[num];
}

/* Method to return all detectors in a vector of strings. */
vector<TString> QwParse::ret_detector() {
    return detectors;
}
