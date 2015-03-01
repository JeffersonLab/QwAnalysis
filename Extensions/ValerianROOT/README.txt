Welcome to this awesome README page.  This is the 411 on ValerianROOT
how to use it and why it is so calming and relaxing!

What is it and what is it's goal?

It's funtion is to take all of the output that the tracking autoplots
give us on the tracking website, and put it into a form which we can
use to mass analyze the data.  Looking at for instance a specific group
of runs that have the same conditions over time.

How does it work?

Unicorns and magicical goblins.

Since in real life we don't have those.  Let's take a walk through
ValerianROOT's world.

1) It grabs all the run numbers (scripts/get_run_list.sh) for a
specific pass of the tracking analysis that is on the website
and sorts them from lowest to highest and writes that to a
text file that is stored in the data directory in its
coorresponding pass subdirectiory.

2) It goes through all of the text files on the website, reads
in the information and rewrites only the parts that are needed
for ValerianROOTTreeMaker (scripts/*pl files).  These files are
outputed to the data directory in the coorresponding pass
subdirectory and the coorresponding script (textfile) directory.
  *Example: If you are gathering the  Qtor current, then the
  scripts/QTOR.pl "program" does that and outputs the files it
  makes for each run to the data/pass<number>/QTOR
  directory*

3) We define all the sructures, everything that is stored
in the ROOT file has a structure, this is defined in the include
directory.  Here there is a file that cooresponds to each text
file output.  They consist of a struct that defines what the
data text file output has in it, and a function which will read in
this information (or Get the information - all these functions
start with Get).
   **one slight exception - Run.h this has includes all the other
    stucts as a element of the struct every other type, beside
    just the run number. -- this is for book keeping and so that
    the run number only needs to be stored by one element,
    rather than the n other structures also having to know the
    run number.**

4) Let ValerianROOT know what structures I have defined that
are o-so-special and importatnt to us.  This is done in the
ValerianLinkDef.h file in the linkdef subdirectory.
  *For every new header file one adds the structure must be
  added into this file in order for it to work.  The added line
  will have the form: #pragma link C++ struct MyAngle_t+;

5) The src directory has in it all the .cc files that go with the header
file for an output.  These only have in the Get function that reads in
the data written out.  The main file will call this function and go here
to have it read in the wanted data from the text file for a run.
  *even though Run.h has all the other stucts in it.  The Run.cc
  file is just like any other.  It doesn't get information about
  the other structures*

6) Make ValerianROOT- ValerianROOT.cc is in charge of making
ValerianROOT.  Here ValerianROOT gets to know about all the
structures that I moreover we are including in the ROOT tree.

7) Make the ROOT file for a pass - ValerianROOTTreeMaker.cc
This has the soll function of makeing the ROOT tree, so it reads
in the data through the src/ files in the structures defined in the
include/ files.

8) Run_it_all.sh - this is a fancy little script that will do just that,
Run it all.  It sets the enviromental variables that are needed by all
working parts (debuging level, pass number, ValerianROOT directory,
website location).  Following this is get the list of runs
(exicuting scripts/get_run_list.sh), get all the txt files from the
website to the "right" form for ValerianROOT, makes ValerianROOT,
makes the ROOT file and saves it.

9) Data directory - this directory is the output directory for data
that is taken from the website.  Unless you are making a ValerianROOT
ROOT file this will probably be empty.

10) Missing directory - this directory is the output directory for a list
of the run numbers that are missing for that data stucture that should be
there since it is in the list of runs gathered from the website.
Unless you are making a ValerianROOT ROOT file this will probably be empty.

11) macros directory - here are macros that you can store and share (cause it
is caring or so I was told in Kindergarden) on your analysis of all the
tracking data.  ROOT macros, like looking at the Q2 over time, etc should
be placed here.

12) runlist directory - a place to keep and share runlist that are of interest.
These MUST be in the form of one run per line starting with the lowest
value and going to highest value - a note to meyself  sort -n FILE
Also please make sure that the name is desriptive enough that one can
tell what the runs have in common.

13) README.txt - well if you are here then you have found
the README and by now know what it is for.

14) CMakeLists.txt, cmake directory, etc - CMake stuff... magical black
box - it works I don't have much more to say on that - sorry

15) Compile_ValerianROOT.sh - script that is used to make ValerianROOT so one
has the ValerianROOT exicutable.  Unless you are making a ROOT file, cause say you
have created a new autoplot this is all you should need.

----------------------------------------------------------------------------

List of files (or directory of files or a combination) and
what there goal is:

- README.txt
    This is the file that tell people how to use and how ValerianROOT
    works.  This is the map for when you are editing ValerianROOT
    because you have added new information to the website that
    is useful (cause that is why it is there) and now it is needed for
    mass analysis.

- ValerianROOT.cc
    This main function is for creating the ValerianROOT version

- ValerianROOTTreeMaker.cc
    The main function for creating the ROOT file.

- src/* && include/*
    The sorce files for each data type, and there header files
     ** the header files have more info on what things are if it is not clear
         from the tree. **
    
- magic_line.txt
    This is a magic line that Wouter used to save me from
    pulling out my hair and possibly going crazy...  It does some
    bash magic that saved me from copy and pasting what
    might have been hundreds of lines, and also the spelling
    errors that come with that.

- Run_it_all.sh
    This file sets up all the envitomental variables that are needed
    for ValerianROOT.  These are really just values that are shared
    between the vast majority of files.
    It then runs all of the scripts that generate the files needed for
    ValerianROOTTreeMaker to make the ROOT file.
    Goes in and comilpes the code and runs ValerianROOTTreeMaker
    producing a ROOT file that then others can use
    ** The goals is this ROOT file is also stored on the website
        That way it will be there for everyone and can get ran when
        a new pass is done  **

- Compile_ValerianROOT.sh
    This file is to compile ValerianROOT so that you have the ValerianROOT
    exicutable sn therefore can read the ROOTfiles

- CMakeList.txt && cmake/*
    The cmake stuff.  Needed to make ValerianROOT, nothing
    else to 'c' here. :P

- scripts/get_run_list.sh
    This bash script goes and get the list of every run that is included
    in a said tracking pass and is on the website (which should be all the
    runs in the pass)

- scripts/CountingPackages.sh
    This bash script goes and count the number of packages for a data struture.
    Part of my debugging - it count the number runs that this data stucture
    has data for and the number of package occurances.  One can take output of
    this, compare it right to the output to the ROOT tree, if they
    match oh happy day and do a cleberatory dance.  If not then either there is
    a bug in ValerianROOT somewhere with the data getting read in or parsed or
    Linux has forgotten how to do its basic commands. - One of those nifity things
    that unless you are adding to ValerianROOT you probably won't use.

- scripts/*.pl
    For these perl script there is on for each text file that is generated by
    the tracking pass (in other words on the website).  
    
- runlist/*
    List of runs that are of interest and have simular conditions.
 
- macros/*
    Place to store ValerianROOT macros
    
- data/ && missing/
    For generation of a new ROOT tree output of perl scripts and some debugging 
    goes here.
    
 If you are adding to this, the frame work is there, you should really only need to 
 copy files and modify to fit your needs and then add to other files a few line.
