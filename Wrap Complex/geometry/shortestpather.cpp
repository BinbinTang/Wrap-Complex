#include <geometry/shortestpather.h>

ShortestPather::
ShortestPather(Complex* comp,StackBVTag& usedVx,Bool closed, BitVector * mask)
: TristModifier (comp->getTrist()),
  comp_ (comp),
  usedVx_(&usedVx),
  traversed_(comp->getBVPool())
{
  pq_.setCompFunc((PriorityQueue<VIndex>::CompFunc) &ShortestPather::compareCB_,this);
  mask_ = new BitVector (comp->getFaceBV (closed? CLOSED_TRI : COMPLETE_TRI));
  if (mask)
    mask_->and_op (*mask);
}


/*-----------------------------------------------------------------------*/

ShortestPather::
~ShortestPather (void)
{
  delete mask_;
}

void ShortestPather::
setMask (Bool closed, BitVector * mask)
{
  *mask_ = comp_->getFaceBV (closed? CLOSED_TRI : COMPLETE_TRI);
  if (mask)
    mask_->and_op(*mask);
}


/*-----------------------------------------------------------------------*/

int ShortestPather::
compareCB_(VIndex& v1,VIndex& v2)
{
  return (distance_[v1] < distance_[v2]);
}

/*-----------------------------------------------------------------------*/

double ShortestPather::
length_ (double x1[3], double x2[3])
{
  double tmp[3];
  return vlength(vsub(x1, x2, tmp));
}

void ShortestPather::
resetInternal (void)
{
  pq_.reset();
  traversed_.reset();
}

void ShortestPather::
reset (void)
{
  pq_.reset();
  traversed_.reset();
  usedVx_->reset();
}

/*-----------------------------------------------------------------------*/

Bool ShortestPather::
findShortestPath (OrTri srctri, OrTri desttri, OrEdgeInfoSeq& result, Bool boundary)
{
  // setup
  result.reset();
  if (boundary)
  {
    OrTri ef1, ef2;
    if (!comp_->getVertBoundary(srctri, ef1, ef2) ||
        !comp_->getVertBoundary(desttri, ef1, ef2))
      return FALSE;
  }
  Bool Done = FALSE;
  VIndex src  = Org_(srctri);
  VIndex dest = Org_(desttri);
  result.src() = src, result.dest() = dest;
  result.destAsOrTri() = desttri;
  elist_[src] = OrEdgeInfo(srctri, 0);
  distance_[src] = 0;
  keyLoc_[src] = pq_.insert(src);
  traversed_.set(src);
  OrTriStack star(10);
  StackBVTag starV(comp_->getBVPool());
  RealPArray * rArray = comp_->getPArray();
  double x1[3], x2[3];

  int maxi = boundary? 1 : 2;

  while(!pq_.empty())
  {
    VIndex currV = pq_.extractMin();
    if(Done = (currV==dest))
      break;
    star.reset();
    starV.reset();
    OrEdgeInfo& einfo = elist_[currV];
    if (boundary)
    {
      OrTri tmp;
      star.push(tmp), star.push(tmp);
      int res = comp_->getVertBoundary(einfo.getOrTri(), star[1], star[2]);
      Assert(res);
    }
    else
      comp_->getPartialDisk(einfo.getOrTri(), star);
    while(!star.empty())
    {
      OrTri ef = star.pop().sym();
      if(!mask_->test(ef.index()))
        continue;
      for(int i=0; i<maxi; i++, ef = ef.enext().sym()) // enumerate both edges of triangle
      {
        OrTri ef1 = ef;
        VIndex efV = Org_(ef1);
        if(starV.test(efV))
          continue;
        starV.set(efV);
        unsigned char flag = IsEdge;
        if (boundary)
        {
          if(comp_->isOutside(ef1))
          {
            flag |= IsUp;
          }
        }
        else
        {
          OrTri ef2 = comp_->FnextComplete(ef1);
          if((ef1.index() != ef2.index()) && (comp_->FnextComplete(ef2).index() == ef1.index())) 
            flag |= IsManifold;
          if(comp_->isOutside(ef1))
          {
            flag |= IsUp;
          }
          else 
          {
            if(flag & IsManifold)
            {
              Assert(comp_->isOutside(ef2));
              ef1 = ef2, flag |= IsUp;
            }
          }
        }
        
        OrEdgeInfo newedge(ef1, flag);
        temp_.push(newedge);
      }
    }

    // construct the information of the shortest distance
    rArray->getPoint(currV, x2);
    while(!temp_.empty())
    {
      OrEdgeInfo curedge = temp_.pop();
      VIndex Nb = Org_(curedge.getOrTri());
      if(usedVx_->test(Nb))
        continue;
      rArray->getPoint(Nb, x1);
      double edgeLen = length_ (x1, x2) + distance_[currV];
      if(!traversed_.test(Nb))
      {
        elist_[Nb] = curedge;
        distance_[Nb] = edgeLen;
        keyLoc_[Nb] = pq_.insert(Nb);
        traversed_.set(Nb);
      }
      else
      {
        if(edgeLen < distance_[Nb])
        {
          distance_[Nb] = edgeLen;
          elist_[Nb]    = curedge;
          pq_.decreaseKey(keyLoc_[Nb]);
        }
      }
    }
  }

  //
  if(!Done)
    return FALSE;
  
  VIndex v = dest;
  OrEdgeInfo lastEdge;
  usedVx_->set(v);
  while(v!=src)
  {
    temp_.push(elist_[v]); 
    v = Dest_(elist_[v].getOrTri());
    usedVx_->set(v);
  }
  
  while(!temp_.empty())
    result.addEdge(lastEdge = temp_.pop().sym(), tr_);

  result.destAsOrTri() = lastEdge.getOrTri().enext();
  
  return Done;
}

