#include "basic.h"
#include "data.h"
#include "rectsel.h"
#include "bitvector.h"

Data::
~Data (void)
{

}
 
//-----------------------------------------------
// Event Data
//-----------------------------------------------

EventData::
EventData (int i1, int i2, int i3, int i4)
{
    i1_=i1;
    i2_=i2;
    i3_=i3;
    i4_=i4;
}

EventData::
~EventData ()
{
}

//-----------------------------------------------
// Selection Data
//-----------------------------------------------
 
SelectionData::
SelectionData (int mark, IndexStack *ids, RectSelection * rectsel) 
		: mark_(mark), ids_(ids), bv_(NULL), rectsel_ (rectsel) 
{
}

SelectionData::
SelectionData (int mark, BitVector *bv, RectSelection * rectsel) 
		: mark_(mark), ids_(NULL), bv_(bv), rectsel_ (rectsel) 
{
}

SelectionData::
~SelectionData (void) 
{ 
	delete rectsel_; 
}

//-----------------------------------------------
// CString Data
//-----------------------------------------------
 
CStringData:: 
CStringData (const RgiCString &c) { c_ = c; } 
 
CStringData:: 
~CStringData () { } 

//-----------------------------------------------
// String Data
//-----------------------------------------------
 
StringData:: 
StringData (const RgiString &c) { c_ = c; } 
 
StringData:: 
~StringData () { } 

//-----------------------------------------------
// File Open Data
//-----------------------------------------------
 
FileOpenData:: 
FileOpenData (void) 
: fileType_(0),
  sample_(100.0f),
  tolerance_(0.0f),
  unitType_(UNKNOWN),
  ptTemplate_(None),
  openAsSurface_(0),
  precisionType_(FromFile),
  useKDTree_(TRUE)
{ 
	
} 
 
FileOpenData:: 
FileOpenData (const char *filename, int fileType, float s, float t,  
		PtTemplate pttmp, const char * ptTmpName) 
    : unitType_(UNKNOWN)
{ 
	Assert (filename); 
	filename_ = filename; 
	fileType_ = fileType; 
	sample_ = s; 
	tolerance_ = t; 
	ptTemplate_ = pttmp; 
 
	templateName_ = ptTmpName; 
	openAsSurface_ = 0;
	useKDTree_ = TRUE;
} 
 
FileOpenData:: 
~FileOpenData () 
{ 
} 
 
FileOpenData:: 
FileOpenData (const FileOpenData & fd) 
{ 
  filename_ = fd.filename_; 
  fileType_ = fd.fileType_; 
  sample_ = fd.sample_; 
  tolerance_ = fd.tolerance_; 
  ptTemplate_ = fd.ptTemplate_; 
  templateName_ = fd.templateName_; 
  openAsSurface_ = fd.openAsSurface_;
  unitType_ = fd.unitType_;
  precisionType_ = fd.precisionType_;
  useKDTree_ = fd.useKDTree_;
} 
 
const FileOpenData & FileOpenData:: 
operator= (const FileOpenData &other) 
{ 
  if (this != &other) 
  { 
	  filename_ = other.filename_; 
	  fileType_ = other.fileType_; 
	  sample_ = other.sample_; 
	  tolerance_ = other.tolerance_; 
	  ptTemplate_ = other.ptTemplate_; 
	  openAsSurface_ = other.openAsSurface_;
	  unitType_ = other.unitType_;
	  templateName_ = other.templateName_; 
	  precisionType_ = other.precisionType_;
	  useKDTree_ = other.useKDTree_;
    } 
  return (*this); 
} 
 
void FileOpenData:: 
setFileName (const char *f) 
{ 
  Assert (f); 
  filename_ = f; 
} 
 
void FileOpenData:: 
setTemplateName (const char *f) 
{ 
  Assert (f); 
  templateName_ = f; 
} 
 
//-----------------------------------------------
// File Save Data
//-----------------------------------------------
 
