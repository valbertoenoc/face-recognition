/**
 * @Class UserManager
 * @details Management tools for load and save xml files
 */
#include "UserManager.h"

/**
 * @author Tarique Cavalcante
 * @brief Constructor
 * @return void
 */
UserManager::UserManager()
{
	this->databaseFilename = NULL;
	maxEnrollment = 20;
}

/**
 * @author Tarique Cavalcante
 * @brief Constructor
 *
 * @param filename(char*) database filename
 * @return void
 */
UserManager::UserManager(char* filename)
{
	this->databaseFilename = filename;
	maxEnrollment = 20;
}

/**
 * @author Tarique Cavalcante
 * @brief Destructor
 * @return void
 */
UserManager::~UserManager()
{
	for(int i = 0; i < numOfUsers(); i++)
		users[i].enroll.clear();

	users.clear();
}

/**
 * @author Tarique Cavalcante
 * @brief Set the database filename
 *
 * @param filename(char*) database filename
 * @return void
 */
void UserManager::setDatabaseFilename(char* filename)
{
	this->databaseFilename = filename;
}

/**
 * @author Thomaz Maia
 * @brief Load users registration by reading xml file
 *
 * @return void
 */
void UserManager::loadRegisters()
{
	User temp;
	Enrollment aux;

	char name[512];
	char auxPath[512];
	int num_enroll;

	CvFileStorage *fs;
	CvFileNode* nodeU, *nodeE;
	fs = cvOpenFileStorage(this->databaseFilename, 0, CV_STORAGE_READ);
	if(!fs){
#if defined(PC)
        sprintf(auxPath, "mkdir FaceRec");system(auxPath);
        sprintf(auxPath, "mkdir FaceRec\\Features");system(auxPath);
        sprintf(auxPath, "mkdir FaceRec\\LOG");system(auxPath);
        sprintf(auxPath, "mkdir FaceRec\\Samples");system(auxPath);
#elif defined __APPLE__
        sprintf(auxPath, "mkdir FaceRec");system(auxPath);
        sprintf(auxPath, "mkdir FaceRec/Features");system(auxPath);
        sprintf(auxPath, "mkdir FaceRec/LOG");system(auxPath);
        sprintf(auxPath, "mkdir FaceRec/Samples");system(auxPath);
#endif
        
		
	}
	int num_users = cvReadIntByName(fs,NULL,"num_users",0);
	//if(users.size() > 0)
		users.clear();

	for(int i = 0; i < num_users; i++){
		sprintf(name,"user_%d",i);
		nodeU = cvGetFileNodeByName(fs, 0, name);
		//User
		temp.userName = cvReadStringByName(fs,nodeU,"userName", 0);
		temp.distance = (float) cvReadRealByName(fs,nodeU,"distance",0);
		num_enroll = cvReadIntByName(fs,nodeU,"num_enroll",0);
		//Enrollment
		for(int j = 0; j < num_enroll; j++){
			sprintf(name,"enroll_%d",j);
			nodeE = cvGetFileNodeByName(fs, nodeU, name);
			aux.date =  cvReadStringByName(fs,nodeE,"date", 0);
			aux.imageSample = cvReadStringByName(fs,nodeE,"imageSample",0);
			aux.featuresFile = cvReadStringByName(fs,nodeE,"featuresFile",0);
			aux.numOfSamples = cvReadIntByName(fs,nodeE,"numOfSamples",0);
			temp.enroll.push_back(aux);
		}
		users.push_back(temp);
		temp.enroll.clear();
	}

	cvReleaseFileStorage(&fs);
}

/**
 * @author Thomaz Maia
 * @brief Save users registration into a xml file
 *
 * @return void
 */
void UserManager::printRegisters()
{
	int num_enroll;
	char name[100];

	CvFileStorage *fs = cvOpenFileStorage(this->databaseFilename, 0, CV_STORAGE_WRITE);

	int num_users = users.size();
	cvWriteInt(fs,"num_users",num_users);


	for(int i = 0; i < num_users; i++){
		sprintf(name,"user_%d",i);
	  cvStartWriteStruct(fs,name, CV_NODE_MAP);
		cvWriteString(fs,"userName",users[i].userName.c_str(),1);
		cvWriteReal(fs,"distance",users[i].distance);
		num_enroll = users[i].enroll.size();
		cvWriteInt(fs,"num_enroll",num_enroll);
		for(int j = 0; j < num_enroll; j++){
			sprintf(name,"enroll_%d",j);
			cvStartWriteStruct(fs,name, CV_NODE_MAP);
			cvWriteString(fs,"date",users[i].enroll[j].date.c_str(),1);
			cvWriteString(fs,"imageSample",users[i].enroll[j].imageSample.c_str(),1);
			cvWriteString(fs,"featuresFile",users[i].enroll[j].featuresFile.c_str(),1);
			cvWriteInt(fs,"numOfSamples",users[i].enroll[j].numOfSamples);
	    cvEndWriteStruct(fs);		
		}
		cvEndWriteStruct(fs);		
	}
	cvReleaseFileStorage(&fs);
}