/*-----------------------------------------------------------------------*/

Bool ShortestPather::
findShortestPathToBoundary (OrTri srctri, OrTri& bdtri, double maxDist)
{
  // setup
  pq_.reset();
  StackBVTag locTraversed(comp_->getBVPool()); // don't use global traverse, 
                          // since it might be in the middle of other operation.
  VIndex src  = Org_(srctri);
  elist_[src] = OrEdgeInfo(srctri, 0);
  distance_[src] = 0;
  keyLoc_[src] = pq_.insert(src);
  locTraversed.set(src);
  OrTri bdtri2;
  OrTriStack star(10);
  StackBVTag starV(comp_->getBVPool());
  RealPArray * rArray = comp_->getPArray();
  double x1[3], x2[3];

  while(!pq_.empty())
  {
    VIndex currV = pq_.extractMin();
    OrEdgeInfo& einfo = elist_[currV];
    if(distance_[currV] > maxDist)
      return FALSE;
    if (comp_->getVertBoundary(einfo.getOrTri(), bdtri, bdtri2))
      return TRUE;
    star.reset();
    starV.reset();
    
    comp_->getPartialDisk(einfo.getOrTri(), star);
    while(!star.empty())
    {
      OrTri ef = star.pop().sym();
      if(!mask_->test(ef.index()))
        continue;
      for(int i=0; i<2; i++, ef = ef.enext().sym()) // enumerate both edges of triangle
      {
        OrTri ef1 = ef;
        VIndex efV = Org_(ef1);
        if(starV.test(efV))
          continue;
        starV.set(efV);
        OrTri ef2 = comp_->FnextComplete(ef1);
        unsigned char flag = IsEdge;
        if((ef1.index() != ef2.index()) && (comp_->FnextComplete(ef2).index() == ef1.index())) 
          flag |= IsManifold;
        if(comp_->isOutside(ef1))
        {
          flag |= IsUp;
        }
        else 
        {
          if(flag & IsManifold)
          {
            Assert(comp_->isOutside(ef2));
            ef1 = ef2, flag |= IsUp;
          }
        }
        
        OrEdgeInfo newedge(ef1, flag);
        temp_.push(newedge);
      }
    }

    // construct the information of the shortest distance
    rArray->getPoint(currV, x2);
    while(!temp_.empty())
    {
      OrEdgeInfo curedge = temp_.pop();
      VIndex Nb = Org_(curedge.getOrTri());
//      if(usedVx_->test(Nb))
//        continue;
      rArray->getPoint(Nb, x1);
      double edgeLen = length_ (x1, x2) + distance_[currV];
      if(!locTraversed.test(Nb))
      {
        elist_[Nb] = curedge;
        distance_[Nb] = edgeLen;
        keyLoc_[Nb] = pq_.insert(Nb);
        locTraversed.set(Nb);
      }
      else
      {
        if(edgeLen < distance_[Nb])
        {
          distance_[Nb] = edgeLen;
          elist_[Nb]    = curedge;
          pq_.decreaseKey(keyLoc_[Nb]);
        }
      }
    }
  }

  return FALSE;
  
}

