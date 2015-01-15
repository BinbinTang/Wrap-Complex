// SkinMesh.cpp: implementation of the SkinMesh class.
//
//////////////////////////////////////////////////////////////////////


#include "SkinMesh.h"
#include <stdlib.h>
#include <gl/gl.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.141592654
#endif

#define MINEDGE(ef) ((ef).index() > mesh_->fnext(ef).index() ?  mesh_->fnext((ef).sym()) : ef )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SkinMesh::SkinMesh(CompDB* cdb)
:cdb_(cdb)
{
	int i;
	alpha_ = 0;
	srand( (unsigned)time( NULL ) );
	for(i=0;i<4;i++)
		visible_[i] = TRUE;
	c = 10;
	mesh_=NULL;
	C_ = 1.0/14;
	Q_ = 2;
}

SkinMesh::~SkinMesh()
{
	if(mesh_)
		delete mesh_;
}

void SkinMesh::genMesh(double a)
{
	alpha_ = a;
	
	genVertexPts_();
	genEdgePts_();
	genFacePts_();
	genTetraPts_();
	
	sculpt_();
	orientMesh_();
	refineMesh();
	computeNormals_();
}
 
void SkinMesh::orientMesh_()
{
	// making all the verion 0 of the OrTri's poiting outwards of the skin
	BitVector needFlip;
	BitVector untouched;
	BitVectorIter bvi(untouched);
	BitVectorIter nfbvi(needFlip);

	IterStack<OrTri> ts;
	VIndex ai,bi,ci;
	Vector3 a,b,c;
	Vector3 ac,abcn;
	TristIter ti(*mesh_);
	OrTri ef,abc;
	Bool found;
	int i;

	for(ti.first();!ti.isDone();ti.next())
		untouched.set(ti.curItem());

	while(untouched.getNumSet())
	{
		found = FALSE;
		for(bvi.first();(!found)&&!bvi.isDone();bvi.next())
		{
			abc = OrTri(bvi.curItem(),0);
			for(i=0;i<3;i++)
			{
				abc = abc.enext();
				if((vmc_[mesh_->org(abc)]&3)==0)
					found=TRUE;
			}
		}

		untouched.clear(abc.index());
		mesh_->triangle(abc,ai,bi,ci);
		a = parray_[ai];
		b = parray_[bi];
		c = parray_[ci];
		ac = cdb_->getCenter(vmc_[ai]);
		abcn = cross_prod(b-a,c-a);
		if(dot_prod(abcn,a-ac)<0)
		{
			needFlip.set(abc.index());
			abc=abc.sym();
		}

		ts.push(abc);

		while(!ts.empty())
		{
			abc = ts.pop();
			for(i=0;i<3;i++)
			{
				ef = mesh_->fnext(abc.sym());
				if(untouched.test(ef.index()))
				{
					untouched.clear(ef.index());
					ts.push(ef);
					if(ef.version()&1)
						needFlip.set(ef.index());
				}
				abc = abc.enext();
			}
		}
	}

	for(nfbvi.first();!nfbvi.isDone();nfbvi.next())
		mesh_->flip(nfbvi.curItem());
}

void SkinMesh::computeNormals_()
{
	// compute the normal of vertices for graphics purpous
	// assumed after the mesh is oriented

	FArrayIter<Vector3> pi(parray_);

	for(pi.first();!pi.isDone();pi.next())
	  narray_[pi.curIndex()] = pointNormal_(pi.curIndex());


	IterStack<OrTri> ts;
	BitVector vnc; // vertex normal computed
	VIndex ai,bi,ci;
	Vector3 a,b,c;
	TristIter ti(*mesh_);
	OrTri ef,abc;
	Vector3 n;

	for(ti.first();!ti.isDone();ti.next())
	{
		abc = OrTri(ti.curItem(),0);
		mesh_->triangle(abc,ai,bi,ci);
		a = parray_[ai];
		b = parray_[bi];
		c = parray_[ci];
		fnarray_[ti.curItem()] = cross_prod(b-a,c-a);
		fnarray_[ti.curItem()].normalize();
	}

}	

void SkinMesh::genVertexPts_()
{
	int i;
	Vector3 v;
	double rand1,rand2,rsqr;
	int numPtAdded;
	int numPt = c * 4 * 3.141592654;
	int ix;
	
	for(i=1;i<=cdb_->numSimplex(0);i++)
	{
		if(!cdb_->redundent(i))
		{
			rsqr = squaredSize(0,i);
			if(rsqr<=0)
				continue;
			v = cdb_->getVertex(i);
			numPtAdded=0;
			while(numPtAdded<numPt) {
				rand1 = rand()*2*M_PI/RAND_MAX;
				rand2 = (2.0*rand())/RAND_MAX-1;
				Vector3 dis(cos(rand1)*sqrt(1-rand2*rand2),sin(rand1)*sqrt(1-rand2*rand2),rand2),p;
				p = v + dis*sqrt(0.5)*sqrt(rsqr);
				if(cdb_->inMixedCell(p.x,cdb_->repTri(i,0),0))
				{

					ix = parray_.newLoc ();
					parray_[ix]=p;
					vmc_[ix]=i<<2;
				}
				numPtAdded++;
			}
		}
	}
}

void SkinMesh::genEdgePts_()
{
	int i,j,ptAdded,numPt;
	VIndex vi[2];
	Vector3 ec,v[2],vn,pt;
	OrTri ef;
	int ix;
	double rsqr,lsqr,r,height[2];
	//	double vw[2];
	
	double pmax,hmax,wpmax,h,p,q,phi,up,vp,p2,p4;
	
	for(i=1;i<=cdb_->numSimplex(1);i++)
	{
		ef = cdb_->repTri(i,1);
		ec = cdb_->getCenter(ef,1);
		vi[0] = cdb_->getIndex(ef,0);
		vi[1] = cdb_->getIndex(cdb_->enext(ef),0);
		v[0] = cdb_->getVertex(vi[0]);
		v[1] = cdb_->getVertex(vi[1]);
		vn = v[0]-ec;
		lsqr = vn.lengthsqr();
		if(lsqr>0)
		{
			vn = vn / sqrt(lsqr);
			height[0] = (v[0]-ec).length();
			height[1] = (v[1]-ec).length();
		} else
		{
			vn = v[1]-ec;
			lsqr = vn.lengthsqr();
			vn = vn / sqrt(lsqr);
			height[1] = ((v[0]-ec)/2).length();
			height[0] = ((v[1]-ec)/2).length();
		} 
		
		rsqr = squaredSize(1,i);
		
		r = sqrt(0.5)*sqrt(fabs(rsqr));
		
		hmax = max(height[0],height[1]);
		
		Matrix33 m(AlignWithYAxis(vn).Inverse());
		if(rsqr==0)
		{
			// didn't do it yet , double cone case
		} else
			if(rsqr>0) // two sheets
			{
				// upper part
				
				for(j=0;j<2;j++)
					if(height[j]/r>1)
					{
						h = height[j]/r;
						pmax = sqrt(h*h-1) /h;
						wpmax = pmax/((1-pmax*pmax)*sqrt(1+pmax*pmax));
						ptAdded=0;
						numPt = c*3.141592654*(sqrt(2.0)/2)* (
							atanh((1+pmax)/sqrt(2*(1+pmax*pmax))) -
							atanh((-1+pmax)/sqrt(2*(1+pmax*pmax))));
						
						while(ptAdded<numPt)
						{
							p = pmax*rand()/RAND_MAX;
							q = wpmax*rand()/RAND_MAX;
							p2 = p*p;
							p4 = p2*p2;
							if(q<p/((1-p2)*sqrt(1+p2)))
							{
								ptAdded++;
								phi = 2*M_PI*rand()/RAND_MAX;
								up = p/sqrt(1-p2);
								pt = ec + (Vector3(up*sin(phi),(j?1:-1)/sqrt(1-p2),up*cos(phi))*r) * m;
								if(cdb_->inMixedCell(pt.x,cdb_->repTri(i,1),1))
								{
									ix = parray_.newLoc ();
									parray_[ix]=pt;
									vmc_[ix]=(i<<2)|1;
								}
							}
						}
					}
			} else {   // one sheet
				for(j=0;j<2;j++)
					if(height[j]>0)
					{
						h = height[j]/r;
						pmax = h/sqrt(h*h+1);
						wpmax = 1.0/((1-pmax*pmax)*sqrt(1+pmax*pmax));
						ptAdded=0;
						numPt = c*3.141592654*(sqrt(2.0)/2)* (
							atanh((1+pmax)/sqrt(2*(1+pmax*pmax))) +
							atanh((-1+pmax)/sqrt(2*(1+pmax*pmax))));
						while(ptAdded<numPt)
						{
							p = pmax*rand()/RAND_MAX;
							p2 = p*p;
							p4 = p2*p2;
							q = wpmax*rand()/RAND_MAX;
							if(q<1.0/((1-p2)*sqrt(1+p2)))
							{
								ptAdded++;
								phi = 2*M_PI*rand()/RAND_MAX;
								up = p/sqrt(1-p2);
								vp = 1/sqrt(1-p2);
								pt = ec + (Vector3(vp*sin(phi),(j?1:-1)*up,vp*cos(phi))*r) * m;
								if(cdb_->inMixedCell(pt.x,cdb_->repTri(i,1),1))
								{

									ix = parray_.newLoc ();
									parray_[ix]=pt;
									vmc_[ix]=(i<<2)|1;

								}

							}
						}
					}
			}
	}
}

void SkinMesh::genFacePts_()
{
	
	int i,j,ptAdded,numPt;
	Vector3 vn,pt;
	Vector3 v[3],tc[2],center;
	OrTri ef;
	TIndex ti[2];
	double height[2],rsqr,pmax,wpmax,up,vp,p,q,phi,r,h,p2,p4;
	int ix;

	for(i=1;i<=cdb_->numSimplex(2);i++)
	{
		ef = cdb_->repTri(i,2);
		v[0] = cdb_->getCenter(ef,0);
		v[1] = cdb_->getCenter(cdb_->enext(ef),0);
		v[2] = cdb_->getCenter(cdb_->enext(cdb_->enext(ef)),0);
		vn = cross_prod(v[1]-v[0],v[2]-v[0]);
		if(ti[0] = cdb_->getIndex(ef,3))
			tc[0] = cdb_->getCenter(ef,3);
		if(ti[1] = cdb_->getIndex(cdb_->sym(ef),3))
			tc[1] = cdb_->getCenter(cdb_->sym(ef),3);
		
		Matrix33 m(AlignWithYAxis(vn).Inverse());
		
		center = cdb_->getCenter(ef,2);
		rsqr = squaredSize(2,i);
		r    = sqrt(0.5)*sqrt(fabs(rsqr));
		// rsqr > 0 : 1 sheet
		// rsqr < 0 : 2 sheet
		
		for(j=0;j<2;j++)
		{
			if(ti[j])
				height[j] = (tc[j]-center).length(); // could improve
			else {
				
				double maxr;
				maxr = (center-v[0]).length();
				maxr = max((center-v[1]).length(),maxr);
				maxr = max((center-v[2]).length(),maxr);				
				maxr /= 2;
				
				if(rsqr>0)
				{
					if(maxr<r)
						height[j] = 0;
					else
						height[j] = sqrt(maxr*maxr+r*r);
				} else
					height[j] = sqrt(maxr*maxr+r*r);
				
			}
		}
		
		for(j=0;j<2;j++)
		{
			if(rsqr==0)
			{
				// double cone;
			} else 
				if (rsqr > 0) // 1 sheet
				{
					h = height[j]/r;
					pmax = h/sqrt(h*h+1);
					wpmax = 1.0/((1-pmax*pmax)*sqrt(1+pmax*pmax));
					ptAdded=0;
					numPt = c*3.141592654*(sqrt(2.0)/2)* (
						atanh((1+pmax)/sqrt(2*(1+pmax*pmax))) +
						atanh((-1+pmax)/sqrt(2*(1+pmax*pmax))));
					while(ptAdded<numPt)
					{
						p = pmax*rand()/RAND_MAX;
						q = wpmax*rand()/RAND_MAX;
						p2 = p*p;
						p4 = p2*p2;
						if(q<1.0/((1-p2)*sqrt(1+p2)))
						{
							ptAdded++;
							phi = 2*M_PI*rand()/RAND_MAX;
							up = p/sqrt(1-p2);
							vp = 1.0/sqrt(1-p2);
							pt = center + (Vector3(vp*sin(phi),(j?-1:1)*up,vp*cos(phi))*r) * m;
							if(cdb_->inMixedCell(pt.x,ef,2))
								{
									ix = parray_.newLoc ();
									parray_[ix]=pt;
									vmc_[ix]=(i<<2)|2;

								}
						}
					}
				} else { // 2 sheet
					h = height[j]/r;
					pmax = sqrt(h*h-1) /h;
					wpmax = pmax/((1-pmax*pmax)*sqrt(1+pmax*pmax));
					ptAdded=0;
					numPt = c*3.141592654*(sqrt(2.0)/2)* (
						atanh((1+pmax)/sqrt(2*(1+pmax*pmax))) -
						atanh((-1+pmax)/sqrt(2*(1+pmax*pmax))));
					while(ptAdded<numPt)
					{
						p = pmax*rand()/RAND_MAX;
						q = wpmax*rand()/RAND_MAX;
						p2 = p*p;
						p4 = p2*p2;
						if(q<p/((1-p2)*sqrt(1+p2)))
						{
							ptAdded++;
							phi = 2*M_PI*rand()/RAND_MAX;
							up = p/sqrt(1-p2);
							pt = center + (Vector3(up*sin(phi),(j?-1:1)/sqrt(1-p2),up*cos(phi))*r) * m;
							if(cdb_->inMixedCell(pt.x,ef,2))
								{

									ix = parray_.newLoc ();
									parray_[ix]=pt;
									vmc_[ix]=(i<<2)|2;
								}

						}
					}
				}
				
		}
}


}

