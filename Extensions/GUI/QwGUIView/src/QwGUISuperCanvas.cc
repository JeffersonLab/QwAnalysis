/*------------------------------------------------------------------------*//*!

 \file QwGUISuperCanvas.cc
 \ingroup Extensions

 \author	Juergen Diefenbach <defi@kph.uni-mainz.de>
 \author	Jeong Han Lee      <jhlee@jlab.org>

  \date		2010-01-25 14:45:50

 \brief        Implementation of ROOT "super" canvas.


*//*-------------------------------------------------------------------------*/

// See detailed history in its header file

#include "QwGUISuperCanvas.h"


#include <cmath>
#include <cstdlib>

ClassImp(QwGUISuperCanvas);

// QwGUISuperCanvas::QwGUISuperCanvas(const Char_t* name, const Char_t* title, Int_t ww, Int_t wh, Float_t menumargin)
//   : TCanvas(name, title, ww, wh), menu_margin(menumargin) 
// { 
//   Initialize(); 
// };

// QwGUISuperCanvas::QwGUISuperCanvas(const Char_t* name, const Char_t* title, 
// 			   Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh, Float_t menumargin)
//   : TCanvas(name, title, wtopx, wtopy, ww, wh), menu_margin(menumargin) 
// { 
//   Initialize(); 
// };

QwGUISuperCanvas::QwGUISuperCanvas(const Char_t* name, const Char_t* title, Int_t ww, Int_t wh)
  : TCanvas(name, title, ww, wh)
{ 
  Initialize(); 
};

QwGUISuperCanvas::QwGUISuperCanvas(const Char_t* name, const Char_t* title, 
				   Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh)
  : TCanvas(name, title, wtopx, wtopy, ww, wh)
{ 
  Initialize(); 
};



QwGUISuperCanvas::QwGUISuperCanvas(const char* name, Int_t ww, Int_t  wh, Int_t  winid)
  : TCanvas(name, ww, wh, winid)
{ 
  Initialize(); 
};



QwGUISuperCanvas::~QwGUISuperCanvas()
{
  //   for (Short_t p=0; p<10; p++)
  //     {
  //       if(peakline[p])
  // 	{
  // 	  delete peakline[p];
  // 	  peakline[p] = NULL;
  // 	}
  //     }
 
  //  if (closebutton)  delete closebutton;  closebutton  = NULL;
  //  if (crossbutton)  delete crossbutton;  crossbutton  = NULL;
  //  if (eventbutton)  delete eventbutton;  eventbutton  = NULL;
  //  if (toolbutton)   delete toolbutton;   toolbutton   = NULL;
  //  if (logxbutton)   delete logxbutton;   logxbutton   = NULL;
  //  if (logybutton)   delete logybutton;   logybutton   = NULL;

  

};

void 
QwGUISuperCanvas::Initialize()
{
  //   closebutton = NULL;
  //   crossbutton = NULL;
  //   eventbutton = NULL;
  //   toolbutton  = NULL;
  //   logxbutton  = NULL;
  //   logybutton  = NULL;
  
  //  SamplingRate = 1.0;
  //  ScrollRate   = 0.03;
  meas_status  = 0;
  meas_x1      = 0.0;
  meas_x2      = 0.0;
  meas_y1      = 0.0;
  meas_y2      = 0.0;

  menu_margin  = 0.0;

  MaxmizedPad_xlow = 0.0;
  MaxmizedPad_ylow = 0.0;
  MaxmizedPad_xup  = 0.0;
  MaxmizedPad_yup  = 0.0;
  
  //   for (Short_t p=0; p<10; p++)
  //     {
  //       peakpos[p]  = 0.0;
  //       peakline[p] = NULL;
  //     };
  
  MaximizedPad = NULL;

  plain = new TStyle("Plain","Plain Style (no colors/fill areas)");
 
  plain->SetCanvasBorderMode(0);
  plain->SetPadBorderMode(0);
  plain->SetPadColor(0);
  plain->SetCanvasColor(0);
  plain->SetTitleColor(0);
  plain->SetStatColor(0);
  plain->cd(); 
  
};

void 
QwGUISuperCanvas::ClearPointers()
{
  MaximizedPad = NULL;
};

