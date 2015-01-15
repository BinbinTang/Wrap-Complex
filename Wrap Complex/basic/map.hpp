template <class KEY, class VALUE>
Assoc<KEY,VALUE>::Assoc(unsigned int h, KEY k)
{
	hash = h;
	key = k;
	next = NULL;
}

template <class KEY, class VALUE>
Assoc<KEY,VALUE>::Assoc(unsigned int h, KEY k, VALUE v)
{
	hash = h;
	key = k;
  value = v;
	next = NULL;
}


//template <class KEY, class VALUE>
//unsigned int Map<KEY,VALUE>::genHashFunc_(KEY& key)
//{
//	return ((unsigned int)key);
//}

template <class KEY, class VALUE, class HASH>
Map<KEY,VALUE,HASH>::Map()
{
//	Assert (hashfunc);
//  hashFunc_ = hashfunc? hashfunc : Map<KEY,VALUE>::genHashFunc_;
	hashTable_ = NULL;
	hashSize_ = 19;
	count_ = 0;
}

template <class KEY, class VALUE, class HASH>
Map<KEY,VALUE,HASH>::~Map()
{
	removeAll();
}

template <class KEY, class VALUE, class HASH>
int Map<KEY,VALUE,HASH>::isEmpty() const
{
	return (count_ == 0);
}

template <class KEY, class VALUE, class HASH>
int Map<KEY,VALUE,HASH>::size() const
{
	return hashSize_;
}

template <class KEY, class VALUE, class HASH>
Assoc<KEY,VALUE>* Map<KEY,VALUE,HASH>::getAssocAt_(KEY& key, unsigned int &hash) const
{
  hash = hashFunc_(key) % hashSize_;

	if (hashTable_ == NULL)
		return NULL;

	for (Assoc<KEY,VALUE>* pAssoc = hashTable_[hash]; pAssoc != NULL; pAssoc = pAssoc->next)
	{
		if (pAssoc->key == key)
			return pAssoc;
	}
	return NULL;
}

template <class KEY, class VALUE, class HASH>
int Map<KEY,VALUE,HASH>::lookUp(KEY key, VALUE &value) const
{
	unsigned int hash;
	Assoc<KEY,VALUE> *pAssoc = getAssocAt_(key,hash);
	if (pAssoc == NULL)
		return 0;

	value = pAssoc->value;
	return 1;
}

template <class KEY, class VALUE, class HASH>
VALUE& Map<KEY,VALUE,HASH>::operator[](KEY key)
{
	unsigned int hash;
	Assoc<KEY,VALUE> *pAssoc;
	if ((pAssoc = getAssocAt_(key, hash)) == NULL)
	{
		if (hashTable_ == NULL)
			initHashTable(hashSize_, 1);

		// it doesn't exist, add a new Association
		pAssoc = new Assoc<KEY,VALUE>(hash, key);
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->next = hashTable_[hash];
		hashTable_[hash] = pAssoc;

		count_++;
	}
	return pAssoc->value;  // return new reference
}

template <class KEY, class VALUE, class HASH>
void Map<KEY,VALUE,HASH>::setAt(KEY key, VALUE value)
{
	(*this)[key] = value;
}

template <class KEY, class VALUE, class HASH>
int Map<KEY,VALUE,HASH>::removeKey(KEY key, VALUE& value)
{
	if (hashTable_ == NULL)
		return 0;

	Assoc<KEY,VALUE> *pAssoc;
	Assoc<KEY,VALUE> **ppAssocPrev = &hashTable_[hashFunc_(key) % hashSize_];
	
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->next)
	{
		if (pAssoc->key == key)
		{
			value = pAssoc->value;
			*ppAssocPrev = pAssoc->next;
			delete pAssoc;
			count_--;
			return 1;
		}
		ppAssocPrev = &pAssoc->next;
	}
	return 0;
}

template <class KEY, class VALUE, class HASH>
void Map<KEY,VALUE,HASH>::removeAll()
{
	if (hashTable_ == NULL)
		return;

	unsigned int hash;
	Assoc<KEY,VALUE>* pAssoc, *pTemp;
	// destroy elements (values and keys)
	for (hash = 0; hash < hashSize_; hash++)
	{
		pAssoc = hashTable_[hash];
		while (pAssoc)
		{
			pTemp = pAssoc;
			pAssoc = pAssoc->next;
			delete pTemp;
		}
	}

	// free hash table
	delete[] hashTable_;
	hashTable_ = NULL;

	count_ = 0;
}

template <class KEY, class VALUE, class HASH>
int Map<KEY,VALUE,HASH>::getHashSize() const
{
	return hashSize_;
}

