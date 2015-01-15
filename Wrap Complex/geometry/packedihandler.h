#ifndef __PACKEDIHANDLER_H__
#define __PACKEDIHANDLER_H__
#include "ihandler.h"


class PackedIndexHandler : public IndexHandler {

public:
	PackedIndexHandler(Complex *comp, BitVector * triangles = NULL,
        unsigned int dimFlag = INDEX_V | INDEX_E | INDEX_F | INDEX_T,
        Bool closed = FALSE, Bool isRandom = TRUE);
	int init(void);
	
	inline EIndex ix_v(VIndex,VIndex);
	inline TIndex ix_v(VIndex,VIndex,VIndex,VIndex);

	int nEdge() { return nEdge_;};
	int nTetra() { return nTetra_;};
	~PackedIndexHandler();

protected:
	IMap up2pEdge_; // unpacked to packed edges mapping
	IMap p2upEdge_; // packed to unpacked edges mapping
	IMap up2pTetra_; // unpacked to packed tetra mapping
	IMap p2upTetra_; // packed to unpacked tetra mapping

	int nEdge_;
	int nTetra_;
};

EIndex PackedIndexHandler::ix_v(VIndex a,VIndex b)
{
	Assert(p2upEdge_[up2pEdge_[IndexHandler::ix_v(a,b)]]
		== IndexHandler::ix_v(a,b));
	return up2pEdge_[IndexHandler::ix_v(a,b)];
}

TIndex PackedIndexHandler::ix_v(VIndex a,VIndex b,VIndex c,VIndex d)
{
	Assert(p2upTetra_[up2pTetra_[IndexHandler::ix_v(a,b,c,d)]]
		== IndexHandler::ix_v(a,b,c,d));
	return up2pTetra_[IndexHandler::ix_v(a,b,c,d)];
}

#endif