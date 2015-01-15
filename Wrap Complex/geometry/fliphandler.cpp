/*
 * fliphandler.cpp -- Primitive flip routines for Delaunay triangulations.
 *
 * author -- Mike Facello (based on Ernst Mucke's original code)
 *
 *           Damrong, edit for dynamic triangulation
 *                    note the use of #ifdef _DYNAMIC_
 *
 * date -- 2/17/96
 *         9/28/96 -- Converted to C++
 */

/*--------------------------------------------------------------------------*/

#include <basic/comment.h>

#include <geometry/fliphandler.h>

/*--------------------------------------------------------------------------*/

ostream&
operator<< (ostream& s, FlipEvent& fe)
{
  /*
  int i;
  switch (fe.flipType_)
    {
    case FLIP_INSERT:
      s << "Adding the following triangles:\n";
      for (i=0; i<=5; i++)
        fe.tr_.printTriangle (s, fe.flippedTri_[i]);
      s << "Link triangles:\n";
      for (i=6; i<=9; i++)
	fe.tr_.printTriangle (s, fe.flippedTri_[i]);
      s << endl;
      break;
    case FLIP_TRIANGLE:
      s << "Replacing triangle:\n";
      fe.tr_.printTriangle (s, fe.flippedTri_[0]);
      s << "     with...\n";
      for (i=1; i<=3; i++)
	fe.tr_.printTriangle (s, fe.flippedTri_[i]);
      s << "Link triangles:\n";
      for (i=4; i<=9; i++)
	fe.tr_.printTriangle (s, fe.flippedTri_[i]);
      s << endl;
      break;
    case FLIP_EDGE:
      s << "Replacing three triangles:\n";
      for (i=0; i<=2; i++)
	fe.tr_.printTriangle (s, fe.flippedTri_[i]);
      s << "     with...\n";
      fe.tr_.printTriangle (s, fe.flippedTri_[3]);
      s << "Link triangles:\n";
      for (i=4; i<=9; i++)
	fe.tr_.printTriangle (s, fe.flippedTri_[i]);
      s << endl;
      break;
    case FLIP_VERTEX:
      s << "Removing the following triangles:\n";
      for (i=0; i<=5; i++)
        fe.tr_.printTriangle (s, fe.flippedTri_[i]);
      s << "Link triangles:\n";
      for (i=6; i<=9; i++)
	fe.tr_.printTriangle (s, fe.flippedTri_[i]);
      s << endl;
      break;
    }
  */
  return s;
}


/*-----------------------------------------------------------------------*/
/*---------------------------- CLASS FLIPHANDLER ------------------------*/
/*-----------------------------------------------------------------------*/

FlipHandler::
FlipHandler (Trist  * tr, RealPArray * rArray, LI_Converter& converter)
  : TristModifier (tr),
    rArray_ (rArray),
		sEval_(converter)
{
  
}

/*--------------------------------------------------------------------------*/

/*
 * insert (ijk, v) -- Insert vertex v inside the tetrahedra 
 *                         containing triangle ijk.  The four new tetrahedra
 *                         are created, and the search data structure is 
 *                         updated.
 */
