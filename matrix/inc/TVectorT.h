// @(#)root/matrix:$Name:  $:$Id: TVectorT.h,v 1.3 2006/01/25 18:49:03 brun Exp $
// Authors: Fons Rademakers, Eddy Offermann   Nov 2003

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TVectorT
#define ROOT_TVectorT

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TVectorT                                                             //
//                                                                      //
// Template class of Vectors in the linear algebra package              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TMatrixT
#include "TMatrixT.h"
#endif
#ifndef ROOT_TMatrixTSym
#include "TMatrixTSym.h"
#endif
#ifndef ROOT_TMatrixTSparse
#include "TMatrixTSparse.h"
#endif

template<class Element> class TVectorT : public TObject {

protected:
  Int_t    fNrows;                // number of rows
  Int_t    fRowLwb;               // lower bound of the row index
  Element *fElements;             //[fNrows] elements themselves

  enum {kSizeMax = 5};             // size data container on stack, see New_m(),Delete_m()
  enum {kWorkMax = 100};           // size of work array's in several routines

  Element  fDataStack[kSizeMax];  //! data container
  Bool_t   fIsOwner;              //!default kTRUE, when Use array kFALSE

  Element* New_m   (Int_t size);
  void     Delete_m(Int_t size,Element*&);
  Int_t    Memcpy_m(Element *newp,const Element *oldp,Int_t copySize,
                    Int_t newSize,Int_t oldSize);

  void     Allocate(Int_t nrows,Int_t row_lwb = 0,Int_t init = 0);

  enum EVectorStatusBits {
    kStatus = BIT(14) // set if vector object is valid
  };

public:

  TVectorT() { fIsOwner = kTRUE; fElements = 0; fNrows = 0; fRowLwb = 0; }
  explicit TVectorT(Int_t n);
  TVectorT(Int_t lwb,Int_t upb);
  TVectorT(Int_t n,const Element *elements);
  TVectorT(Int_t lwb,Int_t upb,const Element *elements);
  TVectorT(const TVectorT            <Element> &another);
  TVectorT(const TMatrixTRow_const   <Element> &mr);
  TVectorT(const TMatrixTColumn_const<Element> &mc);
  TVectorT(const TMatrixTDiag_const  <Element> &md);
  template <class Element2> TVectorT(const TVectorT<Element2> &another) 
  {
    Assert(another.IsValid());
    Allocate(another.GetUpb()-another.GetLwb()+1,another.GetLwb());
    *this = another;
  }
#ifndef __CINT__
  TVectorT(Int_t lwb,Int_t upb,Element iv1, ...);
#endif
  virtual ~TVectorT() { Clear(); }

  inline          Int_t     GetLwb       () const { return fRowLwb; }
  inline          Int_t     GetUpb       () const { return fNrows+fRowLwb-1; }
  inline          Int_t     GetNrows     () const { return fNrows; }
  inline          Int_t     GetNoElements() const { return fNrows; }

  inline          Element  *GetMatrixArray  ()       { return fElements; }
  inline const    Element  *GetMatrixArray  () const { return fElements; }

  inline void     Invalidate ()       { SetBit(kStatus); }
  inline void     MakeValid  ()       { ResetBit(kStatus); }
  inline Bool_t   IsValid    () const { return !TestBit(kStatus); }
  inline Bool_t   IsOwner    () const { return fIsOwner; }
  inline void     SetElements(const Element *elements) { Assert(IsValid());
                                                         memcpy(fElements,elements,fNrows*sizeof(Element)); }
  inline TVectorT<Element> &Shift     (Int_t row_shift)            { fRowLwb += row_shift; return *this; }
         TVectorT<Element> &ResizeTo  (Int_t lwb,Int_t upb);
  inline TVectorT<Element> &ResizeTo  (Int_t n)                    { return ResizeTo(0,n-1); }
  inline TVectorT<Element> &ResizeTo  (const TVectorT<Element> &v) { return ResizeTo(v.GetLwb(),v.GetUpb()); }

         TVectorT<Element> &Use       (Int_t n,Element *data);
         TVectorT<Element> &Use       (Int_t lwb,Int_t upb,Element *data);
         TVectorT<Element> &Use       (TVectorT<Element> &v);
         TVectorT<Element> &GetSub    (Int_t row_lwb,Int_t row_upb,TVectorT<Element> &target,Option_t *option="S") const;
         TVectorT<Element>  GetSub    (Int_t row_lwb,Int_t row_upb,Option_t *option="S") const;
         TVectorT<Element> &SetSub    (Int_t row_lwb,const TVectorT<Element> &source);

  TVectorT<Element> &Zero();
  TVectorT<Element> &Abs ();
  TVectorT<Element> &Sqr ();
  TVectorT<Element> &Sqrt();
  TVectorT<Element> &Invert();
  TVectorT<Element> &SelectNonZeros(const TVectorT<Element> &select);

  Element Norm1   () const;
  Element Norm2Sqr() const;
  Element NormInf () const;
  Int_t   NonZeros() const;
  Element Sum     () const;
  Element Min     () const;
  Element Max     () const;

  inline const Element &operator()(Int_t index) const;
  inline       Element &operator()(Int_t index)       { return (Element&)((*(const TVectorT<Element> *)this)(index)); }
  inline const Element &operator[](Int_t index) const { return (Element&)((*(const TVectorT<Element> *)this)(index)); }
  inline       Element &operator[](Int_t index)       { return (Element&)((*(const TVectorT<Element> *)this)(index)); }

  TVectorT<Element> &operator= (const TVectorT                <Element> &source);
  TVectorT<Element> &operator= (const TMatrixTRow_const       <Element> &mr);
  TVectorT<Element> &operator= (const TMatrixTColumn_const    <Element> &mc);
  TVectorT<Element> &operator= (const TMatrixTDiag_const      <Element> &md);
  TVectorT<Element> &operator= (const TMatrixTSparseRow_const <Element> &md);
  TVectorT<Element> &operator= (const TMatrixTSparseDiag_const<Element> &md);
  template <class Element2> TVectorT<Element> &operator= (const TVectorT<Element2> &source)
  {
    if (!AreCompatible(*this,source)) {
      Error("operator=(const TVectorT2 &)","vectors not compatible");
      this->Invalidate();
      return *this;
    }

    TObject::operator=(source);
    const Element2 * const ps = source.GetMatrixArray();
          Element  * const pt = GetMatrixArray();
    for (Int_t i = 0; i < this->fNrows; i++)
      pt[i] = ps[i];
    return *this;
  }

  TVectorT<Element> &operator= (Element val);
  TVectorT<Element> &operator+=(Element val);
  TVectorT<Element> &operator-=(Element val);
  TVectorT<Element> &operator*=(Element val);

  TVectorT<Element> &operator+=(const TVectorT      <Element> &source);
  TVectorT<Element> &operator-=(const TVectorT      <Element> &source);
  TVectorT<Element> &operator*=(const TMatrixT      <Element> &a);
  TVectorT<Element> &operator*=(const TMatrixTSym   <Element> &a);
  TVectorT<Element> &operator*=(const TMatrixTSparse<Element> &a);

  Bool_t operator==(Element val) const;
  Bool_t operator!=(Element val) const;
  Bool_t operator< (Element val) const;
  Bool_t operator<=(Element val) const;
  Bool_t operator> (Element val) const;
  Bool_t operator>=(Element val) const;

  Bool_t MatchesNonZeroPattern(const TVectorT<Element> &select);
  Bool_t SomePositive         (const TVectorT<Element> &select);
  void   AddSomeConstant      (Element val,const TVectorT<Element> &select);

  void   Randomize            (Element alpha,Element beta,Double_t &seed);

  TVectorT<Element> &Apply(const TElementActionT   <Element> &action);
  TVectorT<Element> &Apply(const TElementPosActionT<Element> &action);

  void Clear(Option_t * /*option*/ ="") { if (fIsOwner) Delete_m(fNrows,fElements);
                                          else fElements = 0; fNrows = 0; }
  void Draw (Option_t *option=""); // *MENU*
  void Print(Option_t *option="") const;  // *MENU*

  ClassDef(TVectorT,4)  // Template of Vector class
};

