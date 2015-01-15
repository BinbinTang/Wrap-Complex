#ifndef __RGI_DATA_H__
#define __RGI_DATA_H__

#include <cstdlib> /* for NULL */
#include <basic/basic.h>
#include <basic/rgitypes.h>
#include <basic/rgicstring.h>
#include <basic/rgistring.h>
class Command;
class IndexStack;
class BitVector;
class RectSelection;
 
class Data
{ 
public: 
	virtual ~Data (void);
protected: 
	Data (void ) {}; 
}; 

template <class T>
class PointData : public Data
{
public:
	PointData(T x, T y, T z): x_(x), y_(y), z_(z) {}
	void get (T &x, T &y, T &z) { x = x_; y= y_; z=z_;}
private:
	T x_, y_, z_;
};
 
template  <class T> 
class GenData : public Data 
{ 
public: 
	GenData(T  i) { data_ = i;}
	T  get () const { return data_;} 
private: 
	T data_; 
}; 
 
template  <class T1, class T2> 
class GenData2 : public Data 
{ 
public: 
	GenData2(T1  i, T2 j) { data1_ = i; data2_ = j;}
	T1  get1 () const { return data1_;}
	T2  get2 () const { return data2_;}
private: 
	T1 data1_; 
	T2 data2_; 
}; 

class EventData : public Data
{
public:
    EventData (int i1, int i2=0, int i3=0, int i4=0);
    ~EventData ();
    int get1 () { return i1_;}
    int get2 () { return i2_;}
    int get3 () { return i3_;}
    int get4 () { return i4_;}
private:
    int i1_,i2_,i3_,i4_;
};

class CStringData : public Data 
{ 
public: 
	CStringData (const RgiCString &); 
	~CStringData (); 
	const RgiCString &get () const { return c_;}
private: 
	RgiCString c_; 
}; 
 
class StringData : public Data 
{ 
public: 
	StringData (const RgiString &); 
	~StringData (); 
	const RgiString &get () const { return c_;}
private: 
	RgiString c_; 
}; 
 
/*
class IndexData : public Data 
{ 
public: 
	IndexData (int mark, IndexStack *ids) : mark_(mark), ids_(ids){}
	int getMark () const { return mark_;}
	IndexStack *getIndices () const { return ids_;}
private: 
	int mark_; 
	IndexStack *ids_; 
}; 
*/
 
class IndexData : public Data 
{ 
public: 
	IndexData (IndexStack *ids) : ids_(ids){}
	IndexStack *getIndices () const { return ids_;}
private: 
	IndexStack *ids_; 
}; 

class SelectionData : public Data 
{ 
public: 
	SelectionData (int mark, IndexStack *ids, RectSelection * rectsel);
	SelectionData (int mark, BitVector *bv, RectSelection * rectsel);
	~SelectionData (void);

	int getMark () const { return mark_; }
	IndexStack *getIndices () const { return ids_; }
	BitVector * getBitVector () const { return bv_; }
	RectSelection * getRectSel () const { return rectsel_; }
private:
	int mark_; 
	IndexStack *ids_;
	BitVector * bv_;
	RectSelection * rectsel_;
};
 
class ProgressData : public Data 
{ 
public: 
  ProgressData (int percentage) : p_(percentage), c_(0) { } 
  int getPercentage (void) { return p_; } 
  void cancel (void) { c_ = 1; } 
  int cancelled (void) { return c_; } 
private: 
  int p_; 
  int c_; 
}; 
 
class CmdData : public Data  
{ 
public: 
	CmdData (Command *cmd) { cmd_ = cmd;} 
	Command *get () const { return cmd_;}
	int getEventName(void)
	    { return(eventName_); }
	void setEventName(int event)
	    { eventName_=event; }

private: 
	Command *cmd_; 
	int eventName_;
}; 
 
 
class FileOpenData : public Data 
{ 
public: 
	enum PtTemplate { None, Current, Load }; 

	enum PrecisionType { Single, Double, FromFile };

	FileOpenData(); 
	FileOpenData (const char *filename, int fileType, float s = 100, float t = 0,  
		PtTemplate pttmp = None, const char * ptTmpName=NULL); 
	 
	virtual ~FileOpenData (); 
	FileOpenData (const FileOpenData &); 
	const FileOpenData & operator= (const FileOpenData & other); 
 
	const char *getFileName () const { return filename_.data() ;}
	void setFileName (const char *f); 
 
	int getFileType () const {return fileType_;}
	void setFileType (int ft) { fileType_ = ft;}

