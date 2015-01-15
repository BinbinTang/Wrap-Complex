#include <sos/sosfloat.h>
#include <geometry/comp.h>
#include <geometry/orienter.h>
#include <geometry/tristconnector.h>

#define LP_SUM 12

//--------------------------------------------------------------------------
// EdgeGlueList: search by Dest

inline TristConnector::EdgeGlueList::
EdgeGlueList (void)
: RArray<OrTri>(6), 
  nUsed(0) 
{ 

}

inline TristConnector::EdgeGlueList::
~EdgeGlueList (void)
{

}

//----------------------------------------------------------------------------

inline void TristConnector::EdgeGlueList::
addNewEdge(OrTri tri)
{
  (*this)[nUsed++] = tri;
}

//----------------------------------------------------------------------------

Bool TristConnector::EdgeGlueList::
existOrTri(OrTri tri)
{
  for(int i=0;i<nUsed;i++)
    if((*this)[i] == tri)
      return TRUE;
  return FALSE;
}

Bool TristConnector::EdgeGlueList::
existEdge(VIndex v2, Trist * tr)
{
  for(int i=0;i<nUsed;i++)
  {
    OrTri ef = (*this)[i];
    if(tr->dest(ef)==v2 /*|| 
      tr->dest(ef.enext())==v2*/)
      return TRUE;
  }
  return FALSE;
}


OrTri TristConnector::EdgeGlueList::
getOneTri(VIndex v2, Trist * tr)
{
  for(int i=0;i<nUsed;i++)
  {
    OrTri ef = (*this)[i];
    if(tr->dest(ef) == v2)
      return ef;
//    ef = ef.enext2().sym();
//    if(tr->dest(ef) == v2)
//      return ef;
  }
  return OrTri(0, 0);
}
//-------------------------------------------------------------------------
// TristConnector
//-------------------------------------------------------------------------

TristConnector::TristConnector (Bool useKdTree, Bool componentShareVertex,
	double epsilon)
: TristModifier (new Trist),
  converter_ (0, 0)
{
  componentShareVertex_ = componentShareVertex ;
  ra_ = new DoublePArray;
  numPtAdded_ = 0;
  actualNumPtAdded_ = 0;
  prevObjsActualNumPtAdded_ = 0;
  spe_ = NULL;
  BZERO(errorMsg_, char, 128);
  tree_ = useKdTree? new KdTree(*ra_, epsilon) : NULL;
  vmap_ = new IMap();
  edgelist_ = NULL;
}

inline OrTri TristConnector::turnEdge_(OrTri tri,VIndex v1,VIndex v2)
{
  tri = Orient_(tri, v1);
  return (Dest_(tri)==v2)? tri : tri.enext2().sym();
}

void TristConnector::
glueTheTriangles (void)
{
  // don't need kdtree anymore
  delete tree_; tree_ = NULL;
  delete vmap_; vmap_ = NULL;
  ra_->resize(ra_->size());
  if (spe_ == NULL) 
		spe_ = new SOS_FloatEvaluator(converter_);

  BitVectorIter ti(tr_->getTriangleBV());
  VIndex v[3];

  int prog1, prog2, prog3, prog4, prog5;
  getProgressBounds (prog1, prog5);
  prog2 = (80*prog1 + 20*prog5)/100;
  prog3 = (50*prog1 + 50*prog5)/100;
  prog4 = (20*prog1 + 80*prog5)/100;
  setProgressBounds(prog1, prog2);
  float r = (float) tr_->numTri();
  int skip = tr_->numTri()/25;
  if (!skip) skip = 1;
  r = r? 1.0f/r : 1.0f;
  // at this time triangles consequtive
  delete edgelist_; edgelist_ = new RObjArray<EdgeGlueList>(ra_->size()+1);
  for(ti.first();!ti.isDone();ti.next())
  {
    FIndex f = ti.curItem();
    OrTri ef(f, 0);
    Triangle_(ef, v[0], v[1], v[2]);
    Assert(v[0] < v[1] && v[1] < v[2]);
    EdgeGlueList& el0 = (*edgelist_)[v[0]];
    if(!el0.existEdge(v[1], tr_))
      el0.addNewEdge(turnEdge_(ef,v[0],v[1]));
    if(!el0.existEdge(v[2], tr_))
      el0.addNewEdge(turnEdge_(ef,v[0],v[2]));
    EdgeGlueList &el1 = (*edgelist_)[v[1]];
    if(!el1.existEdge(v[2], tr_))
      el1.addNewEdge(turnEdge_(ef,v[1],v[2]));
    if(!(f % skip)) progressUpdate (f*r);
  }

  // merge triangles
  setProgressBounds(prog2, prog3);
  for(ti.first();!ti.isDone();ti.next())
  {
    FIndex f = ti.curItem();
    OrTri ef(f,0);
    Triangle_(ef, v[0], v[1], v[2]);
    Assert(v[0] < v[1] && v[1] < v[2]);
    
    Assert(Fvalid_(f));
    fittingTri_(ef,v[0],v[1]);
    
    if(Fvalid_(f))
      fittingTri_(ef,v[0],v[2]);
    if(Fvalid_(f))
      fittingTri_(ef,v[1],v[2]);
    if(!(f % skip)) progressUpdate (f*r);
  }

  delete edgelist_; edgelist_ = NULL; // don't need it anymore
  // setup bitvectors
  TristOrienter orienter(tr_, ra_);
  setProgressCBto(&orienter);
  orienter.setProgressBounds(prog3, prog4);
  orienter.markPrincipal(shadow_, principal_);
  orienter.setProgressBounds(prog4, prog5);
  orienter.orientAll(shadow_, principal_);
  setProgressBounds(prog1, prog5);
}

