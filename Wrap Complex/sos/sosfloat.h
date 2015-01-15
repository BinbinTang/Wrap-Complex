#ifndef __RGI_SOSFLOAT_H__
#define __RGI_SOSFLOAT_H__

#include "sos/sosprim.h"
#include "li/lipoints.h"

class RealPArray;
class LI_Converter;

class SOS_FloatEvaluator : public SOS_PrimEvaluator
{
public:
  SOS_FloatEvaluator (LI_Converter& converter);
  Bool positive3 (RealPArray& rarr, VIndex i, VIndex j, VIndex k, VIndex l);
  Bool above4 (RealPArray& rarr, VIndex i, VIndex j, VIndex k, VIndex l, VIndex m);
  Bool inSphere (RealPArray& rarr, VIndex o, VIndex i, VIndex j, VIndex k, VIndex p);
  Bool inSphereP (RealPArray& rarr, VIndex o, VIndex i, VIndex j, VIndex k, VIndex p);
protected:
  LI_Converter& converter_;
  LI_Point li_, lj_, lk_, ll_, lo_;
};


#endif // __SOSFLOAT_H__