	void setOpenAsSurface (int val) { openAsSurface_ = val; }
	int getOpenAsSurface (void) { return openAsSurface_; }
 
	float getSample () const{ return sample_;}
	void setSample (float s) { sample_ = s;}
 
	float getTolerance () const { return tolerance_;}
	void setTolerance (float t) { tolerance_ = t;}
 
	UnitType getUnitType() const { return unitType_;}
	void setUnitType (int unit) { unitType_ = (UnitType) unit;}
 
	FileOpenData::PtTemplate getTemplateType() const { return ptTemplate_;}
	void setTemplateType (FileOpenData::PtTemplate pt) { ptTemplate_ =pt;}
 
	const char *getTemplateName () const { return templateName_.data() ;}
	void setTemplateName (const char *f); 
 
	int getPrecisionType () const { return precisionType_; }
	void setPrecisionType (int type) { precisionType_ = type; }

	BOOL getUseKDTree () const { return useKDTree_; }
	void setUseKDTree (BOOL onOff) { useKDTree_ = onOff; }

private: 
	RgiCString filename_;			// name of the file to open 
	int fileType_;				// the type of file to open 
	float sample_;			// sample percentage, only applies for vertex data types 
	float tolerance_;		// min dist between two  
	UnitType unitType_;			// type of unit used
	PtTemplate ptTemplate_;	// what type of template to use 
	RgiCString templateName_;			// template file name 
	int openAsSurface_;
	int precisionType_;
	BOOL useKDTree_;
}; 
 
class FileSaveData : public Data 
{ 
public: 
	enum { Com, Clo, Vol, Tri, Edge, Vert, Patches, Max }; 
	 
	FileSaveData(); 
	FileSaveData (const char *filename, int fileType,
		      const int *settings, double scale = 1.0, int resolution=10); 
	 
	virtual ~FileSaveData ();   
	FileSaveData (const FileSaveData &); 
	const FileSaveData & operator= (const FileSaveData & other); 
 
	const char *getFileName () const { return filename_.data(); }
	void setFileName (const char *f); 
 
	int getFileType () const {return fileType_;}
	void setFileType (int ft) { fileType_ = ft;}
 
	int getTypes (int cnt) const { return settings_[cnt];}
	void setTypes (int cnt, int on) { settings_[cnt] = on;}

	void setSensitive (int cnt, int on) { sensitive_[cnt] = on; }
	int getSensitive (int cnt) const { return sensitive_[cnt]; }

	double getScaleFactor (void) const { return scale_; }
	void setScaleFactor (double scale) { scale_ = scale; }

	double getPatchResolution (void) const { return resolution_; }
	void setPatchResolution (int res) { resolution_ = res; }

	void resetSettings (void);

private: 
	RgiCString filename_;			// name of the file to open 
	int fileType_;				// the type of file to open 
	int settings_[Max];   // 
	int sensitive_[Max]; 
	int resolution_;
	double scale_;
}; 


class RelaxData : public Data
{
public:

	void reset();
//	const RelaxData &operator= (const RelaxData &);

	int setIteration (const RgiString &s);
	int strToIteration (const RgiString &s, int &iteration);

	int operator== (const RelaxData&) const;
	
	int iteration;
	int fixBoundary;  
};

class RefineData : public Data
{
public:
	int movePts;
	int fixBoundary;
};

class DecimateData : public Data
{
public:
	void reset(double err, int cnt);
	
	//const DecimateData &operator= (const DecimateData &);
	int operator== (const DecimateData &) const;

	int setMaxErr(const RgiString &s);
	int strToMaxErr(const RgiString &s, double &err);

	int setTriCnt(const RgiString &s);
	int strToTriCnt(const RgiString &s, int&cnt);

	int setReduceTo(const RgiString &s);
	int strToReduceTo(const RgiString &s, double&cnt);
	
	int setTangentLamda(const RgiString &s);
	int strToTangentLamda(const RgiString &s, double &l);
	
	int setMidPlaneLamda(const RgiString &s);
	int strToMidPlaneLamda(const RgiString &s, double &l);
	
	int setCentroidLamda(const RgiString &s);
	int strToCentroidLamda(const RgiString &s, double &l);
	
	int setFunnelLamda(const RgiString &s);
	int strToFunnelLamda(const RgiString &s, double &l);
	
	int setIsocelesLamda(const RgiString &s);
	int strToIsocelesLamda(const RgiString &s, double &l);

	double maxErr;
	int triCnt;
	double  reduceTo;
	int fixBoundary;
	double tangentLamda;
	double midPlaneLamda;
	double centroidLamda;
	double funnelLamda;
	double isocelesLamda;
};