inline int TristConnector::noTriConnectedToThisEdge_(OrTri tri)
{
  return Fnext_(tri).index() == tri.index();
}

void TristConnector::fittingTri_(OrTri tri,VIndex v1,VIndex v2)
{
  OrTri newtri=turnEdge_(tri,v1,v2);
  OrTri representative;
  OrTri current;
  int notri,i;

  if(!(*edgelist_)[v1].existOrTri(newtri))
  {
    representative = (*edgelist_)[v1].getOneTri(v2, tr_);
    if(noTriConnectedToThisEdge_(representative))
    {
      if(Dest_(representative.enext()) ==
         Dest_(newtri.enext()))
        removeDuplicateTri_(representative,newtri);
      else
        Fmerge_(newtri, representative);
    }
    else
    {
      for(current = Fnext_(representative),notri = 1;
        current.index() != representative.index() ; 
        current = Fnext_(current),notri++);
      for(i=0;i<notri;i++,current = Fnext_(current))
      {
        if(Dest_(current.enext()) == 
            Dest_(newtri.enext()))
        {
          removeDuplicateTri_(current,newtri);
          return;
        }
        else
        if( Dest_(Fnext_(current).enext()) == 
            Dest_(newtri.enext()))
        {
          removeDuplicateTri_(Fnext_(current),newtri);
          return;
        }
        else
        if(inOrder_(current,newtri,Fnext_(current)))
        {
          Fmerge_(current,newtri);
          return;
        } 
      }
    }
  }
}

void TristConnector::removeDuplicateTri_(OrTri t1,OrTri t2)
{
  OrTri  t2b = turnEdge_(t2,Org_(t1),Dest_(t1));
  Fmerge_(t1,t2b);
  Fmerge_(t1.enext(), t2b.enext());
  Fmerge_(t1.enext2(), t2b.enext2());
  DelTri_(t2b);
}

inline int TristConnector::below_(VIndex v1,VIndex v2,VIndex v3,VIndex p)
{
  return !spe_->positive3(*ra_, v1,v2,v3,p);
}

int TristConnector::inOrder_(OrTri t1,OrTri t2,OrTri t3)
{
  VIndex v1 = Org_(t1);
  VIndex v2 = Dest_(t1);
  VIndex p1 = Dest_(t1.enext());
  VIndex p2 = Dest_(t2.enext());
  VIndex p3 = Dest_(t3.enext());

  if(below_(v1,v2,p1,p3))
    return below_(v1,v2,p1,p2) && below_(v1,v2,p2,p3);
  else
    return ! (below_(v1,v2,p2,p1) && below_(v1,v2,p3,p2));
}



int TristConnector::addPointWithTri(char* xc,char* yc,char* zc) 
{
  if (!addPoint(xc,yc,zc))
	return NO_TRI_CREATED;
  if((numPtAdded_ % 3)==0)
    return addTri(triVertIdx_[0],triVertIdx_[1],triVertIdx_[2]);
  return NO_TRI_CREATED;
}