void SkinMesh::genTetraPts_()
{
	Vector3 center;
	OrTri ef;
	double rsqr,rand1,rand2;
	int i,numPtAdded,numPt;
	numPt = c * 4 * 3.141592654;
	int ix;

	for(i=1;i<=cdb_->numSimplex(3);i++)
	{
		ef = cdb_->repTri(i,3);
		
		center = cdb_->getCenter(ef,3);
		rsqr = squaredSize(3,i);
		if(rsqr>0)
		{
			numPtAdded=0;
			while(numPtAdded<numPt) {
				numPtAdded++;
				rand1 = rand()*2*M_PI/RAND_MAX;
				rand2 = (2.0*rand())/RAND_MAX-1;
				Vector3 dis(cos(rand1)*sqrt(1-rand2*rand2),sin(rand1)*sqrt(1-rand2*rand2),rand2),p;
				p = center + dis*sqrt(0.5)*sqrt(rsqr);
				if(cdb_->inMixedCell(p.x,ef,3))
				{
					ix = parray_.newLoc ();
					parray_[ix]=p;
					vmc_[ix]=(i<<2)|3;

				}
			}
		}
		
		
	}
}

double SkinMesh::rhoSqr(int vi)
{
	return (cdb_->getCenter(vmc_[vi])-parray_[vi]).lengthsqr();
}

void SkinMesh::collectStatistics()
{
	BitVector vb;
	Vector3 cc,a,b,c;
	Vector3 ab,bc,ca;
	double anglea,angleb,anglec;
	double rho_a,rho_b,rho_c;
	double rho_ab,rho_bc,rho_ca;
	double rho_abc;
	int ai,bi,ci;
	TristIter ti(*mesh_);
	int en,cn,vn,an,dn,adn;
	double Rab,Rbc,Rca;
	double Rabc;
	double angleD;
	int i,deg;
	OrTri ef;
	char str[512];

	en = cn = vn = an = dn = adn = 0;
	// reset constants
	edgel_[MIN] = MAXFLOAT;
	edgel_[AVG] = edgel_[MAX] = 0;
	cirR_[MIN]  = MAXFLOAT;
	cirR_[AVG]  = cirR_[MAX] = 0;
	vDeg_[MIN]  = MAXINT;
	vDeg_[AVG]  = vDeg_[MAX] = 0;
	angle_[MIN] = MAXFLOAT;
	angle_[AVG] = angle_[MAX] = 0;
	dangle_[MIN] = MAXFLOAT;
	dangle_[AVG] = dangle_[MAX] = 0;
	angleDefect_[MIN] = MAXFLOAT;
	angleDefect_[MAX] = MINFLOAT;
	angleDefect_[AVG] = 0;

	for(ti.first();!ti.isDone();ti.next())
	{
		OrTri tri(ti.curItem(),0);
		ai = mesh_->org(tri);
		bi = mesh_->dest(tri);
		ci = mesh_->dest(tri.enext());
		a = parray_[ai];
		b = parray_[bi];
		c = parray_[ci];
		ab = b-a;
		bc = c-b;
		ca = a-c;
		ab.normalize();
		bc.normalize();
		ca.normalize();
		anglea = acos(dot_prod(ab,-ca));
		angleb = acos(dot_prod(bc,-ab));
		anglec = acos(dot_prod(ca,-bc));
		rho_a = rho(ai);
		rho_b = rho(bi);
		rho_c = rho(ci);
		rho_ab = max(rho_a,rho_b);
		rho_bc = max(rho_b,rho_c);
		rho_ca = max(rho_c,rho_a);
		rho_abc = min(rho_a,min(rho_b,rho_c));
		cc = circumCenter(a,b,c);
		Rab = (a-b).length()/2;
		Rbc = (b-c).length()/2;
		Rca = (c-a).length()/2;
		Rabc = (cc-a).length();

		edgel_[MIN] = min(edgel_[MIN],min(Rab/rho_ab,min(Rbc/rho_bc,Rca/rho_ca)));
		edgel_[MAX] = max(edgel_[MAX],max(Rab/rho_ab,max(Rbc/rho_bc,Rca/rho_ca)));
		edgel_[AVG] += Rab/rho_ab + Rbc/rho_bc + Rca/rho_ca;
		en += 3;
		cirR_[MIN]  = min(cirR_[MIN],Rabc/rho_abc);
		cirR_[MAX]  = max(cirR_[MAX],Rabc/rho_abc);
		cirR_[AVG]  += Rabc/rho_abc;
		cn ++;
		angle_[MIN] = min(angle_[MIN],min(anglea,min(angleb,anglec)));
		angle_[MAX] = max(angle_[MAX],max(anglea,max(angleb,anglec)));
		angle_[AVG] += anglea+angleb+anglec;
		an +=3;
		for(i=0;i<3;i++)
		{
			if(!vb.test(mesh_->org(tri)))
			{
				vb.set(mesh_->org(tri));
				deg = 0;
				angleD = 0;
				ef = tri;
				do {
					ef = mesh_->fan(ef);
					angleD += angle(parray_[mesh_->dest(ef)],
						            parray_[mesh_->org(ef)],
									parray_[mesh_->dest(ef.enext())]);
					deg++;
				} while (ef.index()!=tri.index());
				vDeg_[MIN] = min(vDeg_[MIN],deg);
				vDeg_[MAX] = max(vDeg_[MAX],deg);
				vDeg_[AVG] += deg;
				vn ++;
				angleD = 2*M_PI-angleD;
				angleDefect_[MIN] = min(angleDefect_[MIN],angleD);
				angleDefect_[MAX] = max(angleDefect_[MAX],angleD);
				angleDefect_[AVG] += angleD;
				adn++;
			}
			tri = tri.enext();
		}
	}

	edgel_[AVG] /= en; 
/*
	edgel_[AVG] *=2; // since it is Rab instead of l_ab
	edgel_[MIN] *=2;
	edgel_[MAX] *=2;
*/
	cirR_[AVG]/=cn;
	vDeg_[AVG]/=vn;
	angle_[AVG]/=an;
	angleDefect_[AVG]/=adn;

	for(i=0;i<3;i++)
	{
		angle_[i] *= 180/M_PI;
		angleDefect_[i] *= 180/M_PI;
	}

	sprintf(str,"C: %6.4f   Q: %6.4f\n\n",C_,Q_);
	sprintf(&(str[strlen(str)]),
		        "#vertices: %d  #edges: %d   #triangles: %d  genus: %d\n\n",
				parray_.numUsed(),
				3*mesh_->numTri()/2,
				mesh_->numTri(),
				(2-(parray_.numUsed()-3*mesh_->numTri()/2+mesh_->numTri()))/2);
	sprintf(&(str[strlen(str)]),
		"Edge length/rho:   MIN:%6.2f  AVG:%6.2f  MAX:%6.2f\n\n",edgel_[MIN],edgel_[AVG],edgel_[MAX]);
	sprintf(&(str[strlen(str)]),
		"Circumradius/rho:  MIN:%6.2f  AVG:%6.2f  MAX:%6.2f\n\n",cirR_[MIN],cirR_[AVG],cirR_[MAX]);
	sprintf(&(str[strlen(str)]),
		"Vertex Degree:     MIN:%2.0f         AVG:%6.2f    MAX:%2.0f\n\n",vDeg_[MIN],vDeg_[AVG],vDeg_[MAX]);
	sprintf(&(str[strlen(str)]),
		"Triangle Angle:    MIN:%6.2f      AVG:%6.2f  MAX:%6.2f\n\n",angle_[MIN],angle_[AVG],angle_[MAX]);
	sprintf(&(str[strlen(str)]),
		"Angle Defect:   MIN:%6.2f      AVG:%6.2f  MAX:%6.2f\n\n",angleDefect_[MIN],angleDefect_[AVG],angleDefect_[MAX]);

	MessageBox(NULL,str,"Statistics",MB_OK);

}

void SkinMesh::dumpPoints(const char* filename)
{
	FILE *fp = fopen(filename,"w+t");

	fprintf(fp,"solid SKIN\n");
	Vector3 x1,x2,x3;
	VIndex vi1,vi2,vi3;
	TristIter ti(*mesh_);
	for(ti.first();!ti.isDone();ti.next())
	{
		OrTri tri(ti.curItem(),0);
		fprintf(fp,"facet normal 0 0 0\n");
		fprintf(fp,"outer loop\n");
		x1 = parray_[vi1=mesh_->org(tri)];
		x2 = parray_[vi2=mesh_->dest(mesh_->enext(tri))];
		x3 = parray_[vi3=mesh_->dest(tri)];
		fprintf(fp,"vertex %6.4f %6.4f %6.4f %6.4f\n",x1.x[0],x1.x[1],x1.x[2]);
		fprintf(fp,"vertex %6.4f %6.4f %6.4f %6.4f\n",x2.x[0],x2.x[1],x2.x[2]);
		fprintf(fp,"vertex %6.4f %6.4f %6.4f %6.4f\n",x3.x[0],x3.x[1],x3.x[2]);
		fprintf(fp,"endloop\n");
		fprintf(fp,"endfacet\n");
		  
	}
	fprintf(fp,"endsolid");
	fclose(fp);
}