template <class KEY, class VALUE, class HASH>
void Map<KEY,VALUE,HASH>::initHashTable(unsigned int size, int create)
{
	removeAll();

	hashSize_ = size;
	if (create)
	{
		hashTable_ = new Assoc<KEY,VALUE>*[hashSize_];
		memset(hashTable_,0,sizeof(Assoc<KEY,VALUE>*) * hashSize_);			
	}
}

template <class KEY, class VALUE, class HASH>
MAPPOS Map<KEY,VALUE,HASH>::getStartPos() const
{
	if (count_ > 0)
		return MAP_START_POS;
	return NULL;
}

template <class KEY, class VALUE, class HASH>
void Map<KEY,VALUE,HASH>::getNext(MAPPOS &pos, KEY &key, VALUE &value) const
{
	Assert(pos != NULL);

	Assoc<KEY,VALUE>* pAssoc = (Assoc<KEY,VALUE>*)pos;

	unsigned int hash;
	if (pAssoc == (Assoc<KEY,VALUE>*) MAP_START_POS)
	{
		// find the first association
		for (hash = 0; hash < hashSize_; hash++)
			if ((pAssoc = hashTable_[hash]) != NULL)
				break;
		Assert(pAssoc != NULL);  // must find something
	}

	// find next association
	Assoc<KEY,VALUE>* pAssocNext;
	if ((pAssocNext = pAssoc->next) == NULL)
	{
		// go to next bucket
		for (hash = pAssoc->hash + 1;hash < hashSize_; hash++)
			if ((pAssocNext = hashTable_[hash]) != NULL)
				break;
	}

	pos = (MAPPOS) pAssocNext;

	// fill in return data
	key = pAssoc->key;
	value = pAssoc->value;
}

template <class KEY, class VALUE, class HASH>
void Map<KEY,VALUE,HASH>::getAt(MAPPOS pos, KEY &key, VALUE &value) const
{
	Assert(pos != NULL);

	Assoc<KEY,VALUE>* pAssoc = (Assoc<KEY,VALUE>*)pos;

	unsigned int hash;
	if (pAssoc == (Assoc<KEY,VALUE>*) MAP_START_POS)
	{
		// find the first association
		for (hash = 0; hash < hashSize_; hash++)
			if ((pAssoc = hashTable_[hash]) != NULL)
				break;
		Assert(pAssoc != NULL);  // must find something
	}

	// fill in return data
	key = pAssoc->key;
	value = pAssoc->value;
}

//---------------------------------------------------------------------

template <class KEY, class VALUE, class HASH>
MapDumpable<KEY, VALUE,HASH>::
MapDumpable ()
: Map<KEY, VALUE, HASH>()
{

}
#define MAP_MAGIC 100001
template <class KEY, class VALUE, class HASH>
void MapDumpable<KEY, VALUE, HASH>::
binDump (BinOutput& bo)
{
  Magic magic(MAP_MAGIC);
  magic.binDump(bo);
  bo.writeInt(hashSize_);
  bo.writeInt(count_);
  if(!hashSize_)
    return;
  for(int i=0; i<hashSize_; i++)
  {
    Assoc<KEY, VALUE> * pAssoc = hashTable_[i];
    while(pAssoc)
    {
      bo.writeChar(1);
      bo.writeUnsignedInt(pAssoc->hash);
      pAssoc->key.binDump(bo);
      pAssoc->value.binDump(bo);
      pAssoc = pAssoc->next;
    }
    bo.writeChar(0);
  }
}

template <class KEY, class VALUE, class HASH>
Bool MapDumpable<KEY, VALUE, HASH>::
binInit (BinInput& bi)
{
  Magic magic(MAP_MAGIC);
  if(!magic.binInit(bi))
    return FALSE;

  removeAll();

  hashSize_ = bi.readInt();
  count_ = bi.readInt();
  if(!hashSize_)
    return TRUE;
  hashTable_ = new Assoc<KEY,VALUE>*[hashSize_];
  memset(hashTable_,0,sizeof(Assoc<KEY,VALUE>*) * hashSize_);		
  char isElem;
  KEY key;
  unsigned int hash = 0;
  VALUE value;
  Assoc<KEY, VALUE> * pAssoc = NULL, *pAssocNew = NULL;
  for(int i=0; i<hashSize_; i++)
  {
    pAssoc = NULL;
    while(isElem = bi.readChar())
    {
      Assert(isElem == 1);
      hash = bi.readUnsignedInt();
      key.binInit(bi);
      value.binInit(bi);
      pAssocNew = new Assoc<KEY, VALUE>(hash, key, value);
      if (!pAssoc)
        pAssoc = hashTable_[i] = pAssocNew;
      else
        pAssoc = pAssoc->next = pAssocNew;
    }
  }
  return TRUE;
}

