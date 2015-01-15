#ifndef __RGI_MULTITREE_H__
#define __RGI_MULTITREE_H__

#include <basic/rarray.h>

/*************************************

	class MultiTree

  */

#define TREE_EVENT_NEW_TREE     1
#define TREE_EVENT_NEW_OBJ		2
#define TREE_EVENT_DOWN_A_LEVEL 3
#define TREE_EVENT_UP_A_LEVEL	4
#define TREE_EVENT_NEW_SUB_TREE 5
#define TREE_EVENT_TREE_END		6


template <class T> class MultiTree;
template <class T> class SimpleMultiTreeIter;
template <class T> class MultiTreeSubTreeIter;
template <class T> class MultiTreeChildrenIter;

template <class T> 
class MultiTreeNode {
protected:
	RArray<MultiTreeNode<T>*> subTrees_;
	RArray<T*> objs_;
	MultiTreeNode<T>* parent_;

	int nSubTree_;
	int nChild_;

public:
	MultiTreeNode(MultiTreeNode<T>* parent = NULL);
	~MultiTreeNode();

	MultiTreeNode<T>* lastSubTree();
	T* lastChild();

	friend MultiTree<T>;
	friend SimpleMultiTreeIter<T>;
	friend MultiTreeSubTreeIter<T>;
	friend MultiTreeChildrenIter<T>;
};

template <class T> 
class MultiTree {

protected:
	MultiTreeNode<T> *current_;
	MultiTreeNode<T> *root_;
	int nObjs_;
	int currObj_; // the current object idx in the allObjPtrs_ array.
	Bool delPtrWhenSuicide_;
	RArray<T*> allObjPtrs_;

	MultiTreeNode<T>* clone_(MultiTreeNode<T>* src,MultiTreeNode<T>* parent);

public:
	MultiTree(Bool delPtrWhenSuicide=TRUE);
	MultiTree(const MultiTree<T>&);
	~MultiTree();

	void createAndDownALevel();
	void closeAndUpALevel();
	T* currObj();

	void newObject(T*);

	friend SimpleMultiTreeIter<T>; 
	friend MultiTreeSubTreeIter<T>;
	friend MultiTreeChildrenIter<T>;
};



template <class T>
class SimpleMultiTreeIter {

protected:
	const MultiTree<T>     *tree_;
	int objCount_;

public:
	SimpleMultiTreeIter(const MultiTree<T>&);

	virtual void start(); // to start the iteration of the whole tree
	virtual void next();
	virtual Bool isDone();
	virtual T*   currItem();

};

template <class T>
class MultiTreeChildrenIter
{
protected:
	const MultiTreeNode<T> *tn_;
	int   current_;
	MultiTreeChildrenIter(const MultiTreeNode<T>&);
	MultiTreeChildrenIter(const MultiTree<T>&);

public:
	virtual void start(); 
	virtual void next();
	virtual Bool isDone();
	virtual T*   currItem();
	virtual int  size();

	friend MultiTreeSubTreeIter<T>;
};

template <class T>
class MultiTreeSubTreeIter
{
protected:
	const MultiTreeNode<T> *tn_;
	int   current_;

	MultiTreeSubTreeIter<T>* subTreeIter_;
	MultiTreeChildrenIter<T>* objIter_;

public:
	MultiTreeSubTreeIter(const MultiTreeNode<T>*);
	MultiTreeSubTreeIter(const MultiTree<T>&);
	virtual ~MultiTreeSubTreeIter();
	virtual void start(); 
	virtual void nextSubTree(); // a new Iter will create, need to delete it outside i.e., it won't handle the deletion
	virtual Bool isDone();
	virtual MultiTreeNode<T>* subTree();  // for children subtree
	virtual MultiTreeChildrenIter<T>* objIter(); // for its own children obj
	virtual int  size();
};



#include <basic/multitree.hpp>

#endif