double SkinMesh::squaredSize(Dim d,Index idx)
{
/*
	double w,lsqr;
	VIndex vi;
	Vector3 center,v;
	OrTri ef;
	switch(d) {
	case 0:
		w = cdb_->getW(idx);
		return Sign(w)*w*w+Sign(alpha_)*alpha_*alpha_;
	case 1:
		ef = cdb_->repTri(idx,1);
		center = cdb_->getCenter(ef,1);
		vi = cdb_->getIndex(ef,0);
		v = cdb_->getVertex(vi);
		w = cdb_->getW(vi);
		lsqr = (v-center).lengthsqr();
		return lsqr-(Sign(w)*w*w+Sign(alpha_)*alpha_*alpha_);
	case 2:
		ef = cdb_->repTri(idx,2);
		v = cdb_->getCenter(ef,0);
		w   = cdb_->getW(cdb_->getIndex(ef,0));
		center = cdb_->getCenter(ef,2);
		return (v-center).lengthsqr()-(Sign(w)*w*w+Sign(alpha_)*alpha_*alpha_);
	case 3:
		ef = cdb_->repTri(idx,3);
		vi = cdb_->getIndex(ef,0);
		center = cdb_->getCenter(ef,3);
		v = cdb_->getVertex(vi);
		w  = cdb_->getW(vi);
		return (v-center).lengthsqr()-(Sign(w)*w*w+Sign(alpha_)*alpha_*alpha_);
	default:
		Assert(0);
	}
	
	return 0;
*/
	return squaredSize(d,idx,alpha_);
};

double SkinMesh::squaredSize(Dim d,Index idx,double alpha)
{
	double w,lsqr;
	VIndex vi;
	Vector3 center,v;
	OrTri ef;
	switch(d) {
	case 0:
		w = cdb_->getW(idx);
		return Sign(w)*w*w+Sign(alpha)*alpha*alpha;
	case 1:
		ef = cdb_->repTri(idx,1);
		center = cdb_->getCenter(ef,1);
		vi = cdb_->getIndex(ef,0);
		v = cdb_->getVertex(vi);
		w = cdb_->getW(vi);
		lsqr = (v-center).lengthsqr();
		return lsqr-(Sign(w)*w*w+Sign(alpha)*alpha*alpha);
	case 2:
		ef = cdb_->repTri(idx,2);
		v = cdb_->getCenter(ef,0);
		w   = cdb_->getW(cdb_->getIndex(ef,0));
		center = cdb_->getCenter(ef,2);
		return (v-center).lengthsqr()-(Sign(w)*w*w+Sign(alpha)*alpha*alpha);
	case 3:
		ef = cdb_->repTri(idx,3);
		vi = cdb_->getIndex(ef,0);
		center = cdb_->getCenter(ef,3);
		v = cdb_->getVertex(vi);
		w  = cdb_->getW(vi);
		return (v-center).lengthsqr()-(Sign(w)*w*w+Sign(alpha)*alpha*alpha);
	default:
		Assert(0);
	}
	
	return 0;
};


Bool SkinMesh::inBody_(double x[3],Index idx)
{
	Dim d = idx & 3;
	int i = idx >>2;
	OrTri ef = cdb_->repTri(i,d);
	Vector3 center = cdb_->getCenter(ef,d);
	double rsqr = squaredSize(d,i);
	Vector3 vn,ab,ac,p;
	VIndex v1,v2,v3;
		
	Assert(cdb_->inMixedCell(x,ef,d));
		
	switch(d) {
	case 0:
		if((center-Vector3(x)).lengthsqr()<(rsqr/2.0))
			return TRUE;
		break;
	case 1:
		v1 = cdb_->org(ef); 
		v2 = cdb_->dest(ef);
		vn = cdb_->getVertex(v1) - cdb_->getVertex(v2);
		vn.normalize();
		p = ((Vector3(x)-center)*AlignWithYAxis(vn));///sqrt(fabs(rsqr));
		return (p.x[0]*p.x[0]+p.x[2]*p.x[2]-p.x[1]*p.x[1]+0.5*rsqr < 0);
	case 2:
		cdb_->getTrist()->triangle(ef,v1,v2,v3);
		vn = cross_prod(cdb_->getVertex(v2) - cdb_->getVertex(v1),cdb_->getVertex(v3) - cdb_->getVertex(v1));
		vn.normalize();
		p = ((Vector3(x)-center)*AlignWithYAxis(vn));///sqrt(fabs(rsqr));
		return (p.x[0]*p.x[0]+p.x[2]*p.x[2]-p.x[1]*p.x[1]-0.5*rsqr > 0);
	case 3:
		if((center-Vector3(x)).lengthsqr()>(rsqr/2.0))
			return TRUE;
		break;
	default:
		Assert(0);
	};
	
	return FALSE;
}


void SkinMesh::sculpt_()
{
	

	DCompBuilder *dcb = new DCompBuilder(8,8);
	int i;
	TIndex ti1,ti2;
	VIndex vi;
	OrTri ef;
	Index mc;
	double x[4];
	x[3] = 0;
	BitVector tbv_,fbv_;
	BitVector nn;

	tbv_.reset();
	
	FArrayIter<Vector3> fai(parray_);

	for(fai.first();!fai.isDone();fai.next())
	{
		for(i=0;i<3;i++)
			x[i] = fai.curItem().x[i];
		vi = dcb->addPoint(x);
		Assert(vi==fai.curIndex());
	}

	dcb->compute();
	dcb->cleanTriangulation();
	if(mesh_)
		delete mesh_;
	CompDB *meshCDB = new CompDB(dcb);
	
	Vector3 center;
	
	Vector3 v1,v2,v3,v4,n;
	
	
	for(i=1;i<=meshCDB->numSimplex(3);i++)
	{
		ef = meshCDB->repTri(i,3);
		center = meshCDB->getCenter(ef,3);
		
		mc = cdb_->locateMC(center.x,vmc_[meshCDB->org(ef)]);
		
		if(inBody_(center.x,mc))
			tbv_.set(i);
		else
			tbv_.set(i,0);
		
	}
	
	TristIter ti(*(meshCDB->getTrist()));
	fbv_.reset();
	
	for(ti.first();!ti.isDone();ti.next())
	{
		ef = OrTri(ti.curItem(),0);
		ti1 = meshCDB->getIndex(ef,3);
		ti2 = meshCDB->getIndex(meshCDB->sym(ef),3);
		if(ti1&&ti2)
		{
			if(     (tbv_.test(ti1)&&!tbv_.test(ti2)) ||
				(tbv_.test(ti2)&&!tbv_.test(ti1)) )
				fbv_.set(ef.index());
			else
				meshCDB->getTrist()->delTri(ef);
		}
		else // boundary
		{
			if(ti1)
			{
				if(tbv_.test(ti1))
					fbv_.set(ef.index());
				else
					meshCDB->getTrist()->delTri(ef);
			}

				
			if(ti2)
			{
				if(tbv_.test(ti2))
					fbv_.set(ef.index());			
				else
					meshCDB->getTrist()->delTri(ef);
			}
		}
	}
	

	mesh_ = meshCDB->getTrist();

	delete meshCDB;
	delete dcb;
}

Bool SkinMesh::refineMesh()
{
	Bool somethingChanged = TRUE;
	debugPts_.reset();
	debugPts2_.reset();
	debugPi_.reset();
	Bool firsttime =TRUE;
//	char str[255];

	inBodyTotal_ = 0;
	inBodyGeo_ = 0;
	inBodyExp_ = 0;

	somethingChanged = FALSE;
	somethingChanged = contractEdges_();
	somethingChanged |= insertCCenters_();
/*
		sprintf(str,"Total: %d  Geo: %d  Exp: %d",inBodyTotal_,inBodyGeo_,inBodyExp_);
		MessageBox(NULL,str,"InBodyTest",MB_OK);
*/
	computeNormals_();
	return somethingChanged;
}

void SkinMesh::flipAllEdges()
{
	TristIter ti(*mesh_);
	OrTri ef;
	IterStack<OrTri> es;
	for(ti.first();!ti.isDone();ti.next())
	{
		ef = OrTri(ti.curItem(),0);
		es.push(ef);
		es.push(ef.enext());
		es.push(ef.enext2());
	}
	flipEdges_(&es);
	computeNormals_();
}

void SkinMesh::flipEdges_(IterStack<OrTri>* flipQ,IterStack<OrTri>* touchedTri)
{
	OrTri abc,abd,ef;
	Vector3 a,b,c,d,center;
	Vector3 abcn,p,cdpn,abpn;
	VIndex ai,bi,ci,di;
	VIndex ei,fi,gi,hi;
	Index mci;
	Bool concave;
	Bool needFlip;
	BitVector fq;
	double t;
	IStackIter<OrTri> tsi(*flipQ);
	BitVector edgeBV_; // edges on stack
	OrTri e1,e2,e3,e4; // edges to be flipped;
	Bool aboveabc,aboveabd;
	Bool aboveadc,abovecdb;

	inBodyTotal_++;

	for(tsi.first();!tsi.isDone();tsi.next())
	{
		ef = tsi.curItem();
		ef = MINEDGE(ef);
		edgeBV_.set(ef.getIntRep());
	}

	while(!flipQ->empty())
	{
		// try to flip edge ab
		abc = flipQ->pop();
		edgeBV_.clear(MINEDGE(abc).getIntRep());
		if(!mesh_->deleted(abc.index()))
		{
			needFlip=FALSE;
			abd = mesh_->fnext(abc);
			mesh_->triangle(abc,ai,bi,ci);
			di=mesh_->dest(mesh_->enext(abd));
			a = parray_[ai];
			b = parray_[bi];
			c = parray_[ci];
			d = parray_[di];
			center = tetraCenter(a,b,c,d);
			mci = cdb_->locateMC(center.x,vmc_[ai]);

			abcn = cross_prod(b-a,c-a);
			Assert((abc.version()&1)==0);
			{
			if(dot_prod(d-a,abcn)>0)
				concave=TRUE;
			else
				concave=FALSE;
			}

			aboveabc = dot_prod(center-a,abcn)>0;
			aboveabd = dot_prod(center-a,cross_prod(d-a,b-a))>0;
			aboveadc = dot_prod(center-a,cross_prod(d-a,c-a))>0;
			abovecdb = dot_prod(center-b,cross_prod(c-b,d-b))>0;
/*
			if((!aboveabc)&&(!aboveabd))
			{
				if(concave)
					needFlip=TRUE;
			} else if(aboveabc&&aboveabd)
			{
				if(!concave)
					needFlip=TRUE;
			} else
			{
				if(inBody_(center.x,mci))
				{
					if(concave)
						needFlip=TRUE;
				} else
					if(!concave)
						needFlip=TRUE;
			}

*/
			if(concave)
			{
				if( ((!aboveabc)||(!aboveabd))&&(!aboveadc)&&(!abovecdb) )
					needFlip=TRUE;
				else if(   (aboveabc&&aboveabd)&&(aboveadc||abovecdb) )
				{

				} else if(aboveabc&&aboveabd&&(!aboveadc)&&(!abovecdb))
				{
					inBodyGeo_++;
					if(inBody_(center.x,mci))
						needFlip=TRUE;
				}  else {
					inBodyExp_++;
/*
					debugPts_.push(a);
					debugPts_.push(b);
					debugPts_.push(c);
					debugPts_.push(d);
*/
				}

			} else
			{
				if( (aboveabc||aboveabd) && aboveadc && abovecdb )
					needFlip=TRUE;
				else if ( (!aboveabc) && (!aboveabd) && ((!aboveadc)||(!abovecdb)) )
				{
/*
				} else	if(!inBody_(center.x,mci))
					needFlip=TRUE;
*/
				} else if(aboveabc&&aboveabd&&(!aboveadc)&&(!abovecdb))
				{
					inBodyGeo_++;
					if(!inBody_(center.x,mci))
						needFlip=TRUE;
				}  else {
					inBodyExp_++;
/*
					debugPts_.push(a);
					debugPts_.push(b);
					debugPts_.push(c);
					debugPts_.push(d);
*/
				}

			}

			if(needFlip) // check if flippable
			{

				// combinatorial check
				e1 = mesh_->fnext(mesh_->enext(abc)).sym(); // cbe
				ei = mesh_->dest(mesh_->enext(e1));
				e2 = mesh_->fnext(mesh_->enext2(abc)).sym(); //acf
				fi = mesh_->dest(mesh_->enext(e2));
				e3 = mesh_->fnext(mesh_->enext(abd)); // bdg
				gi = mesh_->dest(mesh_->enext(e3));
				e4 = mesh_->fnext(mesh_->enext2(abd)); //dah
				hi = mesh_->dest(mesh_->enext(e4));


				if(  (ei!=di)&&(fi!=di)&&(gi!=ci)&&(hi!=ci))
				{

					p = cross_prod(b-a,d-c);
					cdpn = cross_prod(d-c,p);
					t = dot_prod(cdpn,c-a)/dot_prod(cdpn,b-a);
					if((t>0)&&(t<1))

					{
						/*
						abpn = cross_prod(b-a,p);
						t = dot_prod(abpn,a-c)/dot_prod(abpn,d-c);
						if((t>0)&&(t<1))
						*/
						{
							

							if(!edgeBV_.test(MINEDGE(e1).getIntRep()))
							{
								edgeBV_.set(MINEDGE(e1).getIntRep());
								Assert((e1.version()&1)==0);
								flipQ->push(e1);
							}
							if(!edgeBV_.test(MINEDGE(e2).getIntRep()))
							{
								edgeBV_.set(MINEDGE(e2).getIntRep());
								Assert((e2.version()&1)==0);
								flipQ->push(e2);
							}
							if(!edgeBV_.test(MINEDGE(e3).getIntRep()))
							{
								edgeBV_.set(MINEDGE(e3).getIntRep());
								Assert((e3.version()&1)==0);
								flipQ->push(e3);
							}
							if(!edgeBV_.test(MINEDGE(e4).getIntRep()))
							{
								edgeBV_.set(MINEDGE(e4).getIntRep());
								Assert((e4.version()&1)==0);
								flipQ->push(e4);
							}
/*
							debugPts_.push(a);
							debugPts_.push(b);
							debugPts_.push(c);
							debugPts_.push(d);
*/
							OrTri adc;OrTri bcd;
							ef = mesh_->flipEdge(abc,adc,bcd);
							if(touchedTri)
							{
								touchedTri->push(ef);
								touchedTri->push(mesh_->fnext(ef.sym()));
							}
						}
					}
				}
			}
		}
	}
}

