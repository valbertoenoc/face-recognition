#include "FaceRecognition.h"
#include "UserManager.h"
#include <iostream>
#include <time.h>

#ifndef APPRECFACIAL_H
#define APPRECFACIAL_H


//PARAM1
//0 - distThreshold (minimum distance to classify user)
//1 - enable DoG
//2 - enable gamma
//3 - enable equalization
//4 - setCam 
//5 - MAX_FRAMES


//APPMOD
//1 - recognizeOne
//2 - recognizeAny
//3 - subscribe
//4 - unsubscribe
//5 - removeEnrollment
//6 - listUsers
//7 - listEnrollments
//8 - resetDatabase

struct subject{
    IplImage* img;
    int label;
};

enum{zero, Subscribe, RecognizeOne, RecognizeAny, RemoveUser, RemoveEnrollment, ListUsers, ListEnrollments, ResetDatabase, ResearchAndDevelopment};

//Aplication
int AppRecFacialInit();
int AppRecFacialConfigure(int* param1);							
int AppRecFacialProcess(int app = 0,const char* nameId = 0, int numberId = 0, int auxId = 0);
int AppRecFacialFinalize(void);

//Auxiliary
void AppRecFacialClearScreen(void);
void AppRecFacialOpenDoor(void);
void AppRecFacialSendMessage(string msg);
void AppRecFacialResetDatabase(void);
void AppRecFacialListUsers(void);
void AppRecFacialShowCamOutput(int cam);
void AppRecFacialUpdatePaths(const char* userID);

bool AppRecFacialCheckUserRegistration(const char* userID);

int AppRecFacialSubscribe(const char* userID);
int AppRecFacialRemoveUser(int index);
int AppRecFacialRemoveEnrollment(int userId, int enrollId);
int AppRecFacialRecognizeOne(const char* userID);
int AppRecFacialRecognizeAny(void);
int AppRecFacialListEnrollments(int id);
int AppRecFacialSaveUserOnDatabase(const char* userID);
int AppRecFacialGetLabelFromDatabase(const char* userID);
int AppRecFacialMergeUsers(void);
int AppRecFacialInitDatabase(void);

string AppRecFacialGetStringFromDatabase(int id);
string AppRecFacialGetTime(void);
string AppRecFacialValidateString(string username);

//Research and Development
void saveFace(string userID);
void generateLBPfile();
void releaseImgVector(vector<IplImage*> imgs);
size_t getLabelFromFolder(string dirName);
bool verifyFilename(string regFilePath, string username);
vector<string> getFolderFiles(string dir, string fileExt);
vector<subject> loadYaleFaces(string path);
vector<subject> loadYaleBFaces(string path);
vector<subject> loadFEIFaces(string path);
vector<subject> loadMyDataset(string path);

#endif






