#ifndef KNN_H
#define KNN_H

#include <stdio.h>
#include <vector>
#include <math.h>
#include "Features.h"

class KNN
{
public:
	KNN(void);
	KNN(int k, int numOfClass, int numOfFeatures);
	~KNN(void);

	void setup(int k, int numOfClass, int numOfFeatures);
	void setDistanceOrder(int o);
	void setMinDist(float minDist);
	float* getDistResult(void);
	float getMinDist(void);
	bool loadDatabase(std::vector <Features>& features);
	bool addToDatabase(std::vector <Features>& features);
	bool loadDatabase(char *databaseFilename);

	void normalizeDataBase(void);
	void normalize(Features& sample);
	int recognize(Features sample);
	int partialRecognize(Features sample);
	int recognize(Features sample, int label);
	int partialRecognize(Features sample, int label);
	float crossValidation(float trainingPercentage);
	void enableNormalization(bool norm);
	
		
private:
	bool checkIndex(int i, int *indexVec);
	float computeDistance(int i, Features sample);
	float computePartialDistance(Features sample, Features data, float max);

	int k;
	int numOfClass;
	int numOfFeatures;
	int distanceOrder;
	bool norm;
	std::vector <Features> database;

	float *max, *min;
	float *distResult;
	float minDist;
};
#endif