template<class Element> inline       TVectorT<Element> &TVectorT<Element>::Use           (Int_t n,Element *data) { return Use(0,n-1,data); }
template<class Element> inline       TVectorT<Element> &TVectorT<Element>::Use           (TVectorT &v)
                                                                                         { 
                                                                                           Assert(v.IsValid());
                                                                                           return Use(v.GetLwb(),v.GetUpb(),v.GetMatrixArray());
                                                                                         }
template<class Element> inline       TVectorT<Element>  TVectorT<Element>::GetSub        (Int_t row_lwb,Int_t row_upb,Option_t *option) const
                                                                                         { 
                                                                                           TVectorT tmp;
                                                                                           this->GetSub(row_lwb,row_upb,tmp,option);
                                                                                           return tmp;
                                                                                         }

template<class Element> inline const Element           &TVectorT<Element>::operator()(Int_t ind) const
{
  // Access a vector element.

  Assert(IsValid());
  const Int_t aind = ind-fRowLwb;
  Assert(aind < fNrows && aind >= 0);

  return fElements[aind];
}

template<class Element> Bool_t             operator==   (const TVectorT      <Element>  &source1,const TVectorT      <Element>  &source2);
template<class Element> TVectorT<Element>  operator+    (const TVectorT      <Element>  &source1,const TVectorT      <Element>  &source2);
template<class Element> TVectorT<Element>  operator-    (const TVectorT      <Element>  &source1,const TVectorT      <Element>  &source2);
template<class Element> Element            operator*    (const TVectorT      <Element>  &source1,const TVectorT      <Element>  &source2);
template<class Element> TVectorT<Element>  operator*    (const TMatrixT      <Element>  &a,      const TVectorT      <Element>  &source);
template<class Element> TVectorT<Element>  operator*    (const TMatrixTSym   <Element>  &a,      const TVectorT      <Element>  &source);
template<class Element> TVectorT<Element>  operator*    (const TMatrixTSparse<Element>  &a,      const TVectorT      <Element>  &source);
template<class Element> TVectorT<Element>  operator*    (      Element                   val,    const TVectorT      <Element>  &source);
template<class Element> TVectorT<Element> &Add          (      TVectorT      <Element>  &target,       Element                   scalar, const TVectorT<Element> &source);
template<class Element> TVectorT<Element> &Add          (      TVectorT      <Element>  &target,       Element                         , const TMatrixT      <Element>  &a,
                                                         const TVectorT<Element> &source);
