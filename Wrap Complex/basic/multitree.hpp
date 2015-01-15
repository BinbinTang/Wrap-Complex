#ifndef _MULTITREE_HPP
#define _MULTITREE_HPP
#include <basic/multitree.h>

template <class T>
MultiTree<T>::
MultiTree(const MultiTree& t)
:currObj_(t.currObj_),
delPtrWhenSuicide_(t.delPtrWhenSuicide_)
{
	nObjs_ = 0; 
	root_ = clone_(t.root_,NULL);
	current_ = root_;
	Assert(nObjs_ == t.nObjs_);
}

template <class T>
MultiTreeNode<T>* MultiTree<T>::
clone_(MultiTreeNode<T>* mtn, MultiTreeNode<T>* parent)
{
	int i;
	MultiTreeNode<T>* newtree = new MultiTreeNode<T> (parent);
	for(i=0;i<mtn->nChild_;i++)
	{
		newtree->objs_[i] = new T (*mtn->objs_[i]);
		allObjPtrs_[nObjs_] = newtree->objs_[i];
		nObjs_++;
	}
	newtree->nChild_ = mtn->nChild_;

	for(i=0;i<mtn->nSubTree_;i++)
		newtree->subTrees_[i] = clone_(mtn->subTrees_[i],newtree);
	newtree->nSubTree_ = mtn->nSubTree_;

	return newtree;
}

template <class T>
MultiTree<T>::
MultiTree(Bool delPtrWhenSuicide)
{
  delPtrWhenSuicide_ = delPtrWhenSuicide;
  root_     = new MultiTreeNode<T>(NULL);
  root_->parent_ = NULL;
  current_  = root_;
  nObjs_    = 0;
  currObj_  = -1;
}


template <class T>
void MultiTree<T>::
createAndDownALevel()
{
  current_->subTrees_[current_->nSubTree_] = new MultiTreeNode<T>(current_);
  current_->nSubTree_++;
  current_ = current_->subTrees_[current_->nSubTree_-1];
  currObj_ = -1;
}

template <class T>
void MultiTree<T>::
closeAndUpALevel()
{
	Assert(current_->parent_);
	current_ = current_->parent_;
	currObj_ = current_->nChild_-1;  // make it to the last obj;
}

template <class T>
T* MultiTree<T>::
currObj(){
	Assert(currObj_>=0);
	Assert(currObj_<nObjs_);
	return allObjPtrs_[currObj_];
}


template <class T>
void MultiTree<T>::
newObject(T* Tptr)
{
  current_->objs_[current_->nChild_] = Tptr;
  current_->nChild_++;
  allObjPtrs_[nObjs_] = Tptr;
  currObj_ = nObjs_;
  nObjs_++;
}


template <class T>
MultiTree<T>::
~MultiTree()
{
	int i;
	if(delPtrWhenSuicide_)
		for(i=0;i<nObjs_;i++)
			delete allObjPtrs_[i];
	delete root_;
}


template <class T>
MultiTreeNode<T>::
MultiTreeNode(MultiTreeNode<T>* parent)
:parent_(parent)
{
  nSubTree_ = 0;
  nChild_   = 0;
};

template <class T>
MultiTreeNode<T>::
~MultiTreeNode()
{

	int i;
	for(i=0;i<nSubTree_;i++)
		delete subTrees_[i];
}


template <class T>
SimpleMultiTreeIter<T>::
SimpleMultiTreeIter(const MultiTree<T>& t):tree_(&t)
{
	start();
}

template <class T>
void SimpleMultiTreeIter<T>::
start()
{
	objCount_ = 0;
}

template <class T>
void SimpleMultiTreeIter<T>::
next()
{
	if(!isDone())
		objCount_++;
}

template <class T>
Bool SimpleMultiTreeIter<T>::
isDone()
{
	return objCount_>=tree_->nObjs_;
}

template <class T>
T* SimpleMultiTreeIter<T>::
currItem()
{
	Assert(objCount_<=tree_->nObjs_);
	return tree_->allObjPtrs_[objCount_];
}

template <class T> 
MultiTreeChildrenIter<T>::
MultiTreeChildrenIter(const MultiTree<T>& mt)
{
	tn_ = mt.root_;
	start();
}

template <class T>
MultiTreeChildrenIter<T>::
MultiTreeChildrenIter(const MultiTreeNode<T>& mtn)
{
	tn_ = &mtn;
	start();
}

template <class T>
void MultiTreeChildrenIter<T>::
start()
{
	current_ = 0;
}

template <class T>
Bool MultiTreeChildrenIter<T>::
isDone()
{
	return current_>=tn_->nChild_;
}

template <class T>
void MultiTreeChildrenIter<T>::
next()
{
	if(!isDone())
		current_++;
}

template <class T>
T* MultiTreeChildrenIter<T>::
currItem()
{
	Assert(current_<tn_->nChild_);
	return tn_->objs_[current_];
}

template <class T>
int MultiTreeChildrenIter<T>::
size()
{
	return tn_->nChild_;
}

template <class T>
MultiTreeSubTreeIter<T>::
MultiTreeSubTreeIter(const MultiTree<T>& mt)
:tn_(mt.root_)
{
	subTreeIter_ = NULL;
	objIter_   = NULL;
	start();
}

template <class T>
MultiTreeSubTreeIter<T>::
MultiTreeSubTreeIter(const MultiTreeNode<T>* mtn)
:tn_(mtn)
{
	subTreeIter_ = NULL;
	objIter_   = NULL;
	start();
}

template <class T>
void MultiTreeSubTreeIter<T>::
start()
{
	current_ = 0;
}

template <class T>
Bool MultiTreeSubTreeIter<T>::
isDone()
{
	return current_>=tn_->nSubTree_;
}

template <class T>
int MultiTreeSubTreeIter<T>::
size()
{
	return tn_->nSubTree_;
}

template <class T>
void MultiTreeSubTreeIter<T>::
nextSubTree()
{
	current_++;
/*
	if(!isDone())
	{
		if(subTreeIter_)
			delete subTreeIter_;
		subTreeIter_= new MultiTreeSubTreeIter<T> (tn_->subTrees_[current_]);
	}
*/
}

template <class T>
MultiTreeNode<T>* 
MultiTreeSubTreeIter<T>::
subTree()
{
	/*
	Assert(subTreeIter_);
	return subTreeIter_;
	*/
	if(isDone())
		Assert(0);
	return tn_->subTrees_[current_];
}



template <class T>
MultiTreeSubTreeIter<T>::
~MultiTreeSubTreeIter()
{
	if(subTreeIter_)
		delete subTreeIter_;
	if(objIter_)
		delete objIter_;
}

template <class T>
MultiTreeChildrenIter<T>* MultiTreeSubTreeIter<T>::
objIter()
{
	if(!objIter_)
		objIter_ = new MultiTreeChildrenIter<T> (*tn_);
	return objIter_;
}


#endif