Bool SkinMesh::insertCCenters_()
{
	OrTri abc,abv,bcv,cav;
	OrTri ef,ef2,orgabc;
	VIndex ai,bi,ci,vi;
	Vector3 a,b,c,n,abcn;
	Bool somethingChanged=FALSE;
	TristIter ti(*mesh_);
	StackQueue<OrTri> ts;
	IterStack<OrTri> flipQ;
	Vector3 center,centerOnTri;
	double lssa,lssb,lssc; // length scale square
	double CQ2 = C_*C_*Q_*Q_; // (C*Q)^2
	double Rabc2 ; // R_abc square
	Index mci,mcd;
	Vector3 newPt,p1,p2,pn,cc;
	BitVector tbv;
	IterStack<OrTri> searchS;
	Bool found;
	BitVector trouble;
	IterStack<OrTri> touched;


	for(ti.first();!ti.isDone();ti.next())
		ts.push(OrTri(ti.curItem(),0));

	while(!ts.empty()) 
	{
		abc = ts.get();
		ef = abc;
		if(!mesh_->deleted(abc.index()))
		{
			mesh_->triangle(abc,ai,bi,ci);
			a = parray_[ai];
			b = parray_[bi];
			c = parray_[ci];

			lssa = (cdb_->getCenter(vmc_[ai])-a).lengthsqr();
			lssb = (cdb_->getCenter(vmc_[bi])-b).lengthsqr();
			lssc = (cdb_->getCenter(vmc_[ci])-c).lengthsqr();

			center = circumCenter(a,b,c);

			Rabc2 = (center-a).lengthsqr();

/*
			if(Rabc2<=CQ2*max(lssa,max(lssb,lssc)))
			{
				debugPts_.push(a);
				debugPts_.push(b);
				debugPts_.push(c);
				debugPts_.push((a+b+c)/3);
			}
*/
			if(Rabc2>CQ2*max(lssa,max(lssb,lssc)))
			{
				tbv.reset();
				searchS.reset();
				n = cross_prod(b-a,c-a);
				n.normalize();
				found = FALSE;
				searchS.push(abc);
				tbv.set(abc.index());
				orgabc = abc;

				searchS.push(mesh_->fnext(abc).sym());
				searchS.push(mesh_->fnext(abc.enext()).sym());
				searchS.push(mesh_->fnext(abc.enext2()).sym());

				while((!found)&&(!searchS.empty())) {
					abc = searchS.pop();
					mesh_->triangle(abc,ai,bi,ci);
					a = parray_[ai];
					b = parray_[bi];
					c = parray_[ci];
					if(lineIntersectTri(center,n,a,b,c))
						found = TRUE;
					else {
						if(trouble.test(abc.index()))
						{
							cc = center-((a+b+c)/3);
							pn = cross_prod(n,cc);
							for(int i=0;i<3;i++)
							{
								abc = abc.enext();
								ef = mesh_->fnext(abc.sym());
								if(!tbv.test(ef.index()))
								{
									mesh_->triangle(ef,ai,bi,ci);
									a = parray_[ai];
									b = parray_[bi];
									c = parray_[ci];
//									if(dot_prod(cc,center-((a+b+c)/3))>0)
										if(dot_prod(cross_prod(b-a,c-a),n)>0)
										{
											tbv.set(ef.index());
											searchS.push(ef);
										}

								}
							}
						}

					}
				};
/*
				if(!found)
				{
					ts.push(mesh_->fnext(abc).sym());
					ts.push(mesh_->fnext(abc.enext()).sym());
					ts.push(mesh_->fnext(abc.enext2()).sym());
				}
*/
				if(!found)
				{
					abc = orgabc;
					if(!trouble.test(abc.index()))
					{
//						ts.push(abc);
						trouble.set(abc.index());
						ts.push(abc);
					} 

					else
					{
/*
						mesh_->triangle(abc,ai,bi,ci);
						a = parray_[ai];
						b = parray_[bi];
						c = parray_[ci];

						debugPts_.push(a);
						debugPts_.push(b);
						debugPts_.push(c);
						debugPts_.push((a+b+c)/3);
*/
					}
/*
					ts.push(mesh_->fnext(abc).sym());
					ts.push(mesh_->fnext(abc.enext()).sym());
					ts.push(mesh_->fnext(abc.enext2()).sym());
*/
				}

//				if(lineIntersectTri(center,n,a,b,c))
				if(found)
				{
					trouble.clear(abc.index());
					Assert(lineIntersectTri(center,n,a,b,c));
					abcn = cross_prod(b-a,c-a);
					abcn.normalize();
					centerOnTri = lineIntersectPlane(center,n,abcn,dot_prod(abcn,a));

					mci=cdb_->locateMC(centerOnTri.x,vmc_[ai]);
					mcd=mci&3;
					if(solve_(mci,centerOnTri,n,p1,p2))
					{
						newPt=(centerOnTri-p1).lengthsqr()<(centerOnTri-p2).lengthsqr()?p1:p2;
						Assert(lineIntersectTri(centerOnTri,n,a,b,c));
						vi = parray_.newLoc();
						parray_[vi] = newPt;
						vmc_[vi]=mci;
						abv=mesh_->makeTri(ai,bi,vi);
						bcv=mesh_->makeTri(bi,ci,vi);
						cav=mesh_->makeTri(ci,ai,vi);

						mesh_->fmerge(mesh_->enext(abv),mesh_->enext(mesh_->sym(bcv)));
						mesh_->fmerge(mesh_->enext(bcv),mesh_->enext(mesh_->sym(cav)));
						mesh_->fmerge(mesh_->enext(cav),mesh_->enext(mesh_->sym(abv)));
						mesh_->fmerge(abv,abc);
						mesh_->fmerge(bcv,mesh_->enext(abc));
						mesh_->fmerge(cav,mesh_->enext2(abc));
						mesh_->delTri(abc);
						Assert((abv.version()&1)==0);
						Assert((bcv.version()&1)==0);
						Assert((cav.version()&1)==0);
						flipQ.push(abv);
						flipQ.push(bcv);
						flipQ.push(cav);
						flipEdges_(&flipQ,&touched);
						ts.push(abv);
						ts.push(bcv);
						ts.push(cav);
						while(!touched.empty())
							ts.push(touched.pop());

						somethingChanged = TRUE;
					}
					else
						ts.push(orgabc);
				}
			}
		}
	}
	return somethingChanged;

}


Bool SkinMesh::contractEdges_()
{
	Vector3 a,b;
	VIndex ai,bi,ci,di,vi;
	TristIter ti(*mesh_);
	IterStack<OrTri> flipQ;
	StackQueue<OrTri> ts;
	OrTri abc,bad;
	OrTri bdv,dav,cbv,acv;
	double lssa,lssb; // length scale square
	double Rab2; // R_ab square
	double CQ2 = (C_*C_/(Q_*Q_));
	int i;
	Bool somethingChanged=FALSE;
	IterStack<VIndex> alink;
	OrTri ef;
	Bool linkOk;

	for(ti.first();!ti.isDone();ti.next())
		ts.push(OrTri(ti.curItem(),0));

	while(!ts.empty())
	{
		abc = ts.get();
		for(i=0;i<3;i++)
		{
			if(!mesh_->deleted(abc.index()))
			{
				ai = mesh_->org(abc);
				bi = mesh_->dest(abc);
				a  = parray_[ai];
				b  = parray_[bi];
				lssa = (cdb_->getCenter(vmc_[ai])-a).lengthsqr();
				lssb = (cdb_->getCenter(vmc_[bi])-b).lengthsqr();
				Rab2 = (a-b).lengthsqr()/4.0;
				if(Rab2 < CQ2 * max(lssa,lssb))
				{
					// contract edge ab (triangle abc and abd destroyed)
					alink.reset();
					bad = mesh_->sym(mesh_->fnext(abc));
					bdv = mesh_->sym(mesh_->fnext(mesh_->enext2(bad)));
					dav = mesh_->sym(mesh_->fnext(mesh_->enext(bad)));
					cbv = mesh_->sym(mesh_->fnext(mesh_->enext(abc)));
					acv = mesh_->sym(mesh_->fnext(mesh_->enext2(abc)));


					ci = mesh_->dest(acv);
					di = mesh_->org(dav);

					ef = abc;
					do { // alink : vertices of link(a) except d and c
						vi = mesh_->dest(ef);
						if(ci!=vi)
							if(di!=vi)
								alink.push(vi);
						ef = mesh_->fan(ef);
					} while(ef.index()!=abc.index());

					linkOk = TRUE;
					ef = bad;

					do { // alink : vertices of link(a) except d and c
						vi = mesh_->dest(ef);
						if(ci!=vi)
							if(di!=vi)
							{
								IStackIter<VIndex> isi(alink);
								for(isi.first();linkOk&&!isi.isDone();isi.next())
								{
									if(vi==isi.curItem())
										linkOk=FALSE;
								}
							}
						ef = mesh_->fan(ef);
					} while(linkOk&&(ef.index()!=bad.index()));


					if(linkOk)
					{
						somethingChanged=TRUE;
//						debugPts_.push(b);
						mesh_->updateStarCenter(abc,bi);
						mesh_->delTri(abc);
						mesh_->delTri(bad);
						mesh_->fmerge(bdv,mesh_->sym(dav));
						mesh_->fmerge(cbv,mesh_->sym(acv));
						parray_.delElt(ai);
				
						ef = mesh_->fan(bdv);
						Assert((bdv.version()&1)==0);
						flipQ.push(bdv);
						ts.push(bdv);

						while(ef.index()!=cbv.index())
						{
							Assert((mesh_->enext(ef).version()&1)==0);
							flipQ.push(mesh_->enext(ef));
							ts.push(mesh_->enext(ef));
							ef = mesh_->fan(ef);
						}
						flipEdges_(&flipQ);
					}
				}
			}
			abc = mesh_->enext(abc);
		}
	}
	return somethingChanged;
}