Bool FlipHandler::
insert (OrTri ijk, VIndex v, FlipEvent &flipEvent)
{
  VIndex i, j, k, l;
  OrTri *flippedTri = flipEvent.getAllTri();

  // set the structure
  flipEvent.setType(FLIP_INSERT);
  flippedTri[9] = ijk;
  OrTri &ijv = flippedTri[0], &ikv = flippedTri[1], &jkv = flippedTri[2],
        &ilv = flippedTri[3], &jlv = flippedTri[4], &klv = flippedTri[5],
        &jil = flippedTri[6], &kjl = flippedTri[7], &ikl = flippedTri[8];



  /* Find the other three faces of the tetrahedra */

  jil = Fnext_ (ijk).sym();
  kjl = Fnext_ (ijk.enext()).sym();
  ikl = Fnext_ (ijk.enext2()).sym();

  /* Find the vertex numbers */

  tr_->tetra (ijk, i, j, k, l);

#ifdef _DYNAMIC_
  /* Callback for pre-flip process : Del tetra ijk */
  if (haveTristModCB (Del))
  {
      simplexSet_.clear ();
      simplexSet_.pushSimplex (ijk,3);
      callTristModCB (Del, & simplexSet_);
  }
#endif

  /* Create the 6 new triangles that are needed */

  ijv = MakeTri_(i, j, v);
  jkv = MakeTri_(j, k, v);
  ikv = MakeTri_(i, k, v);
  ilv = MakeTri_(i, l, v);
  jlv = MakeTri_(j, l, v);
  klv = MakeTri_(k, l, v);

  /* Update the face rings for the tetrahedra faces */

  Fmerge_(ijk, ijv);
  Fmerge_(Enext_(ijk), jkv);
  Fmerge_(Enext2_(ijk), Sym_(ikv));
  Fmerge_(Enext_(jil), ilv);
  Fmerge_(Enext_(ikl), klv);
  Fmerge_(Enext_(kjl), jlv);

  /* Create the rings containing only the new faces */

  /* vij -> vil -> vik */

  Fmerge_(Enext2_(ijv), Enext2_(ilv));
  Fmerge_(Enext2_(ilv), Enext2_(ikv));

  /* vjk -> vjl -> vji */

  Fmerge_(Enext2_(jkv), Enext2_(jlv));
  Fmerge_(Enext2_(jlv), Sym_(Enext_(ijv)));

  /* kvi -> kvj -> kvl */

  Fmerge_(Enext_(ikv), Enext_(jkv));
  Fmerge_(Enext_(jkv), Enext2_(Sym_(Enext_(klv))));

  /* lvi -> lvk -> lvj */

  Fmerge_(Enext_(ilv), Enext_(klv));
  Fmerge_(Enext_(klv), Enext_(jlv));

  /* Now, test all of the tetrahedra that were created */

  /*
#ifdef __DEBUG__
  tr_->testTriangle(ijv, i, j, v);
  tr_->testTriangle(ikv, i, k, v);
  tr_->testTriangle(ilv, i, l, v);
  tr_->testTriangle(jkv, j, k, v);
  tr_->testTriangle(jlv, j, l, v);
  tr_->testTriangle(klv, k, l, v);
  tr_->testOpenTetra(ijv, i, j, k, v);
  tr_->testOpenTetra(Sym_(ijv), j, i, l, v);
  tr_->testOpenTetra(Sym_(jkv), k, j, l, v);
  tr_->testOpenTetra(ikv, i, k, l, v);
#endif
*/

#ifdef _DYNAMIC_
  /* Callback for post-flip : Add vertex v, 4 new tetra, 4 new edges incident to v */
  if (haveTristModCB (Add))
  {
      simplexSet_.clear ();
			simplexSet_.pushSimplex (Enext2_(ijv), 0);  // vertex (v)ij

      simplexSet_.pushSimplex (Enext_ (ijv), 1);	// edge (j v)i
      simplexSet_.pushSimplex (Enext_ (ikv), 1);	// edge (k v)i
      simplexSet_.pushSimplex (Enext_ (ilv), 1);	// edge (l v)i
      simplexSet_.pushSimplex (Enext2_(ijv), 1);	// edge (v i)j

      simplexSet_.pushSimplex (ijv, 3);		        // tetra ij v l
      simplexSet_.pushSimplex (ikv, 3);		        // tetra ik v j
      simplexSet_.pushSimplex (jkv, 3);		        // tetra jk v l
      simplexSet_.pushSimplex (ilv, 3);		        // tetra il v k
      callTristModCB (Add, & simplexSet_);
  }
#endif
  return TRUE;
}


/*--------------------------------------------------------------------------*/

/*
 * flipTriangle (xyz) -- attempts to flip the triangle xyz.  Assumes that
 *                        xyz is a link facet, and that p_i, the inserted
 *                        point is on the positive side of xyz.  This means
 *                        that o in the code below is p_i.
 */
