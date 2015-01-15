// compDB.cpp: implementation of the compDB class.
//
//////////////////////////////////////////////////////////////////////

#include "compDB.h"
#include "delone/dcbuilder.h"
#include <basic\vectmat.h>
#include <basic\queue.h>

double *doubleArray;

int compareDouble (const int * a, const int * b)
{
    if(doubleArray[*a]>doubleArray[*b])
        return 1;
    else if(doubleArray[*a]==doubleArray[*b])
        return 0;
    else return -1;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CompDB::CompDB(DCompBuilder* dcb,bool computeAlphaFilter) : 
dcomp_(dcb->getComplex()) ,
rpArray_(dcb->getPArray()),
tr_(dcb->getTrist()),
redundent_(dcb->getRedundentBV()),
computeAlphaFilter_(computeAlphaFilter),
topoChangeTime_(NULL),
rou_(NULL),
dp_(NULL),
master_(NULL),
tetraset_(NULL),
masterIdx_(NULL),
tetraIdx_(NULL)
{
    ih_ = new PackedIndexHandler(dcomp_);
    ih_->init();
    createRecordAndIndices_();

    if(computeAlphaFilter_)
    {
        computeSize_();
        computeHiddenBV_();
		computeNormal_();
		computeMinWallDistSqr();
		//Assert(AfxCheckMemory());
   		computeAlpha();
		makeMasterList();
		//Assert(AfxCheckMemory());
    }
    delete ih_;
};

Trist* CompDB::getTrist()
{
    return dcomp_->getTrist();
};

RealPArray* CompDB::getPArray()
{
    return rpArray_;
};

bool CompDB::topoChg(Index i,Dim d)
{
    switch(d)
    {
    case 0:
        return (!redundent_->test(i))&&!hiddenBV_[0]->test(i);
        break;
    case 1:
    case 2:
        return (!hiddenBV_[d]->test(i))&&!hideBV_[d]->test(i);
        break;
    case 3:
        return !hideBV_[3]->test(i);
    }

    return false;
}

void CompDB::
createRecordAndIndices_()
{
    int i;
    nSimp_[0] = rpArray_->size();
    nSimp_[1] = ih_->nEdge();
    nSimp_[2] = dcomp_->numTriangles();
    nSimp_[3] = ih_->nTetra();
    
    BitVector vbv(nSimp_[0]+1);
    BitVector ebv(nSimp_[1]+1);
    BitVector tbv(nSimp_[3]+1);
    
    //	Assert(dcomp_->numEdges() == nSimp_[1]);
    Assert(dcomp_->numTetra() == nSimp_[3]);
    for(i=0;i<4;i++)
    {
        XTop_[i] = new BitVector(nSimp_[i]+1);
        if(i<3)
            hiddenBV_[i] = new BitVector(nSimp_[i]+1);
        else
            hiddenBV_[i] = NULL;
        simpRec_[i] = new SimpRecord [nSimp_[i]+1];
        if(i)
            hideBV_[i] = new BitVector(nSimp_[i]+1);
        else
            hideBV_[i] = NULL;
    }
    
    TristIter ti(*dcomp_->getTrist());
    
    for(ti.first();!ti.isDone();ti.next())
    {
		minAlpha_ = MAXFLOAT;

        OrTri abc(ti.curItem(),0);
        VIndex a,b,c,d,e;
        EIndex ab,bc,ca,ad,ae;
        TIndex abcd,abce;
        tr_->tetra(abc,a,b,c,d);
        simpRec_[0][a].repTri = abc;
        simpRec_[0][b].repTri = tr_->enext(abc);
        simpRec_[0][c].repTri = tr_->enext2(abc);

		minAlpha_ = min(-getW(a),minAlpha_);
		minAlpha_ = min(-getW(b),minAlpha_);
		minAlpha_ = min(-getW(c),minAlpha_);

        tr_->tetra(tr_->sym(abc),b,a,c,e);
        ab = ih_->ix_v(a,b);
        bc = ih_->ix_v(b,c);
        ca = ih_->ix_v(c,a);
        abcd = (d) ? ih_->ix_v(a,b,c,d) : 0;
        abce = (e) ? ih_->ix_v(b,a,c,e) : 0;
        if(d)
            ad = ih_->ix_v(a,d);
        if(e)
            ae = ih_->ix_v(a,e);
        tr_->setPackedIndex(ti.curItem(),ab,bc,ca,abcd,abce);
        
        if(!ebv.test(ab))
        {
            simpRec_[1][ab].repTri = abc;
            computeEdgeCenter(a,b,simpRec_[1][ab].center);
            ebv.set(ab);
        }
        
        if(!ebv.test(bc))
        {
            simpRec_[1][bc].repTri = tr_->enext(abc);
            computeEdgeCenter(b,c,simpRec_[1][bc].center);
            ebv.set(bc);
        }
        
        if(!ebv.test(ca))
        {
            simpRec_[1][ca].repTri = tr_->enext2(abc);
            computeEdgeCenter(c,a,simpRec_[1][ca].center);
            ebv.set(ca);
        }
        
        simpRec_[2][abc.index()].repTri = abc;
        computeFaceCenter(a,b,c,simpRec_[2][abc.index()].center);
        
        if(d)
            if(!tbv.test(abcd))
            {
                simpRec_[3][abcd].repTri = abc;
                if(ebv.test(ab)&&ebv.test(ca)&&ebv.test(ad))
                {
                    computeTetrahedronCenter(a,b,c,d,simpRec_[3][abcd].center);
                    tbv.set(abcd);
                }
            }	
            if(e)
                if(!tbv.test(abce))
                {
                    simpRec_[3][abce].repTri = tr_->sym(abc);
                    if(ebv.test(ab)&&ebv.test(ca)&&ebv.test(ae))
                    {
                        computeTetrahedronCenter(a,b,c,e,simpRec_[3][abce].center);
                        tbv.set(abce);
                    }
                }
                
    }

};

//added by xinwei, 2003/08/25, find cofaces
int CompDB::getSxIndex(VIndex a, VIndex b)
{
	return ih_->ix_v(a,b);
}

int CompDB::getSxIndex(VIndex a, VIndex b, VIndex c, VIndex d)
{
	return ih_->ix_v(a,b,c,d);
}

void CompDB::computeHiddenBV_()
{
    SizeEvaluator& eval = dcomp_->getSizeEvaluator();
    RealPArray * rArray = dcomp_->getPArray();
    LI_Converter& converter = dcomp_->getConverter();
    int v1, v2, v3;
	OrTri f[4];
	VIndex tv[4];
    EIndex ei1,ei2,ei3;
    int i,j;
    for(i=0;i<3;i++)
        hiddenBV_[i]->reset();
    for(i=1;i<4;i++)
        hideBV_[i]->reset();
    
    for(i=1;i<=nSimp_[1];i++)
    {
        OrTri ef = repTri(i,1);
        v1 = org(ef);
        v2 = dest(ef);
        if(eval.hidden(rArray, converter, v1,v2))
        {
            hiddenBV_[0]->set(v1);
            hideBV_[1]->set(i);
        }
        if(eval.hidden(rArray, converter, v2,v1))
        {
            hiddenBV_[0]->set(v2);
            hideBV_[1]->set(i);
        }
    }
    
    for(i=1;i<=nSimp_[2];i++)
    {
        OrTri ef = repTri(i,2);
        v1 = org(ef);
        v2 = dest(ef);
        v3 = dest(ef.enext());
        ei1 = getIndex(ef,1);
        ei2 = getIndex(ef.enext(),1);
        ei3 = getIndex(ef.enext2(),1);
        
		double vv1[4],vv2[4],vv3[4];
		rArray->getPoint(v1,vv1);
		rArray->getPoint(v2,vv2);
		rArray->getPoint(v3,vv3);
        if(eval.hidden(rArray, converter, v1,v2,v3))
        {
            hiddenBV_[1]->set(ei1);
            hideBV_[2]->set(i);
        }
        if(eval.hidden(rArray, converter, v2,v3,v1))
        {
            hiddenBV_[1]->set(ei2);
            hideBV_[2]->set(i);
        }
        if(eval.hidden(rArray, converter, v3,v1,v2))
        {
            hiddenBV_[1]->set(ei3);
            hideBV_[2]->set(i);
        }
    }

    for(i=1;i<=nSimp_[3];i++)
    {
        OrTri ef = repTri(i,3);
		if(!tr_->isOnHull(ef))
		{
			f[0] = ef;
			f[1] = fnext(ef).sym();
			f[2] = fnext(ef.enext()).sym();
			f[3] = fnext(ef.enext2()).sym();
			for(j=0;j<4;j++)
			{
				tv[j] = dest(enext(fnext(f[j])));
				v1 = org(f[j]);
				v2 = dest(f[j]);
				v3 = dest(f[j].enext());

				if(eval.hidden(rArray,converter, v1,v2,v3,tv[j]))
                {
					hiddenBV_[2]->set(f[j].index());
                    hideBV_[3]->set(i);
                }
			}
		}
    }
    
}

void CompDB::computeNormal_()
{
	int d,i;
	int ai,bi,ci;
	OrTri ef;

	for(d=1;d<3;d++)
	{
		for(i=1;i<=nSimp_[d];i++)
		{
			if(d==1)
			{
				ai = org(repTri(i,d));
				bi = dest(repTri(i,d));
				simpRec_[d][i].n = getVertex(MIN(ai,bi))-getVertex(MAX(ai,bi));
				
			} else
			{
				ai = org(repTri(i,d));
				bi = dest(repTri(i,d));
				ci = dest(repTri(i,d).enext());
				simpRec_[d][i].n = cross_prod(getVertex(ci)-getVertex(bi),getVertex(ai)-getVertex(bi));
			}
			simpRec_[d][i].n.normalize();
			simpRec_[d][i].m = AlignWithYAxis(simpRec_[d][i].n);
			simpRec_[d][i].mi = simpRec_[d][i].m.Inverse();
		}
	}
}


void CompDB::computeAlphaShapeFilter_()
{
	//add double w -- the weight of the original.
	double w;
    int i,d;
    Vector3 v1,v2,v3,v4;
    Vector3 c;
    double aSqr;
    
    Assert(!topoChangeTime_);
    doubleArray = new double [nSimp_[0]+nSimp_[1]+nSimp_[2]+nSimp_[3]];
    int* intArray = new int [nSimp_[0]+nSimp_[1]+nSimp_[2]+nSimp_[3]];
	//------------
	rou_ = new double [nSimp_[0]+nSimp_[1]+nSimp_[2]+nSimp_[3]-redundent_->getNumSet()];
	//------------
    totalNSimp_ = 0;
    for(d=3;d>=0;d--)
    {
        for(i=1;i<=nSimp_[d];i++)
        {
            if(d==0)
                if(redundent_->test(i))
                    continue;
                
                c = getCenter((i<<2)|d);

				//consider the hidden cases for the computation of alpha filter
			    switch (d) {
                case 0:
                    v1 = getVertex(i);
					w = getW(i);

					if(!hidden(i,d))
					{
						//for the M0 case:
						/*--------------------------------------------------------------
						the old formula contains some minor errors,  it should finds out 
						the weight of the ball, while it points to sth else.
						---------------------------------------------------------------*/
						//aSqr = ((c-v1).lengthsqr() - v1.x[4]*v1.x[4]);
						aSqr = - w*w;
						//------------------end of change-------------------------------
						doubleArray[totalNSimp_] = Sign(aSqr)*sqrt(fabs(aSqr));
						intArray[totalNSimp_]=totalNSimp_;
						rou_[totalNSimp_] = doubleArray[totalNSimp_];
						totalNSimp_++;
					}else
					{
						IterStack<OrTri> es;
						OrTri ef, ef2;
						int idx, idx2;
						bool findhide=false;
						Vector3 vh1,vh2,vh3;
					    Vector3 ch;
						double ah, wh;

						ef2 = repTri(i, 0);
						idx = getIndex(ef2, 0);

						edgeStar(idx, &es);

						while(!es.empty())
						{
							ef = es.pop();
								
							idx2 = getIndex(ef, 1);						
														
							if(hide(idx2,1))
							{
								if(!hidden(idx2,1))
								{
									ch = getCenter((idx2<<2)|1);
									
									getVertex(idx2,vh1,vh2);
									wh = getW(org(repTri(idx2, 1)));
									
									ah = ((ch-vh1).lengthsqr() - wh*wh);

									doubleArray[totalNSimp_] = Sign(ah)*sqrt(fabs(ah));
									intArray[totalNSimp_]=totalNSimp_;
									rou_[totalNSimp_] = doubleArray[totalNSimp_];
									totalNSimp_++;
									findhide=true;
									break;
								}
								else
								{
									//find all the triangles include the edge, find the hide triangle
									OrTri ef23, ef3;
									Index fi3;
									Vector3 vh13,vh23,vh33;
									Vector3 ch3;
									double ah3, wh3;

									ef3 = repTri(idx2, 1);
									ef23 = ef3;
									do {
										fi3 = getIndex(ef23, 2);
										if(hide(fi3, 2))
										{
											if(!hidden(fi3, 2))
											{
												ch3 = getCenter((fi3<<2)|2);
												
												getVertex(fi3,vh13,vh23,vh33);
												wh = getW(org(repTri(fi3, 2)));
												// wh3 is not initialized.
												// modify wh3 -> wh
												ah3 = ((ch3-vh13).lengthsqr() - wh*wh);

												doubleArray[totalNSimp_] = Sign(ah3)*sqrt(fabs(ah3));
												intArray[totalNSimp_]=totalNSimp_;
												rou_[totalNSimp_] = doubleArray[totalNSimp_];
												totalNSimp_++;
												findhide=true;
												break;
											}
											else
											{
												//find all the tetrahedra include the triangles, find the hide tetra
												OrTri ef4;
												Index fi14, fi24;
												Vector3 vh14,vh24,vh34, vh44;
												Vector3 ch4;
												double ah4, wh4;

												ef4 = repTri(fi3, 2);
											
												fi14 = getIndex(ef4, 3);
												fi24 = getIndex(ef4.sym(), 3);

												if(hide(fi14, 3))
												{
													ch4 = getCenter((fi14<<2)|3);
													
													getVertex(fi14,vh14,vh24,vh34, vh44);
													wh4 = getW(org(repTri(fi14, 3)));			
												}
												else
												{
													Assert(hide(fi24, 3));
													ch = getCenter((fi24<<2)|3);
													
													getVertex(fi24,vh14,vh24,vh34, vh44);
													wh4 = getW(org(repTri(fi24, 3)));		
												}

												ah4 = ((ch4-vh14).lengthsqr() - wh4*wh4);

												doubleArray[totalNSimp_] = Sign(ah4)*sqrt(fabs(ah4));
												intArray[totalNSimp_]=totalNSimp_;
												rou_[totalNSimp_] = doubleArray[totalNSimp_];
												totalNSimp_++;
												break;
											}
										}
										ef23 = tr_->fnext(ef23);
									} while (ef23!=ef3);

									break;
								}
							}
						}
					//	Assert(findhide==true);
					}

                    break;
                case 1:
                    getVertex(i,v1,v2);
					w = getW(org(repTri(i, 1)));

					if(!hidden(i,d)) //for the M1 case:
					{
						/*--------------------------------------------------------------
						the old formula contains some minor errors,  it should finds out 
						the weight of the ball, while it points to sth else.
						---------------------------------------------------------------*/
						//aSqr = ((c-v1).lengthsqr() - v1.x[4]*v1.x[4]);
						aSqr = ((c-v1).lengthsqr() - w*w);
						//------------------end of change-------------------------------
						doubleArray[totalNSimp_] = Sign(aSqr)*sqrt(fabs(aSqr));
						intArray[totalNSimp_]=totalNSimp_;
						rou_[totalNSimp_] = doubleArray[totalNSimp_];
						totalNSimp_++;
					}
					else{
						//hidden cases
						//find all the triangles include the edge, find the hide triangle
						OrTri ef2, ef;
						Index fi;
					    Vector3 vh1,vh2,vh3;
					    Vector3 ch;
						double ah, wh;

						ef = repTri(i, 1);
						ef2 = ef;
						do {
							fi = getIndex(ef2, 2);
							if(hide(fi, 2))
							{
								ch = getCenter((fi<<2)|2);
								
								getVertex(fi,vh1,vh2,vh3);
								wh = getW(org(repTri(fi, 2)));
								
								ah = ((ch-vh1).lengthsqr() - wh*wh);

								doubleArray[totalNSimp_] = Sign(ah)*sqrt(fabs(ah));
								intArray[totalNSimp_]=totalNSimp_;
								rou_[totalNSimp_] = doubleArray[totalNSimp_];
								totalNSimp_++;
								break;
							}
							ef2 = tr_->fnext(ef2);
						} while (ef2!=ef);
					}

                    break;
                case 2:
                    getVertex(i,v1,v2,v3);
					w = getW(org(repTri(i, 2)));

					if(!hidden(i,2)) //for the M2 case:
					{
					/*--------------------------------------------------------------
					the old formula contains some minor errors,  it should finds out 
					the weight of the ball, while it points to sth else.
					---------------------------------------------------------------*/
					//aSqr = ((c-v1).lengthsqr() - v1.x[4]*v1.x[4]);
					aSqr = ((c-v1).lengthsqr() - w*w);
					//------------------end of change-------------------------------
					doubleArray[totalNSimp_] = Sign(aSqr)*sqrt(fabs(aSqr));
					intArray[totalNSimp_]=totalNSimp_;
					rou_[totalNSimp_] = doubleArray[totalNSimp_];
					totalNSimp_++;
					}
					else	//hidden cases
					{
						//hidden cases
						//find all the tetrahedra include the triangles, find the hide tetra
						OrTri ef;
						Index fi1, fi2;
					    Vector3 vh1,vh2,vh3, vh4;
					    Vector3 ch;
						double ah, wh;

						ef = repTri(i, 2);
					
						fi1 = getIndex(ef, 3);
						fi2 = getIndex(ef.sym(), 3);

						if(hide(fi1, 3))
						{
							ch = getCenter((fi1<<2)|3);
							
							getVertex(fi1,vh1,vh2,vh3, vh4);
							wh = getW(org(repTri(fi1, 3)));			
						}
						else
						{
							Assert(hide(fi2, 3));
							ch = getCenter((fi2<<2)|3);
							
							getVertex(fi2,vh1,vh2,vh3, vh4);
							wh = getW(org(repTri(fi2, 3)));		
						}

						ah = ((ch-vh1).lengthsqr() - wh*wh);

						doubleArray[totalNSimp_] = Sign(ah)*sqrt(fabs(ah));
						intArray[totalNSimp_]=totalNSimp_;
						rou_[totalNSimp_] = doubleArray[totalNSimp_];
						totalNSimp_++;
					}

                    break;
                case 3:
                    getVertex(i,v1,v2,v3,v4);
					w = getW(org(repTri(i, 3)));

					//for the M3 case:
					/*--------------------------------------------------------------
					the old formula contains some minor errors,  it should finds out 
					the weight of the ball, while it points to sth else.
					---------------------------------------------------------------*/
					//aSqr = ((c-v1).lengthsqr() - v1.x[4]*v1.x[4]);
					aSqr = ((c-v1).lengthsqr() - w*w);
					//------------------end of change-------------------------------
					doubleArray[totalNSimp_] = Sign(aSqr)*sqrt(fabs(aSqr));
					intArray[totalNSimp_]=totalNSimp_;
					rou_[totalNSimp_] = doubleArray[totalNSimp_];
					totalNSimp_++;
                
					break;
               
				default:
                    Assert(0);
                }

                
                /*switch (d) {
                case 0:
                    v1 = getVertex(i);
					w = getW(i);
                    break;
                case 1:
                    getVertex(i,v1,v2);
					w = getW(org(repTri(i, 1)));
                    break;
                case 2:
                    getVertex(i,v1,v2,v3);
					w = getW(org(repTri(i, 2)));
                    break;
                case 3:
                    getVertex(i,v1,v2,v3,v4);
					w = getW(org(repTri(i, 3)));
                    break;
                default:
                    Assert(0);
                }
				/*--------------------------------------------------------------
				the old formula contains some minor errors,  it should finds out 
				the weight of the ball, while it points to sth else.
				---------------------------------------------------------------*/
				//aSqr = ((c-v1).lengthsqr() - v1.x[4]*v1.x[4]);
				/*aSqr = ((c-v1).lengthsqr() - w*w);
				//------------------end of change-------------------------------
					doubleArray[totalNSimp_] = Sign(aSqr)*sqrt(fabs(aSqr));
					intArray[totalNSimp_]=totalNSimp_;
				//------
				rou_[totalNSimp_] = doubleArray[totalNSimp_];
				//------
					totalNSimp_++;
				*/
        }
    }
    
    basic_qsort(intArray,0,totalNSimp_-1,&compareDouble);
    topoChangeTime_ = new double [totalNSimp_];
    for(i=0;i<totalNSimp_;i++)
        topoChangeTime_[i] = doubleArray[intArray[i]];
    Assert(totalNSimp_=nSimp_[0]+nSimp_[1]+nSimp_[2]+nSimp_[3]-redundent_->getNumSet());
    delete [] intArray;
    delete [] doubleArray;
}

//-----------------method to compute the alpha value-----------------
//for those simplex whose dimesion is lower than tetrahedron, we 
//compute its miu_low and miu_high
void CompDB::computeAlpha()
{
	computeAlphaShapeFilter_();
	int i,j;

	//initialize the rankTable
	int totalSimp = 0;
	for(i = 0; i < 3; i++)
		rankTable_[i] = new double [2*nSimp_[i]+2];
	for(i=0; i<3; i++)
		for(j=0; j<2*nSimp_[i]+2; j++)
			rankTable_[i][j] = MINUSINFI;
	
	//compute the regular & interior value based on the value provided
	//by computeAlphaShapeFilter_()
	TristIter ti(*dcomp_->getTrist());
    for(ti.first();!ti.isDone();ti.next())
    {
		//1. triangle
		double miuL, miuU;
		OrTri abc(ti.curItem(),0);
		VIndex a,b,c,d,e;
        EIndex ab,bc,ca;
        TIndex abcd,abce;
        
		FIndex curabc=getIndex(abc,2);
        
        tr_->tetra(abc,a,b,c,d);
		tr_->tetra(tr_->sym(abc),b,a,c,e);
        
        abcd = (d) ? ih_->ix_v(a,b,c,d) : 0;
        abce = (e) ? ih_->ix_v(b,a,c,e) : 0;
		if(abcd)
		{
			if(rankTable_[2][curabc]==MINUSINFI && rankTable_[2][curabc+nSimp_[2]]==MINUSINFI)
			{
				rankTable_[2][curabc] = rou_[abcd-1];
				rankTable_[2][curabc+nSimp_[2]] = rou_[abcd-1];
			}

			if(rankTable_[2][curabc]>rou_[abcd-1])
				rankTable_[2][curabc] = rou_[abcd-1];

			if(rankTable_[2][curabc+nSimp_[2]]<rou_[abcd-1])
				rankTable_[2][curabc+nSimp_[2]] =rou_[abcd-1];
		}

		if(abce)
		{
			if(rankTable_[2][curabc]==MINUSINFI && rankTable_[2][curabc+nSimp_[2]]==MINUSINFI)
			{
				rankTable_[2][curabc] = rou_[abce-1];
				rankTable_[2][curabc+nSimp_[2]] = rou_[abce-1];
			}

			if(rankTable_[2][curabc]>rou_[abce-1])
				rankTable_[2][curabc] = rou_[abce-1];

			if(rankTable_[2][curabc+nSimp_[2]]<rou_[abce-1])
				rankTable_[2][curabc+nSimp_[2]] =rou_[abce-1];
		}
		
		//2. edge
		//miuU = rou_[nSimp_[3]+curabc-1];
		miuU = getAlpha(curabc, 2);
		if(hiddenBV_[2]->test(curabc))
			miuL=rankTable_[2][curabc];
		else
			miuL=miuU;
		ab = ih_->ix_v(a,b);
        bc = ih_->ix_v(b,c);
        ca = ih_->ix_v(c,a);
		
		//for edge ab
		if(rankTable_[1][ab]==MINUSINFI && rankTable_[1][ab+nSimp_[1]]==MINUSINFI)
		{
			rankTable_[1][ab] = miuL;
			rankTable_[1][ab+nSimp_[1]] = miuU;
		}

		if(rankTable_[1][ab]>miuL)
			rankTable_[1][ab] = miuL;

		if(rankTable_[1][ab+nSimp_[1]]<miuU)
			rankTable_[1][ab+nSimp_[1]] =miuU;

		//for edge bc
		if(rankTable_[1][bc]==MINUSINFI && rankTable_[1][bc+nSimp_[1]]==MINUSINFI)
		{
			rankTable_[1][bc] = miuL;
			rankTable_[1][bc+nSimp_[1]] = miuU;
		}

		if(rankTable_[1][bc]>miuL)
			rankTable_[1][bc] = miuL;

		if(rankTable_[1][bc+nSimp_[1]]<miuU)
			rankTable_[1][bc+nSimp_[1]] =miuU;

		//for edge ca
		if(rankTable_[1][ca]==MINUSINFI && rankTable_[1][ca+nSimp_[1]]==MINUSINFI)
		{
			rankTable_[1][ca] = miuL;
			rankTable_[1][ca+nSimp_[1]] = miuU;
		}

		if(rankTable_[1][ca]>miuL)
			rankTable_[1][ca] = miuL;

		if(rankTable_[1][ca+nSimp_[1]]<miuU)
			rankTable_[1][ca+nSimp_[1]] =miuU;
	}

	//3. vertex
}

void CompDB::makeMasterList()
{
	int i, j;
	int cur = 0;
	master_ = new MasterEntry[totalNSimp_*3];
	for(i=0; i<totalNSimp_*3; i++){
		master_[i].faceIdx=0;
		master_[i].faceType=5;
		master_[i].rankType=0;
		master_[i].rankValue=0;
	}

	double curAlpha=MINUSINFI;
	alphaN=0;
	masterIdx_ = new int[totalNSimp_+1];
	for(i=0; i<totalNSimp_+1;i++)
		masterIdx_[i]=0;

	for(i=0; i<totalNSimp_; i++)
	{
		if(topoChangeTime_[i]!=curAlpha) {
			alphaN++;
			curAlpha=topoChangeTime_[i];
			masterIdx_[alphaN]=cur;
		}
		else
			continue;

		//1. tetrahedron(check only rou_ value)
		for(j=0; j<nSimp_[3]; j++)
		{
			if(rou_[j]==topoChangeTime_[i])
			{
				//if(j==1933)
				//	rou_[j] = topoChangeTime_[i];
				master_[cur].faceIdx=j+1;
				master_[cur].faceType=3;
				master_[cur].rankType=0;
				master_[cur].rankValue=rou_[j];
				cur++;
			}
		}
		//2. triangle(check both rou_ and rankTable_ value)
		for(j=0; j<nSimp_[2]; j++)
		{
			if(!hiddenBV_[2]->test(j+1))
			{
				if(rou_[nSimp_[3]+j]==topoChangeTime_[i])
				{
					master_[cur].faceIdx=j+1;
					master_[cur].faceType=2;
					master_[cur].rankType=0;
					master_[cur].rankValue=topoChangeTime_[i];
					cur++;
				}
			}
			else{
				if(rankTable_[2][j+1]==topoChangeTime_[i]){
					master_[cur].faceIdx=j+1;
					master_[cur].faceType=2;
					master_[cur].rankType=1;
					master_[cur].rankValue=rankTable_[2][j];
					cur++;
				}
			}
		}
		//3. edge(same as triangle)
		for(j=0; j<nSimp_[1]; j++)
		{
			if(!hiddenBV_[1]->test(j+1))
			{
				if(rou_[nSimp_[3]+nSimp_[2]+j]==topoChangeTime_[i])
				{
					master_[cur].faceIdx=j+1;
					master_[cur].faceType=1;
					master_[cur].rankType=0;
					master_[cur].rankValue=topoChangeTime_[i];
					cur++;
				}
			}
			else{
				if(rankTable_[1][j+1]==topoChangeTime_[i]){
					master_[cur].faceIdx=j+1;
					master_[cur].faceType=1;
					master_[cur].rankType=1;
					master_[cur].rankValue=rankTable_[1][j];
					cur++;
				}
			}
		}
		//4. vertex(check only rou_ value)
		for(j=0; j<nSimp_[0]-redundent_->getNumSet(); j++)
		{                 
			if(rou_[nSimp_[3]+nSimp_[2]+nSimp_[1]+j]==topoChangeTime_[i])
			{
				master_[cur].faceIdx=j+1;
				master_[cur].faceType=0;
				master_[cur].rankType=0;
				master_[cur].rankValue=topoChangeTime_[i];
				cur++;
			}
		}
	}

	
	//make a flag at the end of master list
	//master_[cur].faceType=5;
	sortTetra();
	getDp();
}

double CompDB::getAlpha(int Idx, Dim d)
{
	//int testidx;
	double aSqr;
	double maxA = MINUSINFI;
	//double maxA = topoChangeTime_[nSimp_[0]+nSimp_[1]+nSimp_[2]+nSimp_[3]-redundent_->getNumSet()-1]+1;
	if(Idx<=0)
		return maxA;

	switch(d)
	{
		case 0:
			if(Idx<=nSimp_[0])
			{
				if(!redundent_->test(Idx))
				{
					aSqr = (getCenter((Idx<<2)|d)-getVertex(Idx)).lengthsqr() - getW(Idx)*getW(Idx);
					return Sign(aSqr)*sqrt(fabs(aSqr));
				}
				else return maxA;
			}
			else
				return maxA;
		case 1:
			if(Idx<=nSimp_[1])
			{
				if(!hidden(Idx, d))
				//if(!hiddenBV_[d]->test(Idx))
					return rou_[nSimp_[3]+nSimp_[2]+Idx-1];
				else 
					return rankTable_[1][Idx];
			}
			else
				return maxA;
		case 2:
			if(Idx<=nSimp_[2])
			{
				if(!hidden(Idx,d))
				//if(!hiddenBV_[d]->test(Idx))
					return rou_[nSimp_[3]+Idx-1];
				else 
					return rankTable_[2][Idx];
			}
			else
				return maxA;
		case 3:
			if(Idx<=nSimp_[3])
			{
				rou_[0];
				return rou_[Idx-1];
			}
			else
				return maxA;
		default: return maxA;
	}
}
//----------------end of computeAlpha()------------------------------

Index CompDB::getIndex(OrTri ef,Dim d)
{
//	assert(dcomp_->getTrist()->ix(ef,d));
    return dcomp_->getTrist()->ix(ef,d);
};


void CompDB::computeEdgeCenter(VIndex ui,VIndex vi,double x[3])
{
    double v[4],u[4],lsqr,Usqr,Vsqr;
    rpArray_->getPoint(vi,v);
    rpArray_->getPoint(ui,u);
    lsqr = (v[0]-u[0])*(v[0]-u[0]) +
        (v[1]-u[1])*(v[1]-u[1]) +
        (v[2]-u[2])*(v[2]-u[2]);
    Usqr = Sign(u[3])*u[3]*u[3];
    Vsqr = Sign(v[3])*v[3]*v[3];
    if((lsqr+Vsqr-Usqr<0) // v hidden or 
        ||(lsqr>Vsqr+Usqr)) // u not hidden
        computeEdgeCenter_(ui,vi,x,lsqr,Usqr,Vsqr);
    else
        computeEdgeCenter_(vi,ui,x,lsqr,Vsqr,Usqr);
};

void CompDB::computeTetrahedronCenter(VIndex ai,VIndex bi,VIndex ci,VIndex di,double center[3])
{
    Vector3 a(getVertex(ai));
    Vector3 b(getVertex(bi));
    Vector3 c(getVertex(ci));
    Vector3 d(getVertex(di));
    
    EIndex abi = ih_->ix_v(ai,bi);
    EIndex aci = ih_->ix_v(ai,ci);
    EIndex adi = ih_->ix_v(ai,di);
    
    Vector3 ab = b-a;
    Vector3 ac = c-a;
    Vector3 ad = d-a;
    
    Matrix33 m(ab.x[0],ac.x[0],ad.x[0],
        ab.x[1],ac.x[1],ad.x[1],
        ab.x[2],ac.x[2],ad.x[2]);
    
    Vector3 p(dot_prod(Vector3(simpRec_[1][abi].center),ab),
		      dot_prod(Vector3(simpRec_[1][aci].center),ac),
              dot_prod(Vector3(simpRec_[1][adi].center),ad));
    
    Vector3 res = p*m.Inverse();
    
    center[0] = res.x[0];
    center[1] = res.x[1];
    center[2] = res.x[2];
}

void CompDB::computeFaceCenter(VIndex ai,VIndex bi,VIndex ci,double center[3])
{
    Vector3 a(getVertex(ai));
    Vector3 b(getVertex(bi));
    Vector3 c(getVertex(ci));
    EIndex abi = ih_->ix_v(ai,bi);
    EIndex bci = ih_->ix_v(bi,ci);
    
    Vector3 ab = b-a;
    Vector3 bc = c-b;
    Vector3 abcn = cross_prod(ab,bc);
    
    
    Matrix33 m(ab.x[0],bc.x[0],abcn.x[0],
        ab.x[1],bc.x[1],abcn.x[1],
        ab.x[2],bc.x[2],abcn.x[2]);
    
    Vector3 p(dot_prod(Vector3(simpRec_[1][abi].center),ab),
		      dot_prod(Vector3(simpRec_[1][bci].center),bc),
              dot_prod(abcn,b));
    
    Vector3 res = p*m.Inverse();
    
    center[0] = res.x[0];
    center[1] = res.x[1];
    center[2] = res.x[2];
};

void CompDB::computeEdgeCenter_(VIndex ui,VIndex vi,double x[3],double lsqr,double Usqr,double Vsqr)
{
    
    double u[4],v[4],t;
    rpArray_->getPoint(ui,u);
    rpArray_->getPoint(vi,v);
    
    t = (lsqr+Usqr-Vsqr)/(2*lsqr);
    for(int i=0;i<3;i++)
        x[i] = u[i] + (v[i]-u[i])*t;
};


CompDB::~CompDB()
{
    int i;
    for(i=0;i<4;i++)
    {
        delete XTop_[i];
        if(i<3)
            delete hiddenBV_[i];
		if(i)
			delete hideBV_[i];
        delete [] simpRec_[i];
		if(computeAlphaFilter_)
		{
			delete [] wallDist2_[i];
			delete [] wallDist_[i];
		}
    }
    
    if(topoChangeTime_)
        delete [] topoChangeTime_;

	if(rou_)
		delete [] rou_;
/*	if(masterIdx_)
		delete [] masterIdx_;
	if(master_)
		delete [] master_;
	if(tetraIdx_)
		delete [] tetraIdx_;
	if(tetraset_)
		delete [] tetraset_;
*/

	//if(dp_)
	//	delete [] dp_;
};

bool CompDB::inUpperMixedCell(double x[3],Index mc)
{
    Dim d = mc&3;
	assert((d==1)||(d==2));
    Index ix = mc>>2;
    Assert(d!=0);
    Assert(d!=3);
    Vector3 c = getCenter(mc);
    Vector3 upward;
/*
    Index ai,bi,ci;
    if(d==1)
    {
        ai = org(repTri(mc));
        bi = dest(repTri(mc));
        upward = getVertex(MIN(ai,bi))-getVertex(MAX(ai,bi));
    }

    if(d==2)
    {
        ai = org(repTri(mc));
        bi = dest(repTri(mc));
        ci = dest(repTri(mc).enext());
        upward = cross_prod(getVertex(ci)-getVertex(bi),getVertex(ai)-getVertex(bi));
    }


    return dot_prod(Vector3(x)-c,upward)>0;
*/
	return dot_prod(Vector3(x)-c,simpRec_[d][ix].n)>0;
}

bool CompDB::inMixedCell(double x[3],OrTri ef,Dim d,Index excludeMC)
{
    
    switch (d) {
    case 0:
        return inVertexMixedCell_(x,ef,excludeMC);
        break;
    case 1:
        return inEdgeMixedCell_(x,ef,excludeMC);
        break;
    case 2:
        return inFaceMixedCell_(x,ef,excludeMC);
        break;
    case 3:
        return inTetraMixedCell_(x,ef,excludeMC);
        break;
    default:
        Assert(0);
    }
    
    return false;
};

bool CompDB::inVertexMixedCell_(double p[3],OrTri ef,Index excludeMC)
{
    
    BitVector vbv;
    IterStack<OrTri> s;
    OrTri ef2,ef3;
    VIndex vi,v2i;
    EIndex ei;
    Vector3 center,vc,v,v2;
    Vector3 point(p);
    double x[4];
    vi = tr_->org(ef);
    rpArray_->getPoint(vi,x);
    v.x[0] = x[0];
    v.x[1] = x[1];
    v.x[2] = x[2];
    
    s.push(ef);
    while(!s.empty())
    {
        ef2 = s.pop();
        vbv.set(v2i=tr_->dest(ef2));
        ei = getIndex(ef2,1);
		if(((ei<<2)|1)!=excludeMC)
		{
			v2 = getVertex(v2i);
			center = simpRec_[1][ei].center;
			vc = v2 - v;
			assert(dot_prod(vc,planeNormal(vi<<2,(ei<<2)|1))>0);
			if(dot_prod(vc,(center+v)/2)<dot_prod(vc,point))
				return false;
		}
        ef3=ef2;
        do {
            if(!vbv.test(tr_->dest(tr_->enext(tr_->sym(ef3)))))
                s.push(tr_->enext(tr_->sym(ef3)));
            ef3 = tr_->fnext(ef3);
        } while (ef3!=ef2);
    }
    
    return true;
}

bool CompDB::inEdgeMixedCell_(double p[3],OrTri ef,Index excludeMC)
{
    Vector3 point(p),cv;
    EIndex ei = getIndex(ef,1);
    FIndex fi;
    Vector3 center(simpRec_[1][ei].center),cf;
    Vector3 v1(getVertex(tr_->org(ef)));
    Vector3 v2(getVertex(tr_->dest(ef)));
    Vector3 v3;
    OrTri ef2;
    
    
    cv = v1-v2;
	if((tr_->org(ef)<<2)!=excludeMC)
		if(dot_prod(cv,(center+v1)/2) < dot_prod(cv,point))
			return false;
    cv = v2-v1;
	if((tr_->dest(ef)<<2)!=excludeMC)
		if(dot_prod(cv,(center+v2)/2) < dot_prod(cv,point))
			return false;
    
    ef2 = ef;
    do {
        fi = getIndex(ef2,2);
		if(((fi<<2)|2)!=excludeMC)
		{
			cf = simpRec_[2][fi].center;
			v3 = getVertex(tr_->dest(tr_->enext(ef2)));
			cv = cross_prod(cross_prod(v2-v1,v3-v1),v2-v1);
			assert(dot_prod(cv,planeNormal((ei<<2)|1,(fi<<2)|2))>0);
			if(dot_prod(cv,(center+cf)/2)<dot_prod(cv,point))
				return false;
		}
        ef2 = tr_->fnext(ef2);
    } while (ef2!=ef);
    return true;
};

bool CompDB::inFaceMixedCell_(double p[3],OrTri ef,Index excludeMC)
{
    Vector3 point(p),ec,en,tc,tn;
    int i,j;
    Vector3 v[4];
    EIndex ei;
    VIndex vi[4];
    TIndex ti;
    Vector3 center = simpRec_[2][ef.index()].center;
    OrTri ef2;
    
    for(i=0,ef2=ef;i<3;i++,ef2=tr_->enext(ef2)) {
        ei = getIndex(ef2,1);
        ec = simpRec_[1][ei].center;

        if(((ei<<2)|1)!=excludeMC)
		{
			tr_->triangle(ef2,vi[0],vi[1],vi[2]);
			for(j=0;j<3;j++)
				v[j] = getVertex(vi[j]);
        
			en = cross_prod(v[1]-v[0],cross_prod(v[1]-v[0],v[2]-v[0]));
	//		en = ec-center;
			if(dot_prod(en,v[1]-v[2])<0)
				en = -en;
			assert(dot_prod(en,planeNormal((ef.index()<<2)|2,(ei<<2)|1))>0);
			if(dot_prod(en,(center+ec)/2)<dot_prod(en,point))
				return false;
		}
    }
    
    
    ef2 = ef;
    tn = cross_prod(v[1]-v[0],v[2]-v[0]);
    for(i=0;i<2;i++)
    {
        ti = getIndex(ef2,3);
		Vector3 pn;
        if(ti && (((ti<<2)|3)!=excludeMC))
        {
            tc = getCenter(ef2,3);
			assert(dot_prod(tn,pn = planeNormal((ef.index()<<2)|2,(ti<<2)|3))>0);
			if(dot_prod(tn,(center+tc)/2)<dot_prod(tn,point))
                return false;
        }
        ef2=tr_->sym(ef2);
        tn = -tn;
    }
    
    return true;
};

bool CompDB::inTetraMixedCell_(double p[3],OrTri ef,Index excludeMC)
{
    int i;
    OrTri f[4];
    FIndex fi[4];
    Vector3 fc,tn,point(p),v[3];
    f[0] = ef;
    f[1] = tr_->sym(tr_->fnext(ef));
    f[2] = tr_->sym(tr_->fnext(tr_->enext(ef)));
    f[3] = tr_->sym(tr_->fnext(tr_->enext2(ef)));
    Vector3 tc(simpRec_[3][getIndex(ef,3)].center);
    for(i=0;i<4;i++)
        fi[i] = f[i].index();
    
    for(i=0;i<4;i++)
    {
		if( ((fi[i]<<2)|2) != excludeMC)
		{
			fc = simpRec_[2][fi[i]].center;
			v[0] = getVertex(tr_->org(f[i]));
			v[1] = getVertex(tr_->dest(f[i]));
			v[2] = getVertex(tr_->dest(tr_->enext(f[i])));
        
			tn = -cross_prod(v[1]-v[0],v[2]-v[0]);
			assert(dot_prod(tn,planeNormal((getIndex(ef,3)<<2)|3,(fi[i]<<2)|2))>0);
			if(dot_prod(tn,(tc+fc)/2)<dot_prod(tn,point))
				return false;
		}
    }
    
    return true;
};

Index CompDB::locateMC(double x[3],Index initialMC)
{
    StackQueue<Index> s; // improvement: pqueue instead of queue
    BitVector onq;
    Index i;
    int d,j;
    Index idx,idx2;
    EIndex ei;
    TIndex ti;
    OrTri ef,ef1,ef2;
    IterStack<OrTri> es;
    BitVector ebv;
    
    
    s.push(initialMC);
    while(!s.empty())
    {
        i=s.get();
        idx = i>>2;
        d = i&3;
        ef = repTri(idx,d);
        if(inMixedCell(x,ef,d))
            return i;
        else
        {
            switch(d) {
            case 0:
                ebv.reset();
                es.push(ef);
                while(!es.empty())
                {
                    ef=es.pop();
                    if(!ebv.test(ei=getIndex(ef,1)))
                    {
                        ebv.set(ei);
                        j = (ei<<2)|1;
                        if(!onq.test(j))
                        {
                            s.push(j);
                            onq.set(j);
                        }
                        ef2=ef;
                        do {
                            ei=getIndex(ef1=enext(sym(ef2)),1);
                            if(!ebv.test(ei=getIndex(ef1,1)))
                                es.push(ef1);  
                            ef2=fnext(ef2);
                        } while(ef2.index()!=ef.index());
                    }
                }
                break;
            case 1:
                ef2=ef;
                do {
                    idx2 = (ef2.index()<<2)|2;
                    if(!onq.test(idx2)) // triangles
                    {
                        s.push(idx2);
                        onq.set(idx2);
                    }
                    ef2 = fnext(ef2);
                } while(ef2.index()!=ef.index());
                idx2 = org(ef)<<2;
                if(!onq.test(idx2)) // vertex
                {
                    s.push(idx2);
                    onq.set(idx2);
                }
                idx2 = dest(ef)<<2;
                if(!onq.test(idx2)) // vertex
                {
                    s.push(idx2);
                    onq.set(idx2);
                }
                break;
            case 2:
                // push the 3 edges on to the queue
                for(j=0;j<3;j++)
                {
                    ei = getIndex(ef,1);
                    idx2 = (ei<<2)|1;
                    if(!onq.test(idx2))
                    {
                        s.push(idx2);
                        onq.set(idx2);
                    }
                    ef=enext(ef);
                }
                // push the 2 (or 1) tetrahedron to the queue
                
                for(j=0;j<2;j++) {
                    if(ti=getIndex(ef,3))
                    {
                        idx2 = (ti<<2)|3;
                        if(!onq.test(idx2))
                        {
                            s.push(idx2);
                            onq.set(idx2);
                        }
                    }
                    ef=sym(ef);
                }
                
                break;
            case 3:
                // push the 4 triangles on to the queue
                idx2 = (getIndex(ef,2)<<2)|2;
                if(!onq.test(idx2))
                {
                    s.push(idx2);
                    onq.set(idx2);
                }
                for(j=0;j<3;j++)
                {
                    ef2 = fnext(ef);
                    idx2 = (ef2.index()<<2)|2;
                    if(!onq.test(idx2))
                    {
                        s.push(idx2);
                        onq.set(idx2);
                    }
                    ef=enext(ef);
                }
                break;
            default:
                Assert(0);
            }
        }
    }
    
    Assert(0);
    
    return 0;
}

void CompDB::computeSize_()
{
    
    int d,i;
    OrTri ef;
    VIndex vi;
    double size2;
    
    for(d=0;d<4;d++)
        for(i=1;i<=nSimp_[d];i++)
        {
            if(d==0&&redundent(i))
                continue;
            ef = repTri(i,d);
            vi = org(ef);
            size2 = (getVertex(vi)-getCenter(ef,d)).lengthsqr() 
                - Sign(getW(vi))*getW(vi)*getW(vi);
            simpRec_[d][i].size2 = size2;
        }
}

double CompDB::getSize2(Index i)
{
    return simpRec_[i&3][i>>2].size2;
}

void CompDB::computeMinWallDistSqr()
{
    double minD = MAXFLOAT;
    bool topoChange;
    int i,j,d;
 //   double * dist[4];
    double dst;
    Vector3 c;
    VIndex vi;
    EIndex ei;
    FIndex fi;
//    TIndex ti;
    OrTri ef[4];
    
    for(d=0;d<4;d++)
    {
        wallDist2_[d] = new double [nSimp_[d]+1];
        for(i=1;i<=nSimp_[d];i++)
            wallDist2_[d][i] = MAXFLOAT;
        wallDist_[d] = new double [nSimp_[d]+1];
    }
    
    for(i=1;i<=nSimp_[3];i++)
    {
        ef[0] = repTri(i,3);
        ef[1] = fnext(ef[0]);
        ef[2] = fnext(ef[0].enext());
        ef[3] = fnext(ef[0].enext2());
        c = getCenter(ef[0],3);
        for(j=0;j<4;j++)
        {
            dst = (c-getCenter(ef[j],2)).lengthsqr()/4; // squared
            wallDist2_[3][i] = min(wallDist2_[3][i],dst);
            fi = getIndex(ef[j],2);
            wallDist2_[2][fi] = min(wallDist2_[2][fi],dst);
        }
    }
    
    for(i=1;i<=nSimp_[2];i++)
    {
        ef[0] = repTri(i,2);
        ef[1] = ef[0].enext();
        ef[2] = ef[0].enext2();
        c = getCenter(ef[0],2);
        for(j=0;j<3;j++)
        {
            dst = (c-getCenter(ef[j],1)).lengthsqr()/4; // squared
            wallDist2_[2][i] = min(wallDist2_[2][i],dst);
            ei = getIndex(ef[j],1);
            wallDist2_[1][ei] = min(wallDist2_[1][ei],dst);
        }
    }
    
    for(i=1;i<=nSimp_[1];i++)
    {
        ef[0] = repTri(i,1);
        ef[1] = ef[0].sym();
        c = getCenter(ef[0],1);
        for(j=0;j<2;j++)
        {
            dst = (c-getCenter(ef[j],0)).lengthsqr()/4; // squared
            wallDist2_[1][i] = min(wallDist2_[1][i],dst);
            vi = getIndex(ef[j],0);
            wallDist2_[0][vi] = min(wallDist2_[0][vi],dst);
        }
    }
    
    for(d=0;d<4;d++)
        for(i=1;i<=nSimp_[d];i++)
        {
            switch(d)
            {
            case 0:
                topoChange = !(hiddenBV_[0]->test(i));
                break;
            case 1:
            case 2:
                topoChange = !(hiddenBV_[d]->test(i)||hideBV_[d]->test(i));
                break;
            case 3:
                topoChange = !(hideBV_[3]->test(i));
                break;
            }
            if(topoChange)
                minD = min(minD,wallDist2_[d][i]);
        }
/*
    for(d=0;d<4;d++)
        delete [] dist[d];
*/        
//    return minD;
    for(d=0;d<4;d++)
    {
        for(i=1;i<=nSimp_[d];i++)
            wallDist_[d][i] = sqrt(wallDist2_[d][i]);
    }
}

void CompDB::edgeStar(VIndex vi,IterStack<OrTri>* out)
{
    BitVector endpt;
    IterStack<OrTri> es;
    OrTri ef = repTri(vi,0);
    OrTri ef2,ef1;
    endpt.set(dest(ef));
    es.push(ef);
    out->push(ef);
    
    while(!es.empty())
    {
        ef = es.pop();
        ef1 = ef;
        do {
            ef2 = (ef1.sym()).enext();
            if(!endpt.test(dest(ef2)))
            {
                endpt.set(dest(ef2));
                es.push(ef2);
                out->push(ef2);
            }
            ef1 = fnext(ef1);
        } while(ef1.index()!=ef.index());
    }
}

Vector3 CompDB::planeNormal(Index mc1,Index mc2)
{
	Dim d1 = mc1&3,d2 = mc2&3;
	assert(abs(d1-d2)==1);

	switch(d1) {
	case 0:
		assert(d2==1);
		return planeNormal0to1_(mc1>>2,mc2>>2);
		break;
	case 1:
		assert((d2==0)||(d2==2));
		if(d2==0)
			return -planeNormal0to1_(mc2>>2,mc1>>2);
		else
			return planeNormal1to2_(mc1>>2,mc2>>2);
		break;
	case 2:
		assert((d2==1)||(d2==3));
		if(d2==1)
			return -planeNormal1to2_(mc2>>2,mc1>>2);
		else
			return planeNormal2to3_(mc1>>2,mc2>>2);
		break;
	case 3:
		assert(d2==2);
		return -planeNormal2to3_(mc2>>2,mc1>>2);
		break;
	default:
		assert(0);
		break;
	}
	assert(0);
	return Vector3(0,0,0);


}

Vector3 CompDB::planeNormal0to1_(VIndex vi,EIndex ei)
{
	OrTri ef = repTri(ei,1);
	int vi2 = (tr_->org(ef) == vi? tr_->dest(ef) : tr_->org(ef));
	Vector3 o(simpRec_[1][ei].center);
	Vector3 n;
	n = (o-getVertex(vi))/2;
	if(dot_prod(n,getVertex(vi2)-getVertex(vi))<0)
		n = -n;
	return n;
}

Vector3 CompDB::planeNormal1to2_(EIndex ei,FIndex fi)
{
	OrTri ef = repTri(fi,2);
	int eiv0 = tr_->org(repTri(ei,1));
	Vector3 o(simpRec_[2][fi].center);
	Vector3 n;
//	n = (o-getCenter((ei<<2)|1))/2;
	Vector3 v[3];
	while(tr_->org(ef)!=eiv0)
		ef = ef.enext();
	if(tr_->dest(ef)!=tr_->dest(repTri(ei,1)))
		ef = ef.sym().enext();
	v[0] = getVertex(tr_->org(ef));
	v[1] = getVertex(tr_->dest(ef));
	v[2] = getVertex(tr_->dest(ef.enext()));
	n = cross_prod(v[1]-v[0],cross_prod(v[1]-v[0],v[2]-v[0]));
	if(dot_prod(n,getVertex(tr_->dest(ef.enext()))-getVertex(eiv0)) < 0)
		n = -n;

	return n;
}

Vector3 CompDB::planeNormal2to3_(FIndex fi,TIndex ti)
{
	OrTri ef = repTri(ti,3);
	int fiv0 = tr_->org(repTri(fi,2));
	int vi2=0;
	Vector3 o(simpRec_[3][ti].center);
	Vector3 n;
//	n = (o-getCenter((fi<<2)|2))/2;
	Vector3 v[3];
	getVertex(fi,v[0],v[1],v[2]);
	n = cross_prod(v[1]-v[0],v[2]-v[0]);
	if(ef.index()==fi)
		vi2 = tr_->dest(tr_->fnext(ef).enext());
	else
	{
		while(vi2==0)
		{
			ef = ef.enext();
			if( tr_->fnext(ef).index()==fi)
			{
				vi2 = tr_->dest(ef.enext());
				ef = tr_->fnext(ef).sym();
				assert(vi2==tr_->dest(tr_->fnext(ef).enext()));
			}
		}
	}
	if(dot_prod(n,getVertex(vi2)-getVertex(fiv0))<0)
		n = -n;

	return n;
}

void CompDB::getMCHalfSpaces(Index mc, IterStack<Vector3>* ns,IterStack<double>* ds)
{
	Dim d = mc & 3;
	OrTri ef = repTri(mc);
	switch (d) {
	case 0:
		getMCHalfSpaces0_(ef,ns,ds);
		break;
	case 1:
		getMCHalfSpaces1_(ef,ns,ds);
		break;
	case 2:
		getMCHalfSpaces2_(ef,ns,ds);
		break;
	case 3:
		getMCHalfSpaces3_(ef,ns,ds);
		break;
	}
}

void CompDB::getMCHalfSpaces0_(OrTri ef,IterStack<Vector3>* ns,IterStack<double>* ds)
{
    BitVector vbv;
    IterStack<OrTri> s;
    OrTri ef2,ef3;
    VIndex vi,v2i;
    EIndex ei;
    Vector3 center,vc,v,v2;
    double x[4];
    vi = tr_->org(ef);
    
    s.push(ef);
    while(!s.empty())
    {
        ef2 = s.pop();
        vbv.set(v2i=tr_->dest(ef2));
        ei = getIndex(ef2,1);
        v2 = getVertex(v2i);
        center = simpRec_[1][ei].center;
        vc = v2 - v;
		assert(dot_prod(vc,planeNormal(vi<<2,(ei<<2)|1))>0);
		ns->push(vc);
		ds->push(dot_prod(vc,(center+v)/2));
        ef3=ef2;
        do {
            if(!vbv.test(tr_->dest(tr_->enext(tr_->sym(ef3)))))
                s.push(tr_->enext(tr_->sym(ef3)));
            ef3 = tr_->fnext(ef3);
        } while (ef3!=ef2);
    }

}
void CompDB::getMCHalfSpaces1_(OrTri ef,IterStack<Vector3>* ns,IterStack<double>* ds)
{

}
void CompDB::getMCHalfSpaces2_(OrTri ef,IterStack<Vector3>* ns,IterStack<double>* ds)
{

}
void CompDB::getMCHalfSpaces3_(OrTri ef,IterStack<Vector3>* ns,IterStack<double>* ds)
{

}

void CompDB::clipPolygon(Index mc, RArray<Vector3> *va,int& vn)
{



}

//find the neighbour of a tetrahedron
void CompDB::getNeighbour_(TIndex idx, Index rset[8])
{
	int i;
	//int cur = 0;
	Index abcd, abce;
	//initially, each element in rset is 0
	for(i=0; i<4; i++)
		rset[i] = 0;

	OrTri abc = repTri(idx, 3);
	rset[4]=getIndex(abc,2);
	abcd=getIndex(abc, 3);
	abce=getIndex(tr_->sym(abc), 3);
	if(abcd!=idx) 
		rset[0]=abcd;
	if(abce!=idx)
		rset[0]=abce;

	OrTri abd = tr_->fnext(abc);
	rset[5]=getIndex(abd,2);
	abcd=getIndex(abd, 3);
	abce=getIndex(tr_->sym(abd), 3);
	if(abcd!=idx) 
		rset[1]=abcd;
	if(abce!=idx)
		rset[1]=abce;

	OrTri bcd = tr_->fnext(tr_->enext(abc));
	rset[6]=getIndex(bcd,2);
	abcd=getIndex(bcd, 3);
	abce=getIndex(tr_->sym(bcd), 3);
	if(abcd!=idx) 
		rset[2]=abcd;
	if(abce!=idx)
		rset[2]=abce;

	OrTri acd = tr_->fnext(tr_->enext2(abc));
	rset[7]=getIndex(acd,2);
	abcd=getIndex(acd, 3);
	abce=getIndex(tr_->sym(acd), 3);
	if(abcd!=idx) 
		rset[3]=abcd;
	if(abce!=idx)
		rset[3]=abce;
	/*
	//get all face(sigma, 2)
	Simplex curTetra(idx,3);
	SimplexArray f;
	dcomp_->kFaces(curTetra, 2, &f);

	for(i=0; i<f.size(); i++)
	{
		Simplex curTris = f[i];
		OrTri curTri = repTri(curTris.getIndex());
		if(getIndex(curTri,3)!=idx){
			rset[cur]=getIndex(curTri,3);
			cur++;
		}
		if(getIndex(tr_->sym(curTri), 3)!=idx){
			rset[cur]=getIndex(tr_->sym(curTri), 3);
			cur++;
		}
	}
*/
}

void CompDB::getDp()
{
	int j, k, m;
	int noTetra=nSimp_[3]+1;
    Vector3 a1,a2,a3,a4,b1,b2,b3,c1;
	//double curAlpha;
	double oldAlpha = topoChangeTime_[0]-1;
	//int curMasterIdx = totalNSimp_*3-1;
	//double curAlpha=topoChangeTime_[totalNSimp_-1];
	dp_ = new int[nSimp_[3]+1];
	dp_[0] = nSimp_[3]+1;

	for(j=1;j<nSimp_[3]+1;j++)
		dp_[j]=0;
	//dp_[0]=0;

	for(j=nSimp_[3]-1; j>=0; j--)
	{
		TIndex curTetra = tetraIdx_[j];
		dp_[curTetra] = j;

		getVertex(curTetra,a1,a2,a3,a4);
		c1 = getCenter((curTetra<<2)|3);
			
		Index ck[8];
		getNeighbour_(curTetra, ck);

		for(k=0; k<4; k++)
		{
			double *matrix44 = new double[16];

			getVertex(ck[k+4],b1,b2,b3);
			for(m=0;m<16;m++){
				if(m>=4 && m<7)
					matrix44[m]=b1.x[m-4];
				else if(m>=8 && m<11)
					matrix44[m]=b2.x[m-8];
				else if(m>=12 && m<15)
					matrix44[m]=b3.x[m-12];
				else
					matrix44[m]=1;
			}
			switch(k) {
			case 0:
				for(m=0;m<3;m++)
					matrix44[m]=a4.x[m];
				break;
			case 1:
				for(m=0;m<3;m++)
					matrix44[m]=a3.x[m];
				break;

			case 2:
				for(m=0;m<3;m++)
					matrix44[m]=a1.x[m];
				break;
			case 3:
				for(m=0;m<3;m++)
					matrix44[m]=a2.x[m];
				break;
			}
			double sign1 = det(matrix44,4);

			for(m=0;m<3;m++)
				matrix44[m]=c1.x[m];
			double sign2 = det(matrix44,4);

			if(sign1*sign2 <= 0) 
				if(dp_[ck[k]]>dp_[curTetra]) 
					dp_[curTetra]=dp_[ck[k]];
		}
	}
}

TIndex* CompDB::computePocket(double alpha, double belta)
{
	int i, j, k, x, str, end, yxIdx,o;
	Index p, t;
	//double curAlpha;
	double oldAlpha = topoChangeTime_[0]-1;

	int **yx;
	yx = new int* [2];

	tetraset_ = new int[nSimp_[3]+1];
	for(i=1; i<nSimp_[3]+1; i++)
		tetraset_[i] = nSimp_[3]+1;
	tetraset_[0]=-1;

	for(i=0; i<2; i++) {
		yx[i] = new int[nSimp_[3]+2];
		for(j=0;j<nSimp_[3]+2; j++)
			yx[i][j] = -1;
	}

	str=findI(alpha);
	end=findI(belta);

	for(j=str; j<end; j++) {
		k=dp_[tetraIdx_[j]];
		if(k>nSimp_[3])
			k=0;

		if(yx[0][k] == -1)
			yx[0][k]=tetraIdx_[j];
		else {
			yxIdx=nSimp_[3]+1;
			for(o=yx[0][k]; yx[1][o]!=-1; o=yx[1][o])
				yxIdx=yx[1][o];
			yx[1][o]=tetraIdx_[j];
		}

		for(o=yx[0][j]; o!=-1; o=yx[1][o]) {
			//if(yx[j][o] != -1){
			TIndex c = o;

			//*ADD(o);
			if(tetraset_[c] > nSimp_[3])
				tetraset_[c]=-1;

			Index ck[8];
			getNeighbour_(c,ck);
			//*for all t which is getNeighbour(o) with t belong to Union of tetraset_
			for(x=0;x<4;x++) {
				t=ck[x];

				if(tetraset_[t] <= nSimp_[3]) {
					//*let p be the triangle shared by o and t
					p = ck[x+4];
					//*if p is not in CpxB(alpha) then UNION(SET(o), SET(t)) end if
					if(getAlpha(p,2)>alpha)
						unionset(findset(c),findset(t));
				}
			}//for all tetra neighbor to o
			//}//if
		}//for all tetra o belong to Yj
	}
/*
	//*for j:=i(alpha)+1 to i(belta) do
	for(j=findJ(alpha)+1; j<findJ(belta)+1; j++){
	//*	k:=dp[tetrahedron(j)];
		if(j != findJ(alpha)+1)
			oldAlpha=curAlpha;
		curAlpha = master_[masterIdx_[j]].rankValue;
		if(oldAlpha == curAlpha)
			continue;
		for(i=masterIdx_[j]; i<totalNSimp_*3; i++)
		{
			if(master_[i].faceType != 3)
				break;
			if(master_[i].rankValue != curAlpha)
				break;
			k=dp_[master_[i].faceIdx];
	//*		Yk:=Yk union {tetrahedron(j)};
			yx[k][master_[i].faceIdx]=1;
		}
	//*	for all tetrahedron o belong to Yj do
		for(o=1; o<=nSimp_[3]; o++){
			if(yx[j][o] == 1){
	//			*ADD(o);
				tetraset_[o]=-1;
				Index ck[8];
				getNeighbour_(o,ck);
	//			for all t which is getNeighbour(o) with t belong to Union of tetraset_
				for(x=0;x<4;x++) {
					t=ck[x];
					if(tetraset_[t] <= nSimp_[3]) {
	//					let p be the triangle shared by o and t
						p = ck[x+4];
	//					*if p is not in CpxB(alpha) then UNION(SET(o), SET(t)) end if
						if(rou_[nSimp_[3]+p-1]>alpha)
							unionset(findset(o),findset(t));
					}
				}//for all tetra neighbor to o
			}//if
		}//for all tetra o belong to Yj
	}//for all tetra within the range
*/
	//for(i=0;i<nSimp_[3]+1;i++)
	for(i=0; i<2; i++)
		delete [] yx[i];
		
	//tetraset = new int[nSimp_[3]+1];
	//for(i=0; i<nSimp_[3]+1; i++)
	//	dp_[i];
	//	tetraset[i] = tetraset_[i];

	return tetraset_;
}

int CompDB::findI(double avalue)
{
	int i;
	for(i=0; i<nSimp_[3]; i++) {
		if(getAlpha(tetraIdx_[i],3) > avalue)
			return i;
	}
	return nSimp_[3];
}

double CompDB::det(double *M, int N)
{
	if (N==2)  {
	    return (M[0]*M[3] - M[1]*M[2]);
    }
    else  {
        int new_dimension = N-1;
        double tdet=0;
        double *temp = new double[(N-1)*(N-1)];

        for(int ref_ele=0; ref_ele<N;ref_ele++) {
			int counter=0;

            for (int i=0;i<N;i++) {
				for (int j=0;j<N;j++) {
                    if ((j!= ref_ele) && (i!=0)) {
						temp[counter] = M[(N*i + j)];
                        counter++;
                    } 
				} 
			}

            double t = det(temp, new_dimension);
           
			if ((ref_ele % 2) != 0)
				t = (-1)*t;

            tdet += M[ref_ele]*t;
        }

        return tdet;
	}
}

void CompDB::sortTetra()
{
	tetraIdx_ = new int[nSimp_[3]];
	int curIdx = nSimp_[3];
	int i,j;
	double curAlpha;
	double oldAlpha = topoChangeTime_[0]-1;
	for(j=alphaN; j>0; j--)
	{
		if(j != alphaN)
			oldAlpha=curAlpha;
		curAlpha = master_[masterIdx_[j]].rankValue;
		if(oldAlpha == curAlpha)
			continue;
		for(i=masterIdx_[j]; i<totalNSimp_*3; i++)
		{
			if(master_[i].faceType != 3)
				break;
			if(master_[i].rankValue != curAlpha)
				break;
			TIndex curTetra = master_[i].faceIdx;
			curIdx--;
			tetraIdx_[curIdx]=curTetra;
			//if(curTetra >= 1933)
			//	tetraIdx_[curIdx]=curTetra;
		}
	}

	//for(i=0; i<nSimp_[3];i++)
	//	tetraIdx_[i];
}
   