class OneValData : public Data
{
public:
	virtual int setVal (const RgiString &s);
	virtual int strToVal (const RgiString &s, double &v) = 0;

	double val;
};

class SampleData : public OneValData
{
public:
	int strToVal (const RgiString &s, double &v);
};

class AutoPlotDistData : public OneValData
{
public:
	int strToVal (const RgiString &s, double &v);
};

class SetDepthData : public OneValData
{
public:
	int strToVal (const RgiString &s, double &v);
};

class AddPointPlaneData : public OneValData
{
public:
	int strToVal (const RgiString &s, double &v);
};

class ThickenData : public OneValData
{
public:
	void reset (double val);	
	int operator== (const ThickenData&) const;

	int strToVal (const RgiString &s, double &v);
};

class ShellingData : public OneValData
{
public:
  ShellingData (void);
  ~ShellingData (void) { }
	void reset (double val, int isBiDir);	
	int operator== (const ShellingData&) const;

	int strToVal (const RgiString &s, double &v);
  int biDir;
};

class GenExtCmdData : public Data
{
public:
	GenExtCmdData (unsigned int nID, void *pExtra) { id_ = nID; extra_ = pExtra; }
	
	unsigned int getID() { return id_; }
	void* getExtra() { return extra_; }

private:
	unsigned int id_;
	void *extra_;
};


enum HostIDKey {
	NoID = 0,
	EthernetID,
	DiscID,
	FlexRainbowID,
	FlexDallasID,
	DemoID,
	FloatID,
	MaxID
};

class FlexLMFeatureData : public Data
{
public:
  enum {FEATURE_SIZE = 24, EXP_DATE_SIZE = 11, CODE_SIZE = 12};
  char feature[FEATURE_SIZE+1];
  char exp_date[EXP_DATE_SIZE+1];
  char code[CODE_SIZE+1];
  HostIDKey hostIDKey;
  FlexLMFeatureData(char * _feature = NULL, char * _exp_date = NULL, 
                    char * _code = NULL, HostIDKey _hostIDKey = NoID);
};

class OneValTypedData : public Data
{
	char buf[32];
public:
	enum DataType 
	{	CharData, 
		StringData, 
		IntData, 
		DoubleData 
	} type;

	union DataValue
	{
		char ch;
		char* str;
		int num;
		double dbl;
	} value;

	OneValTypedData(char c);
	OneValTypedData(char* s);
	OneValTypedData(int n);
	OneValTypedData(double d);
	~OneValTypedData();

	void Increment();
	void Decrement();
	char* GetValue();
	void SetValue(const char *data);
};

class AddMovePtData: public OneValData
{
public:
	static const char *const BadFormatEmsg;

	void reset (double val);	
	int operator== (const AddMovePtData&) const;

	int strToVal (const RgiString &s, double &v);
};

class BinInput;
class BinOutput;

class Shape2DData: public Data
{
public:

	// parameters control choice of half-discs
	float remapRankWeight; // controls rank vs angle choice of half-discs
	float remapEdgeR2Max; // used in defining edge rank
	float remapVertexR1Max, remapVertexR2Max; // used in defining vertex rank

  // Character Line Parameters
  float lineR2max;     
    //- All paths with R2 larger than lineR2max are CL's
  float pointR1max;   
    //- All corners with R1 larger than pointR1max are CP's

  // Matching Parameters
  float matchR2max;  
    //- All edges with larger R2 will not be matched.
  float matchDihedralAngleWeight;
    //- Weight for dihedral angle contribution to matching
    //- priority.  Smaller dihedral angles have higher
    //- priority.
  float matchNeighborWeight;
    //- Weight for neighborhood considerations.  If neighbors
    //- have already been matched, an edge will have higher
    //- priority.
  float matchQuadAngleWeight;
    //- Weight for quadrilateral angle considerations.  Any
    //- matches that create angles close to 90 degrees have
    //- higher priority.

  // Path Relaxation
  float alphaPath;
    //- straighten component of path relaxation.
  float betaPath;
    //- curvature component of path relaxation
  int iterPath;
    //- Number of iterations for path relaxation

	float alphaPathEdit;
	  //- straighten component of path relaxation during editing
	float betaPathEdit;
	  //- curvature component of path relaxation during editing
	int iterPathEdit;
	  //- Number of iterations for path relaxation during editing

