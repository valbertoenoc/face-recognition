#ifndef FACE_FEATURES_H
#define FACE_FEATURES_H

#include "cv.h"
#include "highgui.h"
#include "Features.h"
#include <stdio.h>

class LBP
{
	public:
	  enum{LBP_81, LBP_82, LBP_162};

		LBP(void);
		LBP(IplImage* img);
		~LBP(void);

		//Configuration functions
		void setImage(IplImage* img);
		void setLabel(int label);
		int getLabel(void);

		//LBP functions
		void computeLBPFeatures(int type);
		void printLBPFeatures(int label);
		void setLBPSize(int nRow, int nCol);
		void setWeights(int i, int j, int weight);
		int getNumOfLBPFeatures(void);
		Features getLBPFeatures(void);
		char* itoa(int value, char* str, int radix);

		//Image functions
		IplImage* getImage(void);

	private:
		int type;
		
		//LBP functions
		void computeNumOfLBPFeatures(int kernelSize);
		void computeLBP81(void);
		void computeLBP82(void);
		void computeLBP162(void);

		//LBP variables
		int nRow;
		int nCol;
		int numOfLBPFeatures;
		int	*indexLBP;
		int *lbpWeight;
		Features feat;
		
		//Image variables
		IplImage *img;
};

#endif
