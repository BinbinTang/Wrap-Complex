#ifndef __RGI_RECTSEL_H__
#define __RGI_RECTSEL_H__

#include <basic/iterstack.h>
#include <basic/binio.h>

class GeneralPointSelection
{
public:
  enum SelectionMode
       { SELECT_NONE = 0,
         SELECT_ALL = 1,
         SELECT_REGION = 2,
         UNSELECT_REGION = 3};
  virtual ~GeneralPointSelection (void);
  static GeneralPointSelection * 
    createGeneralPointSelection(const GeneralPointSelection& other);
  static GeneralPointSelection * 
    createEmptyGeneralPointSelection(BinInput& bi);
  virtual Bool binInit (BinInput& bi) = 0;
  virtual void binDump (BinOutput& bo) = 0;
  virtual int getId (void) const = 0;
  virtual void selectAll (void) = 0;
  virtual void deselectAll (void) = 0;
  void setSelectMode (SelectionMode mode) { selectMode_ = mode; }
  virtual Bool inside (double x, double y, double z) const = 0;
  Bool inside (float x[3]) const { return inside (x[0], x[1], x[2]); }
  Bool inside (double x[3]) const { return inside (x[0], x[1], x[2]); }
  int getSelectMode (void) const { return selectMode_; }
protected:
  GeneralPointSelection (void) { }
  SelectionMode selectMode_;
};

/******************************************************************
 *                                                                *
 *          class MousePoint, represents mouse selection          *
 *                                                                *
 ******************************************************************/


class MousePoint
{
public:
	int x;
	int y;
	MousePoint (void) { }
  MousePoint (int xm, int ym) : x(xm), y(ym) { }
  MousePoint (const MousePoint &mp) : x(mp.x), y(mp.y) { }
	~MousePoint (void) { }
};

/******************************************************************
 *                                                                *
 *          class PointSelection                                  *
 *                                                                *
 ******************************************************************/

#define SELECTION_MAGIC 903372

class PointSelection : public GeneralPointSelection
{
protected:
  Magic magic_;
  int clipPlanes_;
  double clipPlane_[6*4];

  double m_[16];

  void setupTransformMatrix_ (double *m);
public:
  
 PointSelection(void);
 PointSelection(double *m);
 PointSelection (PointSelection& other);
 virtual ~PointSelection (void);

 static PointSelection * createEmptyPointSelection (BinInput& bi);
 static PointSelection * createPointSelection (PointSelection& other);
 virtual Bool binInit (BinInput& bi);
 virtual void binDump (BinOutput& bo);
 int getId (void) const { return magic_.getId(); };

 int getClipPlanes () const { return clipPlanes_; };
 const double *getClipPlane (int idx) const;
 void addClipPlane (const double *);
  // there are at most 6 clip planes, numbered 0, 1, ..., 5. The definition
  // is the same as used by OpenGL.

 const double * getMatrix (void) { return m_; }
 
 void selectAll (void) { selectMode_ = SELECT_ALL; }
 void deselectAll (void) { selectMode_ = SELECT_NONE; }

 Bool transform (double x, double y, double z, double p[3]) const;
 Bool transform (double x[3], double p[3]) const
                { return transform (x[0], x[1], x[2], p); }
 
 virtual Bool insideXY (double x, double y) const;
 Bool inside (float x[3]) const { return inside (x[0], x[1], x[2]); }
 Bool inside (double x[3]) const { return inside (x[0], x[1], x[2]); }
 Bool inside (double x, double y, double z) const;
};

/******************************************************************
 *                                                                *
 *          class RectSelection                                   *
 *                                                                *
 ******************************************************************/

#define RECTSELECTION_MAGIC 3561

class RectSelection : public PointSelection
{
private:
 int x1_, y1_, x2_, y2_;
 void orderCoords_ (void);
public:
 RectSelection (void);
 RectSelection (int x1, int y1, int x2, int y2, double *m, int order = TRUE);
 RectSelection (RectSelection& other);
 ~RectSelection (void);
 int x1 (void) { return x1_; }
 int x2 (void) { return x2_; }
 int y1 (void) { return y1_; }
 int y2 (void) { return y2_; }
 Bool insideXY (double x, double y) const;
 Bool binInit (BinInput& bi);
 void binDump (BinOutput& bo);
 static Bool isValid (int magic);
};

/******************************************************************
 *                                                                *
 *          class PolySelection                                   *
 *                                                                *
 ******************************************************************/

#define POLYSELECTION_MAGIC 7123
class PolySelection : public PointSelection
{
private:
  IterStack <MousePoint> s_;
public:
  PolySelection (void);
  PolySelection (double *m);
  PolySelection (PolySelection& other);
  ~PolySelection (void);
  int addVertex (int x, int y);
  int addVertices (IterStack<MousePoint>& other);
  int numVertices (void) { return s_.size(); }
  int x (int v) { return s_[v].x; }
  int y (int v) { return s_[v].y; }
  Bool insideXY (double x, double y) const;
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
  static Bool isValid (int magic);
};

/******************************************************************
 *                                                                *
 *          class MaskedSelection                                 *
 *                                                                *
 ******************************************************************/

class MaskedSelection : public PointSelection
{
public:
	MaskedSelection ();
	MaskedSelection (int x0, int y0, int x1, int y1, unsigned char *mask,
		double *transform);
	MaskedSelection (const MaskedSelection &other);
	~MaskedSelection ();
	int x0 () const { return x0_; }
	int y0 () const { return y0_; }
	int x1 () const { return x1_; }
	int y1 () const { return y1_; }
	Bool insideXY (double x, double y) const;
	Bool binInit (BinInput& bi);
	void binDump (BinOutput& bo);
	static Bool isValid (int magic);
	int size () const { return (x1_-x0_) * (y1_-y0_); }
private:
	int x0_, y0_, x1_, y1_;
	unsigned char *mask_;
};

/******************************************************************
 *                                                                *
 *          class MultiplePointSelection                                   *
 *                                                                *
 ******************************************************************/

class MultiplePointSelection : public GeneralPointSelection
{
public:
  MultiplePointSelection (void);
  MultiplePointSelection (MultiplePointSelection& other);
  ~MultiplePointSelection (void);
  int numSelections() { return selStack_.size(); }
  void addSelection(const GeneralPointSelection& sel);
  void reset ();
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
  Bool inside (double x, double y, double z) const;
  void selectAll (void);
  void deselectAll (void);
  static Bool isValid(int magic);
  int getId (void) const { return magic_.getId (); }
private:
  Magic magic_;
  IterStack<GeneralPointSelection *> selStack_;
};

#endif // __RECT_SEL_H__