Bool FlipHandler::
flipTriangle (OrTri xyz, FlipEvent& flipEvent)
     /* assumes is_regular (xyz) == FALSE */
{

  VIndex x, y, z, o, p;
  OrTri xyo = Fnext_ (xyz);
  flipEvent.setType(FLIP_TRIANGLE);
  OrTri *flippedTri = flipEvent.getAllTri();

  Triangle_ (xyz, x, y, z);
  o = Dest_ (Enext_ (xyo));
  p = Dest_ (Enext_ (Fnext_ (Sym_ (xyz))));

  load (xyz, x, y, z, o, p, flippedTri);

#ifdef _DYNAMIC_
  /* Callback for pre-flip : Del 2 tetra incident to xyz */
  if (haveTristModCB (Del))
  {
      simplexSet_.clear ();
      simplexSet_.pushSimplex (flippedTri[4], 3);	// tetra yxoz
      simplexSet_.pushSimplex (flippedTri[7], 3);	// tetra xypz
      callTristModCB (Del, & simplexSet_);
  }

  /* Callback for pre-flip : Upd 3 vertices, 3 edges incident to xyz */
  if (haveTristModCB (Upd))
  {
      simplexSet_.clear ();
			simplexSet_.pushSimplex (flippedTri[5], 0);   // vertex (x)zo
			simplexSet_.pushSimplex (flippedTri[4], 0);   // vertex (y)xo
			simplexSet_.pushSimplex (flippedTri[6], 0);   // vertex (z)yo

      simplexSet_.pushSimplex (flippedTri[4], 1);   // edge (y x)o
      simplexSet_.pushSimplex (flippedTri[5], 1);   // edge (x z)o
      simplexSet_.pushSimplex (flippedTri[6], 1);   // edge (z y)o
      callTristModCB (Upd, & simplexSet_);
  }
#endif

  DelTri_ (xyz);
  diagonalize (xyz, flippedTri, x, y, z, o, p);

#ifdef _DYNAMIC_
  /* Callback for post-flip : Add 3 tetra, 1 edge surrounded by xyz */
  if (haveTristModCB (Add))
  {
      simplexSet_.clear ();
      simplexSet_.pushSimplex (flippedTri[1], 3);		// tetra pxoz
      simplexSet_.pushSimplex (flippedTri[2], 3);		// tetra pyox
      simplexSet_.pushSimplex (flippedTri[3], 3);		// tetra pzoy
      simplexSet_.pushSimplex (Enext2_ (flippedTri[1]), 1);	// edge (o p)x
      callTristModCB (Add, & simplexSet_);
  }
#endif

  return TRUE;
}


/*--------------------------------------------------------------------------*/

/*
 * flipEdge (pox) --    Flips the edge op.  Assumes that
 *                        pox is a link facet, and that p_i, the inserted
 *                        point is on the positive side of pox.  This means
 *                        that y in the code below is p_i.
 */
Bool FlipHandler::
flipEdge (OrTri pox, FlipEvent& flipEvent)
{
  VIndex x, y, z, o, p;
  OrTri poy, poz;
  flipEvent.setType(FLIP_EDGE);
  OrTri *flippedTri = flipEvent.getAllTri();

  Triangle_ (pox, p,  o,  x);
  poy = Fnext_ (pox);
  poz = Fnext_ (poy);
  y = Dest_ (poy.enext());
  z = Dest_ (poz.enext());

  /* flip edge to triangle;
     assuming sos_positive3 (pArray_, o, x, y, z) and
     sos_positive3 (pArray_, p, y, x, z) */

  load2 (pox, x, y, z, o, p, flippedTri);

#ifdef _DYNAMIC_
  /* Callback for pre-flip : Del 3 tetra, 1 edge incident to edge op */
  if (haveTristModCB (Del))
  {
      simplexSet_.clear ();
      simplexSet_.pushSimplex (flippedTri[0], 3);	// tetra po xy
      simplexSet_.pushSimplex (flippedTri[1], 3);	// tetra po yz
      simplexSet_.pushSimplex (flippedTri[2], 3);	// tetra po zx

      simplexSet_.pushSimplex (flippedTri[0], 1);	// edge (p o)x
      callTristModCB (Del, & simplexSet_);
  }
    
  /* Callback for pre-flip : Upd 6 edges adjacent to op, and their 5 vertices */
  if (haveTristModCB (Upd))
  {
      simplexSet_.clear ();
			simplexSet_.pushSimplex (flippedTri[9], 0); // vertex (x)pz
			simplexSet_.pushSimplex (flippedTri[5], 0); // vertex (y)px
			simplexSet_.pushSimplex (flippedTri[7], 0); // vertex (z)py
			simplexSet_.pushSimplex (flippedTri[4], 0); // vertex (o)yx
			simplexSet_.pushSimplex (Enext_(flippedTri[9]), 0); // vertex (p)zx

      simplexSet_.pushSimplex (flippedTri[4], 1);	// edge (o y)x
      simplexSet_.pushSimplex (flippedTri[6], 1);	// edge (o z)y
      simplexSet_.pushSimplex (flippedTri[8], 1);	// edge (o x)z
      simplexSet_.pushSimplex (flippedTri[5], 1);	// edge (y p)x
      simplexSet_.pushSimplex (flippedTri[7], 1);	// edge (z p)y
      simplexSet_.pushSimplex (flippedTri[9], 1);	// edge (x p)z
      callTristModCB (Upd, & simplexSet_);
  }
#endif
  
  DelTri_ (pox);         // Each of these triangles is now
  DelTri_ (poy);         // a search triangle.            
  DelTri_ (poz);
  cut (flippedTri, x, y, z, o, p);

#ifdef _DYNAMIC_
  /* Callback for post-flip : Add 2 tetra replacing edge op */
  if (haveTristModCB (Add))
  {
      simplexSet_.clear ();
      simplexSet_.pushSimplex (flippedTri[4], 3);	// tetra oyxz
      simplexSet_.pushSimplex (flippedTri[5], 3);	// tetra ypxz
      callTristModCB (Add, & simplexSet_);
  }
#endif

  return TRUE;
}

