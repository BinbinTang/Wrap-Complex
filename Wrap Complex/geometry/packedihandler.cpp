#include "packedihandler.h"

PackedIndexHandler::PackedIndexHandler(Complex *comp, BitVector * triangles, 
									   unsigned int dimFlag, Bool closed, Bool isRandom):
IndexHandler (comp,triangles,dimFlag,closed,isRandom)
{

}

PackedIndexHandler::~PackedIndexHandler()
{
}

int PackedIndexHandler::init()
{
	if(!IndexHandler::init())
		return FALSE;

	IndexIter eii(*this,1); // remap edge

	nEdge_ = 0;
	for(eii.first();!eii.isDone();eii.next())
	{
		nEdge_++;
		p2upEdge_[nEdge_] = eii.curIndex();
		up2pEdge_[eii.curIndex()] = nEdge_;
	}

	IndexIter tii(*this,3); // remap tetra

	nTetra_ = 0;
	for(tii.first();!tii.isDone();tii.next())
	{
		nTetra_++;
		p2upTetra_[nTetra_] = tii.curIndex();
		up2pTetra_[tii.curIndex()] = nTetra_;
	}

	return TRUE;
}

