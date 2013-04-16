#include "be_alloc.h"
#include <math.h>

namespace	be{
	mem_pool	alloc::m_pool[alloc::BUCK_NUM]	=	{mem_pool(8),
	mem_pool(16),mem_pool(24),mem_pool(32),mem_pool(64),mem_pool(72),
	mem_pool(80),mem_pool(88),mem_pool(96),mem_pool(104),mem_pool(112),
	mem_pool(120),mem_pool(128),};
	class alloc_initor
	{
	public:
		alloc_initor()
		{
			for(long	i	=	0;	i	<	alloc::BUCK_NUM;	++i)
			{
				long	sz	=	alloc::MIN_SIZE	*	(i	+	1);
				long	cap	=	(long)(mem_pool::EXPANSION_SIZE	/	sqrt(double(i+1)));
				long	exp	=	cap	?	cap	:	1;
				//alloc::m_pool[i]	=	new	mem_pool(sz,	cap,	exp);
			}
			return;
		}

	protected:
	private:
	};

	alloc_initor	g_allocinit;


};