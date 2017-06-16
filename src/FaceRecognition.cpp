/**
 * @Class FaceRecognition
 * @details Management tools for facial recognition.
 */
#include "FaceRecognition.h"

/**
 * @author Tarique Cavalcante
 * @brief Constructor
 * @return void
 */
FaceRecognition::FaceRecognition(void)
{
	gamma = 1;
	elipse = false;
	DoG = false;
	equaliz = false;
	gamma = false;

	
	frame = NULL;
	processedFace = NULL;

	faceClassifier = ( CvHaarClassifierCascade* )cvLoad("haarcascade_frontalface_alt.xml", 0, 0, 0 );
	storageFace = cvCreateMemStorage( 0 );

	setSizeResized(190, 250);
}

/**
 * @author Tarique Cavalcante
 * @brief Constructor
 *
 * @param cascadePath(string) haar cascade classifier path
 * @return void
 */
FaceRecognition::FaceRecognition(string cascadePath)
{
	gamma = 1;
	elipse = false;
	DoG = false;
	equaliz = false;
	gamma = false;

	
	frame = NULL;
	processedFace = NULL;

	faceClassifier = ( CvHaarClassifierCascade* )cvLoad(cascadePath.c_str(), 0, 0, 0 );
	storageFace = cvCreateMemStorage( 0 );

	setSizeResized(190, 250);
}

/**
 * @author Tarique Cavalcante
 * @brief Constructor
 *
 * @param image(IplImage*) image to be set on FaceRecognition class
 * @param label(int) label to identify user
 * @return void
 */
FaceRecognition::FaceRecognition(IplImage *image, int label)
{
	gamma = 1;
	elipse = false;
	DoG = false;
	equaliz = false;
	gamma = false;

	frame = NULL;
	processedFace = NULL;

	faceClassifier = ( CvHaarClassifierCascade* )cvLoad("haarcascade_frontalface_alt.xml", 0, 0, 0 );
	storageFace = cvCreateMemStorage( 0 );

	setImage(image, label);
	setSizeResized(190, 250);
}

/**
 * @author Tarique Cavalcante
 * @brief Destructor
 * @return void
 */
FaceRecognition::~FaceRecognition(void)
{
	if(frame != NULL)
		cvReleaseImage(&frame);
	
	if(processedFace != NULL)
		cvReleaseImage(&processedFace);
	
	if(features.size() != 0)
		features.clear();

	if(detectedFaces.size() != 0)
		detectedFaces.clear();

	if(auxFeat.size() != 0)
		auxFeat.clear();

	if(labels.size() != 0)
		labels.clear();

	if(auxData.size() != 0)
		auxData.clear();

	cvReleaseMemStorage(&storageFace);
	cvReleaseHaarClassifierCascade(&faceClassifier);
}

/**
 * @author Valberto Enoc
 * @brief Sets new values for width and height to resize image
 *
 * @param widthResized(int) new value for width
 * @param heightResized(int) new value for height
 * @return void
 */
void FaceRecognition::setSizeResized(int widthResized, int heightResized)
{
	this->widthResized = widthResized;
	this->heightResized = heightResized;
}

/**
 * @author Tarique Cavalcante
 * @brief Sets image to Face Recognition class.
 *
 * @param image(IplImage*) image to be processed
 * @param label(int) value used to identify user
 * @return void
 */
void FaceRecognition::setImage(IplImage* image, int label)
{
	lbpFeatures.setLabel(label);

	if(this->frame != NULL)
		cvReleaseImage(&this->frame);

	if(this->processedFace != NULL)
		cvReleaseImage(&this->processedFace);

	this->frame = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
	this->processedFace = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);

	cvCopy(image, this->frame);
	cvCopy(image, this->processedFace);
}

void FaceRecognition::setLBPimage(IplImage *image, int label)
{
    lbpFeatures.setImage(image);
}


/**
 * @author Tarique Cavalcante
 * @brief Returns original frame from camera.
 *
 * @return (IplImage*) camera frame.
 */
IplImage* FaceRecognition::getImage(void)
{
	return frame;
}

/**
 * @author Valberto Enoc
 * @brief Sets tolerance distance to consider recognition.
 *
 * @param minDist(float) minimum distance value
 * @return void
 */