/*--------------------------------------------------------------------------*/

/*
 * flipVertex (ivj) --
 *        This procedure will attempt to flip the vertex that is the
 *        destination of triangle ef.
 */
Bool FlipHandler::
flipVertex (OrTri ivj, FlipEvent& flipEvent)
{
  flipEvent.setType(FLIP_VERTEX);
  OrTri *flippedTri = flipEvent.getAllTri();

  load3 (ivj, flippedTri);

#ifdef _DYNAMIC_
  /* Callback for pre-flip : Del v, and 4 tetra, 4 edges incident to v */
  if (haveTristModCB (Del))
  {
      simplexSet_.clear ();
      simplexSet_.pushSimplex (flippedTri[3], 0); // vertex (v)jk

      simplexSet_.pushSimplex (flippedTri[2], 1);	// edge (i v)l
      simplexSet_.pushSimplex (flippedTri[3], 1);	// edge (v j)k
      simplexSet_.pushSimplex (flippedTri[4], 1);	// edge (v k)l
      simplexSet_.pushSimplex (flippedTri[5], 1);	// edge (v l)j

      simplexSet_.pushSimplex (flippedTri[6], 3);	// tetra ijk v
      simplexSet_.pushSimplex (flippedTri[7], 3);	// tetra ikl v
      simplexSet_.pushSimplex (flippedTri[8], 3);	// tetra ilj v
      simplexSet_.pushSimplex (flippedTri[9], 3);	// tetra kjl v
      callTristModCB (Del, & simplexSet_);
  }
  /* Callback for pre-flip : Upd 4 vertices, 6 edges forming tetra ijkl around v */
  if (haveTristModCB (Upd))
  {
      simplexSet_.clear ();
      simplexSet_.pushSimplex (flippedTri[6], 0);           // vertex (i)jk
      simplexSet_.pushSimplex (flippedTri[9], 0);           // vertex (k)jl
      simplexSet_.pushSimplex (Enext_ (flippedTri[9]), 0);  // vertex (j)lk
      simplexSet_.pushSimplex (Enext2_(flippedTri[9]), 0);  // vertex (l)kj

      simplexSet_.pushSimplex (flippedTri[6], 1);		        // edge (i j)k
      simplexSet_.pushSimplex (flippedTri[7], 1);	          // edge (i k)l
      simplexSet_.pushSimplex (flippedTri[8], 1);	          // edge (i l)j
      simplexSet_.pushSimplex (flippedTri[9], 1);	          // edge (k j)l
      simplexSet_.pushSimplex (Enext_ (flippedTri[9]), 1);	// edge (j l)k
      simplexSet_.pushSimplex (Enext2_(flippedTri[9]), 1);	// edge (l k)j
      callTristModCB (Upd, & simplexSet_);
  }
#endif

  for (int i=0; i<=5; i++)
    DelTri_ (flippedTri[i]);

#ifdef _DYNAMIC_
  /* Callback for post-flip : Add new tetra ijkl */
  if (haveTristModCB (Add))
  {
      simplexSet_.clear ();
      simplexSet_.pushSimplex (flippedTri[6], 3);	// tetra ijk l
      callTristModCB (Add, & simplexSet_);
  }
#endif
  return TRUE;
}