Bool SkinMesh::solve_(Index mc,Vector3 v,Vector3 n,Vector3& p1,Vector3& p2)
{

	Dim d = mc&3;
	Index mci = mc>>2;
	double t1,t2;
	double r2 = squaredSize(d,mci)/2.0;
	Vector3 center = cdb_->getCenter(mc);
	Vector3 cv;
	Vector3 vn;
	double a,b,c,delta,delta2;
	Vector3 v1,v2,v3,n2;

	switch (d) 
	{
	case 0:
	case 3:
		cv = v - center;
		if(r2<0)
			return FALSE;
		a = n.lengthsqr();
		b = 2*dot_prod(n,cv);
		c = cv.lengthsqr()-r2;
		break;
	case 1:
		cdb_->getVertex(mci,v1,v2);
		vn = v1-v2;
		n2 = n * AlignWithYAxis(vn);
		cv = (v - center) * AlignWithYAxis(vn);
		a = n2.lengthsqr()-2*n2.x[1]*n2.x[1];
		b = 2*dot_prod(n2,cv)-4*cv.x[1]*n2.x[1];
		c = cv.lengthsqr()-2*cv.x[1]*cv.x[1]+r2;
		break;
	case 2:
		cdb_->getVertex(mci,v1,v2,v3);
		vn = cross_prod(v3-v1,v2-v1);
		n2 = n * AlignWithYAxis(vn);
		cv = (v - center) * AlignWithYAxis(vn);
		a = n2.lengthsqr()-2*n2.x[1]*n2.x[1];
		b = 2*dot_prod(n2,cv)-4*cv.x[1]*n2.x[1];
		c = cv.lengthsqr()-2*cv.x[1]*cv.x[1]-r2;
		break;
	}

	delta = b*b-4*a*c;
	if(delta<0)
		return FALSE;
	delta2 = sqrt(delta);
	t1 = (-b+delta2)/(2*a);
	t2 = (-b-delta2)/(2*a);
	p1 = v+n*t1;
	p2 = v+n*t2;

	return TRUE;
}


void SkinMesh::draw() {

	Vector3 x1,x2,x3;
	VIndex vi1,vi2,vi3;
	TristIter ti(*mesh_);
//	glColor3f(0,0,1);
	int shadeModel;
	glGetIntegerv(GL_SHADE_MODEL,&shadeModel);

	for(ti.first();!ti.isDone();ti.next())
	{
		OrTri tri(ti.curItem(),0);
		glEnable(GL_LIGHTING);
		glBegin(GL_POLYGON);
		  x1 = parray_[vi1=mesh_->org(tri)];
		  x2 = parray_[vi2=mesh_->dest(mesh_->enext(tri))];
		  x3 = parray_[vi3=mesh_->dest(tri)];
		  if(shadeModel==GL_FLAT)
			glNormal3dv(fnarray_[ti.curItem()].x);
		  else
			glNormal3dv(narray_[vi1].x);
		  glVertex3dv(x1.x);
		  if(shadeModel==GL_SMOOTH)
			  glNormal3dv(narray_[vi3].x);
		  glVertex3dv(x3.x);
		  if(shadeModel==GL_SMOOTH)
			  glNormal3dv(narray_[vi2].x);
		  glVertex3dv(x2.x);
		glEnd();
/*
		glDisable(GL_LIGHTING);
		glBegin(GL_LINE_STRIP);
		  x1 = parray_[vi1=mesh_->org(tri)];
		  x2 = parray_[vi2=mesh_->dest(mesh_->enext(tri))];
		  x3 = parray_[vi3=mesh_->dest(tri)];
		  glVertex3dv((x1+narray_[vi1]*0.1).x);
		  glVertex3dv((x3+narray_[vi3]*0.1).x);
		  glVertex3dv((x2+narray_[vi2]*0.1).x);
		  glVertex3dv((x1+narray_[vi1]*0.1).x);
		glEnd();
*/
/*
		glColor3f(0,0,1);
		glBegin(GL_LINE_STRIP);
		  x1 = parray_[vi1=mesh_->org(tri)];
		  x2 = parray_[vi2=mesh_->dest(mesh_->enext(tri))];
		  x3 = parray_[vi3=mesh_->dest(tri)];
		  glVertex3dv(x1.x);
		  glVertex3dv(x3.x);
		  glVertex3dv(x2.x);
		glEnd();
*/
/*
		// draw normal for debug
		glBegin(GL_LINE_STRIP);
		  glVertex3dv(x1.x);
		  glVertex3dv((x1+narray_[vi1]*10).x);
		glEnd();
*/
	}
	glDisable(GL_LIGHTING);

	glColor3f(0,1,0);
	glPointSize(5);
	IStackIter<Vector3> vi(debugPts_);
	IStackIter<Vector3> vib(debugPts2_);
/*
	int i=0;
	for(vi.first();!vi.isDone();vi.next())
	{
		glBegin(GL_LINE_STRIP);
		glVertex3dv((vi.curItem()+narray_[vi.curIndex()]).x);
		glVertex3dv(vi.curItem().x);
		glEnd();
	}
*/
	glColor3f(1,0,0);
	glBegin(GL_POINTS);
	for(vib.first();!vib.isDone();vib.next())
		glVertex3dv(vib.curItem().x);
	glEnd();

	glColor3f(0,0,1);
	glBegin(GL_POINTS);
	for(vi.first();!vi.isDone();vi.next())
		glVertex3dv(vi.curItem().x);
	glEnd();
/*
	Vector3 v[4];
	int i=0;
	for(vi.first();!vi.isDone();vi.next())
	{
		v[i] = vi.curItem();
		if(i==3)
		{
			glBegin(GL_LINE_STRIP);
				glVertex3dv(v[0].x);
				glVertex3dv(v[1].x);
				glVertex3dv(v[2].x);
				glVertex3dv(v[0].x);
				glVertex3dv(v[3].x);
				glVertex3dv(v[2].x);
				glVertex3dv(v[1].x);
				glVertex3dv(v[3].x);
			glEnd();
		}
		i++;
		if(i>=4)
			i = 0;

	}
*/


//	glEnable(GL_LIGHTING);
/*
	int i,j;
	IStackIter<Vector3> *vi;
	double x[4];
	OrTri ef;
	VIndex vi1,vi2,vi3;
*/

/*	
	BitVectorIter bvi(fbv_);
	
	Vector3 v1,v2,v3,n;
	
//	glPointSize(1);
//	glColor3d(1,0,1);
	for(bvi.first();!bvi.isDone();bvi.next())
	{
		ef = meshCDB->repTri(bvi.curItem(),2);
		meshCDB->getTrist()->triangle(ef,vi1,vi2,vi3);
//		glBegin(GL_LINE_STRIP);
		glBegin(GL_POLYGON);
		v1 = meshCDB->getVertex(vi1);
		v2 = meshCDB->getVertex(vi2);
		v3 = meshCDB->getVertex(vi3);
		n  = cross_prod(v2-v1,v3-v1);
//		n.normalize();
		glNormal3dv(n.x);
		glVertex3dv(v1.x);
		glVertex3dv(v2.x);
		glVertex3dv(v3.x);
//		glVertex3dv(v1.x);
		glEnd();
	}
	
/*	
	BitVectorIter bvi2(tbv_);
	Vector3 v1,v2,v3,v4,center;
	
	for(bvi2.first();!bvi2.isDone();bvi2.next())
	{
		ef = meshCDB->repTri(bvi2.curItem(),3);
		center = meshCDB->getCenter(ef,3);
		
		meshCDB->getTrist()->tetra(ef,vi1,vi2,vi3,vi4);
		v1 = meshCDB->getVertex(vi1);
		v2 = meshCDB->getVertex(vi2);
		v3 = meshCDB->getVertex(vi3);
		v4 = meshCDB->getVertex(vi4);
		v = (v1+v2+v3+v4)/4.0;
		
		glPointSize(1);
		glColor3d(1,0,0);
		glBegin(GL_LINE_STRIP);
		glVertex3dv(v.x);
		glVertex3dv(center.x);
		glEnd();
		glPointSize(7);
		glColor3d(1,1,0);
		glBegin(GL_POINTS);
		glVertex3dv(center.x);
		glEnd();
		
	}
*/

	/*
	for(i=0;i<4;i++) {
	switch(i) {
	case 0:
	glColor3d(1,0,0);
	break;
	case 1:
	glColor3d(0,1,1);
	break;
	case 2:
	glColor3d(0,1,0);
	break;
	case 3:
	glColor3d(1,1,1);
	break;
	}
	
	  if(visible_[i])
	  {
	  for(j=1;j<=cdb_->numSimplex(i);j++)
	  {
	  glBegin(GL_POINTS);
	  vi = new IStackIter<Vector3>(pts_[i][j]);
	  for(vi->first();!vi->isDone();vi->next())
	  glVertex3fv(vi->curItem().fp());
	  delete vi;
	  glEnd();
	  }
	  }
	  }
	*/
}


void SkinMesh::drawMesh(double offset) {

	Vector3 x1,x2,x3;
	VIndex vi1,vi2,vi3;
	TristIter ti(*mesh_);
	glColor3f(0,1,0);

	for(ti.first();!ti.isDone();ti.next())
	{
		OrTri tri(ti.curItem(),0);
		glDisable(GL_LIGHTING);
		glBegin(GL_LINE_STRIP);
		  x1 = parray_[vi1=mesh_->org(tri)];
		  x2 = parray_[vi2=mesh_->dest(mesh_->enext(tri))];
		  x3 = parray_[vi3=mesh_->dest(tri)];
		  glVertex3dv((x1+narray_[vi1]*offset).x);
		  glVertex3dv((x3+narray_[vi3]*offset).x);
		  glVertex3dv((x2+narray_[vi2]*offset).x);
		  glVertex3dv((x1+narray_[vi1]*offset).x);
		glEnd();

	}
}

double SkinMesh::currentAlpha_(VIndex vi)
{

	Index mc = vmc_[vi];
	Dim d = mc&3;
	Index idx = mc >> 2;
	double w,lsqr,a2;
	Vector3 c = cdb_->getCenter(mc);

	switch(d) {

	case 0:
		w = cdb_->getW(idx);
		lsqr = 2*(c-parray_[vi]).lengthsqr();
		a2 = (lsqr-Sign(w)*w*w);
		return Sign(a2)*sqrt(fabs(a2));
		break;
	case 1:
		return alpha_;
	case 2:
	case 3:
		break;
	};
	return 0;
}

Vector3 SkinMesh::pointNormal_(int vi)
{
	Vector3 p = parray_[vi];
	Index mc = vmc_[vi];
	Dim d = mc &3;
	Index idx = mc >> 2;
	Vector3 n,ne,a,b,c;
	Vector3 o = cdb_->getCenter(mc);
	Vector3 op = p-o;
	switch (d) {
	case 0:
		n = p-o;
		break;
	case 3:
		n = o-p;
		break;
	case 1:
		ne = cdb_->getVertex(cdb_->org(cdb_->repTri(idx,d))) - 
			 cdb_->getVertex(cdb_->dest(cdb_->repTri(idx,d)));
		if(dot_prod(ne,op)<0)
			ne = -ne;
		n = (op-ne*dot_prod(ne,op)/ne.lengthsqr())*2-op;
		break;
	case 2:
		a = cdb_->getVertex(cdb_->org(cdb_->repTri(idx,d)));
		b = cdb_->getVertex(cdb_->dest(cdb_->repTri(idx,d)));
		c = cdb_->getVertex(cdb_->dest(cdb_->repTri(idx,d).enext()));
		ne = cross_prod(b-a,c-a);
		if(dot_prod(ne,op)<0)
			ne = -ne;
		n = op-(op-ne*dot_prod(ne,op)/ne.lengthsqr())*2;
		break;
	}

	n.normalize();
	return n;
}