void FaceRecognition::setMinDist(float minDist)
{
	knn.setMinDist(minDist);
}

/**
 * @author Valberto Enoc
 * @brief Gets the vector of knn distance result
 *
 * @return (float*) vector corresponding to all the distances
 */
float* FaceRecognition::getDistResult(void)
{
	return knn.getDistResult();
}

/**
 * @author Valberto Enoc
 * @brief Gets the knn minimal distance result.
 *
 * @return (float) value corresponding to the minimal distance.
 */
float FaceRecognition::getMinDist(void)
{
	return knn.getMinDist();
}

/**
 * @author Tarique Cavalcante
 * @brief Calls function on lbpFeatures that returns processed image.
 *
 * @return (IplImage*) Processed image to be returned
 */
IplImage* FaceRecognition::getProcessedImage(void)
{
	return processedFace;
}

/**
 * @author Samuel Luz / Valberto Enoc
 * @brief Detects face on original frame converted to grayscale
 *
 * @return (bool) logic value to identify if detection was successful
 * @return void
 */
bool FaceRecognition::detectFace(bool isFaceImg)
{
	IplImage *aux = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);

	if(frame->nChannels == 3)
		cvCvtColor(frame, aux, CV_BGR2GRAY);
	else
		cvCopy(frame, aux);

	int i;
	CvRect *r = NULL;
	if (!isFaceImg)
	{
		CvSeq *faces = cvHaarDetectObjects(aux, faceClassifier, storageFace, 1.1, 1, 0, cvSize( 120, 120) ); //250, 250

		for( i = 0 ; i < ( faces ? faces->total : 0 ) ; i++ ) 
		{
			r = ( CvRect* )cvGetSeqElem( faces, i );

			//center face
			//cvRectangle(frame, cvPoint(r->x, r->y), cvPoint(r->x + r->width, r->y + r->height), CV_RGB(0, 255, 0), 3);
			//cvSaveImage("datectedFace1.jpg", frame);

			r->x += (int)(r->width * 0.125);
			r->width = (int)(r->width*0.75);
//			cvRectangle(frame, cvPoint(r->x, r->y), cvPoint(r->x + r->width, r->y + r->height), CV_RGB(255, 0, 0), 3);
//			cvSaveImage("datectedFace2.jpg", frame);
		}
	}
	else
		i = 1;


	if(i > 0)
	{
		if (!isFaceImg)
		{
			cvSetImageROI(aux, *r);
			cvSetImageROI(frame, *r);
		}

		//cvSaveImage("faceImage.jpg", frame);

		if(processedFace != NULL)
			cvReleaseImage(&processedFace);

		processedFace = cvCreateImage(cvSize(widthResized, heightResized), IPL_DEPTH_8U, 1);
		cvResize(aux, processedFace, CV_INTER_LINEAR);

		//cvSaveImage("before gamma 1.jpg", processedFace);

		if(gamma)
			applyGammaCorrection();
//
//		//            cvSaveImage("after gamma 1.jpg", processedFace);
//
		if(DoG)
			DogFilter();
//		//            cvSaveImage("after dog 2.jpg", processedFace);
//
		if(equaliz)
			equalizeHistogram();
		//            cvSaveImage("after equalization 3.jpg", processedFace);

		/*cvNamedWindow("DEBUG");
		cvShowImage("DEBUG", processedFace);
		cvWaitKey(1);*/

		lbpFeatures.setImage(processedFace);
	}

	cvReleaseImage(&aux);

	if(i > 0)
		return true;
	else
		return false;
}

/**
 * @author Tarique Cavalcante
 * @brief enables ellipse Mask on pre-processing.
 *
 * @param enable(bool) flag to enable ellipse mask on segmented face image.
 * @return void
 */
void FaceRecognition::enableElipse(bool enable)
{
	elipse = enable;
}

/**
 * @author Tarique Cavalcante
 * @brief enables equalization on pre-processing.
 *
 * @param enable(bool) flag to enable equalization on segmented face image
 * @return void
 */
void FaceRecognition::enableEqualization(bool enable)
{
	equaliz = enable;
}

