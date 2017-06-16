/**
 * @Class LBP
 * @details Preprocessing and calculation of face features.
 */
#include "LBP.h"
#include <iostream>

 /**
 * @brief Empty constructor
 * @details Initializes the class
 */
LBP::LBP(void)
{
	img = NULL;
	indexLBP = NULL;
	lbpWeight = NULL;

	type = 0;
  numOfLBPFeatures = -1;
	nCol = 1;
	nRow = 1;
}

/**
 * @brief Constructor
 * @details Initializes the class
 * @param image(IplImage*) image to be set on LBP class
 */
LBP::LBP(IplImage* image)
{
	type = 0;
	numOfLBPFeatures = -1;
	
	indexLBP = NULL;
	img = NULL;
	lbpWeight = NULL;

	setImage(image);
	nCol = 1;
	nRow = 1;
}

/**
 * @brief Destructor
 * @details 
 */
LBP::~LBP(void)
{
	if(indexLBP != NULL)
		delete [] indexLBP;
		
	if(img != NULL)
		cvReleaseImage(&img);

		//FILE *featuresFile;
	if(lbpWeight != NULL)
		delete []lbpWeight;
}

/**
 * @brief Converts decimal number selected base number
 * @details On this code, it is being used strictly to convert decimal number to binary number
 *
 * @param value(int) decimal value to be converted
 * @param str(char*) vector of char to be filled with binary number
 * @param radix() base to which the decimal value will be converted
 */
char* LBP::itoa(int value, char* str, int radix)
{

    static char dig[] = "0123456789" "abcdefghijklmnopqrstuvwxyz";

    int n = 0, neg = 0;
    unsigned int v;

		
		if (radix == 10 && value < 0) 
		{
			value = -value;
			neg = 1;
    }

    v = value;
		do 
		{
			str[n++] = dig[v%radix];
			v /= radix;
    }while (v);

    if(neg)
			str[n++] = '-';

    str[n] = '\0';

//		for (p = str, q = p + (n-1); p < q; ++p,    q)
//            c = *p, *p = *q, *q = c;

    return str;
}

/**
 * @brief Sets face image on class
 * @details 
 *
 * @param image(IplImage*) image to be set on LBP class
 */
void LBP::setImage(IplImage* image)
{
	if(this->img != NULL)
		cvReleaseImage(&this->img);

	this->img = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
	cvCopy(image, this->img);
}

/**
 * @brief Sets label to identify face
 * @details
 *
 * @param lab(int) integer value used to associate face with
 */
void LBP::setLabel(int lab)
{
	feat.label = lab;
}

/**
 * @brief Returns label set on feat object
 * @details
 *
 * @param
 * @return label(int) label to be returned
 */
int LBP::getLabel(void)
{
	return feat.label;
}


/**
 * @brief Returns number of LBP features computed
 * @details
 *
 * @return numOfLBPFeatures(int) number of LBP features
 */
int LBP::getNumOfLBPFeatures(void)
{
	return numOfLBPFeatures;
}

/**
 * @brief Computes number of LBP features depending on LBP type
 * @details LBP features can be computed in three different types, LBP_(8,1), LBP_(8,2), LBP_(16,2)
 *
 * @param type(int) selects one of available LBP types to be computed
 */
void LBP::computeNumOfLBPFeatures(int type)
{
	int lbpKernelOrder;
	if(type == LBP_81 || type == LBP_82)
	{
		lbpKernelOrder = 8;
	}
	else if(type == LBP_162)
	{
		lbpKernelOrder = 16;
	}

	//FILE *uniTest = fopen("uniformTest.txt", "w");//DEBUG
	int i, j, transitions = 0;
	char* bin = NULL;
	int uniform = 0;
	
	if(indexLBP != NULL)
		delete [] indexLBP;
	indexLBP = new int[int(pow(2.0,lbpKernelOrder))];
	for(i = 0; i < pow(2.0,lbpKernelOrder); i++)
		indexLBP[i] = -1;
	
	if(bin != NULL)
		delete [] bin;
	bin = new char[lbpKernelOrder];

	for(i = 0; i < pow(2.0,lbpKernelOrder); i++)
	{
		itoa(i,bin,2); 

		for(int x = 0; x < lbpKernelOrder; x++)
			if(bin[x] != 48 && bin[x] != 49)
				bin[x] = 48;
	  
		
		for(j = 0; j < lbpKernelOrder; j++)
		{
			if(j == lbpKernelOrder - 1)
			{
				if(bin[j] != bin[0])
					transitions++;
			}
			else if(bin[j] != bin[j+1])
				transitions++;
		}
		if(transitions <=2)
		{
			indexLBP[i] = uniform;
		//fprintf(uniTest, "%d \n", i);//DEBUG
			uniform++;
		}

		transitions = 0;
		
	}
	//fclose(uniTest);//DEBUG

	for(i = 0; i < pow(2.0,lbpKernelOrder); i++)
	{
		if(indexLBP[i] == -1)
			indexLBP[i] = uniform;
	}
	numOfLBPFeatures = uniform;
}