void SkinMesh::finalUpdate(double alpha)
{
	BitVector safebv;
	FArrayIter<Vector3> pi(parray_);
	IterStack<OrTri> es;
	int i;

	for(pi.first();!pi.isDone();pi.next())
	{
		if(alpha>alpha_)
		  safebv.set(pi.curIndex(),growPt_(pi.curIndex(),alpha));
		else
		  safebv.set(pi.curIndex(),shrinkPt_(pi.curIndex(),alpha));
	}
	TristIter ti(*mesh_);
	for(ti.first();!ti.isDone();ti.next())
	{
		OrTri ef(ti.curItem(),0);
		for(i=0;i<3;i++)
		{
			if((!safebv.test(mesh_->org(ef)))||(!safebv.test(mesh_->dest(ef))))
			{
				es.push(ef);
			}
			ef = ef.enext();
		}
	}
	alpha_ = alpha;
//	flipEdges_(&es);
//	refineMesh();
//	flipAllEdges();
	computeNormals_();
}

Bool SkinMesh::testInsertCCenters_()
{
	OrTri abc,abv,bcv,cav;
	OrTri ef,ef2,orgabc;
	VIndex ai,bi,ci;
	Vector3 a,b,c,n,abcn;
	Bool somethingChanged=FALSE;
	TristIter ti(*mesh_);
	StackQueue<OrTri> ts;
	IterStack<OrTri> flipQ;
	Vector3 center,centerOnTri;
	double lssa,lssb,lssc; // length scale square
	double CQ2 = C_*C_*Q_*Q_; // (C*Q)^2
	double Rabc2 ; // R_abc square
	Vector3 newPt,p1,p2,pn,cc;
	BitVector tbv;
	IterStack<OrTri> searchS;
	BitVector trouble;


	for(ti.first();!ti.isDone();ti.next())
		ts.push(OrTri(ti.curItem(),0));

	while(!ts.empty()) 
	{
		abc = ts.get();
		ef = abc;
		if(!mesh_->deleted(abc.index()))
		{
			mesh_->triangle(abc,ai,bi,ci);
			a = parray_[ai];
			b = parray_[bi];
			c = parray_[ci];

			lssa = (cdb_->getCenter(vmc_[ai])-a).lengthsqr();
			lssb = (cdb_->getCenter(vmc_[bi])-b).lengthsqr();
			lssc = (cdb_->getCenter(vmc_[ci])-c).lengthsqr();

			center = circumCenter(a,b,c);

			Rabc2 = (center-a).lengthsqr();

			if(Rabc2>CQ2*max(lssa,max(lssb,lssc)))
			{
				debugPts2_.push(a);
				debugPts2_.push(b);
				debugPts2_.push(c);
			}
		}
	}
	return somethingChanged;

}

Bool SkinMesh::growPt_(int vi,double alpha)
{
	Index lastmc = vmc_[vi];
	Index newmc = vmc_[vi];
	Index startmc = vmc_[vi];

	Bool done=FALSE; // just a moron flag for now

	while(!done)
	{
		switch (vmc_[vi]&3) {
		case 0:
			/*
			done = TRUE;
			break;*/
			newmc = growPt0_(vi,alpha);
			if(newmc==lastmc)
				done = TRUE;
			break;
		case 1:
			newmc = growPt1_(vi,alpha);
			if(newmc==vmc_[vi])
				done = TRUE;
			break;
		case 2:
			newmc = growPt2_(vi,alpha);
			if(newmc==vmc_[vi])
				done = TRUE;
			break;
		case 3:

			newmc = growPt3_(vi,alpha);
			if(newmc==vmc_[vi])
				done = TRUE;
			break;
		default:
			Assert(0);
			break;
		}
		lastmc = vmc_[vi];
		vmc_[vi] = newmc;
	}


	if(startmc==vmc_[vi])
		if(( (startmc&3)==0 )|| ((startmc&3)==3))
			return TRUE;
	return FALSE;
}

Index SkinMesh::growPt0_(int vi,double alpha)
{
	Vector3 p = parray_[vi];
	Vector3 np,np2,pln;
	Vector3 o = cdb_->getCenter(vmc_[vi]);
	double lsqr = (p-o).lengthsqr();
	VIndex idx = vmc_[vi]>>2;
	Assert((vmc_[vi]&3)==0);
	IterStack<OrTri> es;
	OrTri ef;
	VIndex vi2;
	Index nmc = -999;


	np = o +  (p-o)*sqrt(0.5)*sqrt(squaredSize(0,idx,alpha))/sqrt(lsqr);
	if(cdb_->inMixedCell(np.x,cdb_->repTri(idx,0),0))
	{
		parray_[vi] = np;
		return vmc_[vi];
	}
	else
	{
		cdb_->edgeStar(idx,&es);
		while(!es.empty())
		{
			ef = es.pop();
			vi2 = cdb_->dest(ef);
			pln = cdb_->getVertex(vi2)-o;
			if(dot_prod(p-o,pln)>0)
			{
				np2=lineIntersectPlane(p,p-o,pln,dot_prod(pln,(o+cdb_->getCenter(ef,1))/2));
				if(dot_prod(p-o,np2-o)>0)
					if( (np2-o).lengthsqr()<(np-o).lengthsqr())
					{
						np = np2;
						nmc = (cdb_->getIndex(ef,1)<<2)|1;
					}
			}
		}
		if(nmc!=-999)
			parray_[vi] = np;
		else {
			cdb_->edgeStar(idx,&es);
			while(!es.empty())
			{
				ef = es.pop();
				vi2 = cdb_->dest(ef);
				pln = cdb_->getVertex(vi2)-o;
				if(dot_prod(p-o,pln)>0)
				{
					np2=lineIntersectPlane(p,p-o,pln,dot_prod(pln,(o+cdb_->getCenter(ef,1))/2));
					if( (np2-o).lengthsqr()<(np-o).lengthsqr())
					{
						np = np2;
						nmc = (cdb_->getIndex(ef,1)<<2)|1;
					}
				}
			}
		}
	}

	if(nmc==-999)
	{
//		parray_[vi] = o +  (p-o)*sqrt(0.5)*sqrt(squaredSize(0,idx,alpha))/sqrt(lsqr);
		debugPts2_.push(parray_[vi]);
		return vmc_[vi];
	}
	return nmc;

}

Index SkinMesh::growPt1_(int vi,double alpha)
{
	Index mc = vmc_[vi];
	Assert((mc&3)==1);
	OrTri ef = cdb_->repTri(mc);
	OrTri ef2,efs;
	VIndex vi1,vi2;
	Vector3 ne,o,p,op,c,oc,odash;
	Vector3 pp,v3,vp[2],ptn=pointNormal_(vi);
	Vector3 p0;
	double lsqr,tdash;
	double rsqr=squaredSize(1,mc>>2,alpha)/2;
	double x1,x2,x1x2;
	Vector3 pnew;
	Dim d = -999;

	vi1 = cdb_->org(ef);
	vi2 = cdb_->dest(ef);

	p  = parray_[vi];
	vp[0] = cdb_->getVertex(vi1);
	vp[1] = cdb_->getVertex(vi2);
	ne = vp[1]-vp[0];
	o  = cdb_->getCenter(mc);
	op = p-o;
	ne.normalize();  // necessary?

	if(dot_prod(ne,ptn)<0)
	{   
		p0 = (o+vp[0])/2;
		ne = -ne;
		ef = ef.sym();
	} else
	{
		p0 = (o+vp[1])/2;
	}
	
	Matrix33 m = AlignWithYAxis(ne,op),mi;

	Assert(dot_prod(ne,op)<0);

	pp = op*m;
	x1x2 = pp.x[0]*pp.x[1];
	Assert(x1x2<0);
	x2 = -sqrt( (rsqr+sqrt(rsqr*rsqr+4*x1x2*x1x2)) / 2);
	x1 = x1x2/x2;
	mi = m.Inverse();
	pnew = Vector3(x1,x2,0)*mi + o;
	if(cdb_->inMixedCell(pnew.x,ef,1))
	{
		parray_[vi] = pnew;
		return vmc_[vi];
	}

	lsqr = x1*x1;

	ef2 = ef;
	do {
		c = (cdb_->getCenter(ef2,2)+o)/2;
		v3 = cdb_->getVertex(cdb_->dest(ef2.enext()));
		oc = c-o;
		if(dot_prod(oc,v3-o)<0)
			oc = -oc;
		
		if(dot_prod(oc,op)>0)
		{

			odash = intersect3Planes(ne,dot_prod(ne,o),
									 cross_prod(ne,op),dot_prod(cross_prod(ne,op),p),
									 oc,dot_prod(oc,c));
			if(lsqr> (odash-o).lengthsqr())
			{
				efs = ef2;
				d = 2;
				lsqr = (odash-o).lengthsqr();
			}
		}
		ef2 = cdb_->fnext(ef2);
	}  while (ef2.index()!=ef.index());

	if(dot_prod(ptn,p0-o)<0)
	{
		if(lsqr> x1x2*x1x2/(p0-o).lengthsqr())
		{
			lsqr = x1x2*x1x2/(p0-o).lengthsqr();
			d = 0;
			efs = ef.sym();			
		}
	}

	Assert((d==0)||(d==2));
	tdash = sqrt(lsqr);
	parray_[vi] = Vector3(tdash,x1x2/tdash,0)*mi + o;
	return cdb_->getIndex(efs,d)<<2|d;
}

Index SkinMesh::growPt2_(int vi,double alpha)
{
	Index mc = vmc_[vi];
	Assert((mc&3)==2);
	OrTri ef = cdb_->repTri(mc);
	OrTri ef2,efs;
	Dim d=-999;
	VIndex vi1,vi2,vi3;
	Vector3 ne,o,p,op,odash,c,oc,pln;
	Vector3 pp,v3,ptn=pointNormal_(vi);
	double hsqr,tdash;
	double rsqr=-squaredSize(2,mc>>2,alpha)/2;
	double x1,x2,x1x2;
	Vector3 pnew;


	vi1 = cdb_->org(ef);
	vi2 = cdb_->dest(ef);
	vi3 = cdb_->dest(ef.enext());

	p  = parray_[vi];
	ne = cross_prod(cdb_->getVertex(vi2)-cdb_->getVertex(vi1),cdb_->getVertex(vi3)-cdb_->getVertex(vi1));
	o  = cdb_->getCenter(mc);
	op = p-o;
	ne.normalize();  // necessary?
	if(dot_prod(ne,ptn)>0)
		ne = -ne;
	
	Matrix33 m = AlignWithYAxis(ne,op),mi;

	Assert(dot_prod(ne,op)<0);

	pp = op*m;
	x1x2 = pp.x[0]*pp.x[1];
	x2 = -sqrt( (rsqr+sqrt(rsqr*rsqr+4*x1x2*x1x2)) / 2);
	x1 = x1x2/x2;
	Assert(x1x2<0);
	Assert(x1>0);
	mi = m.Inverse();
	pnew = Vector3(x1,x2,0)*mi + o;
	if(cdb_->inMixedCell(pnew.x,ef,2))
	{
		parray_[vi] = pnew;
		return vmc_[vi];
	}

	hsqr = x2*x2;

	ef2 = ef;
	do {
		if(cdb_->getIndex(ef2,3))  // if exist a tetrahedron coface
		{
			c = (cdb_->getCenter(ef2,3)+o)/2;
			v3 = cdb_->getVertex(cdb_->dest(cdb_->fnext(ef2).enext()));
			oc = c-o;
			if(dot_prod(oc,ptn)>0)
			{
				Assert(dot_prod(oc,ptn)>0);
				if(oc.lengthsqr()<hsqr)
				{
					efs = ef2;
					d = 3;
					hsqr = oc.lengthsqr();
				}
			}
		}
		ef2 = ef2.sym();
	}  while (ef2.version()!=ef.version());


	for(int i=0;i<3;i++)
	{
		ef2 = ef2.enext();
		c = (cdb_->getCenter(ef2,1)+o)/2;
		v3 = cdb_->getVertex(cdb_->dest(ef2.enext()));
		oc = c-o;
		if(dot_prod(oc,v3-cdb_->getVertex(cdb_->org(ef2)))>0) // hidden
		{
			pln = -oc;
			if(dot_prod(ptn,pln)>0)
			{
				odash = intersect3Planes(pln,dot_prod(pln,c),ne,dot_prod(ne,o),
										 cross_prod(ptn,ne),dot_prod(cross_prod(ptn,ne),p));
				if(x1x2*x1x2/(odash-o).lengthsqr()<hsqr)
				{
					efs = ef2;
					d = 1;
					hsqr = x1x2*x1x2/(odash-o).lengthsqr();
				}
			}
		}
	}

	tdash = -x1x2/sqrt(hsqr);
	parray_[vi] = Vector3(tdash,x1x2/tdash,0)*mi + o;
	Assert(d!=-999);

	return (cdb_->getIndex(efs,d)<<2)|d;
	
}