FileSaveData:: 
FileSaveData () 
{ 
  fileType_ = 0;
  scale_ = 1.0;
  resolution_ = 10;
	resetSettings();
	
} 

void FileSaveData::
resetSettings (void)
{
  for (int i=0; i<FileSaveData::Max; i++) 
  {
		settings_[i] = 0;
    sensitive_[i] = 0;
  }
  setScaleFactor (1.0);
  setPatchResolution (10);
}
 
FileSaveData:: 
FileSaveData (const char *filename, int fileType, const int *settings,
	      double scale, int resolution) 
{ 
	filename_ = filename; 

	fileType_ = fileType;; 
	for (int i = 0; i<FileSaveData::Max; i++) 
		settings_[i] = settings[i];
	scale_ = scale;
	resolution_ = resolution;
} 
 
FileSaveData:: 
~FileSaveData () 
{ 
} 
 
FileSaveData:: 
FileSaveData (const FileSaveData & fd) 
{ 
	filename_ = fd.filename_; 

  fileType_ = fd.fileType_; 
  for (int i = 0; i<FileSaveData::Max; i++) 
		settings_[i] = fd.settings_[i];
  scale_ = fd.scale_;
  resolution_ = fd.resolution_;
} 
 
const FileSaveData & FileSaveData:: 
operator= (const FileSaveData &other) 
{ 
	if (this != &other) 
	{ 
		filename_ = other.filename_; 

		fileType_ = other.fileType_; 
		for (int i = 0; i<FileSaveData::Max; i++) 
			settings_[i] = other.settings_[i];
		scale_ = other.scale_;
		resolution_ = other.resolution_;
	} 
	return (*this); 
} 
 
void FileSaveData:: 
setFileName (const char *f) 
{ 
	filename_ = f; 
}

//-----------------------------------------------
// Relax Data
//-----------------------------------------------

void RelaxData::
reset ()
{
	iteration = 1;
	fixBoundary= 0;
}

int RelaxData::
operator== ( const RelaxData &other) const
{
	return (
		(iteration== other.iteration) &&
		(fixBoundary == other.fixBoundary));
}

int RelaxData::
setIteration (const RgiString &s)
{
	return  strToIteration (s, iteration);
}

int RelaxData::
strToIteration (const RgiString &s, int &iteration)
{
	BOOL success;
	int tmp = s.toInt (&success);
	if (! success || (tmp<0))
		return 0;
	iteration = tmp;
	return 1;
}

//-----------------------------------------------
// Decimate Data
//-----------------------------------------------

double theDecimateIsocelesFactor = 0.0;

void DecimateData::
reset (double err, int cnt)
{
	maxErr = err;	
	triCnt = cnt;
	reduceTo = 100;
	fixBoundary = 0;
	tangentLamda = 1.0;
	midPlaneLamda = 0.0;
	centroidLamda = 0.0;
	funnelLamda = 0.0;
	isocelesLamda = theDecimateIsocelesFactor;
}

int DecimateData::
setMaxErr(const RgiString &s)
{
	return  strToMaxErr (s, maxErr);
}

int DecimateData::
strToMaxErr(const RgiString &s, double &err)
{
	BOOL success;
	double tmp = s.toDouble (&success);
	if (! success || (tmp<0))
		return 0;
	err = tmp;
	return 1;
}

int DecimateData::
setTriCnt(const RgiString &s)
{
	return  strToTriCnt (s, triCnt);
}

int DecimateData::
strToTriCnt(const RgiString &s, int&cnt)
{
	BOOL success;
	int tmp = s.toInt (&success);
	if (! success || (tmp<0))
		return 0;
	cnt = tmp;
	return 1;
}

int DecimateData::
setReduceTo(const RgiString &s)
{
	return  strToReduceTo (s, reduceTo);
}

