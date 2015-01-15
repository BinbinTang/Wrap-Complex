inline OrIndex::
OrIndex (Index pathId, Bool reversed)
: rep_ (pathId << 1 | (reversed? 1 : 0))
{

}

inline OrIndex::
OrIndex (const OrIndex& other)
: rep_ (other.rep_)
{

}

inline Bool OrIndex::
version (void) const
{
  return rep_ & 01;
}

inline Index OrIndex::
index (void) const
{
  return rep_ >> 1;
}

inline OrIndex OrIndex::
sym (void) const
{
  return OrIndex (rep_ >> 1, !(rep_ & 01));
}

inline unsigned OrIndex::
getIntRep (void) const
{
  return rep_;
}

inline void OrIndex::
setIntRep (unsigned rep)
{
  rep_ = rep;
}

inline Bool OrIndex::
operator!= (const OrIndex& other) const
{
  return (rep_ != other.rep_);
}

inline Bool OrIndex::
operator== (const OrIndex& other) const
{
  return (rep_ == other.rep_);
}

inline Bool OrIndex::binInit (BinInput& bi)
{
  rep_ = bi.readUnsignedInt(); return TRUE;
}

inline void OrIndex::binDump (BinOutput& bo)
{
  bo.writeUnsignedInt(rep_);
}

inline OrPath::
OrPath (Index pathId, Bool reversed) 
: OrIndex(pathId, reversed)
{

}

inline OrPath::
OrPath (const OrIndex& other)
: OrIndex(other)
{

}

inline OrPath OrPath::
sym (void) const
{
  return OrPath (rep_ >> 1, !(rep_ & 01));
}

inline OrPos::
OrPos (Index pathId, Bool reversed) 
: OrIndex(pathId, reversed)
{

}

inline OrPos::
OrPos (const OrIndex& other)
: OrIndex(other)
{

}

inline OrPos OrPos::
sym (void) const
{
  return OrPos (rep_ >> 1, !(rep_ & 01));
}