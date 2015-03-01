void rootlogon()
{
	gSystem->Load("libCint");
  TStyle *pubStyle= new TStyle("publication","publication quality plots style");

//for TPaveText, TLegend etc
  pubStyle->SetFillStyle(0);
// use plain black on white colors
  pubStyle->SetFrameBorderMode(0);
  pubStyle->SetCanvasBorderMode(0);
  pubStyle->SetPadBorderMode(0);
  pubStyle->SetPadColor(0);
  pubStyle->SetCanvasColor(0);
  pubStyle->SetStatColor(0);
  pubStyle->SetFillColor(0);

  // set the paper & margin sizes
  pubStyle->SetPaperSize(20,26);
  pubStyle->SetPadTopMargin(0.06);//0.05);
  //pubStyle->SetPadRightMargin(0.09);//till 26Sep2014
  pubStyle->SetPadRightMargin(0.089);//0.05);
  pubStyle->SetPadBottomMargin(0.15);
  pubStyle->SetPadLeftMargin(0.15);
  //pubStyle->SetPadLeftMargin(0.12);//till 26Sep2014

  // use large Times-Roman fonts
  pubStyle->SetTextFont(132);
  pubStyle->SetTextSize(0.08);
  pubStyle->SetLabelFont(132,"xyz");
  pubStyle->SetLabelSize(0.05,"xyz");
  pubStyle->SetTitleSize(0.05,"xyz");
  pubStyle->SetTitleOffset(1.3,"xyz");
  //pubStyle->SetLabelFont(132,"y");
  //pubStyle->SetLabelFont(132,"z");
  //pubStyle->SetLabelSize(0.05,"y");
  //pubStyle->SetTitleSize(0.05,"y");
  //pubStyle->SetLabelSize(0.05,"z");
  //pubStyle->SetTitleSize(0.06,"z");

  // use bold lines and markers
  pubStyle->SetMarkerStyle(20);
  pubStyle->SetHistLineWidth(1.85);
  pubStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars and y error bar caps
  pubStyle->SetErrorX(0.001);

  // do not display any of the standard histogram decorations
  pubStyle->SetOptTitle(0);
  pubStyle->SetOptStat(1);
  pubStyle->SetOptFit(1);

  pubStyle->SetPalette(1);
  //pubStyle->SetPadBorderSize(3);
  pubStyle->SetFrameLineWidth(3);
  // put tick marks on top and RHS of plots
  //pubStyle->SetPadTickX(1);
  pubStyle->SetPadTickY(1);
}