/*-----------------------------------------------------------------------*/

Bool ShortestPather::
findShortestPathToSets (OrTri srctri, OrTri& bdtri, 
                        ComplexEdgeSet * edges, BitVector * verts, double maxDist)
{
  // setup
  pq_.reset();
  StackBVTag locTraversed(comp_->getBVPool()); // don't use global traverse, 
                          // since it might be in the middle of other operation.
  VIndex src  = Org_(srctri);
  elist_[src] = OrEdgeInfo(srctri, 0);
  distance_[src] = 0;
  keyLoc_[src] = pq_.insert(src);
  locTraversed.set(src);
  OrTri bdtri2;
  OrTriStack star(10);
  StackBVTag starV(comp_->getBVPool());
  RealPArray * rArray = comp_->getPArray();
  double x1[3], x2[3];

  while(!pq_.empty())
  {
    VIndex currV = pq_.extractMin();
    OrEdgeInfo& einfo = elist_[currV];
    if(distance_[currV] > maxDist)
      return FALSE;
    bdtri = einfo.getOrTri();
    if (verts && verts->test(Org_(bdtri)))
      return TRUE;
    if (comp_->getVertBoundary(bdtri, bdtri, bdtri2, edges))
      return TRUE;
    star.reset();
    starV.reset();
    
    comp_->getPartialDisk(einfo.getOrTri(), star);
    while(!star.empty())
    {
      OrTri ef = star.pop().sym();
      if(!mask_->test(ef.index()))
        continue;
      for(int i=0; i<2; i++, ef = ef.enext().sym()) // enumerate both edges of triangle
      {
        OrTri ef1 = ef;
        VIndex efV = Org_(ef1);
        if(starV.test(efV))
          continue;
        starV.set(efV);
        OrTri ef2 = comp_->FnextComplete(ef1);
        unsigned char flag = IsEdge;
        if((ef1.index() != ef2.index()) && (comp_->FnextComplete(ef2).index() == ef1.index())) 
          flag |= IsManifold;
        if(comp_->isOutside(ef1))
        {
          flag |= IsUp;
        }
        else 
        {
          if(flag & IsManifold)
          {
            Assert(comp_->isOutside(ef2));
            ef1 = ef2, flag |= IsUp;
          }
        }
        
        OrEdgeInfo newedge(ef1, flag);
        temp_.push(newedge);
      }
    }

    // construct the information of the shortest distance
    rArray->getPoint(currV, x2);
    while(!temp_.empty())
    {
      OrEdgeInfo curedge = temp_.pop();
      VIndex Nb = Org_(curedge.getOrTri());
//      if(usedVx_->test(Nb))
//        continue;
      rArray->getPoint(Nb, x1);
      double edgeLen = length_ (x1, x2) + distance_[currV];
      if(!locTraversed.test(Nb))
      {
        elist_[Nb] = curedge;
        distance_[Nb] = edgeLen;
        keyLoc_[Nb] = pq_.insert(Nb);
        locTraversed.set(Nb);
      }
      else
      {
        if(edgeLen < distance_[Nb])
        {
          distance_[Nb] = edgeLen;
          elist_[Nb]    = curedge;
          pq_.decreaseKey(keyLoc_[Nb]);
        }
      }
    }
  }

  return FALSE;
  
}


/*-----------------------------------------------------------------------*/

Shortest2DPather::
Shortest2DPather(Complex* comp, StackBVTag& usedVx, Bool closed, BitVector * mask)
  : ShortestPather (comp, usedVx ,closed, mask),
    useSelection_(FALSE)
{

}

void Shortest2DPather::
setSelection (PointSelection * sel)
{
  if (sel)
    trans_ = *sel, useSelection_ = TRUE;
  else
    useSelection_ = FALSE;
}


/*-----------------------------------------------------------------------*/

double Shortest2DPather::
length_ (double x1[3], double x2[3])
{
  double p1[3];
  if (!useSelection_)
    return vlength(vsub(x1, x2, p1));
  double p2[3];
  trans_.transform (x1, p1);
  trans_.transform (x2, p2);
  p1[0] -= p2[0], p1[1] -= p2[1];
  return sqrt(p1[0]*p1[0] + p1[1]*p1[1]);
}

/*-----------------------------------------------------------------------*/
