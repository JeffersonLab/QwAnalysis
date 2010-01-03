/*------------------------------------------------------------------------*//*!

 \file TSuperCanvas.cc
 \ingroup Extensions

 \author	Juergen Diefenbach <defi@kph.uni-mainz.de>
 \author	Jeong Han Lee      <jhlee@jlab.org>

 \date		2009-12-27 18:06:23

 \brief        Implementation of ROOT "super" canvas.


*//*-------------------------------------------------------------------------*/

// They're meant to supplement some "missing" methods, which are very useful 
// in everyday live, but anyhow for some reasons not implemented in original ROOT.
//

// This TSuperCanvas was degined for the PVA4 experiment at MAMI, Germany. 
// The second author (Lee) obtained permission from the PVA4 collaboration
// in order to use and modify this code for the Qweak experiment at JLab. 
//

// TO DO...
// * must test this class into "a stand-alone GUI program" with valgrind


// Select a pad with middle mouse button (wheel)
//
// *) "g" : grid on/off
// *) "m" : zoom on/off
// *) "l" : y log scale on/off

// *) "f" : FFT on with a selection of TH1 histogram

// *) measure a distance between two points (A, B)  on a selected pad,
//    Shift + mouse 1 (left) down (click) on A and + click on B
//    The distance will be dumped on "xterminal" as the following example:
//    dist. meas.: dX = 1.20e+01 dY = -6.90e+00 dist = 1.39e+01

// *) more ....


#include "TSuperCanvas.h"
#include <math.h>


ClassImp(TSuperCanvas);

TSuperCanvas::TSuperCanvas(const Char_t* name, const Char_t* title, Int_t ww, Int_t wh, Float_t menumargin)
  : TCanvas(name, title, ww, wh), menu_margin(menumargin) 
{ 
  Initialize(); 
};

TSuperCanvas::TSuperCanvas(const Char_t* name, const Char_t* title, 
			   Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh, Float_t menumargin)
  : TCanvas(name, title, wtopx, wtopy, ww, wh), menu_margin(menumargin) 
{ 
  Initialize(); 
};

TSuperCanvas::~TSuperCanvas()
{
 for (Short_t p=0; p<10; p++)
   {
     if(peakline[p])
       {
	 delete peakline[p];
	 peakline[p] = NULL;
       }
   }
 
 if (MaximizedPad) delete MaximizedPad; MaximizedPad = NULL;
 if (closebutton)  delete closebutton;  closebutton  = NULL;
 if (crossbutton)  delete crossbutton;  crossbutton  = NULL;
 if (eventbutton)  delete eventbutton;  eventbutton  = NULL;
 if (toolbutton)   delete toolbutton;   toolbutton   = NULL;
 if (logxbutton)   delete logxbutton;   logxbutton   = NULL;
 if (logybutton)   delete logybutton;   logybutton   = NULL;
};

void 
TSuperCanvas::Initialize()
{
  closebutton = NULL;
  crossbutton = NULL;
  eventbutton = NULL;
  toolbutton  = NULL;
  logxbutton  = NULL;
  logybutton  = NULL;
  
  SamplingRate = 1.0;
  ScrollRate   = 0.03;

  meas_status  = 0;
  meas_x1      = 0.0;
  meas_x2      = 0.0;
  meas_y1      = 0.0;
  meas_y2      = 0.0;

  MaxmizedPad_xlow = 0.0;
  MaxmizedPad_ylow = 0.0;
  MaxmizedPad_xup  = 0.0;
  MaxmizedPad_yup  = 0.0;

  for (Short_t p=0; p<10; p++)
    {
      peakpos[p]  = 0.0;
      peakline[p] = NULL;
    };

  MaximizedPad = NULL;

};

void TSuperCanvas::ClearPointers()
{
  MaximizedPad = NULL;
};