/*--------------------------------------------------------------------------*/

ostream&  
operator<< (ostream& s, FlipHandler& fh)
{
/*
  s << "Primitive Flip Operations:" << endl << endl;
  s << '\t' << "Number of insertions: " << setw(14) << fh.numInserts_ 
    << endl;
  s << '\t' << "Number of triangle flips: " << setw(10) << fh.numTflips_ 
    << endl;
  s << '\t' << "Number of edge flips: " << setw(14) << fh.numEflips_
    << endl;
  s << '\t' << "Number of vertex flips: " << setw(12) << fh.numVflips_
    << endl;
  s << endl << endl;
*/
  return s;
}


/*--------------------------------------------------------------------------*/

//
// PRIVATE METHODS
//

/*--------------------------------------------------------------------------*/

/*
 * load () -- These are used to compute the triangles involved
 *                         in the flips and return them in an array.  load
 * 			   is used by flipTriangle.
 */

void FlipHandler::
load (OrTri xyz, VIndex x, VIndex y, VIndex z, VIndex o, VIndex p, 
      OrTri flippedTri[/*10*/])
     /* used to return:  
	          flippedTri[0] = xyo,  flippedTri[1] = yzo,  
		  flippedTri[2] = zxo,  flippedTri[3] = yxp,  
		  flippedTri[4] = xzp,  flippedTri[5] = zyp */
     /* Now returns:
	   Deleted:
 	       flippedTri[0] = xyz,   
           Added:
               flippedTri[1] = pxo,   flippedTri[2] = pyo,   
	       flippedTri[3] = pzo,   
           Link:
               flippedTri[4] = yxo,   flippedTri[5] = xzo,   
	       flippedTri[6] = zyo,   flippedTri[7] = xyp,   
	       flippedTri[8] = yzp,   flippedTri[9] = zxp
                                                                   */
     /* Conversion:
	old flippedTri[0] = Sym(flippedTri[4]);
	old flippedTri[1] = Sym(flippedTri[6]);
	old flippedTri[2] = Sym(flippedTri[5]);
	old flippedTri[3] = Sym(flippedTri[7]);
	old flippedTri[4] = Sym(flippedTri[9]);
	old flippedTri[5] = Sym(flippedTri[8]);
	*/
{
  Touch_args (x && y && z && o && p);

  flippedTri[0] = xyz;                       /* xyz */
  flippedTri[4] = Sym_(Fnext_(xyz));           /* yxo */
  flippedTri[5] = Enext_ (Sym_ (Fnext_ (Enext_ (flippedTri[4]))));      /* xzo */
  flippedTri[6] = Enext_ (Sym_ (Fnext_ (Enext_ (flippedTri[5]))));      /* zyo */
  flippedTri[7] = Sym_(Fnext_(Sym_(xyz)));                              /* xyp */
  flippedTri[8] = Enext_ (Sym_ (Fnext_ (Enext_ (flippedTri[7]))));      /* yzp */
  flippedTri[9] = Enext_ (Sym_ (Fnext_ (Enext_ (flippedTri[8]))));      /* zxp */

  Assert (tr_->testTriangle (flippedTri[0], x, y, z));
  Assert (tr_->testTriangle (flippedTri[4], y, x, o));
  Assert (tr_->testTriangle (flippedTri[5], x, z, o));
  Assert (tr_->testTriangle (flippedTri[6], z, y, o));
  Assert (tr_->testTriangle (flippedTri[7], x, y, p));
  Assert (tr_->testTriangle (flippedTri[8], y, z, p));
  Assert (tr_->testTriangle (flippedTri[9], z, x, p));
}

/*--------------------------------------------------------------------------*/

