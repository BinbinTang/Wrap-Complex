#ifndef __RGI_FACEPOINT_H__
#define __RGI_FACEPOINT_H__

#include <basic/ortri.h>

class FacePoint
{
public:
	OrTri abc;
	double point[3];
	FacePoint(void);
	FacePoint(OrTri ef, const double pt[3]);
	FacePoint(OrTri ef, double x, double y, double z);
	FacePoint(const FacePoint& fp);
	FacePoint& operator=(const FacePoint &fp);
	int operator==(const FacePoint &fp) const;
	int valid(void);
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
};

inline ostream &operator<< (ostream& s, FacePoint& fp) 
{ 
  return s << fp.abc << " " << fp.point[0] 
           << " " << fp.point[1] << " " << fp.point[2]; 
}


inline FacePoint::
FacePoint(void) : abc(0,0)
{
	point[0] = point[1] = point[2] = 0;
}

inline FacePoint::
FacePoint(OrTri ef, const double pt[3]) : abc(ef)
{ 
    point[0] = pt[0], point[1] = pt[1], point[2] = pt[2];
}

inline FacePoint::
FacePoint(OrTri ef, double x, double y, double z) : abc(ef)
{ 
    point[0] = x;
    point[1] = y;
    point[2] = z;
}

inline FacePoint::
FacePoint(const FacePoint& fp)
: abc(fp.abc)
{
  point[0] = fp.point[0], point[1] = fp.point[1], point[2] = fp.point[2];
}

inline FacePoint& FacePoint::operator=(const FacePoint &fp)
{
		abc = fp.abc;
    point[0] = fp.point[0], point[1] = fp.point[1], point[2] = fp.point[2];
    return (*this);
}

inline int FacePoint::operator==(const FacePoint &fp) const
{
		OrTri ef = fp.abc;
    return (abc.index() == ef.index() && 
      point[0] == fp.point[0] && 
      point[1] == fp.point[1] && 
      point[2] == fp.point[2]);
}

inline int FacePoint::valid()
{
		return (abc.index() != 0);
}

inline Bool FacePoint::binInit (BinInput& bi)
{
  return abc.binInit(bi) && bi.readDoubles(point, 3);
}

inline void FacePoint::binDump (BinOutput& bo)
{
  abc.binDump(bo);
  bo.writeDoubles(point, 3);
}

class FacePointStack : public IterStackDumpable <FacePoint>
{
public:
	FacePointStack (int size = 20) : IterStackDumpable<FacePoint>(size) { }
};

class FacePointArray : public FreeArrayDumpable<FacePoint>
{
public:
  FacePointArray (int size = 20) : FreeArrayDumpable<FacePoint>(size) { }
  FacePointArray (const FacePointArray& arr) : FreeArrayDumpable<FacePoint>(arr) { }
  const FacePointArray &operator=(const FacePointArray &arr)
  { FreeArrayDumpable<FacePoint>::operator=(arr); return *this; }
};

#endif //FACEPOINT_H
