/*------------------------------------------------------------------------*//*!

 \defgroup QwGUIComboBox QwGUIComboBox

 \section helpbrowser


*//*-------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*//*!

 \file QwGUIComboBox.cc
 \author Michael Gericke

 \brief Combo box frame class for the QwGUI executable


*//*-------------------------------------------------------------------------*/

#include <QwGUIComboBox.h>

ClassImp(QwGUIComboBox);


QwGUIComboBox::QwGUIComboBox(const TGWindow *p, const char* txt, Int_t id ,UInt_t options, Pixel_t back)
  : TGComboBox(p,txt,id,options,back) 
{
//   strcpy(dMainName, mainname);
//   strcpy(dObjName, objName);
  
//   Connect("IsClosing(const char*)",dMainName,(void*)main,"OnObjClose(const char*)");    
};

QwGUIComboBox::~QwGUIComboBox()
{
//   IsClosing(dObjName);
};

void QwGUIComboBox::PopThisUp()
{
  int      ax, ay;
  Window_t wdummy;
  gVirtualX->TranslateCoordinates(fId, fComboFrame->GetParent()->GetId(),
				  0, fHeight, ax, ay, wdummy);
  // Drop down listbox of combo box should react to pointer motion...
  fListBox->GetContainer()->AddInput(kPointerMotionMask);
  fComboFrame->PlacePopup(ax, ay, fWidth-2, fComboFrame->GetDefaultHeight());
  fDDButton->SetState(kButtonUp);

}
