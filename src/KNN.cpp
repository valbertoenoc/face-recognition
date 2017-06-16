/**
 * @Class KNN
 * @details Distance classifier algorithm.
 */

#include <iostream>//DEBUG
#include <string.h>
#include "KNN.h"

#define square(x) ((x)*(x))
#define MAX 3.402823466e+38F // FLT_MAX
#define MIN 1.175494351e-38F // FLT_MIN


/**
 * @brief Constructor
 * @details Initializes the class
 */
KNN::KNN(void)
{
	k = 3;
	numOfClass = -1;
	numOfFeatures = -1;
	distanceOrder = 2;
	max = NULL;
	min = NULL;
	minDist = 9;
	norm = false;

	distResult = new float[2*k];
}

/**
 * @brief Constructor
 * @details Initializes the class
 * @param k(int) integer value to specify number of neighbors
 * @param numOfClass(int) number of classes to be classified
 * @param numOfFeatures(int) number of features used to represent class
 */
KNN::KNN(int k, int numOfClass, int numOfFeatures)
{
	this->k = k;
	this->numOfClass = numOfClass;
	this->numOfFeatures = numOfFeatures;
	distanceOrder = 2;
	max = new float[numOfFeatures];
	min = new float[numOfFeatures];
	minDist = 9;
	norm = false;

	distResult = new float[2*k];
}

/**
 * @brief Destructor
 * @details 
 */
KNN::~KNN(void)
{
	if(max != NULL)
		delete [] max;
	
	if(min != NULL)
		delete [] min;

	if(distResult != NULL)
		delete []distResult;

	if(database.size() > 0)
		database.clear();
}

/**
 * @brief Sets classifier configuration parameters
 *
 * @param k(int) integer value to specify number of neighbors
 * @param numOfClass(int) number of classes to be classified
 * @param numOfFeatures(int) number of features used to represent class
 */
void KNN::setup(int k, int numOfClass, int numOfFeatures)
{
	this->k = k;
	this->numOfClass = numOfClass;
	this->numOfFeatures = numOfFeatures;

	if(max != NULL)
		delete [] max;
	max = new float[numOfFeatures];
	
	if(min != NULL)
		delete [] min;
	min = new float[numOfFeatures];

	if(distResult != NULL)
		delete []distResult;

	distResult = new float[2*k];
}

/**
 * @brief Sets distance order to be computed on classifier
 * @details o = 2 means euclidean distance between vectors
 *
 * @param o(int) integer value to specify distance order
 */
void KNN::setDistanceOrder(int o)
{
  distanceOrder = o;
}

/**
 * @brief Loads database of objects to be classified
 * @details 
 *
 * @param features(vector<Features>) vector of Features to represent one object
 * @return (bool) bool value to identify class
 */
bool KNN::loadDatabase(std::vector <Features>& features)
{
	if(numOfFeatures < 1)
	{
		printf("Number of features not set.\n");
		return false;
	}
	if(database.size() > 0)
		database.clear();

	database = features;
	
	for(int i = 0; i < numOfFeatures; i++)										
	{
		max[i] = database[0][i];
		min[i] = database[0][i];
	}
	
	for(int j = 1; j < database.size(); j++)
		for(int i = 0; i < numOfFeatures; i++)										
		{
			if(min[i] > database[j][i])
				min[i] = database[j][i];
			if(max[i] < database[j][i])
				max[i] = database[j][i];
		}
    
    return true;
}

/**
 * @brief Loads database of objects to be classified
 * @details 
 *
 * @param features(vector<Features>) vector of Features to represent one object
 * @return (bool) bool value to identify class
 */
bool KNN::addToDatabase(std::vector <Features>& features)
{
	if(numOfFeatures < 1)
	{
		printf("Number of features not set.\n");
		return false;
	}

	for(int i = 0; i < features.size(); i++)
		database.push_back(features[i]);
	
	for(int j = 0; j < features.size(); j++)
		for(int i = 0; i < numOfFeatures; i++)										
		{
			if(min[i] > features[j][i])
				min[i] = features[j][i];
			if(max[i] < features[j][i])
				max[i] = features[j][i];
		}
    return true;
}

/**
 * @brief Loads database of objects to be classified from file
 * @details 
 *
 * @param databaseFilename(char*) path name of database file
 * @return (bool) bool value to confirm if class was recognized correctly
 */