void 
QwGUISuperCanvas::Divide(Int_t nx, Int_t ny, Float_t xmargin, Float_t ymargin, 
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
  if (xmargin > 0 && ymargin > 0) {
    //general case
    dy = 1/Double_t(ny);
    dx = 1/Double_t(nx);
    for (iy=0;iy<ny;iy++) {
      y2 = 1 - iy*dy - ymargin;
      y1 = y2 - dy + 2*ymargin;
      if (y1 < 0) y1 = 0;
      if (y1 > y2) continue;
      for (ix=0;ix<nx;ix++) {
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
  else  {
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
    for (Int_t i=0;i<nx;i++) {
      x1 = i*dx+xl;
      x2 = x1 + dx;
      if (i == 0)    x1 = 0;
      if (i == nx-1) x2 = 1-xr;
      for (Int_t j=0;j<ny;j++) {
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

// void 
// QwGUISuperCanvas::MakeMenu(QwGUISuperCanvas **me, Float_t menumargin)
// {
//   Float_t x      = 0.0;
//   Float_t dx     = 0.2;
//   Float_t margin = 0.0;
//   margin = menu_margin;

//   if (menumargin>0.0) margin = menumargin;
//   if (me)
//     {
//       closebutton = new TButton("close", Form("((TCanvas*)%p)->Close(); (*((TCanvas**)%p))=NULL;", *me, me), x, 0.0, x+dx/1.5, margin);
//     }
//   else
//     {
//       closebutton = new TButton("close", Form("((TCanvas*)%p)->Close();", this), x, 0.0, x+dx/1.5, margin);
//     }
  
//   closebutton->Draw(); x+=dx/1.5;
//   crossbutton = new TButton("crosshair", Form("((TCanvas*)%p)->SetCrosshair(!(((TCanvas*)%p)->HasCrosshair()));", this, this), x, 0.0, x+dx, margin);
//   crossbutton->Draw(); x+=dx;
//   eventbutton = new TButton("event status", Form("((TCanvas*)%p)->ToggleEventStatus();", this), x, 0.0, x+dx, margin);
//   eventbutton->Draw(); x+=dx;
//   toolbutton  = new TButton("toolbar", Form("((TCanvas*)%p)->ToggleToolBar();", this), x, 0.0, x+dx, margin);
//   toolbutton->Draw(); x+=dx;
  
//   logxbutton  = new TButton("lg x", "int val=gPad->GetLogx(); gPad->SetLogx(!val);", x, 0.0, x+dx/2.25, margin);
//   logxbutton->Draw(); x+=dx/2.25;
//   logybutton  = new TButton("lg y", "int val=gPad->GetLogy(); gPad->SetLogy(!val);", x, 0.0, x+dx/2.25, margin);
//   logybutton->Draw(); x+=dx/2.25;

//   return;
// };


void 
QwGUISuperCanvas::HandleInput(EEventType event, Int_t px, Int_t py)
//  :TCanvas::HandleInput(event, px, py)
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
    {
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
	  
	  if (axis==NULL)  return;
	  

	  scroll_rate = 0.03;

	  Int_t    first = axis->GetFirst();
	  Int_t    last  = axis->GetLast();
	  Int_t    dist  = Int_t( Double_t (last-first)*scroll_rate);
	  
	  if (py==4114) dist *= -1;
	  
	  Double_t low   = axis->GetBinLowEdge(first+dist);
	  Double_t up    = axis->GetBinUpEdge(last+dist);
	  
	  axis->SetRangeUser(low, up);
	  
	  if (strstr(selected->IsA()->GetName(), "TH1"))    ((TH1*)selected)->Draw();
	  // if (strstr(selected->IsA()->GetName(), "TH2"))   ((TH2*)selected)->Draw();
	  if (strstr(selected->IsA()->GetName(), "TGraph")) ((TGraph*)selected)->Draw();
	  Update();
	};
      //-----------------------------------------------------------------//
      // 'px' contains the ASCII-code of the pressed key:
      if (py=='f') // show FFT of selected histogram
	{
	  DiscreteFourierTransform(px, py);
	};
      //-----------------------------------------------------------------//
      //     if ( (px>='0') && (px<='9') )      //     SAVE PEAK POSIITONS      //
      // 	{
      // 	  if (meas_status==1) // user has selected coordinated by shift-click on pad?
      // 	    {
      // 	      Int_t p = px-'0';
      // 	      peakpos[p]  = meas_x1;
      // 	      peakline[p] = new TLine(peakpos[p], 0., peakpos[p], meas_y1);
      // 	      if ( p==0 )
      // 		{ 
      // 		  printf("\r pedestal position saved...\n");
      // 		  peakline[p]->SetLineColor(kBlue);
      // 		}
      // 	      else
      // 		{
      // 		  printf("\r peak position %d saved...\n", p);
      // 		  peakline[p]->SetLineColor(kRed);
      // 		};
      // 	      peakline[p]->Draw();
      // 	      meas_status=0; // reset measurement status
      // 	      Update();
      // 	    };
      // 	};
      //-----------------------------------------------------------------//
      //       if (px=='c')                        //      DELETE PEAKS           //
      // 	{
      // 	  for (Int_t p=0; p<10; p++)
      // 	    {
      // 	      if (peakline[p]) 
      // 		{
      // 		  delete peakline[p];
      // 		  peakpos[p]=0.0;
      // 		}
      // 	      printf("peak positions have been deleted...\n");
      // 	    };
      // 	  Update();
      // 	};
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
      //       if (px=='C')                        //        CONFIGURE            //
      // 	{
      // 	  Configure();
      // 	};
      //-----------------------------------------------------------------//
      if (px=='m')                        //         MAGNIFY             //
	{
	  MagnifyPad();
	  
	};
      //-----------------------------------------------------------------//
      if(( py=='x') || (py == 'y') ) // PROJECTION X, Y of TH2
	{
	  SliceTH2(px,py);
	  Update();
	}
      if (px=='s')
	{
	  ShowPadSummary();
	}
      return;
    };
  //-----------------------------------------------------------------//
  // finally, if not yet returned, call default HandleInput method:
  TCanvas::HandleInput(event, px, py);
  return;
};

void 
QwGUISuperCanvas::Measure(Int_t px, Int_t py)
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

// void 
// QwGUISuperCanvas::Configure()
// {
//   Char_t input[512];
//   SamplingRate = 1.0;
//   printf(" --- Configuration of QwGUISuperCanvas: ---\n\n");
//   printf("Sampling rate: (currently %6.2e Hz) ", SamplingRate); 
//   scanf("%s", input);
  
//   if (atof(input)>0.0) SamplingRate = atof(input);
//   printf("  SamplingRate = %6.2e Hz\n\n", SamplingRate);
//   printf("Scroll rate: (currently %6.2e %%) ", ScrollRate); 
//   scanf("%s", input);

//   if ((atof(input)>0.0)&&(atof(input)<100.0))   SamplingRate = atof(input)/100.0;
//   printf("  ScrollRate = %6.2e %%\n", ScrollRate*100.0);
//   return;
// };



// "x" (120,120)  with selecting TH1 object
//             Do X projection 
// "CTRL + x" (24, 120) with selecting TH1 object
//               Close the canvas from the result of Projection X
//
// "y" (121,121)  with selecting TH1 object
//             Do Y projection 
// "CTRL + y" (25, 121) with selecting TH1 object
//               Close the canvas from the result of Projection Y
void 
QwGUISuperCanvas::SliceTH2(Int_t px, Int_t py)
{

  SetCurrentPad();
  TObject *current_obj = gPad -> GetSelected();
  TPad    *current_pad = (TPad*) gPad;

  if( (current_obj->InheritsFrom("TH2")) )
    {
      QwGUISuperCanvas *slice_canvas = (QwGUISuperCanvas*)gROOT->GetListOfCanvases()->FindObject("slice_canvas");
      
      if(slice_canvas) 
	{
	  slice_canvas -> Close(); delete slice_canvas; slice_canvas = NULL;
	}
      
      slice_canvas = new QwGUISuperCanvas("slice_canvas","Projection Canvas",20,20,640,480);
      
      if( (px==24) || (px==25) )
	{
	  slice_canvas -> Close(); delete slice_canvas; slice_canvas = NULL;
	  SetBackBeforePad(current_pad);
	  return;
	}
      else
	{
	  Int_t    pxy        = 0;
	  Float_t upxy        = 0.0;
	  Float_t   xy        = 0.0;
	  Int_t bin_number    = 0;
	  TH2 *selected_hist2 = (TH2*) current_obj;  
	  TH1D *sliced_hist   = NULL;
	  
	  if( py == 'x' )
	    {
	      pxy         = current_pad -> GetEventY();
	      upxy        = current_pad -> AbsPixeltoY(pxy);
	      xy          = current_pad -> PadtoY(upxy);
	      bin_number  = selected_hist2->GetYaxis()->FindBin(xy);
	      sliced_hist = selected_hist2->ProjectionX("", bin_number, bin_number);
	      sliced_hist -> GetXaxis()->SetTitle( selected_hist2->GetXaxis()->GetTitle() );
	      sliced_hist -> SetTitle(Form("%s Projection of bin = %d", selected_hist2-> GetTitle(), bin_number));
	    }
	  if( py == 'y')
	    {
	      pxy         = current_pad -> GetEventX();
	      upxy        = current_pad -> AbsPixeltoX(pxy);
	      xy          = current_pad -> PadtoX(upxy);
	      bin_number  = selected_hist2->GetXaxis()->FindBin(xy);
	      sliced_hist = selected_hist2->ProjectionY("", bin_number, bin_number);
	      sliced_hist -> GetXaxis()->SetTitle( selected_hist2->GetYaxis()->GetTitle() );
	      sliced_hist -> SetTitle(Form("%s Projection of bin = %d", selected_hist2-> GetTitle(), bin_number));
	    }
	  
	  //      printf("pxy %3d, upxy %4.2f xy %4.2f bin %d\n", pxy, upxy, xy, bin_number);
	  
	  sliced_hist  -> SetName("Projection");
	  sliced_hist  -> SetLineColor(kRed);
	  sliced_hist  -> SetFillColor(38);
	  sliced_hist  -> Draw();

	  //slice_canvas -> ToggleToolTips();
	  slice_canvas -> Update();
	}
    }
  else
    {
      printf("%s QwGUISuperCanvas: Sorry, no Slice possible for non-TH2 objects...%s\n", BOLD, NORMAL);
    }
  
  SetBackBeforePad(current_pad);
  return;
};





// "f" (102,102)  with selecting TH1 object
//               Do  FFT of the selected TH1 (mouse over)
// "CTRL + f" (6, 102) with selecting TH1 object
//               Close the FFT result canvas
//
// It is not allowed to do FFT again on the result of FFT
// because I don't know how to get back the original Pad 
// between difference canvases

void 
QwGUISuperCanvas::DiscreteFourierTransform(Int_t px, Int_t py)
{
  if( py != 'f')  
    {
      printf("something wrong \n");
      return;
    }

  SetCurrentPad();
  
  TObject *current_obj = gPad -> GetSelected();
  TPad    *current_pad = (TPad*) gPad;
  
  //   Int_t current_canvas_id = gPad->GetCanvasID();
  //   Int_t open_canvas_id = 0;
  //   PrintPad(current_pad);
  // printf("%s name %s\n", current_obj->GetTitle(), current_obj->GetName());

  if( strstr(current_obj->GetName(), "FFT") )
    {
      printf("%s QwGUISuperCanvas: Sorry, no FFT possible for a FFT result...%s\n", BOLD, NORMAL);
      SetBackBeforePad(current_pad);
      return;
    }
  if( (current_obj->InheritsFrom("TH1")) )
    {
      QwGUISuperCanvas *dft_canvas = (QwGUISuperCanvas*)gROOT->GetListOfCanvases()->FindObject("dft_canvas");
      
      if(dft_canvas)
	{
	  dft_canvas -> Close(); delete dft_canvas; dft_canvas = NULL;
	}
      
      dft_canvas = new QwGUISuperCanvas("dft_canvas","Projection Canvas",20,20,640,480);
      //       open_canvas_id = dft_canvas -> GetCanvasID();
      
      if( (px==6) ) // CTRL+f (6,102)
	{
	  dft_canvas -> Close(); delete dft_canvas; dft_canvas = NULL;
	  SetBackBeforePad(current_pad);
	  return;
	}
      else
	{
	  
	  
	  TH1 *hist    = (TH1*) current_obj;
	  TH1 *fft_out = NULL;
	  fft_out = hist->FFT(fft_out, "Mag R2C");
	  
	  if (fft_out)
	    {
	      fft_out -> SetTitle( Form("%s sepctrum", hist->GetTitle()) );
	      fft_out -> SetName("FFT");
	      fft_out -> SetLineColor(kRed);
	      fft_out -> Draw();
	      
	      TAxis *xa = fft_out->GetXaxis();
	      xa -> SetRangeUser(xa->GetBinLowEdge(1), xa->GetBinUpEdge(xa->GetNbins())/2.0);
	      //  gPad->SetLogy();
	      //  dft_canvas -> ToggleToolTips();
	      dft_canvas -> Update();
	    }
	  else
	    {  
	      printf("%s QwGUISuperCanvas: Sorry, not able to create FFT, check ROOT installation for FFT support...%s\n", BOLD, NORMAL);
	    }
	}
    }
  else
    {
      printf("%s QwGUISuperCanvas: Sorry, no FFT possible for non-TH1 graphics objects...%s\n", BOLD, NORMAL);
    }
  
  //  if(current_canvas_id == open_canvas_id)
  //     SetBackBeforePad((TPad*)gPad);
  //   else
  SetBackBeforePad(current_pad);
  
  return;
  
}


void 
QwGUISuperCanvas::MagnifyPad()
{
  SetCurrentPad();
  
  // If there's already a maximized pad, shrink it to normal size again:
  TVirtualPad *thispad = gPad;
  if ( MaximizedPad )
    {
      //printf(" shrinking old maximized pad to ( %lf ; %lf ) ( %lf ; %lf )...\n",
      //     MaxmizedPad_xlow, MaxmizedPad_ylow, MaxmizedPad_xup, MaxmizedPad_yup);
      MaximizedPad->SetPad(MaxmizedPad_xlow, MaxmizedPad_ylow, MaxmizedPad_xup, MaxmizedPad_yup);
    };
  
  // If the former maxmized pad is no equal to the selected pad, maximize selected pad:
  if ( MaximizedPad!=thispad )
    {
      //printf(" maximizing selected pad...\n");
      // store original size of selected pad:
      thispad->GetPadPar(MaxmizedPad_xlow, MaxmizedPad_ylow, MaxmizedPad_xup, MaxmizedPad_yup);
      thispad->SetPad(0.05, 0.05, 0.95, 0.95);
      thispad->Pop();
      MaximizedPad = thispad;
    }
  else
    {
      MaximizedPad = NULL;
    }
  
  this -> Update();
  return;
}

void
QwGUISuperCanvas::SetBackBeforePad(TPad* pad)
{
  this -> cd (pad ->GetNumber());
  this -> SetSelectedPad( pad );
  this -> SetClickSelectedPad( pad );
  this -> Update();
  return;
};

// Thre are three types of Pads in one canvas
// 1) fSelectedPad      -> following the mouse point over the canvas
// 2) fClickSelectedPad -> once one click the left mouse button over the pad
// 3) gPad              -> after cd()...
// I want the current pad will be a pad when a key is pressed over this pad.

void
QwGUISuperCanvas::SetCurrentPad(Bool_t debug)
{
  TVirtualPad *spad = NULL;
  Int_t scanvas_id  = 0;

  spad = this ->GetSelectedPad();
  scanvas_id = spad->GetCanvasID();
  // make the SelectedPad and the ClickSelectedPad are the same.
  this-> SetClickSelectedPad((TPad*)spad);
  
  if(debug)
    {
      printf(" CANVAS ID %d    ----------\n", scanvas_id); 
      printf("     SelectedPad ----------\n"); PrintPad(spad);
      printf("ClickSelectedPad ----------\n"); PrintPad(fClickSelectedPad);
      printf("Before Move the Current Pad\n"); PrintPad((TVirtualPad*)gPad);
    }
  // move the Current Pad to the selected Pad (follows the mouse point)
  this -> cd( spad->GetNumber() );
  // update the canvas.
  this -> Update();
  
  if(debug)
    {
      printf("After Move the Current Pad \n"); PrintPad((TVirtualPad*)gPad);
    }
  
  return ;
}

void
QwGUISuperCanvas::PrintPad(TVirtualPad* pad)
{
  TObject     *obj  = NULL;
  obj = pad -> GetSelected();
  TVirtualPad *mpad = NULL;
  mpad = pad -> GetMother();
  
  printf("*** Mom Pad %30s : Num %2d CvsID %2d EvID %2d \n",
	 mpad->GetName(), mpad->GetNumber(), mpad->GetCanvasID(), mpad->GetEvent());
  printf("***     Pad %30s : Num %2d CvsID %2d EvID %2d : obj %s\n", 
	 pad->GetName(), pad->GetNumber(), pad-> GetCanvasID(), pad->GetEvent(),
	 obj->GetName());
  
  //  printf(" %s pad %d canvas id %d Event %d &s :  obj %s\n", 
  //	 mpad->GetNumber(), mpad-> GetCanvasID(), mpad->GetEvent(), mpad->GetName()
  //	 mobj->GetName());
  
  
}

void
QwGUISuperCanvas::SetAutomaticFontSize()
{
  return;
}

void
QwGUISuperCanvas::ShowPadSummary()
{
  SetCurrentPad();
  TPad    *current_pad = (TPad*) gPad;
  Int_t pad_id = current_pad -> GetNumber();
  //  current_pad-> SetToolTipText(Form("id %d", pad_id));
  printf("id %d", pad_id);

  return;
}

#if defined(__MAKECINT__)
#pragma link C++ class QwGUISuperCanvas;
#endif