/**
 * @author Tarique Cavalcante
 * @brief Add an user to database
 *
 * @param userName(string) Subscribed username
 * @param distance (float) Subscribed user distance
 * @param date(string) Enrollment date
 * @param imageSample(string) Path to an image sample
 * @param featuresFile(string) Path to the user features file
 * @param numOfSamples(int) Number of subscribed samples
 * @return void
 */
void UserManager::addUser(string userName, float distance, string date, string imageSample, string featuresFile,	int numOfSamples)
{
	Enrollment aux;
	aux.date = date;
	aux.imageSample = imageSample;
	aux.featuresFile = featuresFile;
	aux.numOfSamples = numOfSamples;

	User temp;
	temp.distance = distance;
	temp.userName = userName;
	temp.enroll.push_back(aux);

	users.push_back(temp);
}
	
/**
 * @author Tarique Cavalcante
 * @brief Remove and user from database
 *
 * @param userID(int) User ID to be removed
 * @return void
 */
void UserManager::removeUser(int userID)
{
	if(userID > users.size())
		return;

	unsigned long nEnroll = users[userID].enroll.size();
	
	FILE *auxFile;
	for(int i = 0; i < nEnroll; i++)
	{
		//deletar os arquivos nesses enderecos
		if((auxFile = fopen(users[userID].enroll[i].featuresFile.c_str(), "r")))
		{
			fclose(auxFile);
			remove(users[userID].enroll[i].featuresFile.c_str());
		}

		if((auxFile = fopen(users[userID].enroll[i].imageSample.c_str(), "r")))
		{
			fclose(auxFile);
			remove(users[userID].enroll[i].imageSample.c_str());
		}
		
	}

	//remover usuario do registro
	users.erase(users.begin() + userID);
}

/**
 * @author Tarique Cavalcante
 * @brief Remove a full user enrollment from database
 *
 * @param userID(int) User ID to be removed
 * @param enrollNumber(int) Enrollment number to be removed
 * @return void
 */
void UserManager::removeEnrollment(int userID, int enrollNumber)
{
	//deletar arquivo de features e imagem 
	//deletar os arquivos nesses enderecos
	if(userID > users.size())
		return;
	else if(enrollNumber > users[userID].enroll.size() || enrollNumber < 0)
		return;

	FILE *auxFile;
	if((auxFile = fopen(users[userID].enroll[enrollNumber].featuresFile.c_str(), "r")))
	{
		fclose(auxFile);
		remove(users[userID].enroll[enrollNumber].featuresFile.c_str());
	}

	if((auxFile = fopen(users[userID].enroll[enrollNumber].imageSample.c_str(), "r")))
	{
		fclose(auxFile);
		remove(users[userID].enroll[enrollNumber].imageSample.c_str());
	}

	//remover inscricao do registro
	users[userID].enroll.erase(users[userID].enroll.begin() + enrollNumber);
}
	
/**
 * @author Tarique Cavalcante
 * @brief Update an enrollment
 *
 * @param userID(int) User ID to be updated
 * @param date(string) Date to be updated
 * @param imageSample(string) Path to the image sample to be updated
 * @param featuresFile(string) Path to the features file to be updated
 * @param numOfSamples(int) Number of samples to be updated
 * @return void
 */
void UserManager::updateEnrollment(int userID, string date, string imageSample, string featuresFile,	int numOfSamples)
{
	Enrollment aux;
	aux.date = date;
	aux.imageSample = imageSample;
	aux.featuresFile = featuresFile;
	aux.numOfSamples = numOfSamples;

	if(users[userID].enroll.size() >= maxEnrollment)
		removeEnrollment(userID, 0);
		
	users[userID].enroll.push_back(aux);
}

/**
 * @author Tarique Cavalcante
 * @brief Get the number of subscribed users
 *
 * @return (int) number of subscribed users
 */
int UserManager::numOfUsers()
{
	return users.size();
}

/**
 * @author Tarique Cavalcante
 * @brief Get the number of enrollments
 *
 * @return (int) number of subscribed enrollments
 */
int UserManager::numOfEnrollments()
{
	int nEnroll = 0;
	for(int i = 0; i < numOfUsers(); i++)
		nEnroll += users[i].enroll.size();

	return nEnroll;
}

/**
 * @author Tarique Cavalcante
 * @brief Get the number of enrollments
 *
 * @return (int) number of subscribed enrollments of an user 
 */
int UserManager::numOfEnrollments(int userId)
{
	return users[userId].enroll.size();;
}