void 
TSuperCanvas::Divide(Int_t nx, Int_t ny, Float_t xmargin, Float_t ymargin, 
		     Int_t color)
{
  // this method was derived from TCanvas::Divide
  if (!IsEditable()) return;

  if (gThreadXAR) {
    void *arr[7];
    arr[1] = this; 
    arr[2] = (void *)&nx;
    arr[3] = (void *)&ny;
    arr[4] = (void *)&xmargin; 
    arr[5] = (void *)&ymargin; 
    arr[6] = (void *)&color;
    if ((*gThreadXAR)("PDCD", 7, arr, 0)) return;
  }

  TPad *padsav = (TPad*)gPad;
  cd();
  if (nx <= 0) nx = 1;
  if (ny <= 0) ny = 1;

  Int_t    ix,iy;
  Double_t x1,y1,x2,y2;
  Double_t dx,dy;
  TPad     *pad;

  Char_t *name  = new Char_t [strlen(GetName())+6];
  Char_t *title = new Char_t [strlen(GetTitle())+6];
  Int_t n = 0;

  if (color == 0) color = GetFillColor();
  // special bottom margin for menu:
  Float_t margin = menu_margin;
//   if (menumargin>=0.0)
//     margin = menumargin;
  if (xmargin > 0 && ymargin > 0)
    {
      //general case
      dy = 1/Double_t(ny);
      dx = 1/Double_t(nx);
      for (iy=0;iy<ny;iy++)
	{
	  y2 = 1 - iy*dy - ymargin;
	  y1 = y2 - dy + 2*ymargin;
	  if (y1 < 0) y1 = 0;
	  if (y1 > y2) continue;
	  for (ix=0;ix<nx;ix++)
	    {
	      x1 = ix*dx + xmargin;
	      x2 = x1 +dx -2*xmargin;
	      if (x1 > x2) continue;
	      n++;
	      sprintf(name,"%s_%d",GetName(),n);
	      // rescale and shift y1, y2 according to menumargin:
	      Float_t y1m = y1*(1.0-margin)+margin;
	      Float_t y2m = y2*(1.0-margin)+margin;
	      pad = new TPad(name,name,x1,y1m,x2,y2m,color);
	      pad->SetNumber(n);
	      pad->Draw();
	    }
	}
    } 
  else 
    {
      // special case when xmargin <= 0 && ymargin <= 0
      Double_t xl = GetLeftMargin();
      Double_t xr = GetRightMargin();
      Double_t yb = GetBottomMargin();
      Double_t yt = GetTopMargin();
      xl /= (1-xl+xr)*nx;
      xr /= (1-xl+xr)*nx;
      yb /= (1-xl+xr)*ny;
      yt /= (1-xl+xr)*ny;
      SetLeftMargin(xl);
      SetRightMargin(xr);
      SetBottomMargin(yb);
      SetTopMargin(yt);
      dx = (1-xl-xr)/nx;
      dy = (1-yb-yt)/ny;
      Int_t number = 0;
      for (Int_t i=0;i<nx;i++)
	{
	x1 = i*dx+xl;
	x2 = x1 + dx;
	if (i == 0)    x1 = 0;
	if (i == nx-1) x2 = 1-xr;
	for (Int_t j=0;j<ny;j++)
	  {
	    number = j*nx + i +1;
	    y2     = 1 -j*dy -yt;
	    y1     = y2 - dy;

	    if (j == 0)    y2 = 1-yt;
	    if (j == ny-1) y1 = 0;
	    sprintf(name,"%s_%d",GetName(),number);
	    sprintf(title,"%s_%d",GetTitle(),number);
	    pad = new TPad(name,title,x1,y1,x2,y2);
	    pad->SetNumber(number);
	    pad->SetBorderMode(0);
	    if (i == 0)    pad->SetLeftMargin(xl*nx);
	    else           pad->SetLeftMargin(0);
	    pad->SetRightMargin(0);
	    pad->SetTopMargin(0);
	    if (j == ny-1) pad->SetBottomMargin(yb*ny);
	    else           pad->SetBottomMargin(0);
	    pad->Draw();
	  }
	}
    }
  delete [] name;
  delete [] title;

  Modified();
  if (padsav) padsav->cd();

  return;
};

void 
TSuperCanvas::MakeMenu(TSuperCanvas **me, Float_t menumargin)
{
  Float_t x      = 0.0;
  Float_t dx     = 0.2;
  Float_t margin = menu_margin;
  if (menumargin>0.0)
    margin = menumargin;
  if (me)
    closebutton = new TButton("close", Form("((TCanvas*)%p)->Close(); (*((TCanvas**)%p))=NULL;", *me, me), x, 0.0, x+dx/1.5, margin);
  else
    closebutton = new TButton("close", Form("((TCanvas*)%p)->Close();", this), x, 0.0, x+dx/1.5, margin);
  
  closebutton->Draw(); x+=dx/1.5;
  crossbutton = new TButton("crosshair", Form("((TCanvas*)%p)->SetCrosshair(!(((TCanvas*)%p)->HasCrosshair()));", this, this), x, 0.0, x+dx, margin);
  crossbutton->Draw(); x+=dx;
  eventbutton = new TButton("event status", Form("((TCanvas*)%p)->ToggleEventStatus();", this), x, 0.0, x+dx, margin);
  eventbutton->Draw(); x+=dx;
  toolbutton  = new TButton("toolbar", Form("((TCanvas*)%p)->ToggleToolBar();", this), x, 0.0, x+dx, margin);
  toolbutton->Draw(); x+=dx;

  logxbutton  = new TButton("lg x", "int val=gPad->GetLogx(); gPad->SetLogx(!val);", x, 0.0, x+dx/2.25, margin);
  logxbutton->Draw(); x+=dx/2.25;
  logybutton  = new TButton("lg y", "int val=gPad->GetLogy(); gPad->SetLogy(!val);", x, 0.0, x+dx/2.25, margin);
  logybutton->Draw(); x+=dx/2.25;

  return;
};