/**
 * @author Tarique Cavalcante
 * @brief enables DoG filter on pre-processing.
 *
 * @param enable(bool) flag to enable DoG filter on segmented face image
 * @return void
 */
void FaceRecognition::enableDoGFilter(bool enable)
{
	DoG = enable;
}

/**
 * @author Tarique Cavalcante
 * @brief enables Gamma Correction on pre-processing.
 *
 * @param enable(bool) flag to enable Gamma Correction on segmented face image
 * @return void
 */
void FaceRecognition::enableGammaCorrection(bool enable)
{
	gamma = enable;
}

/**
 * @author Tarique Cavalcante
 * @brief Sets value for gamma.
 *
 * @param gamma(float) value to be used on gammaCorrection()
 */
void FaceRecognition::setGamma(float gamma)
{
	this->gamma = gamma;
}

/**
 * @author Valberto Enoc
 * @brief Sets DoG filter parameters.
 *
 * @param size1(int) gaussian mask size for first image (extern) 
 * @param size2(int) gaussian mask size for second image (intern) 
 * @param sigma1(float) standard deviation for gaussian filter on first image (extern)
 * @param sigma2(float) standard deviation for gaussian filter on first image (intern)
 * @return void
 */
void FaceRecognition::setDoGFilter(int size1, int size2, float sigma1, float sigma2)
{
	DogSize1 = size1;
	DogSize2 = size2;
	DogSigma1 = sigma1;
	DogSigma2 = sigma2;
}

/**
 * @author Valberto Enoc
 * @brief Computes LBP features.
 *
 * @param type(int) selects one of available LBP types to be computed
 * @return void
 */
void FaceRecognition::computeLBP(int type)
{
	lbpFeatures.computeLBPFeatures(type);
	features.push_back(lbpFeatures.getLBPFeatures());
}

/**
 * @author Valberto Enoc
 * @brief Prints features on a file.
 *
 * @param filename(char*) name of the file to be written on
 * @param label(int) optional parameters to set label manually
 * @return void
 */
void FaceRecognition::printFeatures(const char* filename, int label, bool printLast)
{
	bool manualLabel = false;
	if(label > -1)
		manualLabel = true;

	FILE* dataFile = fopen(filename, "a");

	if(!dataFile)
		return;

	unsigned int n;
	int i;

	for(n = 0; n < features.size(); n++)
	{
        if (printLast) {
            n = features.size() - 1;
        }
        
		fprintf(dataFile, "\n");
		for(i = 0; i < features[n].size; i++)
			fprintf(dataFile, "%d\t", features[n][i]);

		if(manualLabel)
			fprintf(dataFile, "%d", label);
	}
	fclose(dataFile);

	/*std::ofstream fout (filename);
	for(n = 0; n < features.size(); n++)
	{
		for(i = 0; i < features[n].size; i++)
			fout << features[n][i] << '\t';

		if(manualLabel)
			fout << label << '\n';
		else
			fout << features[n].label + 1 << '\n';
	}*/
}

/**
 * @author Tarique Cavalcante
 * @brief Load LBP features from a file.
 *
 * @param filename(char*) name of the file to be load
 * @param numOfFeatures(int) number of features in file
 * @return void
 */
void FaceRecognition::loadFeatures(char* filename, int numOfFeatures)
{
	int i, label;
	float temp;
	Features aux;
	
	FILE* featuresFile = fopen(filename, "r");
	
	if(!featuresFile)
		return;

	aux.size = numOfFeatures;
	aux.data = new float[aux.size];

	while(!feof(featuresFile))
	{
		for(i = 0; i < numOfFeatures; i++)
		{
			fscanf(featuresFile, "%f", &temp);
			aux.data[i] = temp;
		}
		fscanf(featuresFile, "%d", &label);
		aux.label = label;
		features.push_back(aux);
	}

	fclose(featuresFile);
	//delete [] aux.data;
}

/**
 * @author Thomaz / Valberto / Tarique
 * @brief Clear the features loaded.
 *
 * @param void
 * @return void
 */
void FaceRecognition::clearFeatures(void)
{
	features.clear();
}

/**
 * @author Thomaz / Valberto / Tarique
 * @brief Add a feature do the class.
 *
 * @param F(Features) feature to be added into class.
 * @return void
 */