int DecimateData::
strToReduceTo(const RgiString &s, double &v)
{
	BOOL success;
	double tmp = s.toDouble (&success);
	if (! success || (tmp<0) || (tmp>100))
		return 0;
	v= tmp;
	return 1;
}

int DecimateData::
setTangentLamda(const RgiString &s)
{
	return strToTangentLamda(s, tangentLamda);
}

int DecimateData::
strToTangentLamda(const RgiString &s, double &l)
{
	BOOL success;
	double tmp = s.toDouble(&success);
	if (!success || tmp < 0.0) {
		return 0;
	}

	l = tmp;
	return 1;
}

int DecimateData::
setMidPlaneLamda(const RgiString &s)
{
	return strToMidPlaneLamda(s, midPlaneLamda);
}

int DecimateData::
strToMidPlaneLamda(const RgiString &s, double &l)
{
	BOOL success;
	double tmp = s.toDouble(&success);
	if (!success || tmp < 0.0) {
		return 0;
	}

	l = tmp;
	return 1;
}

int DecimateData::
setCentroidLamda(const RgiString &s)
{
	return strToCentroidLamda(s, centroidLamda);
}

int DecimateData::
strToCentroidLamda(const RgiString &s, double &l)
{
	BOOL success;
	double tmp = s.toDouble(&success);
	if (!success || tmp < 0.0) {
		return 0;
	}

	l = tmp;
	return 1;
}

int DecimateData::
setFunnelLamda(const RgiString &s)
{
	return strToFunnelLamda(s, funnelLamda);
}

int DecimateData::
strToFunnelLamda(const RgiString &s, double &l)
{
	BOOL success;
	double tmp = s.toDouble(&success);
	if (!success || tmp < 0.0) {
		return 0;
	}

	l = tmp;
	return 1;
}

int DecimateData::
setIsocelesLamda(const RgiString &s)
{
	return strToIsocelesLamda(s, isocelesLamda);
}

int DecimateData::
strToIsocelesLamda(const RgiString &s, double &l)
{
	BOOL success;
	double tmp = s.toDouble(&success);
	if (!success || tmp < 0.0) {
		return 0;
	}

	l = tmp;
	return 1;
}

int DecimateData::
operator== (const DecimateData & other) const
{
	return (
		(fabs(maxErr - other.maxErr)<=0.00000001) &&
		(triCnt == other.triCnt) &&
		(fabs(reduceTo - other.reduceTo) <=0.00000001) &&
		(fixBoundary == other.fixBoundary) &&
		(tangentLamda == other.tangentLamda) &&
		(midPlaneLamda == other.midPlaneLamda) &&
		(centroidLamda == other.centroidLamda) &&
		(funnelLamda == other.funnelLamda) &&
		(isocelesLamda == other.isocelesLamda));
}

//-----------------------------------------------
// OneVal Data
//-----------------------------------------------

int OneValData::
setVal (const RgiString &s)
{
	return strToVal (s, val);
}

int SampleData::
strToVal (const RgiString &s, double &v)
{
	BOOL success;
	double tmp = s.toDouble (&success);
	if (!success || (tmp<0) || (tmp>100))
		return 0;
	v= tmp;
	return 1;
}

int AutoPlotDistData::
strToVal (const RgiString &s, double &v)
{
	BOOL success;
	double tmp = s.toDouble (&success);
	if (!success || (tmp<0))
		return 0;
	v= tmp;
	return 1;
}

int SetDepthData::
strToVal (const RgiString &s, double &v)
{
	BOOL success;
	double tmp = s.toDouble (&success);
	if (!success || (tmp<0) || (tmp > 100))
		return 0;
	v= tmp;
	return 1;
}

int AddPointPlaneData::
strToVal (const RgiString &s, double &v)
{
	BOOL success;
	double tmp = s.toDouble (&success);
	if (!success || (tmp<0) || (tmp > 100))
		return 0;
	v= tmp;
	return 1;
}