void 
TSuperCanvas::HandleInput(EEventType event, Int_t px, Int_t py)
{
  //printf("event=%d @( %d ; %d )\n", event, px, py);
  //-----------------------------------------------------------------//
  if (event==7) // i.e. mouse button 1 down + shift pressed
    {
      Measure(px, py);
      return;
    };
  //-----------------------------------------------------------------//
  if (event==kKeyPress)
    if ((py==4114)||(py==4116))
      {
	TObject *selected = gPad->GetSelected();
	TAxis   *axis     = NULL;
	
	if (strstr(selected->IsA()->GetName(), "TH1"))
	  axis = ((TH1*)selected)->GetXaxis();
	//if (strstr(selected->IsA()->GetName(), "TH2"))
	//axis = ((TH2*)selected)->GetXaxis();
	if (strstr(selected->IsA()->GetName(), "TGraph"))
	  axis = ((TGraph*)selected)->GetXaxis();
	
	if (axis==NULL)
	  return;
	
	Int_t    first = axis->GetFirst();
	Int_t    last  = axis->GetLast();
	Int_t    dist  = Int_t( Double_t (last-first)*ScrollRate);

	if (py==4114) dist *= -1;
	
	Double_t low   = axis->GetBinLowEdge(first+dist);
	Double_t up    = axis->GetBinUpEdge(last+dist);
	
	axis->SetRangeUser(low, up);
	
	if (strstr(selected->IsA()->GetName(), "TH1"))
	  ((TH1*)selected)->Draw();
	//if (strstr(selected->IsA()->GetName(), "TH2"))
	//  ((TH2*)selected)->Draw();
	if (strstr(selected->IsA()->GetName(), "TGraph"))
	  ((TGraph*)selected)->Draw();
	
	Update();
      };
  //-----------------------------------------------------------------//
  if (event==kKeyPress)
    {
      // 'px' contains the ASCII-code of the pressed key:
      if (px=='f') // show FFT of selected histogram
	{
	  TObject *graphics = gPad->GetSelected();
	  if (strstr(graphics->IsA()->GetName(), "TH1"))
	    {
	      TH1* ffth=NULL;
	      ffth = ((TH1*)graphics)->FFT(ffth, (Option_t*)"MAG");
	      if (ffth)
		{
		  ffth->SetTitle(Form("%s sepctrum", (TH1*)graphics->GetTitle()));
		  ffth->Draw();
		}
	      else
		{
		  printf("%s TSuperCanvas: Sorry, not able to create FFT, check ROOT installation for FFT support...%s\n", BOLD, NORMAL);
		}
	      TAxis *xa = ffth->GetXaxis();
	      xa->SetRangeUser(xa->GetBinLowEdge(1), xa->GetBinUpEdge(xa->GetNbins())/2.0);
	      gPad->SetLogy();
	      Update();
	      printf("FFT for >%s< generated...\n", graphics->GetName());
	    }
	  else
	    printf("%s TSuperCanvas: Sorry, no FFT possible for non-TH1 graphics objects...%s\n", BOLD, NORMAL);
	  return;
	};
      //-----------------------------------------------------------------//
      if ( (px>='0') && (px<='9') )      //     SAVE PEAK POSIITONS      //
	{
	  if (meas_status==1) // user has selected coordinated by shift-click on pad?
	    {
	      Int_t p = px-'0';
	      peakpos[p]  = meas_x1;
	      peakline[p] = new TLine(peakpos[p], 0., peakpos[p], meas_y1);
	      if ( p==0 )
		{ 
		  printf("\r pedestal position saved...\n");
		  peakline[p]->SetLineColor(kBlue);
		}
	      else
		{
		  printf("\r peak position %d saved...\n", p);
		  peakline[p]->SetLineColor(kRed);
		};
	      peakline[p]->Draw();
	      meas_status=0; // reset measurement status
	      Update();
	    };
	  return;
	};
      //-----------------------------------------------------------------//
      if (px=='c')                        //      DELETE PEAKS           //
	{
	  for (Int_t p=0; p<10; p++)
	    {
	      if (peakline[p]) 
		{
		  delete peakline[p];
		  peakpos[p]=0.0;
		}
	      printf("peak positions have been deleted...\n");
	    };
	  Update();
	};
      //-----------------------------------------------------------------//
      if (px=='l')                        //          LOG Y              //
	{
	  gPad->SetLogy(!(gPad->GetLogy()));
	  Update();
	};
      //-----------------------------------------------------------------//
      if (px=='g')                        //           GRID              //
	{
	  gPad->SetGridx(!(gPad->GetGridx()));
	  gPad->SetGridy(!(gPad->GetGridy()));
	  Update();
	};
      //-----------------------------------------------------------------//
      if (px=='C')                        //        CONFIGURE            //
	{
	  Configure();
	};
      //-----------------------------------------------------------------//
      if (px=='m')                        //         MAGNIFY             //
	{
	  TVirtualPad *thispad = gPad;
	  // If there's already a maximized pad, shrink it to normal size again:
	  if (MaximizedPad)
	    {
	      //printf(" shrinking old maximized pad to ( %lf ; %lf ) ( %lf ; %lf )...\n",
	      //     MaxmizedPad_xlow, MaxmizedPad_ylow, MaxmizedPad_xup, MaxmizedPad_yup);
	      MaximizedPad->SetPad(MaxmizedPad_xlow, MaxmizedPad_ylow, MaxmizedPad_xup, MaxmizedPad_yup);
	    };
	  // If the former maxmized pad is no equal to the selected pad, maximize selected pad:
	  if (MaximizedPad!=thispad)
	    {
	      //printf(" maximizing selected pad...\n");
	      // store original size of selected pad:
	      thispad->GetPadPar(MaxmizedPad_xlow, MaxmizedPad_ylow, MaxmizedPad_xup, MaxmizedPad_yup);
	      thispad->SetPad(0.05, 0.05, 0.95, 0.95);
	      thispad->Pop();
	      MaximizedPad = thispad;
	    }
	  else
	    MaximizedPad = NULL;
	  thispad->Update();
	};
    };
  //-----------------------------------------------------------------//
  // finally, if not yet returned, call default HandleInput method:
  TCanvas::HandleInput(event, px, py);
  return;
};