/**
 * @brief Computes LBP type (8,1)
 * @details
 *
 * @param nRow(int) number of rows to devide image subspace to compute LBP
 * @param nCol(int) number of columms to devide image subspace to compute LBP
 */
void LBP::computeLBP81()
{
	unsigned i, j, x, y, m, n;
	
	int kernelSize = 3;
	int kernelCenter = 1;
	computeNumOfLBPFeatures(LBP_81);
	
	feat.size = (numOfLBPFeatures+1) * nRow * nCol;

	if(feat.data != NULL)
		delete [] feat.data;
	feat.data = new float[feat.size];

	for(i = 0; i < feat.size; i++)
		feat.data[i] = 0;

	int subSpaceHeight = img->height / nRow;
	int subSpaceWidth = img->width / nCol;
	
	int lbpCode = 0;
	int lbpAux = 1;

	//calculating LBP Features/////////////////////////////
	
	//LBP CALC
	int cont = -1;
	int p, center;
	for(m = 0; m < img->height; m += subSpaceHeight)
		for(n = 0; n < img->width; n += subSpaceWidth)
		{
			cont++;
			
			//cvLine(img, cvPoint(n,0), cvPoint(n,img->height), CV_RGB(0,0,0), 3);//DEBUG
			//cvLine(img, cvPoint(0,m), cvPoint(img->width,m), CV_RGB(0,0,0), 3); //DEBUG
			
			for(i = m; i < (subSpaceHeight + m) && (i < img->height - kernelSize); i++)
				for(j = n; j < (subSpaceWidth + n)  && (j < img->width - kernelSize); j++)
				{
					
					center = cvGetReal2D(img, i + kernelCenter, j + kernelCenter);	
					for(y = i; y < (i + kernelSize); y++)
						for(x = j; x < (j + kernelSize); x++)
						{
							if(!(y == i+kernelCenter && x == j+kernelCenter))
							{
							  p = cvGetReal2D(img, y, x);		 
								//////////////////////
								/*
								if(lbpWeight[cont] == 0)
									cvSet2D(img, y, x, CV_RGB(0,0,0));
								if(lbpWeight[cont] == 4)
									cvSet2D(img, y, x, CV_RGB(255,255,255));
								*/
								///////////////////////
								if(p >= center)
							  	lbpCode += lbpAux;	
								lbpAux *= 2;
							}
						}
					feat.data[indexLBP[lbpCode] + cont*(numOfLBPFeatures+1)]++;
					lbpAux = 1;
					lbpCode = 0;
				}
				for(int wIndex = 0; wIndex < numOfLBPFeatures + 1; wIndex++)
					feat.data[wIndex + cont] *= lbpWeight[cont];
				
	}

}

/**
 * @brief Computes LBP type (8,2)
 * @details
 *
 * @param nRow(int) number of rows to devide image subspace to compute LBP
 * @param nCol(int) number of columms to devide image subspace to compute LBP
 */