//-----------------------------------------------
// Thicken Data
//-----------------------------------------------

void ThickenData::
reset (double v)
{
	val= v;
}

int ThickenData::
operator== (const ThickenData &other) const
{
	// for some reason the two value are slightly different
	// even though they should be exactly the same.  it could
	// sscanf does not convert correctly from string to double
	return ((fabs(val- other.val) <=0.00000001));
}

int ThickenData::
strToVal (const RgiString &s, double &v)
{
	BOOL success;
	double tmp = s.toDouble (&success);
	if (! success)
		return 0;

	v= tmp;
	return 1;
}

//-----------------------------------------------
// Shelling Data
//-----------------------------------------------

ShellingData::
ShellingData (void)
{
  val = 0;
  biDir = 0;
}

void ShellingData::
reset (double v, int isBiDir)
{
	val = v, biDir = isBiDir;
}

int ShellingData::
operator== (const ShellingData &other) const
{
	// for some reason the two value are slightly different
	// even though they should be exactly the same.  it could
	// sscanf does not convert correctly from string to double
	return (biDir==other.biDir && (fabs(val- other.val) <=0.00000001));
}

int ShellingData::
strToVal (const RgiString &s, double &v)
{
	BOOL success;
	double tmp = s.toDouble (&success);
	if (! success)
		return 0;
	v= tmp;
	return 1;
}

//-----------------------------------------------
// AddMovePt Data
//-----------------------------------------------

const char *const AddMovePtData::BadFormatEmsg = 
	"Magnitude must be a real number.";

void AddMovePtData::
reset (double v)
{
	val = v;
}

int AddMovePtData::
operator== (const AddMovePtData &other) const
{
	// for some reason the two value are slightly different
	// even though they should be exactly the same.  it could
	// sscanf does not convert correctly from string to double
	return ((fabs(val- other.val) <=0.00000001));
}

int AddMovePtData::
strToVal (const RgiString &s, double &v)
{
	BOOL success;
	double tmp = s.toDouble (&success);
	if (! success)
		return 0;
	v= tmp;
	return 1;
}

//-----------------------------------------------
// OneValTypedData 
//-----------------------------------------------

OneValTypedData::
OneValTypedData(char c) 
{ 
	type = CharData; 
	value.ch = c; 
}

OneValTypedData::
OneValTypedData(char* s) 
{ 
	type = StringData; 
	value.str = new char[32]; 
	//strncpy(value.str,s,31); 
	strncpy_s(value.str,32,s,31); 
	value.str[31] = '\0'; 
}

OneValTypedData::
OneValTypedData(int n) 
{ 
	type = IntData; 
	value.num = n; 
}

OneValTypedData::
OneValTypedData(double d) 
{ 
	type = DoubleData; 
	value.dbl = d; 
}

OneValTypedData::
~OneValTypedData() 
{ 
	if (type == StringData) 
		delete []value.str; 
}

void OneValTypedData::
Increment() 
{ 
	switch (type)
	{
		case CharData: 
			value.ch++;
			break;
		case IntData:
			value.num++;
			break;
		case DoubleData:
			value.dbl++;
			break;
		default:
			break;
	}
}

void OneValTypedData::
Decrement() 
{ 
	switch (type)
	{
		case CharData: 
			value.ch--;
			break;
		case IntData:
			value.num--;
			break;
		case DoubleData:
			value.dbl--;
			break;
		default:
			break;
	}
}

char* OneValTypedData::
GetValue()
{
	ostringstream oss;
	switch (type)
	{
		case CharData: 
			buf[0] = value.ch;
			buf[1] = '\0';
			break;
		case StringData:
			//strncpy(buf,value.str,31);
			strncpy_s(buf,32,value.str,31);
			buf[31] = '\0';
			break;
		case IntData:
			oss << value.num;
			//sprintf(buf,"%d",value.num);
			break;
		case DoubleData:
			oss << value.dbl;
			//sprintf(buf,"%g",value.dbl);
			break;
		default:
			break;
	}
	//return buf;
	return (char*) oss.str().c_str();
}