Index SkinMesh::growPt3_(int vi,double alpha)
{
	Vector3 p = parray_[vi];
	Vector3 newp,newp2,pln,c,trin;
	Vector3 o = cdb_->getCenter(vmc_[vi]);
	Vector3 op = p-o;
	double lsqr = op.lengthsqr();
	VIndex idx = vmc_[vi]>>2;
	Assert((vmc_[vi]&3)==3);
	IterStack<OrTri> fs;
	OrTri ef;
	VIndex vi1,vi2,vi3;
	Vector3 v1,v2,v3;
	Index nmc=0;


	newp = o +  op*sqrt(0.5)*sqrt(squaredSize(3,idx,alpha))/sqrt(lsqr);
	if(cdb_->inMixedCell(newp.x,cdb_->repTri(idx,3),3))
	{
		parray_[vi] = newp;
		return vmc_[vi];
	}

	ef = cdb_->repTri(vmc_[vi]);

	fs.push(ef);
	fs.push(cdb_->fnext(ef).sym());
	fs.push(cdb_->fnext(ef.enext()).sym());
	fs.push(cdb_->fnext(ef.enext2()).sym());
	while(!fs.empty())
	{
		ef = fs.pop();
		vi1 = cdb_->org(ef);
		vi2 = cdb_->dest(ef);
		vi3 = cdb_->dest(ef.enext());
		c = (cdb_->getCenter(ef,2)+o)/2;
		v1 = cdb_->getVertex(vi1);
		v2 = cdb_->getVertex(vi2);
		v3 = cdb_->getVertex(vi3);
		pln = -cross_prod(v2-v1,v3-v1);

		if(dot_prod(op,pln)<0)
		{
			newp2=lineIntersectPlane(p,op,pln,dot_prod(pln,c));
			if( (newp2-p).lengthsqr()<(newp-p).lengthsqr())
			{
				newp = newp2;
				nmc = (cdb_->getIndex(ef,2)<<2)|2;
			}
		}

	}
	if(nmc==0)
	{
		debugPts_.push(newp);
		return vmc_[vi];
	}
	parray_[vi] = newp;
	return nmc;

}

Bool SkinMesh::shrinkPt_(int vi,double alpha)
{
	Index lastmc = vmc_[vi];
	Index newmc = vmc_[vi];
	Index startmc = vmc_[vi];

	Bool done=FALSE; // just a moron flag for now

	while(!done)
	{
		switch (vmc_[vi]&3) {
		case 0:
			newmc = shrinkPt0_(vi,alpha);
			if(newmc==lastmc)
				done = TRUE;
			break;
		case 1:
			newmc = shrinkPt1_(vi,alpha);
			if(newmc==vmc_[vi])
				done = TRUE;
			break;
		case 2:
			newmc = shrinkPt2_(vi,alpha);
			if(newmc==vmc_[vi])
				done = TRUE;
			break;
		case 3:
			newmc = shrinkPt3_(vi,alpha);
			if(newmc==vmc_[vi])
				done = TRUE;
			break;
		default:
			Assert(0);
			break;
		}
		lastmc = vmc_[vi];
		vmc_[vi] = newmc;
	}


	if(startmc==vmc_[vi])
		if(( (startmc&3)==0 )|| ((startmc&3)==3))
			return TRUE;
	return FALSE;
}

Index SkinMesh::shrinkPt0_(int vi,double alpha)
{
	Vector3 p = parray_[vi];
	Vector3 np,np2,pln;
	Vector3 o = cdb_->getCenter(vmc_[vi]);
	double lsqr = (p-o).lengthsqr();
	VIndex idx = vmc_[vi]>>2;
	Assert((vmc_[vi]&3)==0);
	IterStack<OrTri> es;
	OrTri ef;
	VIndex vi2;
	Index nmc = -999;


	np = o +  (p-o)*sqrt(0.5)*sqrt(squaredSize(0,idx,alpha))/sqrt(lsqr);
	if(cdb_->inMixedCell(np.x,cdb_->repTri(idx,0),0))
	{
		parray_[vi] = np;
		return vmc_[vi];
	}
	else
	{
		cdb_->edgeStar(idx,&es);
		while(!es.empty())
		{
			ef = es.pop();
			vi2 = cdb_->dest(ef);
			pln = cdb_->getVertex(vi2)-o;
			if(dot_prod(p-o,pln)<0)
			{
				np2=lineIntersectPlane(p,p-o,pln,dot_prod(pln,(o+cdb_->getCenter(ef,1))/2));
				if(dot_prod(p-o,np2-o)>0)
					if( (np2-o).lengthsqr()>(np-o).lengthsqr())
					{
						np = np2;
						nmc = (cdb_->getIndex(ef,1)<<2)|1;
					}
			}
		}
		if(nmc!=-999)
			parray_[vi] = np;
	}

	if(nmc==-999)
	{
//		parray_[vi] = o +  (p-o)*sqrt(0.5)*sqrt(squaredSize(0,idx,alpha))/sqrt(lsqr);
		debugPts2_.push(parray_[vi]);
		return vmc_[vi];
	}
	return nmc;

/*
	Vector3 op = parray_[vi];
	Vector3 np,np2,pn;
	Vector3 c = cdb_->getCenter(vmc_[vi]);
	double lsqr = (op-c).lengthsqr();
	VIndex idx = vmc_[vi]>>2;
	Assert((vmc_[vi]&3)==0);
	IterStack<OrTri> es;
	OrTri ef;
	VIndex vi2;
	Index nmc;


	np = c +  (op-c)*sqrt(0.5)*sqrt(squaredSize(0,idx,alpha))/sqrt(lsqr);
	if(cdb_->inMixedCell(np.x,cdb_->repTri(idx,0),0))
	{
		parray_[vi] = np;
		return vmc_[vi];
	}
	else {
		Assert(0);
		return 0;
	}
*/
}

Index SkinMesh::shrinkPt1_(int vi,double alpha)
{
	Index mc = vmc_[vi];
	Assert((mc&3)==1);
	OrTri ef = cdb_->repTri(mc);
	OrTri ef2,efs;
	VIndex vi1,vi2;
	Vector3 ne,o,p,op,c,oc,odash;
	Vector3 pp,v3,vp[2],pn=pointNormal_(vi);
	Vector3 p0;
	double lsqr,tdash;
	double rsqr=squaredSize(1,mc>>2,alpha)/2;
	double x1,x2,x1x2;
	Vector3 pnew;
	Dim d = -999;

	vi1 = cdb_->org(ef);
	vi2 = cdb_->dest(ef);

	p  = parray_[vi];
	vp[0] = cdb_->getVertex(vi1);
	vp[1] = cdb_->getVertex(vi2);
	ne = vp[1]-vp[0];
	o  = cdb_->getCenter(mc);
	op = p-o;
	ne.normalize();  // necessary?

	if(dot_prod(ne,pn)<0)
	{   
		p0 = (o+vp[1])/2;
		ne = -ne;
	} else
	{
		p0 = (o+vp[0])/2;
		ef = ef.sym();
	}
	
	Matrix33 m = AlignWithYAxis(ne,op),mi;

	Assert(dot_prod(ne,op)<0);

	pp = op*m;
	x1x2 = pp.x[0]*pp.x[1];
	Assert(x1x2<0);
	x2 = -sqrt( (rsqr+sqrt(rsqr*rsqr+4*x1x2*x1x2)) / 2);
	x1 = x1x2/x2;
	mi = m.Inverse();
	pnew = Vector3(x1,x2,0)*mi + o;
	if(cdb_->inMixedCell(pnew.x,ef,1))
	{
		parray_[vi] = pnew;
		return vmc_[vi];
	}


	lsqr = x1*x1;


	ef2 = ef;

	do {
		c = (cdb_->getCenter(ef2,2)+o)/2;
		v3 = cdb_->getVertex(cdb_->dest(ef2.enext()));
		oc = c-o;
		if(dot_prod(oc,v3-o)<0)
			oc = -oc;
		
		if(dot_prod(oc,op)<0)
		{

			odash = intersect3Planes(ne,dot_prod(ne,o),
									 cross_prod(ne,op),dot_prod(cross_prod(ne,op),p),
									 oc,dot_prod(oc,c));
			if(dot_prod(odash-o,op)>0)
				if(lsqr< (odash-o).lengthsqr())
				{
					efs = ef2;
					d = 2;
					lsqr = (odash-o).lengthsqr();
				}
		}
		ef2 = cdb_->fnext(ef2);
	}  while (ef2.index()!=ef.index());


//	if(dot_prod(pn,p0-o)>0)

	{
		if(lsqr< x1x2*x1x2/(p0-o).lengthsqr())
		{
			lsqr = x1x2*x1x2/(p0-o).lengthsqr();
			d = 0;
			efs = ef.sym();			
		}
	}

	Assert((d==0)||(d==2));
	tdash = sqrt(lsqr);
	parray_[vi] = Vector3(tdash,x1x2/tdash,0)*mi + o;
	return cdb_->getIndex(efs,d)<<2|d;

/*
	Index mc = vmc_[vi];
	Assert((mc&3)==1);
	OrTri ef = cdb_->repTri(mc>>2,1);
	OrTri ef2,efs;
	VIndex vi1,vi2;
	Vector3 ne,o,p,op,odash,c,oc;
	Vector3 pp,v3,vp[2],pn=pointNormal_(vi);
	double lsqr,tdash,hsqr;
	double rsqr=squaredSize(1,mc>>2,alpha)/2;
	double x1,x2,x1x2;
	Vector3 pnew;
	int i;
	Dim d = -999;

	vi1 = cdb_->org(ef);
	vi2 = cdb_->dest(ef);

	p  = parray_[vi];
	vp[0] = cdb_->getVertex(vi1);
	vp[1] = cdb_->getVertex(vi2);
	ne = vp[0]-vp[1];
	o  = cdb_->getCenter(mc);
	op = p-o;
	ne.normalize();  // necessary?

	if(dot_prod(ne,pointNormal_(vi))<0)
		ne = -ne;
	
	Matrix33 m = AlignWithYAxis(ne,op),mi;

	if(dot_prod(ne,op)<0)
	{
		pp = op*m;
		x1x2 = pp.x[0]*pp.x[1];
		x2 = -sqrt( (rsqr+sqrt(rsqr*rsqr+4*x1x2*x1x2)) / 2);
		x1 = x1x2/x2;
		mi = m.Inverse();
		pnew = Vector3(x1,x2,0)*mi + o;
		if(cdb_->inMixedCell(pnew.x,ef,1))
		{
			parray_[vi] = pnew;
			return vmc_[vi];
		}

		lsqr = x1*x1;

		ef2 = ef;
		for(i=0;i<2;i++)
		{
			c = (vp[i]+o)/2;
			oc = vp[i]-vp[i?0:1];
			hsqr = (o-c).lengthsqr();

			if(((dot_prod(oc,pn)>0)&&(alpha>alpha_)) ||
			   ((dot_prod(oc,pn)<0)&&(alpha<alpha_)) )
			{
//				if((lsqr>(x1x2*x1x2)/hsqr)&&(alpha<alpha_))
				{
					efs = i ? ef.sym() : ef;
					d = 0;
					lsqr = (x1x2*x1x2)/hsqr;
				}
			}
		}

		Assert(d==0);
		tdash = sqrt(lsqr);
		parray_[vi] = Vector3(tdash,x1x2/tdash,0)*mi + o;
		return cdb_->getIndex(efs,d)<<2|d;
	} 

	Assert(0);
	return 0;
*/
}

