// @(#)root/graf:$Id$
// Author: Rene Brun   12/12/94

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include <stdlib.h>

#include "Riostream.h"
#include "TROOT.h"
#include "TLine.h"
#include "TVirtualPad.h"
#include "TClass.h"
#include "TVirtualX.h"
#include "TMath.h"


ClassImp(TLine)


//______________________________________________________________________________
//
// A Graphical line
//


//______________________________________________________________________________
TLine::TLine(): TObject(), TAttLine()
{
   // Line default constructor.

   fX1=0; fY1=0; fX2=0; fY2=0;
}


//______________________________________________________________________________
TLine::TLine(Double_t x1, Double_t y1, Double_t x2, Double_t  y2)
      :TObject(), TAttLine()
{
   // Line normal constructor.

   fX1=x1; fY1=y1; fX2=x2; fY2=y2;
}


//______________________________________________________________________________
TLine::~TLine()
{
   // Line default destructor.
}


//______________________________________________________________________________
TLine::TLine(const TLine &line) : TObject(line), TAttLine(line)
{
   // Line copy constructor.

   fX1=0; fY1=0; fX2=0; fY2=0;
   ((TLine&)line).Copy(*this);
}


//______________________________________________________________________________
void TLine::Copy(TObject &obj) const
{
   // Copy this line to line.

   TObject::Copy(obj);
   TAttLine::Copy(((TLine&)obj));
   ((TLine&)obj).fX1 = fX1;
   ((TLine&)obj).fY1 = fY1;
   ((TLine&)obj).fX2 = fX2;
   ((TLine&)obj).fY2 = fY2;
}


//______________________________________________________________________________
Int_t TLine::DistancetoPrimitive(Int_t px, Int_t py)
{
   // Compute distance from point px,py to a line.

   if (!TestBit(kLineNDC)) return DistancetoLine(px,py,gPad->XtoPad(fX1),gPad->YtoPad(fY1),gPad->XtoPad(fX2),gPad->YtoPad(fY2));
   Double_t x1 = gPad->GetX1() + fX1*(gPad->GetX2()-gPad->GetX1());
   Double_t y1 = gPad->GetY1() + fY1*(gPad->GetY2()-gPad->GetY1());
   Double_t x2 = gPad->GetX1() + fX2*(gPad->GetX2()-gPad->GetX1());
   Double_t y2 = gPad->GetY1() + fY2*(gPad->GetY2()-gPad->GetY1());
   return DistancetoLine(px,py,x1,y1,x2,y2);
}


//______________________________________________________________________________
TLine *TLine::DrawLine(Double_t x1, Double_t y1, Double_t x2, Double_t  y2)
{
   // Draw this line with new coordinates.

   TLine *newline = new TLine(x1, y1, x2, y2);
   TAttLine::Copy(*newline);
   newline->SetBit(kCanDelete);
   newline->AppendPad();
   return newline;
}


//______________________________________________________________________________
TLine *TLine::DrawLineNDC(Double_t x1, Double_t y1, Double_t x2, Double_t  y2)
{
   // Draw this line with new coordinates in NDC.

   TLine *newline = DrawLine(x1, y1, x2, y2);
   newline->SetBit(kLineNDC);
   return newline;
}