void 
TSuperCanvas::Measure(Int_t px, Int_t py)
{
  
  Double_t dx, dy;
  
  TPad *pad = Pick(px, py, NULL);
  pad->DrawCrosshair();
  
  switch(meas_status)
    {
    case 0:
      meas_x1     = pad->XtoPad(pad->AbsPixeltoX(px));
      meas_y1     = pad->YtoPad(pad->AbsPixeltoY(py));
      meas_status = 1;
      printf("\n");
      printf("dist. meas.: "); fflush(stdout);
      break;
    case 1:
      meas_x2     = pad->XtoPad(pad->AbsPixeltoX(px));
      meas_y2     = pad->XtoPad(pad->AbsPixeltoY(py));
      dx          = meas_x2-meas_x1;
      dy          = meas_y2-meas_y1;
      meas_status = 0;
      printf("dX = %s%6.2e%s dY = %s%6.2e%s dist = %s%6.2e%s\n", 
	     BOLD, dx, NORMAL, BOLD, dy, NORMAL, BOLD, sqrt(dx*dx+dy*dy), NORMAL);
      break;
    };

  return;
};

void 
TSuperCanvas::Configure()
{
  Char_t input[512];
  printf(" --- Configuration of TSuperCanvas: ---\n\n");
  printf("Sampling rate: (currently %6.2e Hz) ", SamplingRate); 
  scanf("%s", input);
  
  if (atof(input)>0.0) SamplingRate = atof(input);
  printf("  SamplingRate = %6.2e Hz\n\n", SamplingRate);
  printf("Scroll rate: (currently %6.2e %%) ", ScrollRate); 
  scanf("%s", input);

  if ((atof(input)>0.0)&&(atof(input)<100.0))   SamplingRate = atof(input)/100.0;
  printf("  ScrollRate = %6.2e %%\n", ScrollRate*100.0);
  return;
};


#if defined(__MAKECINT__)
#pragma link C++ class TSuperCanvas;
#endif