template<class Element> TVectorT<Element> &Add          (      TVectorT      <Element>  &target,       Element                         , const TMatrixTSym   <Element>  &a,
                                                         const TVectorT<Element> &source);
template<class Element> TVectorT<Element> &Add          (      TVectorT      <Element>  &target,       Element                         , const TMatrixTSparse<Element>  &a,
                                                         const TVectorT<Element> &source);
template<class Element> TVectorT<Element> &AddElemMult  (      TVectorT      <Element>  &target,       Element                   scalar, const TVectorT<Element> &source1,
                                                         const TVectorT      <Element>  &source2);
template<class Element> TVectorT<Element> &AddElemMult  (      TVectorT      <Element>  &target,       Element                   scalar, const TVectorT<Element> &source1,
                                                         const TVectorT      <Element>  &source2,const TVectorT      <Element>  &select);
template<class Element> TVectorT<Element> &AddElemDiv   (      TVectorT      <Element>  &target,       Element                   scalar, const TVectorT<Element> &source1,
                                                         const TVectorT      <Element>  &source2);
template<class Element> TVectorT<Element> &AddElemDiv   (      TVectorT      <Element>  &target,       Element                   scalar, const TVectorT<Element> &source1,
                                                         const TVectorT      <Element>  &source2,const TVectorT      <Element>  &select);
template<class Element> TVectorT<Element> &ElementMult  (      TVectorT      <Element>  &target, const TVectorT      <Element>  &source);
template<class Element> TVectorT<Element> &ElementMult  (      TVectorT      <Element>  &target, const TVectorT      <Element>  &source, const TVectorT<Element> &select);
template<class Element> TVectorT<Element> &ElementDiv   (      TVectorT      <Element>  &target, const TVectorT      <Element>  &source);
template<class Element> TVectorT<Element> &ElementDiv   (      TVectorT      <Element>  &target, const TVectorT      <Element>  &source, const TVectorT<Element> &select);
template<class Element1,class Element2>
                        Bool_t             AreCompatible(const TVectorT      <Element1> &source1,const TVectorT      <Element2> &source2,      Int_t              verbose=0);
template<class Element> void               Compare      (const TVectorT      <Element>  &source1,const TVectorT      <Element>  &source2);
template<class Element> Bool_t             VerifyVectorValue
                                                        (const TVectorT      <Element>  &m,            Element                  val,           Int_t              verbose=1,
                                                         Element maxDevAllow=0.0);
template<class Element> Bool_t             VerifyVectorIdentity
                                                        (const TVectorT      <Element>  &m1,     const TVectorT      <Element> &m2,            Int_t              verbose=1,
                                                         Element maxDevAllow=0.0);
#endif