void LBP::computeLBP82()
{
	unsigned i, j, m, n;
	
	int kernelSize = 5;
	int kernelCenter = 2;
	numOfLBPFeatures = 58;
	int indexLBP8[] = {0, 1, 2, 3, 4, 5, 58, 6, 7, 8, 58, 9, 58, 58, 58, 10, 11, 12, 58, 13, 58, 58, 58, 14, 58, 58, 58, 58, 58, 58, 58, 
		                15, 16, 17, 58, 18, 58, 58, 58, 19, 58, 58, 58, 58, 58, 58, 58, 20, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 
										58, 58, 58, 21, 22, 23, 58, 24, 58, 58, 58, 25, 58, 58, 58, 58, 58, 58, 58, 26, 58, 58, 58, 58, 58, 58, 58, 58, 58, 
										58, 58, 58, 58, 58, 58, 27, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 
										58, 58, 58, 58, 58, 58, 58, 58, 58, 28, 29, 30, 58, 31, 58, 58, 58, 32, 58, 58, 58, 58, 58, 58, 58, 33, 58, 58, 58, 
										58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 34, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 
										58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 35, 36, 37, 58, 38, 58, 58, 58, 39, 58, 58, 58, 58, 58, 
										58, 58, 40, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 41, 42, 43, 58, 44, 58, 58, 58, 45, 58, 58, 
										58, 58, 58, 58, 58, 46, 47, 48, 58, 49, 58, 58, 58, 50, 51, 52, 58, 53, 54, 55, 56, 57};
	
feat.size = (numOfLBPFeatures+1) * nRow * nCol;

	if(feat.data != NULL)
		delete [] feat.data;
	feat.data = new float[feat.size];

	memset(feat.data, 0, sizeof(float)*feat.size);
	
	int subSpaceHeight = img->height / nRow;
	int subSpaceWidth = img->width / nCol;

	//calculating LBP Features/////////////////////////////
	int cont = -1;
	int lbpCode = 0;
	int center;
	int mean;
	unsigned int height = img->height;
	unsigned int width = img->width;
	
	int *imgMat = new int[height*width];
	for(i = height*width - 1; i > 0; --i)
		imgMat[i] = cvGetReal1D(img, i);

	for(m = 0; m < height; m += subSpaceHeight)
		for(n = 0; n < width; n += subSpaceWidth)
		{
			cont++;			
			//cvLine(img, cvPoint(n,0), cvPoint(n,img->height), CV_RGB(0,0,0), 3);//DEBUG
			//cvLine(img, cvPoint(0,m), cvPoint(img->width,m), CV_RGB(0,0,0), 3); //DEBUG
			
			for(i = m; i < (unsigned)(subSpaceHeight + m) && (i < height - kernelSize); i++)
				for(j = n; j < (unsigned)(subSpaceWidth + n)  && (j < width - kernelSize); j++)
				{
					center = *(imgMat+((i+kernelCenter)*width + (j+kernelCenter)));

					if(*(imgMat+((i+0)*width + (j+2))) >= center)
						lbpCode += 2;
					if(*(imgMat+((i+2)*width + (j+0))) >= center)
						lbpCode += 8;
					if(*(imgMat+((i+2)*width + (j+4))) >= center)
						lbpCode += 16;
					if(*(imgMat+((i+4)*width + (j+2))) >= center)
						lbpCode += 64;
					
					mean = *(imgMat+((i+0)*width + (j+0))) + *(imgMat+((i+0)*width + (j+1))) +
						     *(imgMat+((i+1)*width + (j+0))) + *(imgMat+((i+1)*width + (j+1)));
					mean /= 4;
					if(mean >= center)
						lbpCode += 1;
					
					mean = *(imgMat+((i+0)*width + (j+3))) + *(imgMat+((i+0)*width + (j+4))) +
						     *(imgMat+((i+1)*width + (j+3))) + *(imgMat+((i+1)*width + (j+4)));
					mean /= 4;
					if(mean >= center)
						lbpCode += 4;

					mean = *(imgMat+((i+3)*width + (j+3))) + *(imgMat+((i+3)*width + (j+4))) +
						     *(imgMat+((i+4)*width + (j+3))) + *(imgMat+((i+4)*width + (j+4)));
					mean /= 4;
					if(mean >= center)
						lbpCode += 128;

					mean = *(imgMat+((i+3)*width + (j+0))) + *(imgMat+((i+4)*width + (j+0))) +
						     *(imgMat+((i+3)*width + (j+1))) + *(imgMat+((i+3)*width + (j+1)));
					mean /= 4;
					if(mean >= center)
						lbpCode += 32;

					///////////////////////
					//if(lbpWeight[cont] == 0)
					//	cvSet2D(img, i, j, CV_RGB(0,0,0));
					///////////////////////
                    int a = indexLBP8[lbpCode] + cont*(numOfLBPFeatures+1);
					feat.data[a]++;
					lbpCode = 0;
				}
				//for(int wIndex = 0; wIndex < numOfLBPFeatures + 1; wIndex++)
					//feat.data[wIndex + cont] *= lbpWeight[cont];
		}	
		
		delete [] imgMat;
}