void OneValTypedData::
SetValue(const char *data)
{
	switch (type)
	{
		case CharData: 
			value.ch = data[0];
			break;
		case StringData:
			//strncpy(value.str,data,31); 
			strncpy_s(value.str, 32, data,31); 
			value.str[31] = '\0';				
			break;
		case IntData:
			value.num = atoi(data);
			break;
		case DoubleData:
			value.dbl = atof(data);
			break;
		default:
			break;
	}
}

FlexLMFeatureData::
FlexLMFeatureData (char * _feature, char * _exp_date, 
                    char * _code, HostIDKey _hostIDKey)
{
  if (_feature)
  {
    //strncpy(feature, _feature, FEATURE_SIZE);
    strncpy_s(feature, FEATURE_SIZE + 1, _feature, FEATURE_SIZE);
  }
  else
    BZERO(feature, char, FEATURE_SIZE+1);
  if (_exp_date)
  {
    //strncpy(exp_date, _exp_date, EXP_DATE_SIZE);
	strncpy_s(exp_date, EXP_DATE_SIZE + 1, _exp_date, EXP_DATE_SIZE);
  }
  else
    BZERO(exp_date, char, EXP_DATE_SIZE+1);
  if (_code)
  {
    //strncpy(code, _code, CODE_SIZE);
	strncpy_s(code, CODE_SIZE + 1, _code, CODE_SIZE);
  }
  else
    BZERO(code, char, CODE_SIZE+1);
  hostIDKey = _hostIDKey;
}

//---------------------------------------------------------------------------

Shape2DData::
Shape2DData (void)
: remapRankWeight(1.0f), remapEdgeR2Max(0.3f), 
  remapVertexR1Max(0.3f), remapVertexR2Max(0.3f),
  lineR2max(0.3f), pointR1max(0.3f),
  matchR2max(0.3f), matchDihedralAngleWeight(0.0f),
  matchNeighborWeight(10.0f), matchQuadAngleWeight(10.0f),
  alphaPath(0.4f), betaPath(0.0f), iterPath(80), 
	alphaPathEdit(0.1f), betaPathEdit(0.3f), iterPathEdit(150),
  clr2(0.4f), cleqsp(0.5f), clenergy(0.02f), clObserveRadius(0.02f),
  sampleSize(10),
  alphaPatch(0.1f), betaPatch(0.1f), iterPatch(20),
  gridUres(20), gridVres(20),
  alphaGrid(0.4f), betaGrid(0.4f), iterGrid(10),
  nurbRes(16), nurbEvalRes (20)
{

}

void Shape2DData::
resetState (void)
{
  gridUres = gridVres = 20;
	sampleSize = 10;
	nurbRes = 16;
  lineR2max = pointR1max = 0.3f;
}

void Shape2DData::setEditRelaxParams (float _alphaPathEdit, float _betaPathEdit, int _iterPathEdit)
{
  alphaPathEdit = _alphaPathEdit, betaPathEdit = _betaPathEdit, iterPathEdit = _iterPathEdit;
}

void Shape2DData::getEditRelaxParams (float &_alphaPathEdit, float &_betaPathEdit, int &_iterPathEdit)
{
  _alphaPathEdit = alphaPathEdit, _betaPathEdit = betaPathEdit, _iterPathEdit = iterPathEdit;
}

void Shape2DData::setRankParams (float _lineR2max, float _pointR1max)
{
  lineR2max = _lineR2max, pointR1max = _pointR1max;
}

void Shape2DData::getRankParams (float& _lineR2max, float& _pointR1max)
{
  _lineR2max = lineR2max, _pointR1max = pointR1max;
}