bool KNN::loadDatabase(char *databaseFilename)
{
	if(numOfFeatures < 1)
	{
		printf("Number of features not set.\n");
		return false;
	}

	FILE *databaseFile = fopen(databaseFilename, "r");
	if(!databaseFile)
	{
		printf("file not found.\n");
		return false;
	}

	float tempVal;
	int lab;
	bool flag = true;
		
	Features aux;
	aux.size = numOfFeatures;
	aux.data = new float[numOfFeatures];

	for(int i = 0; i < numOfFeatures; i++)										
	{
		max[i] = MIN;
		min[i] = MAX;
		flag = false;
	}
	while(!feof(databaseFile))																	
	{
		for(int i = 0; i < numOfFeatures; i++)										
		{
			fscanf(databaseFile, "%f", &tempVal);
			aux.data[i] = tempVal;
				
			if(min[i] > tempVal)
				min[i] = tempVal;
			if(max[i] < tempVal)
				max[i] = tempVal;
		}
		fscanf(databaseFile, "%d", &lab);
		aux.label = lab;
		database.push_back(&aux);
	}
	fclose(databaseFile);
	return true;	
}

/**
 * @brief Computes minimum distance of sample Features object to every object on database
 * @details 
 *
 * @param k(int) number of neighbors to keep minimum distance
 * @param sample(Features) vector of features that represents object to be recognized
 * @return d(float) float value to identify class
 */
float KNN::computeDistance(int k, Features sample)
{
	unsigned i;
	double vectorSum = 0;
	float dif, d1, d2;

	for(i = 0; i < numOfFeatures; i++)
	{
		d1 = database[k].data[i];
		d2 = sample.data[i];
		dif = d2 - d1;
		vectorSum += pow(dif, distanceOrder);
	}
	
	double exp = 1.0/distanceOrder;
	float d = pow(vectorSum, exp);
	d = fabs(d);

	return d;
}

/**
 * @brief Computes minimum distance of sample Features object to every object on database
 * @details 
 *
 * @param k(int) number of neighbors to keep minimum distance
 * @param sample(Features) vector of features that represents object to be recognized
 * @return d(float) float value to identify class
 */
float KNN::computePartialDistance(Features sample, Features data, float max)
{
	double vectorSum = 0;
	for(unsigned int i = 0; i < (unsigned) numOfFeatures; i++)
	{
		vectorSum += square(sample.data[i] - data.data[i]);
		if(vectorSum > max*max)
		{
			vectorSum = MAX;
			return(float(vectorSum));
		}
	}
	vectorSum = sqrt(vectorSum);
	return(float(vectorSum));
}
/**
 * @brief Recognizes sample object calculating minimum distance of it to every object on database
 * @details 
 *
 * @param sample(Features) vector of features that represents object to be recognized
 * @param label(int) integer value that represents user to be identified
 * @return (bool) bool value to confirm if class was recognized correctly
 */
int KNN::partialRecognize(Features sample, int label)
{
	unsigned int i;
	float dist;
	float min = MAX;
	
	if(norm)
		normalize(sample);

	for(i = 0; i < database.size(); i++)
	{
		min = MAX;
		if(database[i].label == label)
		{
			dist = computePartialDistance(sample, database[i], min);
		  //dist = computeDistance(i, sample);
		  if(min > fabs(dist))
		  	min = dist;			
		}
	}
	
	distResult[0] = min;

	if(min < minDist) 
	  return 1;
	else
		return 0;
}

/**
 * @brief Recognizes sample object calculating minimum distance of it to every object on database
 * @details 
 *
 * @param sample(Features) vector of features that represents object to be recognized
 * @return (bool) bool value to confirm if class was recognized correctly
 */
int KNN::partialRecognize(Features sample)
{
	unsigned int i, j;
	float dist = 0;
	float max = MAX;
	
//	float *distances = new float[database.size()];
	int *indexVec = new int[k];
	int label = 0;

	for(j = 0; j < (unsigned) k; j++)
	{
		max = MAX;
		for(i = 0; i < database.size(); i++)
		{
			dist = computePartialDistance(sample,database[i],max);
			if((dist < max) && (!checkIndex(i,indexVec)) && (dist < minDist))
			{
				max = dist;
				label = database[i].label;
				indexVec[j] = i;
			}
		}
		distResult[2*j] = max;
		distResult[2*j+1] = (float) label;
	}
	 
	int* countVec = new int[numOfClass+1];
	memset(countVec,0,(numOfClass+1)*sizeof(int));
	//for(i = 0; i < (unsigned) numOfClass+1; i++) countVec[i] = 0;

	for(i = 0; i < (unsigned) k; i++)
		countVec[int(distResult[2*i+1])]++;

	max = 0;
	int result = -1;
	for(i = 0; i < (unsigned) numOfClass + 1; i++)
		if(max < countVec[i]){
			max = (float) countVec[i];
			result = i;
		}

	return result;
}

