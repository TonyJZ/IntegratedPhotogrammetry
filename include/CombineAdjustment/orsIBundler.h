#ifndef _ORS_INTERFACE_BUNDLER_H_
#define _ORS_INTERFACE_BUNDLER_H_

class orsIBundler
{
public:

	virtual ~orsIBundler(){};

	virtual int bundle(void *adata){return 0;};
};

#endif