/**
 * @brief Computes LBP type (16,2)
 * @details
 *
 * @param nRow(int) number of rows to devide image subspace to compute LBP
 * @param nCol(int) number of columms to devide image subspace to compute LBP
 */ 
void LBP::computeLBP162()
{
	unsigned i, j, x, y, m, n;
	
	int kernelSize = 5;
	int kernelCenter = 2;
	computeNumOfLBPFeatures(LBP_162);
	
	feat.size = (numOfLBPFeatures+1) * nRow * nCol;

	if(feat.data != NULL)
		delete [] feat.data;
	feat.data = new float[feat.size];

	for(i = 0; i < feat.size; i++)
		feat.data[i] = 0;

	int subSpaceHeight = img->height / nRow;
	int subSpaceWidth = img->width / nCol;
	
	int lbpCode = 0;
	int lbpAux = 1;

	//calculating LBP Features/////////////////////////////
	//LBP CALC
	int cont = -1;
	int p, center;
	int debug = 0;
	for(m = 0; m < img->height; m += subSpaceHeight)
		for(n = 0; n < img->width; n += subSpaceWidth)
		{
			cont++;
			
			cvLine(img, cvPoint(n,0), cvPoint(n,img->height), CV_RGB(0,0,0), 3);//DEBUG
			cvLine(img, cvPoint(0,m), cvPoint(img->width,m), CV_RGB(0,0,0), 3); //DEBUG
	
			for(i = m; i < (subSpaceHeight-kernelSize) + m; i++)
				for(j = n; j < (subSpaceWidth-kernelSize) + n; j++)
				{
					debug = 0;
					center = cvGetReal2D(img, i + kernelCenter, j + kernelCenter);	
					for(y = i; y < (i + kernelSize); y++)
						for(x = j; x < (j + kernelSize); x++)
						{
							if((y <= i+kernelCenter - 1 || y >= i+kernelCenter + 1) &&
								 (x <= j+kernelCenter - 1 || x >= j+kernelCenter + 1))
							{
								debug++;
							  p = cvGetReal2D(img, y,x);		  
								if(p >= center)
							  	lbpCode += lbpAux;	
								lbpAux *= 2;
							}
								///////////////////////
								if(lbpWeight[cont] == 0)
									cvSet2D(img, y, x, CV_RGB(0,0,0));
								///////////////////////
						}				
					feat.data[indexLBP[lbpCode] + cont*(numOfLBPFeatures+1)]++;
					lbpAux = 1;
					lbpCode = 0;
				}

	 }
}

/**
 * @brief Returns computed LBP features array
 * @details
 *
 */
Features LBP::getLBPFeatures(void)
{
	return feat;
}


/**
 * @brief selects LBP type and calls function to compute it
 * @details Selects the LBP type (8,1; 8,2; 16,2) and sets the number of subspaces 
 * @param type(int) LBP type
 * @param nRow(int) number of subspace rows to divide image
 * @param nCol(int) number of subspace columns to divide image
 */
void LBP::computeLBPFeatures(int type)
{
	switch(type)
	{
		case LBP_81: computeLBP81();
								break;
		case LBP_82: computeLBP82();
								break;
		case LBP_162: computeLBP162();
								break;
	}	
}

/**
 * @brief returns processed image
 *
 * @return (Iplimage*) processed image. 
 */
IplImage* LBP::getImage(void)
{
	return img;
}

/**
 * @brief Sets number of subspaces to divide face image.
 * @details LBP is computed on a set of subspaces of the face image and then concatanated. This function difines the number of subspaces.
 *
 * @param nRow(int) number of divisions to make on the image height
 * @param nCol(int) number of divisions to make on the image width
 */
void LBP::setLBPSize(int nRow, int nCol)
{
	this->nRow = nRow;
	this->nCol = nCol;
	if(lbpWeight != NULL)
		delete []lbpWeight;

	lbpWeight = new int[nRow*nCol];
	for(int i = 0; i < nRow*nCol; i++)
		lbpWeight[i] = 1;
	
}

/**
 * @brief Sets weight values to apply on LBP histogram computed on specific subspace.
 * @details Subspace is selected by coordinates i, j.
 *
 * @param i(int) position on y axis where subspace is located
 * @param j(int) position on x axis where subspace is located
 * @param weight(int) number of divisions to make on the image height
 */
void LBP::setWeights(int i, int j, int weight)
{
	lbpWeight[i*nRow + j] = weight;
}