//______________________________________________________________________________
void TLine::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
   // Execute action corresponding to one event.
   //  This member function is called when a line is clicked with the locator
   //
   //  If Left button clicked on one of the line end points, this point
   //     follows the cursor until button is released.
   //
   //  if Middle button clicked, the line is moved parallel to itself
   //     until the button is released.

   Int_t kMaxDiff = 20;
   static Int_t d1,d2,px1,px2,py1,py2;
   static Int_t pxold, pyold, px1old, py1old, px2old, py2old;
   static Bool_t p1, p2, pL;
   Double_t dpx,dpy,xp1,yp1;
   Int_t dx, dy;

   if (!gPad->IsEditable()) return;

   switch (event) {

   case kButton1Down:
      gVirtualX->SetLineColor(-1);
      TAttLine::Modify();  //Change line attributes only if necessary

      // No break !!!

   case kMouseMotion:

      if (TestBit(kLineNDC)) {
         px1 = gPad->UtoPixel(fX1);
         py1 = gPad->VtoPixel(fY1);
         px2 = gPad->UtoPixel(fX2);
         py2 = gPad->VtoPixel(fY2);
      } else {
         px1 = gPad->XtoAbsPixel(gPad->XtoPad(fX1));
         py1 = gPad->YtoAbsPixel(gPad->YtoPad(fY1));
         px2 = gPad->XtoAbsPixel(gPad->XtoPad(fX2));
         py2 = gPad->YtoAbsPixel(gPad->YtoPad(fY2));
      }
      p1 = p2 = pL = kFALSE;

      d1  = abs(px1 - px) + abs(py1-py); //simply take sum of pixels differences
      if (d1 < kMaxDiff) { //*-*================>OK take point number 1
         px1old = px1; py1old = py1;
         p1 = kTRUE;
         gPad->SetCursor(kPointer);
         return;
      }
      d2  = abs(px2 - px) + abs(py2-py); //simply take sum of pixels differences
      if (d2 < kMaxDiff) { //*-*================>OK take point number 2
         px2old = px2; py2old = py2;
         p2 = kTRUE;
         gPad->SetCursor(kPointer);
         return;
      }

      pL = kTRUE;
      pxold = px; pyold = py;
      gPad->SetCursor(kMove);

      break;

   case kButton1Motion:

      if (p1) {
         gVirtualX->DrawLine(px1old, py1old, px2, py2);
         gVirtualX->DrawLine(px, py, px2, py2);
         px1old = px;
         py1old = py;
      }
      if (p2) {
         gVirtualX->DrawLine(px1, py1, px2old, py2old);
         gVirtualX->DrawLine(px1, py1, px, py);
         px2old = px;
         py2old = py;
      }
      if (pL) {
         gVirtualX->DrawLine(px1, py1, px2, py2);
         dx = px-pxold;  dy = py-pyold;
         px1 += dx; py1 += dy; px2 += dx; py2 += dy;
         gVirtualX->DrawLine(px1, py1, px2, py2);
         pxold = px;
         pyold = py;
      }
      break;

   case kButton1Up:

      if (gROOT->IsEscaped()) {
         gROOT->SetEscape(kFALSE);
         break;
      }

      if (TestBit(kLineNDC)) {
         dpx  = gPad->GetX2() - gPad->GetX1();
         dpy  = gPad->GetY2() - gPad->GetY1();
         xp1  = gPad->GetX1();
         yp1  = gPad->GetY1();
         if (p1) {
            fX1 = (gPad->AbsPixeltoX(px)-xp1)/dpx;
            fY1 = (gPad->AbsPixeltoY(py)-yp1)/dpy;
         }
         if (p2) {
            fX2 = (gPad->AbsPixeltoX(px)-xp1)/dpx;
            fY2 = (gPad->AbsPixeltoY(py)-yp1)/dpy;
         }
         if (pL) {
            fX1 = (gPad->AbsPixeltoX(px1)-xp1)/dpx;
            fY1 = (gPad->AbsPixeltoY(py1)-yp1)/dpy;
            fX2 = (gPad->AbsPixeltoX(px2)-xp1)/dpx;
            fY2 = (gPad->AbsPixeltoY(py2)-yp1)/dpy;
         }
      } else {
         if (p1) {
            fX1 = gPad->PadtoX(gPad->AbsPixeltoX(px));
            fY1 = gPad->PadtoY(gPad->AbsPixeltoY(py));
         }
         if (p2) {
            fX2 = gPad->PadtoX(gPad->AbsPixeltoX(px));
            fY2 = gPad->PadtoY(gPad->AbsPixeltoY(py));
         }
         if (pL) {
            fX1 = gPad->PadtoX(gPad->AbsPixeltoX(px1));
            fY1 = gPad->PadtoY(gPad->AbsPixeltoY(py1));
            fX2 = gPad->PadtoX(gPad->AbsPixeltoX(px2));
            fY2 = gPad->PadtoY(gPad->AbsPixeltoY(py2));
         }
      }
      if (TestBit(kVertical)) {
         if (p1) fX2 = fX1;
         if (p2) fX1 = fX2;
      }
      if (TestBit(kHorizontal)) {
         if (p1) fY2 = fY1;
         if (p2) fY1 = fY2;
      }
      gPad->Modified(kTRUE);
      gVirtualX->SetLineColor(-1);
      break;

   case kButton1Locate:

      ExecuteEvent(kButton1Down, px, py);
      while (1) {
         px = py = 0;
         event = gVirtualX->RequestLocator(1,1,px,py);

         ExecuteEvent(kButton1Motion, px, py);

         if (event != -1) {                     // button is released
            ExecuteEvent(kButton1Up, px, py);
            return;
         }
      }
   }
}


//______________________________________________________________________________
void TLine::ls(Option_t *) const
{
   // List this line with its attributes.

   TROOT::IndentLevel();
   printf("%s  X1=%f Y1=%f X2=%f Y2=%f\n",IsA()->GetName(),fX1,fY1,fX2,fY2);
}


//______________________________________________________________________________
void TLine::Paint(Option_t *)
{
   // Paint this line with its current attributes.

   if (TestBit(kLineNDC)) PaintLineNDC(fX1,fY1,fX2,fY2);
   else                   PaintLine(gPad->XtoPad(fX1),gPad->YtoPad(fY1),gPad->XtoPad(fX2),gPad->YtoPad(fY2));
}


