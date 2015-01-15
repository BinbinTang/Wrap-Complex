#ifndef __RGI_MAP_H__
#define __RGI_MAP_H__

#include <basic/binio.h>

typedef void* MAPPOS;
#define MAP_START_POS ((MAPPOS) -1)

template <class KEY, class VALUE>
class Assoc
{
public:
	Assoc(unsigned int h, KEY k);
  Assoc(unsigned int h, KEY k, VALUE v);
	unsigned int hash;
	KEY key;
	VALUE value;
	Assoc<KEY,VALUE> *next;
};

template <class KEY>
class HashFunc
{
public:
	int operator () (const KEY& key) const { return (int) key; }
};

template <class KEY, class VALUE, class HASH=HashFunc<KEY> >
class Map
{
public:
	Map();
	~Map();

	int isEmpty() const;
	int size() const;

	int lookUp(KEY key, VALUE& value) const;

	VALUE& operator[](KEY key);
	void setAt(KEY key, VALUE value);
	
	int removeKey(KEY key, VALUE &value);
	void removeAll();

	MAPPOS getStartPos() const;
	void getNext(MAPPOS &pos, KEY &key, VALUE &value) const;
	void getAt(MAPPOS pos, KEY &key, VALUE &value) const;

	int getHashSize() const;
	void initHashTable(unsigned int size, int create = 1);
protected:
	Assoc<KEY,VALUE>* getAssocAt_(KEY& key, unsigned int &hash) const;
	Assoc<KEY,VALUE>** hashTable_;
	unsigned int hashSize_;
	int count_;
	HASH hashFunc_;
};

template <class KEY, class VALUE, class HASH>
class MapDumpable : public Map<KEY, VALUE, HASH>
{
public:
  MapDumpable();
  ~MapDumpable (void) { }
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
};

#include <basic/map.hpp>

#endif //MAP_H
