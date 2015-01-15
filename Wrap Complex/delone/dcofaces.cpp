
#include <delone/dcofaces.h>

//------------------------------------------------------------------------

DCofaces::
DCofaces (DComplex * dc) : Cofaces(dc),
    dc_ (dc), iHandler_(dc->getIndexHandler()), dfs_(NULL)
{
  
}

//------------------------------------------------------------------------

DCofaces::
~DCofaces (void)
{

}

//------------------------------------------------------------------------

inline int DCofaces::
doTheCallback_ (Ksimplex sigma)
{ 
  return arg_.ksimpCB? (arg_.obj->*arg_.ksimpCB)(sigma) : 
	       (arg_.obj->*arg_.simpCB)(makeSimplex_(sigma)); 
}
//-------------------------------------------------------------------------

Bool DCofaces::
cofaces (Ksimplex sigma, Dim k, KsimplexCB cbFunc, RgiObject * obj)
{
  Dim d = sigma.getDim();
	if (d == k)
	{
    return (obj->*cbFunc)(sigma);
	}
	Assert (d < k);
	arg_ = Arg(cbFunc, 0, obj);
  return (this->*cofacesFunc_(d, k))(getFace_(sigma));
}

Bool DCofaces::
cofaces (Simplex sigma, Dim k, SimplexCB cbFunc, RgiObject * obj)
{
  Dim d = sigma.getDim();
	if (d == k)
	{
    return (obj->*cbFunc)(sigma);
	}
	Assert (d < k);
	arg_ = Arg(0, cbFunc, obj);
  return (this->*cofacesFunc_(d, k))(getFace_(sigma));
}

Bool DCofaces::
enumKsimplicies (Dim k, KsimplexCB cbFunc, RgiObject * obj)
{
  BitVectorIter iter(iHandler_->getBitVector(k));
	for (iter.first(); !iter.isDone(); iter.next())
	{
    if (!(obj->*cbFunc) (makeKsimplex_(iter.curItem(), k)))
		  return FALSE;
	}
	return TRUE;
}

//-------------------------------------------------------------------------

Bool DCofaces::
enumSimplicies (Dim k, SimplexCB cbFunc, RgiObject * obj)
{
  BitVectorIter iter(iHandler_->getBitVector(k));
  for (iter.first(); !iter.isDone(); iter.next())
	{
    if (!(obj->*cbFunc) (Simplex(iter.curItem(), k)))
		  return FALSE;
	}

	return TRUE;
}

//-------------------------------------------------------------------------

OrTriCB DCofaces::
cofacesFunc_(Dim d, Dim k)
{
  Assert (d < k);
	if (d)
	{
    switch (d + k)
		{
     case 3: return (OrTriCB) &DCofaces::cofaces12_;
		 case 4: return (OrTriCB) &DCofaces::cofaces13_;
		 case 5: return (OrTriCB) &DCofaces::cofaces23_;
		 default: Assert_always(0);
		}
	}

	switch (k)
	{ 
    case 1: return (OrTriCB) &DCofaces::cofaces01_;
		case 2: return (OrTriCB) &DCofaces::cofaces02_;
		case 3: return (OrTriCB) &DCofaces::cofaces03_;
		default: Assert_always(0);
	}
	return NULL;
}

//-------------------------------------------------------------------------

Bool DCofaces::
cofaces12_ (OrTri ef)
{
  OrTri efFirst = ef;
  if (!doTheCallback_ (Ksimplex (ef, 2)))
	  return FALSE;

  while (isFiniteSimplexFacet_ (ef)) {
    ef = Fnext_ (ef);
    if (ef == efFirst) return TRUE;
    if (!doTheCallback_ (Ksimplex (ef, 2)))
	    return FALSE;
  } 

  ef = Sym_ (efFirst);
  while (isFiniteSimplexFacet_ (ef)) {
    ef = Fnext_ (ef);
    Assert (ef != efFirst);
    if (!doTheCallback_ (Ksimplex (ef, 2)))
	    return FALSE;
  }
  return TRUE;
}

//-------------------------------------------------------------------------

Bool DCofaces::
cofaces13_ (OrTri ef)
{
  OrTri efFirst = ef;

  while (isFiniteSimplexFacet_ (ef)) 
	{
    if (!doTheCallback_ (Ksimplex (ef, 3)))
	   return FALSE;
    ef = Fnext_ (ef);
    if (ef == efFirst) return TRUE;
  } 
    
  ef = Sym_ (efFirst);
  while (isFiniteSimplexFacet_ (ef)) 
	{
    Assert (Org_ (Sym_ (ef)) == Org_ (efFirst));
    Assert (Dest_ (Sym_ (ef)) == Dest_ (efFirst));
		if (!doTheCallback_ (Ksimplex (Sym_(ef = Fnext_(ef)), 3)))
	    return FALSE;
    Assert (ef != efFirst);
  }
  return TRUE;
}