//______________________________________________________________________________
void TLine::PaintLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
   // Draw this line with new coordinates.

   TAttLine::Modify();  //Change line attributes only if necessary
   gPad->PaintLine(x1,y1,x2,y2);
}


//______________________________________________________________________________
void TLine::PaintLineNDC(Double_t u1, Double_t v1, Double_t u2, Double_t v2)
{
   // Draw this line with new coordinates in NDC.

   TAttLine::Modify();  //Change line attributes only if necessary
   gPad->PaintLineNDC(u1,v1,u2,v2);
}


//______________________________________________________________________________
void TLine::Print(Option_t *) const
{
   // Dump this line with its attributes.

   printf("%s  X1=%f Y1=%f X2=%f Y2=%f",IsA()->GetName(),fX1,fY1,fX2,fY2);
   if (GetLineColor() != 1) printf(" Color=%d",GetLineColor());
   if (GetLineStyle() != 1) printf(" Style=%d",GetLineStyle());
   if (GetLineWidth() != 1) printf(" Width=%d",GetLineWidth());
   printf("\n");
}


//______________________________________________________________________________
void TLine::SavePrimitive(ostream &out, Option_t * /*= ""*/)
{
    // Save primitive as a C++ statement(s) on output stream out

   if (gROOT->ClassSaved(TLine::Class())) {
      out<<"   ";
   } else {
      out<<"   TLine *";
   }
   out<<"line = new TLine("<<fX1<<","<<fY1<<","<<fX2<<","<<fY2
      <<");"<<endl;

   SaveLineAttributes(out,"line",1,1,1);

   out<<"   line->Draw();"<<endl;
}


//______________________________________________________________________________
Bool_t TLine::IsHorizontal()
{
   // Check whether this line is to be drawn horizontally.

   return TestBit(kHorizontal);
}


//______________________________________________________________________________
Bool_t TLine::IsVertical()
{
   // Check whether this line is to be drawn vertically.

   return TestBit(kVertical);
}


//______________________________________________________________________________
void TLine::SetNDC(Bool_t isNDC)
{
   // Set NDC mode on if isNDC = kTRUE, off otherwise

   ResetBit(kLineNDC);
   if (isNDC) SetBit(kLineNDC);
}


//______________________________________________________________________________
void TLine::SetHorizontal(Bool_t set /*= kTRUE*/)
{
   // Force the line to be drawn horizontally.
   // Makes fY2 equal to fY1. The line length is kept.
   // TArrow and TGaxis also get this function by inheritance.

   SetBit(kHorizontal, set);
   if (set) {
      SetVertical(kFALSE);
      Int_t px1 = gPad->XtoAbsPixel(fX1);
      Int_t px2 = gPad->XtoAbsPixel(fX2);
      Int_t py1 = gPad->YtoAbsPixel(fY1);
      Int_t py2 = gPad->YtoAbsPixel(fY2);
      Int_t l   = Int_t(TMath::Sqrt((px2-px1)*(px2-px1)+(py2-py1)*(py2-py1)));
      if (fX2 >= fX1) fX2 = gPad->AbsPixeltoX(px1+l);
      else            fX2 = gPad->AbsPixeltoX(px1-l);
      fY2 = fY1;
   }
}


//______________________________________________________________________________
void TLine::SetVertical(Bool_t set /*= kTRUE*/)
{
   // Force the line to be drawn vertically.
   // Makes fX2 equal to fX1. The line length is kept.
   // TArrow and TGaxis also get this function by inheritance.

   SetBit(kVertical, set);
   if (set) {
      SetHorizontal(kFALSE);
      Int_t px1 = gPad->XtoAbsPixel(fX1);
      Int_t px2 = gPad->XtoAbsPixel(fX2);
      Int_t py1 = gPad->YtoAbsPixel(fY1);
      Int_t py2 = gPad->YtoAbsPixel(fY2);
      Int_t l   = Int_t(TMath::Sqrt((px2-px1)*(px2-px1)+(py2-py1)*(py2-py1)));
      if (fY2 >= fY1) fY2 = gPad->AbsPixeltoY(py1-l);
      else            fY2 = gPad->AbsPixeltoY(py1+l);
      fX2 = fX1;
   }
}


//______________________________________________________________________________
void TLine::Streamer(TBuffer &R__b)
{
   // Stream an object of class TLine.

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      if (R__v > 1) {
         R__b.ReadClassBuffer(TLine::Class(), this, R__v, R__s, R__c);
         return;
      }
      //====process old versions before automatic schema evolution
      TObject::Streamer(R__b);
      TAttLine::Streamer(R__b);
      Float_t x1,y1,x2,y2;
      R__b >> x1; fX1 = x1;
      R__b >> y1; fY1 = y1;
      R__b >> x2; fX2 = x2;
      R__b >> y2; fY2 = y2;
      //====end of old versions

   } else {
      R__b.WriteClassBuffer(TLine::Class(),this);
   }
}