void FaceRecognition::addFeatures(Features F)
{
	features.push_back(F);
}

/**
 * @author Thomaz / Valberto / Tarique
 * @brief Add a feature to the knn database.
 *
 * @param features(vector <Features>) feature to be added into database.
 * @return void
 */
void FaceRecognition::addToDatabase(std::vector <Features>& features, int label)
{
	for(int i = 0; i < features.size(); i++)
		features[i].label = label;

	knn.addToDatabase(features);
}

/**
 * @author Thomaz / Valberto / Tarique
 * @brief Gets the last feature of the feature vector.
 *
 * @param void
 * @return (feature) the last feature.
 */
Features FaceRecognition::getFeatures(void)
{
	return features[features.size()-1];
}


/**
 * @author Thomaz / Valberto / Tarique
 * @brief Access the total features database.
 *
 * @return (vector) the database.
 */
std::vector <Features>& FaceRecognition::getDatabase(void)
{
	return features;
}

/**
 * @author Tarique Cavalcante
 * @brief Sets paramaters of KNN class.
 *
 * @param numOfClass(int) number of classes to be recognized
 * @param numOfFeatures(int) number of features to be recognized
 * @param norm(bool) flag enabling data normalization
 * @param k(int) number of neighbors to compute distance
 * @param dist(int) minimum distance to recognize class
 * @return void
 */
void FaceRecognition::setKNN(int numOfClass, int numOfFeatures, bool norm, int k, int dist)
{
	knn.setDistanceOrder(dist);
	knn.setup(k, numOfClass, numOfFeatures);
	if(features.size() > 0)
		knn.loadDatabase(features);
	knn.enableNormalization(norm);
	if(norm)
		knn.normalizeDataBase();
}

/**
 * @author Tarique Cavalcante
 * @brief Sets paramaters of KNN class.
 *
 * @param filename(char*) filename to load database from
 * @param numOfClass(int) number of classes to be recognized
 * @param norm(bool) flag enabling data normalization
 * @param k(int) number of neighbors to compute distance
 * @param dist(int) minimum distance to recognize class
 * @return void
 */
void FaceRecognition::setKNN(char* filename, int numOfClass, bool norm, int k, int dist)
{
	knn.setDistanceOrder(dist);
	knn.setup(k, numOfClass, features[0].size);
	knn.loadDatabase(filename);
	knn.enableNormalization(norm);
	if(norm)
		knn.normalizeDataBase();
}

/**
 * @author Valberto Enoc / Thomaz Maia
 * @brief Calls function from lbpFeatures to recognize user.
 *
 * @return (int) recognized user label
 */
int FaceRecognition::recognizeKNN(void)
{
	return knn.partialRecognize(lbpFeatures.getLBPFeatures());
	//return knn.recognize(lbpFeatures.getLBPFeatures());
}

/**
 * @author Tarique Cavalcante
 * @brief Calls function from lbpFeatures to recognize class.
 *
 * @param F(Features) object from Features class to be recognized
 * @return (int) recognized class label
 */
int FaceRecognition::recognizeKNN(Features F)
{
	return knn.recognize(F);
}

/**
 * @author Tarique Cavalcante
 * @brief Calls function from lbpFeatures to recognize class.
 *
 * @param label(int) label to identify user to be recognized
 * @return (bool) logic value to validate if user was correctly recognized
 */
bool FaceRecognition::recognizeKNN(int label)
{
	return knn.partialRecognize(lbpFeatures.getLBPFeatures());
	//return knn.recognize(features.at(features.size() - 1),label);
		//lbpFeatures.getLBPFeatures(), label);
}

/**
 * @author Valberto Enoc
 * @brief Calls function from lbpFeatures to recognize class.
 * @details An object from Features class is submitted to the classifier. The classifier confirms if the submitted image corresponds to the informed user label.
 *
 * @param	F(Features) object from Features class to be recognized
 * @param label(int) label to identify user to be recognized
 * @return (bool) logic value to validate if user was correctly recognized
 */
bool FaceRecognition::recognizeKNN(Features F, int label)
{
	return knn.recognize(F, label);
}