int TristConnector::addPointWithTri(double xc[3]) 
{
  addPoint(xc);
  if((numPtAdded_ % 3)==0)
    return addTri(triVertIdx_[0],triVertIdx_[1],triVertIdx_[2]);
  return NO_TRI_CREATED;
}

int TristConnector::addTri(VIndex v1in,VIndex v2in, VIndex v3in) 
{
  VIndex v1,v2,v3;
  v1 = (*vmap_)[v1in];
  v2 = (*vmap_)[v2in];
  v3 = (*vmap_)[v3in];
  if((v1==v2)||(v1==v3)||(v2==v3))
    return NO_TRI_CREATED;
  sort3(v1,v2,v3);
  OrTri newtri = MakeTri_(v1,v2,v3);
  return newtri.index();
}

int TristConnector::addPoint(char* xc,char* yc,char* zc) 
{
  char* s[3];
  s[0] = xc;
  s[1] = yc;
  s[2] = zc;
  double x[3];
  for(int i=0;i<3;i++)
  {
    if(!setPrecision(s[i]))
	  return 0;
    
  }
  if(!(sscanf_s(s[0],"%lf",&x[0]) &&
       sscanf_s(s[1],"%lf",&x[1]) &&
       sscanf_s(s[2],"%lf",&x[2])))
  {
    //strcpy(errorMsg_, "Invalid numeric value");
	strcpy_s(errorMsg_, 128, "Invalid numeric value");
	return 0;
  }
  addPoint(x);
  return 1;
}

int TristConnector::addPoint(double x[3])
{
  numPtAdded_ ++;
  if(!tree_)
  {
    ra_->addPoint(x);
    actualNumPtAdded_++;
    (*vmap_)[numPtAdded_] = actualNumPtAdded_ + prevObjsActualNumPtAdded_;
    triVertIdx_[numPtAdded_ % 3] = numPtAdded_;
	  return 1;
  }
  // now we use KdTree
  int oldptn = tree_->nOfPoints();
  (*vmap_)[numPtAdded_] = tree_->addPoint(x);// + prevObjsActualNumPtAdded_;
  triVertIdx_[numPtAdded_ % 3] = numPtAdded_;
  int newptn = tree_->nOfPoints();
  if(oldptn!=newptn)
  {
    actualNumPtAdded_++;
	  return 1;
  }
  return 0;
}

OrTri TristConnector::flip_(OrTri tri)
{
  VIndex v1 = Org_(tri), v2 = Dest_(tri);
  Flip_(tri);
  return turnEdge_(tri, v1, v2);
}

void TristConnector::newObject()
{
  if(componentShareVertex_)
    return;
  
  prevObjsActualNumPtAdded_ += actualNumPtAdded_;
  actualNumPtAdded_ = 0;
  if(tree_)
  {
    tree_->reset();
  }
}


void TristConnector::printVIdx(OrTri t)
{
  printf("Idx (%4d %4d %4d)\n",Org_(t),Dest_(t),Dest_(t.enext()));
}

void TristConnector::setEpsilon(double epsilon)
{
  if(tree_) tree_->setEpsilon(epsilon);
}

double TristConnector::getEpsilon (void)
{
  return tree_? tree_->getEpsilon() : 0;
}

Trist * TristConnector::getTrist (void)
{
  Trist * tr = tr_; tr_ = NULL; return tr;
}


void TristConnector::CODFS_(OrTri tri,BitVector* flipped,
                           Bool isprincipal,int version)
{
  VIndex v1,v2;
  OrTriStack s;
  s.push(tri);
  
  while (!s.empty())
  {
    tri = s.pop();
    if(shadow_.test(tri.index()))
      continue;
    if(flipped->test(tri.index()))
      continue;
    flipped->set(tri.index());
    
    int i;
    OrTri t = tri,next,oldnext;
    for(i=0;i<3;i++,t=t.enext())
    {
      next = oldnext = Fnext_(t).sym();
      if(next.index() == t.index() ||
         flipped->test(next.index()) ||
         shadow_.test(next.index()))
        continue;
      if(principal_.test(next.index()) == isprincipal)
      {
        v1 = Org_(next);
        v2 = Dest_(next);
        if( ((int)next.version()%2) != version )
          next = flip_(next);
        principal_.set(next.index(), principal_.test(oldnext.index()));
        next = turnEdge_(next,v1,v2);
        s.push(next);
      }
    }
  }
}


