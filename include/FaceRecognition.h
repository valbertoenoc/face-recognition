#ifndef FACERECOGNITION_H
#define FACERECOGNITION_H

#include "cv.h"
#include "highgui.h"
#include "LBP.h"
#include "KNN.h"
#include <vector>
#include <fstream>
#include <string>
#include <math.h>

#define LOG_TAG "NDK" // text for log tag
#define LOG_DEBUG "NDK"



#define resWidth 75//189
#define resHeight 100//252

using namespace std;

class FaceRecognition
{
	public: 
		FaceRecognition(void);
		FaceRecognition(string cascadePath);
		FaceRecognition(IplImage *image, int label = -1);
		~FaceRecognition(void);
	 
		//configuration functions
		void setProcessedImage(void);
		void setImage(IplImage *image, int label = -1);
		void setSizeResized(int widthResized, int heightResized);
		IplImage* getImage(void);
		IplImage* getProcessedImage(void);

		//Detection functions
		bool detectFace(bool isFaceImg = false);

		//Preprocess functions
		void enableElipse(bool enable);
		void enableEqualization(bool enable);
		void enableDoGFilter(bool enable);
		void enableGammaCorrection(bool enable);
		void setGamma(float gamma = 0.2);
		void setDoGFilter(int size1 = 27, int size2 = 9, float sigma1 = 6, float sigma2 = 0.25);
		void equalizeHistogram(void);
		void applyGammaCorrection(void);
		void DogFilter(void);


		//General Features Functions
        void printFeatures(const char* filename, int label = -1, bool printLast = false);
		void loadFeatures(char* filename, int numOfFeatures);
		void clearFeatures(void);
		void addFeatures(Features);
		void addToDatabase(std::vector <Features>& features, int label);
		std::vector <Features>& getDatabase(void);
		Features getFeatures(void);


		
		//LBP Features function
		void computeLBP(int type);
        void setLBPimage(IplImage *image, int label = -1);
		void setLBPSize(int nRow = 1, int nCols = 1);
		void setWeights(int x, int y, int weight);
		

		//KNN Classifier function
		void setKNN(int numOfClass, int numOfFeatures, bool norm, int k = 3, int dist = 2);
		void setKNN(char* filename, int numOfClass, bool norm, int k = 3, int dist = 2);
		void setMinDist(float minDist = 9);
		float getMinDist(void);
		float* getDistResult(void);
		int recognizeKNN();
		int recognizeKNN(Features F);
		bool recognizeKNN(int label);
		bool recognizeKNN(Features F, int label);			
		
	private:		
		IplImage *frame;
		IplImage *processedFace;
		int widthResized, heightResized;
		
		LBP lbpFeatures;
		KNN knn;

		//Preprocess parameters
		float gamma;
		bool elipse, DoG, equaliz, gammaCorrection;
		int DogSize1, DogSize2;
		float DogSigma1, DogSigma2;

		std::vector <Features> features;
		std::vector <Features> auxFeat;
		std::vector <IplImage*> detectedFaces;
		std::vector <int> labels;
		std::vector <float*> auxData;

		CvHaarClassifierCascade *faceClassifier;
		CvMemStorage *storageFace;
};

#endif