void FlipHandler::
load2 (OrTri pox, VIndex x, VIndex y, VIndex z, VIndex o, VIndex p, 
       OrTri flippedTri[/*10*/])
     /* used to return:  
	          flippedTri[0] = xyo,  flippedTri[1] = yzo,  
		  flippedTri[2] = zxo,  flippedTri[3] = yxp,  
		  flippedTri[4] = xzp,  flippedTri[5] = zyp */
     /* Now returns:
           Deleted:
	       flippedTri[0] = pox,   flippedTri[1] = poy,   
	       flippedTri[2] = poz,
           Added:
	       flippedTri[3] = xyz,   
           Link:
               flippedTri[4] = oyx,   flippedTri[5] = ypx,   
	       flippedTri[6] = ozy,   flippedTri[7] = zpy,   
	       flippedTri[8] = oxz,   flippedTri[9] = xpz
                                                                     */
     /* Conversion:
	old flippedTri[0] = Sym(Enext(flippedTri[4]));
	old flippedTri[1] = Sym(Enext(flippedTri[6]));
	old flippedTri[2] = Sym(Enext(flippedTri[8]));
	old flippedTri[3] = Sym(Enext2(flippedTri[5]));
	old flippedTri[4] = Sym(Enext2(flippedTri[9]));
	old flippedTri[5] = Sym(Enext2(flippedTri[7]));
	*/
{
  Touch_args (x && y && z && o && p);

  flippedTri[0] = pox;                       /* pox */
  flippedTri[1] = Fnext_(pox);                /* poy */
  flippedTri[2] = Fnext_(flippedTri[1]);           /* poz */
  flippedTri[4] = Enext_ (Sym_ (Fnext_ (Enext_ (flippedTri[0]))));      /* oyx */
  flippedTri[5] = Enext_ (Sym_ (Fnext_ (Enext_ (flippedTri[4]))));      /* ypx */
  flippedTri[6] = Enext_ (Sym_ (Fnext_ (Enext_ (flippedTri[1]))));      /* ozy */
  flippedTri[7] = Enext_ (Sym_ (Fnext_ (Enext_ (flippedTri[6]))));      /* zpy */
  flippedTri[8] = Enext_ (Sym_ (Fnext_ (Enext_ (flippedTri[2]))));      /* oxz */
  flippedTri[9] = Enext_ (Sym_ (Fnext_ (Enext_ (flippedTri[8]))));      /* xpz */

  Assert (tr_->testTriangle (flippedTri[0], p, o, x));
  Assert (tr_->testTriangle (flippedTri[1], p, o, y));
  Assert (tr_->testTriangle (flippedTri[2], p, o, z));
  Assert (tr_->testTriangle (flippedTri[4], o, y, x));
  Assert (tr_->testTriangle (flippedTri[5], y, p, x));
  Assert (tr_->testTriangle (flippedTri[6], o, z, y));
  Assert (tr_->testTriangle (flippedTri[7], z, p, y));
  Assert (tr_->testTriangle (flippedTri[8], o, x, z));
  Assert (tr_->testTriangle (flippedTri[9], x, p, z));
}

/*--------------------------------------------------------------------------*/

/*
 * load3 (ivj, flippedTri) --
 *           This procedure loads the array flippedTri with triangles needed
 *           for a vertex flip.  NOTE:  The vertex being deleted is j!!!
 */
void FlipHandler::
load3 (OrTri ivj, OrTri flippedTri[/*10*/])
     /* Returns:
	   flippedTri[0] = ivj,   flippedTri[1] = ivk,   
	   flippedTri[2] = ivl,   flippedTri[3] = vjk,   
	   flippedTri[4] = vkl,   flippedTri[5] = vlj,
	   flippedTri[6] = ijk,   flippedTri[7] = ikl,   
	   flippedTri[8] = ilj,   flippedTri[9] = kjl
                                                          */

{
  /* If e = ivj, then flippedTri is loaded as follows:  */

  flippedTri[0] = ivj;				                /* ivj */
  flippedTri[1] = Fnext_(ivj);			                /* ivk */
  flippedTri[2] = Fnext_(flippedTri[1]);			/* ivl */
  flippedTri[3] = Fnext_(Enext_(ivj));			        /* vjk */
  flippedTri[4] = Fnext_(Enext_(flippedTri[1]));		/* vkl */
  flippedTri[5] = Fnext_(Enext_(flippedTri[2]));		/* vlj */
  flippedTri[6] = Sym_(Fnext_(Enext2_(ivj)));		        /* ijk */
  flippedTri[7] = Sym_(Fnext_(Enext2_(flippedTri[1])));	        /* ikl */
  flippedTri[8] = Sym_(Fnext_(Enext2_(flippedTri[2])));	        /* ilj */
  flippedTri[9] = Sym_(Fnext_(Enext_(flippedTri[3])));	        /* kjl */
}