Index SkinMesh::shrinkPt2_(int vi,double alpha)
{
	Index mc = vmc_[vi];
	Assert((mc&3)==2);
	OrTri ef = cdb_->repTri(mc);
	OrTri ef2,efs;
	Dim d=-999;
	VIndex vi1,vi2,vi3;
	Vector3 ne,o,p,op,odash,c,oc,pln;
	Vector3 pp,v3,ptn=pointNormal_(vi);
	double hsqr,tdash;
	double rsqr=-squaredSize(2,mc>>2,alpha)/2;
	double x1,x2,x1x2;
	Vector3 pnew;


	vi1 = cdb_->org(ef);
	vi2 = cdb_->dest(ef);
	vi3 = cdb_->dest(ef.enext());

	p  = parray_[vi];
	ne = cross_prod(cdb_->getVertex(vi2)-cdb_->getVertex(vi1),cdb_->getVertex(vi3)-cdb_->getVertex(vi1));
	o  = cdb_->getCenter(mc);
	op = p-o;
	ne.normalize();  // necessary?
	if(dot_prod(ne,ptn)>0)
		ne = -ne;
	
	Matrix33 m = AlignWithYAxis(ne,op),mi;

	Assert(dot_prod(ne,op)<0);

	pp = op*m;
	x1x2 = pp.x[0]*pp.x[1];
	x2 = -sqrt( (rsqr+sqrt(rsqr*rsqr+4*x1x2*x1x2)) / 2);
	x1 = x1x2/x2;
	Assert(x1x2<0);
	Assert(x1>0);
	mi = m.Inverse();
	pnew = Vector3(x1,x2,0)*mi + o;
	if(cdb_->inMixedCell(pnew.x,ef,2))
	{
		parray_[vi] = pnew;
		return vmc_[vi];
	}

	hsqr = x2*x2;

	ef2 = ef;

	do {
		if(cdb_->getIndex(ef2,3))  // if exist a tetrahedron coface
		{
			c = (cdb_->getCenter(ef2,3)+o)/2;
			v3 = cdb_->getVertex(cdb_->dest(cdb_->fnext(ef2).enext()));
			oc = c-o;

			if(dot_prod(oc,v3-o)>0)
				pln = oc;
			else
				pln = -oc;
/*
			if(dot_prod(oc,ptn)<0)
				if(dot_prod(oc,op)>0)
*/
			if(dot_prod(oc,op)>0)
				if(dot_prod(ptn,pln)<0)
			{
//				Assert(dot_prod(oc,ptn)<0);
				if(oc.lengthsqr()>hsqr)
				{
					efs = ef2;
					d = 3;
					hsqr = oc.lengthsqr();
				}
			}
		}
		ef2 = ef2.sym();
	}  while (ef2.version()!=ef.version());


	for(int i=0;i<3;i++)
	{
		ef2 = ef2.enext();
		c = (cdb_->getCenter(ef2,1)+o)/2;
		v3 = cdb_->getVertex(cdb_->dest(ef2.enext()));
		oc = c-o;
//		if(dot_prod(oc,v3-cdb_->getVertex(cdb_->org(ef2)))<0) // not hidden
		{
			if(dot_prod(oc,v3-cdb_->getVertex(cdb_->org(ef2)))<0)
				pln = oc;
			else
				pln = -oc;
			if(dot_prod(ptn,pln)<0) 
			{
				odash = intersect3Planes(pln,dot_prod(pln,c),ne,dot_prod(ne,o),
										 cross_prod(ptn,ne),dot_prod(cross_prod(ptn,ne),p));
				if(x1x2*x1x2/(odash-o).lengthsqr()>hsqr)
				{
					efs = ef2;
					d = 1;
					hsqr = x1x2*x1x2/(odash-o).lengthsqr();
				}
			}
		}
	}

	tdash = -x1x2/sqrt(hsqr);
	parray_[vi] = Vector3(tdash,x1x2/tdash,0)*mi + o;
	Assert(d!=-999);

	return (cdb_->getIndex(efs,d)<<2)|d;

/*
	Index mc = vmc_[vi];
	Assert((mc&3)==2);
	OrTri ef = cdb_->repTri(mc);
	OrTri ef2,efs;
	Dim d=-999;
	VIndex vi1,vi2,vi3;
	Vector3 ne,o,p,op,odash,c,oc;
	Vector3 pp,v3,pn=pointNormal_(vi);
	double hsqr,tdash,lsqr;
	double rsqr=-squaredSize(2,mc>>2,alpha)/2;
	double x1,x2,x1x2;
	Vector3 pnew;


	vi1 = cdb_->org(ef);
	vi2 = cdb_->dest(ef);
	vi3 = cdb_->dest(ef.enext());

	p  = parray_[vi];
	ne = cross_prod(cdb_->getVertex(vi2)-cdb_->getVertex(vi1),cdb_->getVertex(vi3)-cdb_->getVertex(vi1));
	o  = cdb_->getCenter(mc);
	op = p-o;
	ne.normalize();  // necessary?
	if(dot_prod(ne,pointNormal_(vi))>0)
		ne = -ne;
	
	Matrix33 m = AlignWithYAxis(ne,op),mi;

	if(dot_prod(ne,op)<0)
	{
		pp = op*m;
		x1x2 = pp.x[0]*pp.x[1];
		x2 = -sqrt( (rsqr+sqrt(rsqr*rsqr+4*x1x2*x1x2)) / 2);
		x1 = x1x2/x2;
		Assert(x1>0);
		mi = m.Inverse();
		pnew = Vector3(x1,x2,0)*mi + o;
		if(cdb_->inMixedCell(pnew.x,ef,2))
		{
			parray_[vi] = pnew;
			return vmc_[vi];
		}

		hsqr = x2*x2;

		ef2 = ef;

		ef2 = ef;
		do {
			c = (cdb_->getCenter(ef2,1)+o)/2;
			v3 = cdb_->getVertex(cdb_->dest(ef2.enext()));
			oc = c-o;
			if(dot_prod(oc,v3-o)>0)
				oc = -oc;
			if(dot_prod(oc,op)>0)
			{
				odash = intersect3Planes(ne,dot_prod(ne,o),
										 cross_prod(ne,op),dot_prod(cross_prod(ne,op),p),
										 oc,dot_prod(oc,c));
				lsqr = (odash-o).lengthsqr();
				if(hsqr < x1x2*x1x2/lsqr)
				{
					efs = ef2;
					d = 1;
					hsqr = x1x2*x1x2/lsqr;
				}
			}
			ef2 = cdb_->enext(ef2);
		}  while (ef2.version()!=ef.version());

		tdash = -x1x2/sqrt(hsqr);
		parray_[vi] = Vector3(tdash,x1x2/tdash,0)*mi + o;
		Assert(d>=0);
		return (cdb_->getIndex(efs,d)<<2)|d;
	} 

	Assert(0);
	return 0;
*/
}

Index SkinMesh::shrinkPt3_(int vi,double alpha)
{

	Vector3 p = parray_[vi];
	Vector3 newp,newp2,pln,c,trin;
	Vector3 o = cdb_->getCenter(vmc_[vi]);
	Vector3 op = p-o;
	double lsqr = op.lengthsqr();
	VIndex idx = vmc_[vi]>>2;
	Assert((vmc_[vi]&3)==3);
	IterStack<OrTri> fs;
	OrTri ef;
	VIndex vi1,vi2,vi3;
	Vector3 v1,v2,v3;
	Index nmc=0;


	newp = o +  op*sqrt(0.5)*sqrt(squaredSize(3,idx,alpha))/sqrt(lsqr);
	if(cdb_->inMixedCell(newp.x,cdb_->repTri(idx,3),3))
	{
		parray_[vi] = newp;
		return vmc_[vi];
	}

	ef = cdb_->repTri(vmc_[vi]);

	fs.push(ef);
	fs.push(cdb_->fnext(ef).sym());
	fs.push(cdb_->fnext(ef.enext()).sym());
	fs.push(cdb_->fnext(ef.enext2()).sym());
	while(!fs.empty())
	{
		ef = fs.pop();
		vi1 = cdb_->org(ef);
		vi2 = cdb_->dest(ef);
		vi3 = cdb_->dest(ef.enext());
		c = (cdb_->getCenter(ef,2)+o)/2;
		v1 = cdb_->getVertex(vi1);
		v2 = cdb_->getVertex(vi2);
		v3 = cdb_->getVertex(vi3);
		pln = -cross_prod(v2-v1,v3-v1);

		if(dot_prod(op,pln)>0)
		{
			newp2=lineIntersectPlane(p,op,pln,dot_prod(pln,c));
			if( (newp2-p).lengthsqr()<(newp-p).lengthsqr())
			{
				newp = newp2;
				nmc = (cdb_->getIndex(ef,2)<<2)|2;
			}
		}

	}
	if(nmc==0)
	{
		debugPts_.push(newp);
		return vmc_[vi];
	}
	parray_[vi] = newp;
	return nmc;

/*
	Vector3 p = parray_[vi];
	Vector3 np,np2,pn,c;
	Vector3 o = cdb_->getCenter(vmc_[vi]);
	Vector3 op = p-o;
	double lsqr = op.lengthsqr();
	VIndex idx = vmc_[vi]>>2;
	Assert((vmc_[vi]&3)==3);
	IterStack<OrTri> fs;
	OrTri ef;
	VIndex vi2;
	Index nmc=0;


	np = o +  op*sqrt(0.5)*sqrt(squaredSize(3,idx,alpha))/sqrt(lsqr);
	if(cdb_->inMixedCell(np.x,cdb_->repTri(idx,3),3))
	{
		parray_[vi] = np;
		return vmc_[vi];
	}
	else
	{
		OrTri ef = cdb_->repTri(vmc_[vi]);

		if(alpha<alpha_)
		{
			fs.push(ef);
			fs.push(cdb_->fnext(ef).sym());
			fs.push(cdb_->fnext(ef.enext()).sym());
			fs.push(cdb_->fnext(ef.enext2()).sym());
			while(!fs.empty())
			{
				ef = fs.pop();
				vi2 = cdb_->dest(cdb_->fnext(ef).enext());
				c = cdb_->getCenter(ef,2);
				pn = c - o;
				if(dot_prod(pn,c-cdb_->getVertex(vi2))<0)
					pn = -pn;

				if(dot_prod(op,pn)>0)
				{
					np2=lineIntersectPlane(op,np-op,pn,dot_prod(pn,(c+o)/2));
					if( (np2-op).lengthsqr()<(np-op).lengthsqr())
					{
						np = np2;
						nmc = (cdb_->getIndex(ef,2)<<2)|2;
					}
				}

			}
			if(nmc==0)
			{
				debugPts_.push(np);
				return vmc_[vi];
			}
			parray_[vi] = np;
			return nmc;
		}
	}

	Assert(0);
	return nmc;
*/
}
