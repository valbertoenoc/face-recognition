#ifndef FEATURES_H
#define FEATURES_H


#include <stdio.h>

struct Features
{
	public:
		Features();
		Features(const Features&);
		Features(Features*);
		~Features();
		
		void featCat(Features&);
		void featSum(Features&);

		int operator[] (const int&);

		int size;
		int label;
		float* data;
};
#endif