void TristConnector::addShadowTri_ (void)
{
  // by this time they could be reoriented
  BitVectorPool pool;
  StackBVTag star(pool);
  
  float r = actualNumPtAdded_? 1.0f/actualNumPtAdded_ : 1.0f;
  float fraction = 0.0f;
  int skip = actualNumPtAdded_/20;
  if (!skip) skip = 1;

  for (int v1=1; v1<=actualNumPtAdded_; v1++)
  {
    if(v1 % skip == 0) progressUpdate (fraction);
    fraction += r;
    EdgeGlueList& list = (*edgelist_)[v1];
    if (list.nUsed <= 1)
      continue; // 
    OrTri ef0 = Orient_(list[0], v1);
    Assert(Org_(ef0) == v1);
    VIndex v2 = Dest_(ef0);
    tr_->getCompleteStar(ef0, star);
    for (int i=1; i<list.nUsed; i++)
    {
      OrTri efi = Orient_(list[i], v1);
      Assert(Org_(efi) == v1);
      if (star.test(efi.index()))
        continue;
      tr_->getCompleteStar(efi, star); // merge stars
      // now create shadow triangle
      VIndex v3 = Dest_(efi);
      OrTri shadowTri = MakeTri_(v1, v2, v3);
      Fmerge_(ef0, shadowTri);
      Fmerge_(efi, shadowTri.enext2().sym());
      shadow_.set(shadowTri.index());
    }
    star.reset();
  }
}

//-------------------------------------------------------------------------------

DoublePArray * TristConnector::getDoublePArray()
{
  DoublePArray * rArray = ra_;
  ra_ = NULL;
  return rArray;
}

BitVector& TristConnector::getShadow()
{
  return shadow_;
}

BitVector& TristConnector::getPrincipal()
{
  return principal_;
}

//------------------------------------------------------------------------------

void TristConnector::setPrecision(int lMax,int pMax)
{
  converter_.setPrecision (lMax, pMax);
}

void TristConnector::getPrecision(int& lMax, int& pMax)
{
  converter_.getPrecision (lMax, pMax);
}

//------------------------------------------------------------------------

void sort3 (VIndex &v1,VIndex &v2,VIndex &v3)
{
  VIndex temp; 
  if(v1>v2)
  {
    temp = v1;
    v1 = v2;
    v2 = temp;
  }
  if(v2>v3)
  {
    temp = v2;
    v2 = v3;
    v3 = temp;
    if(v1>v2) 
    {
      temp = v1;
      v1 = v2;
      v2 = temp;
    }
  }
}

//-----------------------------------------------------------------------------

TristConnector::~TristConnector()
{
	delete tr_;
  delete spe_;
  delete ra_;
  delete tree_;
  delete vmap_;
  delete edgelist_;
}

//------------------------------------------------------------------------------

int TristConnector::setPrecision (const char * s)
{
	int left, right, zero;
	if (!s || !findDigits (s, left, right, zero))
	{
		//strcpy(errorMsg_, "Invalid numeric value");
		strcpy_s(errorMsg_, 128, "Invalid numeric value");
		return 0;
	}

	// reserve additional digit, in case of modification of
	// real values.
	int lMax = MAX (left+1, 0) + 1;
	int pMax = MAX (-right, 0);

	if (zero) {
		lMax = pMax = 0;
	}

	int lMaxOrig, pMaxOrig;
	converter_.getPrecision(lMaxOrig, pMaxOrig);
	int lMaxNew = lMaxOrig, pMaxNew = pMaxOrig;

  if(lMaxNew < lMax) lMaxNew = lMax;
  if(pMaxNew < pMax) pMaxNew = pMax;
  if ((lMaxNew + pMaxNew) > LP_SUM + 1) pMaxNew = LP_SUM + 1 - lMaxNew;
  if (pMaxNew < 0) pMaxNew = 0;
  if ((lMaxNew + pMaxNew) > LP_SUM + 1)
  {
	  //strcpy(errorMsg_,"Number of significant digits is too large");
	  strcpy_s(errorMsg_, 128, "Number of significant digits is too large");
	  return 0;
  }

	if ((lMaxNew != lMaxOrig) || (pMaxNew != pMaxOrig))
  {
		converter_.setPrecision (lMaxNew, pMaxNew);
    if (tree_)
    {
      double newEpsilon = 0.5/converter_.getScale();
      if (newEpsilon < tree_->getEpsilon())
        tree_->setEpsilon(newEpsilon);
    }
  }

  return 1;
}

//--------------------------------------------------------------------