/**
 * @author Thomaz
 * @brief Check if an index has been computed as a K-NN
 *
 * @param i(int) Vector index
 * @return (bool) true if this index is already computed, false otherwise
 */

bool KNN::checkIndex(int i, int* vet)
{
	for(int j = 0; j < k; j++)
		if(i == vet[j])
			return(true);
	return(false);
}
/**
 * @brief Recognizes sample object calculating minimum distance of it to every object on database
 * @details 
 *
 * @param sample(Features) vector of features that represents object to be recognized
 * @param label(int) integer value that represents user to be identified
 * @return (bool) bool value to confirm if class was recognized correctly
 */
int KNN::recognize(Features sample, int label)
{
	unsigned int i;
	float dist;
	float min = MAX;
	
	if(norm)
		normalize(sample);


	for(i = 0; i < database.size(); i++)
	{
		if(database[i].label == label)
		{
		  dist = computeDistance(i, sample);
		  if(min > fabs(dist))
		  	min = dist;
			
		}

	}
	
	distResult[0] = min;

	if(min < minDist) 
	  return 1;
	else
		return 0;
}

/**
 * @brief Recognizes sample object calculating minimum distance of it to every object on database
 * @details 
 *
 * @param sample(Features) vector of features that represents object to be recognized
 * @return (bool) bool value to confirm if class was recognized correctly
 */
int KNN::recognize(Features sample)
{
	unsigned int i, j;
	float dist;
	float *distances;
	int *labelVec;
	
	distances = new float[database.size()];
	labelVec = new int[k];
	
	if(norm)
		normalize(sample);

	
	for(i = 0; i < database.size(); i++)
	{
		dist = computeDistance(i, sample);
		distances[i] = dist;
	}
		
	int index = -1;

	for(j = 0; j < k; j++)
	{
		float min = MAX;
		for(i = 0; i < database.size(); i++)
		{
			if(min > distances[i])
			{
				min = distances[i];
				 if(min < minDist)
				 {
					 labelVec[j] = database[i].label;
					 index = i;
				 }
				 else
					 labelVec[j] = 0;
			}
		}
		
		if(index != -1)
			distances[index] = MAX;

		distResult[2*j] = min;
		distResult[2*j+1] = labelVec[j];
	}
	
	int *countVec = new int[numOfClass + 1];
	for(i = 0; i <  numOfClass + 1; i++)
		countVec[i] = 0;

	for(i = 0; i < k; i++)
		countVec[labelVec[i]]++;

	int max = 0;
	int result = -1;
	for(i = 0; i < numOfClass + 1; i++)
	{
		if(max < countVec[i])
		{
			max = countVec[i];
			result = i;
		}
	}

	/*delete[] countVec;
	delete[] labelVec;
	delete[] distances;*/

	return result;
}

/**
 * @brief Normalizes database between 0 and 1
 */
void KNN::normalizeDataBase(void)
{
	float d;
	for(int i = 0; i < database.size(); i++)
		for(int j = 0; j < numOfFeatures; j++)
		{
			d = database[i][j];
			d = (database[i][j] - min[j])*2;
			d /= (float)(max[j] - min[j]);
			d = d-1;
			database[i].data[j] = d;//(float)(database[i][j] - min[j])/(float)(max[j] - min[j]);
			//d = database[i].data[j];
		}
}

/**
 * @brief Normalizes single object  between 0 and 1 to be compared to database
 * @param sample(Features) object to be normalized
 */
void KNN::normalize(Features& sample)
{
	double d;
	for(int j = 0; j < numOfFeatures; j++)
	{
		d = sample[j];
		d = (sample[j] - min[j])*2;
		d /= (double)(max[j] - min[j]);
		d = d - 1;
		sample.data[j] = d;
	}
}

/**
 * @brief Sets tolerance distance to consider recognition.
 *
 * @param minDist(float) minimum distance value
 */
void KNN::setMinDist(float minDist)
{
	this->minDist = minDist;
}

/**
* @brief Returns a vector with k lower distances (with labels)
* 
* @return (float) k lower distances
*/
float* KNN::getDistResult(void)
{
	return distResult;
}

/**
* @brief Returns the user threshold
* 
* @return (float) threshold
*/
float KNN::getMinDist(void)
{
	return this->minDist;
}

/**
* @brief Enable normalization
*
* @param norm(bool) boolean normalization 
*/
void KNN::enableNormalization(bool norm)
{
	this->norm = norm;
}