//-------------------------------------------------------------------------

Bool DCofaces::
cofaces23_ (OrTri ef)
{
	if (isFiniteSimplexFacet_(ef))
	 if (!doTheCallback_ (Ksimplex (ef, 3)))
	   return FALSE;

	ef = Sym_(ef);
  if (isFiniteSimplexFacet_(ef))
	 if (!doTheCallback_ (Ksimplex (ef, 3)))
	   return FALSE;

	return TRUE;
}

//-------------------------------------------------------------------------

Bool DCofaces::
report01_ (OrTri ef)
{
  VIndex i = Dest_(ef);
	// int i = getIndex_ (ef, 1);
	if (!dfs_->custTag.test(i))
	{
   if (!doTheCallback_ (Ksimplex (ef, 1)))
	  return FALSE;
    dfs_->custTag.set(i);
	}
	ef = Enext_(Sym_(ef));
  i = Dest_(ef);
	// i = getIndex_ (ef, 1);
  if (!dfs_->custTag.test(i))
	{
    if (!doTheCallback_ (Ksimplex (ef, 1)))
	   return FALSE;
    dfs_->custTag.set(i);
	}
	return TRUE;
}

//-------------------------------------------------------------------------

Bool DCofaces::
report02_ (OrTri ef)
{
  Assert (!dfs_->triTag.test(ef.index()));
  return doTheCallback_ (Ksimplex (ef, 2));
}

//---------------------------------------------------------------------------

Bool DCofaces::
report03_ (OrTri ef)
{
  Assert (!dfs_->triTag.test(ef.index()));
  if (isFiniteSimplexFacet_ (ef)) 
	{
		if (!dfs_->triTag.test(Fnext_(ef).index()) &&
		    !dfs_->triTag.test(Fnext_(Enext2_(ef)).index())) 
		  {
		    if(!doTheCallback_ (Ksimplex (ef, 3)))
	        return FALSE;
			}
  } 
    
	ef = Sym_(ef);
  if (!isFiniteSimplexFacet_(ef) ||
	    dfs_->triTag.test(Fnext_(ef).index()) ||
      dfs_->triTag.test(Fnext_(Enext_(ef)).index())) 
		return TRUE;
  Assert (Org_(Enext_(ef)) == Org_(Sym_(ef)));
	return doTheCallback_ (Ksimplex (Enext_(ef), 3));
}

//-------------------------------------------------------------------------

Bool DCofaces::cofaces0i_ (OrTri ef)
{
   OrTri tmp;
	 dfs_->s.push(ef);
	 while(!dfs_->s.empty())
	 {
	   ef = dfs_->s.pop();
	   if (dfs_->triTag.test(ef.index()))
		   continue;
		 if (!(this->*dfs_->report)(ef))
		  return FALSE;
	   dfs_->triTag.set(ef.index());
	   if (isFiniteSimplexFacet_ (ef))
	   {
       dfs_->s.push(tmp = Enext_(Fnext_(Enext2_(ef))));
			 dfs_->s.push(tmp = Fnext_(ef));
	   }
	 }
	 return TRUE;
}


Bool DCofaces::
cofaces0_ (OrTri ef, OrTriCB reportFunc)
{
  DFSArg dfs(dc_->getBVPool(), reportFunc); // for DFS over triangles
	dfs_ = &dfs;
  if (!cofaces0i_(ef))
	  return FALSE;
	ef = Sym_(ef);
	if (!isFiniteSimplexFacet_ (ef))
	  return TRUE;
	if (!cofaces0i_(Enext_(Fnext_(ef))))
	  return FALSE;
	return cofaces0i_(Fnext_(Enext_(ef)));
}

//-------------------------------------------------------------------------

Bool DCofaces::
cofaces01_ (OrTri ef)
{
	return cofaces0_(ef, (OrTriCB) &DCofaces::report01_);
}

//-------------------------------------------------------------------------

Bool DCofaces::
cofaces02_ (OrTri ef)
{
  return cofaces0_(ef, (OrTriCB) &DCofaces::report02_);
}

//-------------------------------------------------------------------------

Bool DCofaces::
cofaces03_ (OrTri ef)
{
  return cofaces0_(ef, (OrTriCB) &DCofaces::report03_);
}

//-------------------------------------------------------------------------