  // Character Line Relaxation Parameters
  float clr2;
    //- R2 component of character line relaxtion
  float clObserveRadius;
   // - radius within which model curvature is accumulated (as fraction of min bbox length)
  float cleqsp;
	//- equal spacing component of character line relaxation
  float clenergy;
    //- normal path relaxtion (straightness+curvature) component of character line relaxation
  
  int sampleSize;
    //- the sampling size for editing of character line and patch boundaries.

  // Patch Relaxation Parameters
  float alphaPatch;
    //- patch relaxation scale factor on fairness force.
  float betaPatch;
    //- equal spacing component of patch relaxation
  int iterPatch;
    //- Number of iterations for patch relaxation.

  // Grid construction and relaxation Parameters
  int gridUres;
    //- Number of grid lines in U direction
  int gridVres;
    //- Number of grid lines in V direction
  float alphaGrid;
    //- equal arc-length force component in grid construction.
  float betaGrid;
    //- fairness force component in grid construction.
  int iterGrid;
    //- Number of iterations for grid relaxation.

  // Nurb Construction Parameters
  int nurbRes;
    //- Number of control lines in both U and V directions.
  int nurbEvalRes;
    //- Number of lines used for rendering/evaluating Nurbs.

  Shape2DData (void);
	void setRemapParams(float _remapRankWeight, float _remapEdgeR2Max, float _remapVertexR1Max, float _remapVertexR2Max);
	void getRemapParams(float &_remapRankWeight, float &_remapEdgeR2Max, float &_remapVertexR1Max, float &_remapVertexR2Max);
  void setRankParams (float _lineR2max, float _pointR1max);
  void setMatchParams (float _matchR2max, float _matchDihedralAngleWeight, 
    float _matchNeighborWeight, float _matchQuadAngleWeight);
  void setCLRelaxParams (float _alphaPath, float _betaPath, int _iterPath, 
    float _clr2, float _cllength, float _clenergy, float _clObserveRadius);
  void setEditRelaxParams (float _alphaPathEdit, float _betaPathEdit, int _iterPathEdit);
	void setPatchParams (float _alphaPatch, float _betaPatch, int _iterPatch);
  void setGridParams (float _alphaGrid, float _betaGrid, int _iterGrid);
  void setNurbParams (int _nurbRes, int _nurbEvalRes);
  
  void getRankParams (float& _lineR2max, float& _pointR1max);
  void getMatchParams (float& _matchR2max, float& _matchDihedralAngleWeight, 
    float& _matchNeighborWeight, float& _matchQuadAngleWeight);
  void getCLRelaxParams (float &_alphaPath, float &_betaPath, int &_iterPath, 
    float &_clr2, float &_cllength, float &_clenergy, float &_clObserveRadius);
	void getEditRelaxParams (float &_alphaPathEdit, float &_betaPathEdit, int &_iterPathEdit);
  void getGridParams (int& _gridUres, int& _gridVres, float& _alphaGrid, float& _betaGrid, int& _iterGrid);
  void getPatchParams (float& _alphaPatch, float& _betaPatch, int& _iterPatch);
  void getNurbParams (int& _nurbRes, int& _nurbEvalRes);
  void resetState (void);
  
  void binDump (BinOutput& bo);
  Bool binInit (BinInput& bi);
};

// This is the data passed with the CB when tile stuff is accessed.
class TileData : public Data
{
public:
	TileData(int cb, int event, int ival):
		cb_(cb),
		event_(event),
		ival_(ival),
		sval_(),
		data_(0)
		{};
	TileData(int cb, int event, const RgiString &sval):
		cb_(cb),
		event_(event),
		ival_(0),
		sval_(sval),
		data_(0)
		{};
	TileData(int cb, int event, Data *data):
		cb_(cb),
		event_(event),
		ival_(0),
		sval_(),
		data_(data)
		{};
	~TileData() {};

	enum EventTypes {
		UpArrow=0,			// Up Arrow clicked
		DownArrow,			// Down arrow clicked
		ReturnPressed,		// Return pressed in textfield
		LostFocus,			// Textfield lost focus
		Clicked,			// Toggle button clicked
		Selected,			// ComboBox selection
		Ok,					// Ok Operation
		Apply,				// Apply Operation
		Cancel				// Cancel Operation
	};

	int getCallback(void) { return(cb_); };
	int getEvent(void) { return(event_); };
	int getIntVal(void) { return(ival_); };
	const RgiString &getStrVal(void) const { return(sval_); };
	Data *getData(void) { return(data_); };

private:
	int cb_,event_,ival_;
	RgiString sval_;
	Data *data_;
};
 
#endif 