/**
 * @author Valberto Enoc
 * @brief Sets number of subspaces to divide face image.
 * @details LBP is computed on a set of subspaces of the face image and then concatanated. This function difines the number of subspaces.
 *
 * @param nRow(int) number of divisions to make on the image height
 * @param nCol(int) number of divisions to make on the image width
 * @return void
 */
void FaceRecognition::setLBPSize(int nRow, int nCol)
{
	lbpFeatures.setLBPSize(nRow, nCol);
	widthResized += widthResized % nCol;
	heightResized += heightResized % nRow;
}

/**
 * @author Valberto Enoc
 * @brief Sets weight values to apply on LBP histogram computed on specific subspace.
 * @details Subspace is selected by coordinates i, j.
 *
 * @param i(int) position on y axis where subspace is located
 * @param j(int) position on x axis where subspace is located
 * @param weight(int) number of divisions to make on the image height
 * @return void
 */
void FaceRecognition::setWeights(int i, int j, int weight)
{
	lbpFeatures.setWeights(i, j, weight);
}

/**
 * @author Valberto Enoc
 * @brief Equalizes image histogram.
 *
 * @return void
 */
void FaceRecognition::equalizeHistogram(void)
{
	int counter;
	int grayLevels = pow(2.0, processedFace->depth);
	int newGrayLevels = pow(2.0, processedFace->depth);

	unsigned int* hist;
	unsigned int* acum;
	unsigned int sum;
	float c;

	counter = processedFace->width*processedFace->height;
	sum = 0;

	grayLevels = pow(2.0, processedFace->depth);
	hist = new unsigned int[grayLevels];
	acum = new unsigned int[grayLevels];

	memset(hist,0,grayLevels*sizeof(unsigned int));
	memset(acum,0,grayLevels*sizeof(unsigned int));

	uchar *canal;
	for(int i = 0; i < processedFace->height; i++)
	{
		canal = (uchar*)processedFace->imageData + i*processedFace->widthStep;
		for(int j = 0; j < processedFace->width; j++)
			hist[canal[j]] += 1;
	}

	for(int i = 0; i < grayLevels; i++)
	{
		sum += hist[i];
		acum[i] = sum;
	}

	c = (float)(newGrayLevels)/(float)(counter);
	int k;
	uchar *canalK;

	for(int i = 0; i < processedFace->height; i++)
	{
		canal = (uchar*)processedFace->imageData + i*processedFace->widthStep;
		canalK = (uchar*)processedFace->imageData + i*processedFace->widthStep;
		for(int j = 0; j < processedFace->width; j++)
		{
			k = canalK[j];
			canal[j] = acum[k]*c;
		}
	}
	
	
	delete []hist;
	delete []acum;

}

/**
 * @author Valberto Enoc
 * @brief Corrects gamma on the image
 *
 * @return void
 */
void FaceRecognition::applyGammaCorrection(void)
{
  float pix;
	              

	for(int i = 0; i < processedFace->width * processedFace->height; i++)
  {
		pix = cvGetReal1D(processedFace, i);
		if(pix == 0)
			pix = 1;
    pix = pix/255;
    pix = pow(pix, gamma);
    pix *= 255;
		cvSetReal1D(processedFace, i, (int)pix);
	}     
}

/**
 * @author Valberto Enoc
 * @brief Applies DoG filter to image
 * @details Applies Difference of Gaussian Filter. It subtracts two processed images with two diferrent gaussian, then subtracts its result from the original image.  
 *
 * @return void
 */
void FaceRecognition::DogFilter(void)
{

	IplImage *g1 = cvCreateImage(cvGetSize(processedFace), 8, 1);
	IplImage *g2 = cvCreateImage(cvGetSize(processedFace), 8, 1);
	IplImage *auxDog = cvCreateImage(cvGetSize(processedFace), 8, 1);
	
	cvSmooth(processedFace, g1, CV_GAUSSIAN, DogSize1, DogSize1, DogSigma1);
	cvSmooth(processedFace, g2, CV_GAUSSIAN, DogSize2, DogSize2, DogSigma2);

	cvSub(g1,g2,auxDog);
	cvSub(processedFace, auxDog, processedFace);

	cvReleaseImage(&g1);
	cvReleaseImage(&g2);
	cvReleaseImage(&auxDog);


}