void Shape2DData::setMatchParams (float _matchR2max, float _matchDihedralAngleWeight, 
                                  float _matchNeighborWeight, float _matchQuadAngleWeight)
{
  matchR2max = _matchR2max, matchDihedralAngleWeight = _matchDihedralAngleWeight, 
    matchNeighborWeight = _matchNeighborWeight, matchQuadAngleWeight = _matchQuadAngleWeight;
}

void Shape2DData::getMatchParams (float& _matchR2max, float& _matchDihedralAngleWeight, 
                                  float& _matchNeighborWeight, float& _matchQuadAngleWeight)
{
  _matchR2max = matchR2max, _matchDihedralAngleWeight = matchDihedralAngleWeight;
  _matchNeighborWeight = matchNeighborWeight, _matchQuadAngleWeight = matchQuadAngleWeight;
}

void Shape2DData::
setCLRelaxParams ( float _alphaPath, float _betaPath, int _iterPath, 
                  float _clr2, float _cleqsp, float _clenergy, float _clObserveRadius)
{
  iterPath = _iterPath, clr2 = _clr2, 
  cleqsp = _cleqsp,
  clenergy = _clenergy, alphaPath = _alphaPath,
  betaPath = _betaPath;
  clObserveRadius = _clObserveRadius;
}

void Shape2DData::
getCLRelaxParams ( float& _alphaPath, float& _betaPath, int& _iterPath, 
                  float& _clr2, float& _cleqsp, float& _clenergy, float & _clObserveRadius)
{
  _iterPath = iterPath, _clr2 = clr2, 
  _cleqsp = cleqsp,
  _clenergy = clenergy, _alphaPath = alphaPath,
  _betaPath = betaPath;
  _clObserveRadius = clObserveRadius;
}

void Shape2DData::
setPatchParams (float _alphaPatch, float _betaPatch, int _iterPatch)
{
  alphaPatch = _alphaPatch, betaPatch = _betaPatch, iterPatch = _iterPatch;
}

void Shape2DData::
getPatchParams (float& _alphaPatch, float& _betaPatch, int& _iterPatch)
{
  _alphaPatch = alphaPatch, _betaPatch = betaPatch, _iterPatch = iterPatch;
}

void Shape2DData::
setGridParams (float _alphaGrid, float _betaGrid, int _iterGrid)
{
  alphaGrid = _alphaGrid, betaGrid = _betaGrid, iterGrid = _iterGrid;
}

void Shape2DData::
getGridParams (int& _gridUres, int& _gridVres, float& _alphaGrid, 
       float& _betaGrid, int& _iterGrid)
{
  _gridUres = gridUres, _gridVres = gridVres, _alphaGrid = alphaGrid,
  _betaGrid = betaGrid, _iterGrid = iterGrid;
}

void Shape2DData::
setNurbParams (int _nurbRes, int _nurbEvalRes)
{
  nurbRes = _nurbRes; nurbEvalRes = _nurbEvalRes;
}

void Shape2DData::
getNurbParams (int& _nurbRes, int& _nurbEvalRes)
{
  _nurbRes = nurbRes; _nurbEvalRes = nurbEvalRes;
}

void Shape2DData::
setRemapParams(float _remapRankWeight, float _remapEdgeR2Max, 
							 float _remapVertexR1Max, float _remapVertexR2Max)
{
	remapRankWeight = _remapRankWeight;
	remapEdgeR2Max = _remapEdgeR2Max;
	remapVertexR1Max = _remapVertexR1Max;
	remapVertexR2Max = _remapVertexR2Max;
}

void Shape2DData::
getRemapParams(float &_remapRankWeight, float &_remapEdgeR2Max, 
							 float &_remapVertexR1Max, float &_remapVertexR2Max)
{
	_remapRankWeight = remapRankWeight;
	_remapEdgeR2Max = remapEdgeR2Max;
	_remapVertexR1Max = remapVertexR1Max;
	_remapVertexR2Max = remapVertexR2Max;
}



