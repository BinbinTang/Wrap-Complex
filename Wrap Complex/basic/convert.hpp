template<class T>
void decodeInteger (T *target, char *source, size_t size, size_t count)
{
	while (count-- > 0)
	{
		T result = ((T)source[0] >= 0) ? (T)0 : (T)-1;
		for (size_t i=0; i<size; i++)
		{
			result <<= 8;
			char next = source[i];
			unsigned char tmp = (unsigned char) next;
			T add = (T) tmp;
			result |= add;
		}
		*(target++) = result;
		source += size;
	}
}

template<class T>
void decodeFloating (T *target, char *source, size_t size, size_t count)
{
	while (count-- > 0)
	{
		char *tmp = (char *)target;
		for (size_t i=0; i<size; i++)
		{
#if defined(BIG_ENDIAN)
			tmp[i] = source[i];
#else
			tmp[i] = source[size-1 - i];
#endif
		}
		target++;
		source += size;
	}
}

template<class T>
void encodeInteger (char *target, T *source, size_t size, size_t count)
{
	while (count-- > 0)
	{
		T next = *source;
		for (size_t i=0; i<size; i++)
		{
			target[size-1 - i] = next & ((1<<8) - 1);
			next >>= 8;
		}
		target += size;
		source++;
	}
}

template<class T>
void encodeFloating (char *target, T *source, size_t size, size_t count)
{
	while (count-- > 0)
	{
		char *tmp = (char *)source;
		for (size_t i=0; i<size; i++)
		{
#if defined(BIG_ENDIAN)
			target[i] = tmp[i];
#else
			target[i] = tmp[size-1 - i];
#endif
		}
		target += size;
		source ++;
	}
}
