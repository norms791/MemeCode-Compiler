#if !defined(MEMECODE_CUADRUPLE_H__)
#define MEMECODE_CUADRUPLE_H__

#include "Scanner.h"
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <stack>
#include <queue>

using namespace std;

namespace MemeCode{

	class Quadruple {
		public:
			int first;
			int secnd;
			int third;
			int fourth;
			
			Quadruple(int first, int second, int third, int fourth) {
				this->first = first;
				this->secnd = second;
				this->third = third;
				this->fourth = fourth;
			}		
	};

};

#endif