/*--------------------------------------------------------------------------*/

/*
 * diagonalize () and cut ()  -- This two functions are refinements of
 *        flipTriangle and flipEdge respectively.  They perform the
 *        trist manipulations necessary to update the triangle
 *        structure.
 */

void FlipHandler::
diagonalize (OrTri xyz, OrTri flippedTri[/*10*/], 
	     VIndex x, VIndex y, VIndex z, VIndex o, VIndex p)
     /* assumes that we have a double tetrahedron;
         creates 3 new triangles, i.e, 3*3 edfacets   */
{
  OrTri xpo, pyo, pzo, oxz, zpx, xpz, zox, yoz, pyz;

  Touch_args (xyz.getIntRep ());

  xpo = MakeTri_ (x, p, o);
  flippedTri[1] = Sym_(xpo);
  flippedTri[2] = pyo = MakeTri_ (p, y, o);
  flippedTri[3] = pzo = MakeTri_ (p, z, o);
  
/*
  oxz = Sym (Enext (flippedTri[2]));
  zpx = Enext (flippedTri[4]);
  xpz = Sym (Enext (zpx));
  zox = Enext2 (oxz);
  yoz = Enext (Sym (flippedTri[1]));
  pyz = Sym (Enext (flippedTri[5]));
*/

  oxz = Enext2_ (flippedTri[5]);
  xpz = Enext_ (flippedTri[9]);
  zpx = Sym_ (Enext_ (xpz));
  zox = Enext2_ (oxz);
  yoz = Enext_ (flippedTri[6]);
  pyz = Enext2_ (flippedTri[8]);

  Assert (tr_->testTriangle(oxz, o, x, z));
  Assert (tr_->testTriangle(xpz, x, p, z));
  Assert (tr_->testTriangle(zpx, z, p, x));
  Assert (tr_->testTriangle(zox, z, o, x));
  Assert (tr_->testTriangle(yoz, y, o, z));
  Assert (tr_->testTriangle(pyz, p, y, z));

  Fsplice_ (xpz, xpo);
  Fsplice_ (oxz, Enext2_ (xpo));
  Fsplice_ (pyz, pyo);
  Fsplice_ (yoz, Enext_ (pyo));
  Fsplice_ (Sym_ (zpx), pzo);
  Fsplice_ (zox, Enext_ (pzo));
  Fsplice_ (Enext_ (xpo), Enext_ (Sym_ (pyo)));
  Fsplice_ (Enext_ (Sym_ (pyo)), Enext_ (Sym_ (pzo)));

  Assert (diagonalizeTest (xpo, x, y, z, p, o));
}

/*--------------------------------------------------------------------------*/

#ifdef __DEBUG__

Bool FlipHandler::
diagonalizeTest (OrTri xpo, VIndex x, VIndex y, VIndex z, VIndex p, VIndex o)
{
  OrTri one, two, three;
  one   = Enext_ (xpo);
  two   = Fnext_ (one);
  three = Fnext_ (two);
  Assert_always (    (one == Fnext_ (three))
                 and tr_->testTriangle (one,   p, o, x)
                 and tr_->testTriangle (two,   p, o, y)
                 and tr_->testTriangle (three, p, o, z)
                 and tr_->testOpenTetra (one,   p, o, z, x)
                 and tr_->testOpenTetra (two,   p, o, x, y)
                 and tr_->testOpenTetra (three, p, o, y, z));
  return (TRUE);
}

#endif

/*--------------------------------------------------------------------------*/

void FlipHandler::
cut (OrTri flippedTri[/*10*/],
     VIndex x, VIndex y, VIndex z, VIndex o, VIndex p)
     /* assumes that we have a double tetrahedron;
	creates 1 new triangle, i.e, 1*3 edfacets + syms !! (every else too)*/
{
  OrTri xyz;

  Touch_args (o && p);

  flippedTri[3] = xyz = MakeTri_ (x, y, z);

  Fsplice_ (Enext2_ (flippedTri[5]), xyz);                    // xyp
  Fsplice_ (Enext2_ (flippedTri[7]), Enext_ (xyz));            // yzp
  Fsplice_ (Enext2_ (flippedTri[9]), Enext_ (Enext_ (xyz)));    // zxp
}

/*--------------------------------------------------------------------------*/

