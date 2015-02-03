#ifndef _MEMTOOL_H_
#define _MEMTOOL_H_

#define SAFE_RELEASE( p )	if (p){ p->Release(); p = NULL; }
#define SAFE_DELETE( p )	if (p){ delete(p); p = NULL; }
#define SAFE_DELETE_ARRAY( p )	if (p){ delete [] (p); p = NULL; }


#endif//_MEMTOOL_H_