const int Shape2DDataMagic = 6996;
const int Shape2DDataVersion = 1;

Bool Shape2DData::
binInit (BinInput& bi)
{
  Magic magic(Shape2DDataMagic, Shape2DDataVersion);
  if(!magic.binInit(bi))
    return FALSE;
  lineR2max = bi.readFloat();
  pointR1max = bi.readFloat();
  matchR2max = bi.readFloat();
  matchDihedralAngleWeight = bi.readFloat();
  matchNeighborWeight = bi.readFloat();
  matchQuadAngleWeight = bi.readFloat();
  clr2 = bi.readFloat();
  clenergy = bi.readFloat();
  alphaPath = bi.readFloat();
  betaPath = bi.readFloat();
  iterPath = bi.readInt();
  alphaPatch = bi.readFloat();
  betaPatch = bi.readFloat();
  iterPatch = bi.readInt();
  alphaGrid = bi.readFloat();
  betaGrid = bi.readFloat();
  iterGrid = bi.readInt();
  gridUres = bi.readInt();
  gridVres = bi.readInt();
  nurbRes = bi.readInt();
  sampleSize = bi.readInt();
  cleqsp = bi.readFloat();
  nurbEvalRes = bi.readInt();

	alphaPathEdit = bi.readFloat();
	betaPathEdit = bi.readFloat();
	iterPathEdit = bi.readInt();
  
  Shape2DData other; // get defaults
	if(!iterPathEdit && !betaPathEdit && !alphaPathEdit)
	{
		other.getEditRelaxParams(alphaPathEdit, betaPathEdit, iterPathEdit);
	}

  remapRankWeight = bi.readFloat();
  remapEdgeR2Max = bi.readFloat();
  remapVertexR1Max = bi.readFloat();
	remapVertexR2Max = bi.readFloat();

	if(!remapRankWeight && !remapEdgeR2Max && !remapVertexR1Max && !remapVertexR2Max)
	{
		other.getRemapParams(remapRankWeight, remapEdgeR2Max, remapVertexR1Max, remapVertexR2Max);
	}

  clObserveRadius = bi.readFloat();
  if(!clObserveRadius) clObserveRadius = other.clObserveRadius;

  for(int i = 0; i<118; i++)
    bi.readInt();
  return TRUE;
}

void Shape2DData::
binDump (BinOutput& bo)
{
  Magic magic(Shape2DDataMagic, Shape2DDataVersion);
  magic.binDump(bo);
  bo.writeFloat(lineR2max);
  bo.writeFloat(pointR1max);
  bo.writeFloat(matchR2max);
  bo.writeFloat(matchDihedralAngleWeight);
  bo.writeFloat(matchNeighborWeight);
  bo.writeFloat(matchQuadAngleWeight);
  bo.writeFloat(clr2);
  bo.writeFloat(clenergy);
  bo.writeFloat(alphaPath);
  bo.writeFloat(betaPath);
  bo.writeInt(iterPath);
  bo.writeFloat(alphaPatch);
  bo.writeFloat(betaPatch);
  bo.writeInt(iterPatch);
  bo.writeFloat(alphaGrid);
  bo.writeFloat(betaGrid);
  bo.writeInt(iterGrid);
  bo.writeInt(gridUres);
  bo.writeInt(gridVres);
  bo.writeInt(nurbRes);
  bo.writeInt(sampleSize);
  bo.writeFloat(cleqsp);
  bo.writeInt(nurbEvalRes);
	bo.writeFloat(alphaPathEdit);
	bo.writeFloat(betaPathEdit);
	bo.writeInt(iterPathEdit);
	bo.writeFloat(remapRankWeight);
	bo.writeFloat(remapEdgeR2Max);
	bo.writeFloat(remapVertexR1Max);
	bo.writeFloat(remapVertexR2Max);
  bo.writeFloat(clObserveRadius);
  for(int i = 0; i<118; i++)
    bo.writeInt(0);
}
