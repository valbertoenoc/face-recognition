/**
 * @Class AppRecFacial
 * @details Management tools for the  application .
 */
#include "AppRecFacial.h"

#ifdef __APPLE__
#include <dirent.h>
#elif _WIN32
#include <io.h>
#endif // __APPLE__

#define MAX_USERS 30
#define AppSuccess 1
#define AppFailed 0
#define COMANDO "@@@LC&"

FaceRecognition *FR;
FaceRecognition *FRsub;
UserManager UM;

char supportFilesPath[512] = "FaceRec";

char mergedDataPath[512] = "FaceRec/mergedDatabase.dat";
char databasePath[512] = "FaceRec/database.xml";
char logPath[512] = "FaceRec/LOG/";
char samplePathInit[512] = "FaceRec/Samples/";
char samplePath[512] = " ";
char featPathInit[512] = "FaceRec/Features/";
char featPath[512] = " ";
char cascadePath[512] = "FaceRec/haarcascade_frontalface_alt.xml";

char dataExt[128] = ".dat";
char sampleExt[128] = ".jpg";

int application = 0;
int setCam = 0;
int MAX_FRAMES = 0;
int technique = 1;
int nFeatures;
clock_t T0, T;

bool normalize = false;
ofstream LOGFILE;

/**
 * @author Valberto
 * @brief Initialize the application loading the database.
 *
 * @param app(int) the application mode.
 * @param tech(int) the technique used in the application.
 * @return (int) application failed or successful application.
 */
int AppRecFacialInit()
{
	sprintf(logPath, "FaceRec/LOG/LOG_%s", AppRecFacialGetTime().c_str());

    FILE* f = fopen("filetest.txt", "w");
    fprintf(f, "a");
    fclose(f);

#if defined _WIN32
	sprintf(mergedDataPath, "FaceRec\\mergedDatabase.dat");
	sprintf(databasePath, "FaceRec\\database.xml");
	sprintf(logPath, "FaceRec\\LOG\\LOG_%s.txt", AppRecFacialGetTime().c_str());
	sprintf(featPathInit, "FaceRec\\Features\\");
	sprintf(samplePathInit, "FaceRec\\Samples\\");
	sprintf(cascadePath, "FaceRec\\haarcascade_frontalface_alt.xml");
#elif defined __APPLE__
    sprintf(mergedDataPath, "FaceRec/mergedDatabase.dat");
    sprintf(databasePath, "FaceRec/database.xml");
    sprintf(logPath, "FaceRec/LOG/LOG_%s.txt", AppRecFacialGetTime().c_str());
    sprintf(featPathInit, "FaceRec/Features/");
    sprintf(samplePathInit, "FaceRec/Samples/");
    sprintf(cascadePath, "FaceRec/haarcascade_frontalface_alt.xml");
#endif

	T0 = clock();
	LOGFILE << AppRecFacialGetTime() << "   Initializing...\n";

	UM.setDatabaseFilename(databasePath);
	UM.loadRegisters();
    
   	LOGFILE.open(logPath, ofstream::out);

	LOGFILE << AppRecFacialGetTime() << "   Users:" << UM.numOfUsers() << endl << "   Enrollments:" << UM.numOfEnrollments() << endl;

	FR = new FaceRecognition(cascadePath);
	FRsub = new FaceRecognition(cascadePath);

	nFeatures = 1475;
	
	if(!AppRecFacialInitDatabase())
	{
		return AppFailed;
	}
	
	T = (1000*(clock() - T0))/CLOCKS_PER_SEC;

	LOGFILE << AppRecFacialGetTime() << "   Application Initialized.\t" << "Time: " << T << endl;
	

	return AppSuccess;
}

/**
 * @author Valberto
 * @brief Configure the application paremeters.
 *
 * @param param(int*) vector with used parameters.
 * @return (int) successful application.
 */
int AppRecFacialConfigure(int* param)
{
	T0 = clock();

	std::cout << "   Configuring application...\n";
	LOGFILE << AppRecFacialGetTime() << "   Configuring application...\n";

	FR->setGamma((float)0.2);
	FR->setDoGFilter(29, 9, 6, 0.25);
	FR->setLBPSize(5, 5);
	FRsub->setGamma((float)0.2);
	FRsub->setDoGFilter(29, 9, 6, 0.25);
	FRsub->setLBPSize(5, 5);
	
	FR->setMinDist((float)param[0]);
	FR->enableDoGFilter(param[1]);
	FR->enableGammaCorrection(param[2]);
	FR->enableEqualization(param[3]);
	FRsub->setMinDist((float)param[0]);
	FRsub->enableDoGFilter(param[1]);
	FRsub->enableGammaCorrection(param[2]);
	FRsub->enableEqualization(param[3]);

	setCam = param[4];
	MAX_FRAMES = param[5];

	T = (1000*(clock() - T0))/CLOCKS_PER_SEC;

	std::cout << "   Application configured\n";
	LOGFILE << AppRecFacialGetTime() << "   Application configured\t" << "Time: " << T << endl;

	
	return AppSuccess;
}

/**
 * @author Valberto
 * @brief Check the application mode.
 *
 * @param app(int) the application mode.
 * @param id(const char*) the user name.
 * @param enrollNumber(int) the enrollment number.
 * @return (int) application failed or successful application.
 */
int AppRecFacialProcess(int app, const char* nameId, int numberId, int auxId)
{
    char nameBuffer[512];
    string username;
    
	float t = 0;
	t = (float)cvGetTickCount();
	
	if(app != 0)
		application= app;

    switch(application)
    {
        case RecognizeOne:
            if(AppRecFacialRecognizeOne(nameId));
            //AppRecFacialOpenDoor();
            
            t = (float)cvGetTickCount() - t;
            t /= ((float)cvGetTickFrequency()*1000);
            printf("\n\t>> Processing Time: %.2fms\n", t);
            
            break;
        case RecognizeAny:
            //if(AppRecFacialRecognizeAny());

			if (AppRecFacialRecognizeAny())
			{
				
				return 1;
			}
			else
			{
				return 0;
			}

            //AppRecFacialOpenDoor();
            
            t = (float)cvGetTickCount() - t;
            t /= ((float)cvGetTickFrequency()*1000);
            printf("\n\t>> Processing Time: %.2fms\n", t);
            
            break;
        case Subscribe:
            return AppRecFacialSubscribe(nameId);
            break;
        case RemoveUser:
            return AppRecFacialRemoveUser(numberId);
            break;
        case RemoveEnrollment:
            return AppRecFacialRemoveEnrollment(numberId, auxId);
            break;
        case ListUsers:
            AppRecFacialListUsers();
            break;
        case ListEnrollments:
            return AppRecFacialListEnrollments(numberId);
        case ResearchAndDevelopment:

            
            char appReD;
            
            cin.getline(nameBuffer, 512);
            appReD = nameBuffer[0];
            
            switch (appReD) {
                    
                case '1':
//                    printf("\tUsage: \n");
//                    printf("\t1. Position user in front of camera. \n");
//                    printf("\t2. Type user name. \n");
//                    printf("\t3. Verify user posture. \n");
//                    printf("\t4. Press Enter. \n");
//                    printf("\t(*) Type 'CANCEL' to cancel operation. \n\n");
//                    printf("\t>> Type your name: ");
//                    
//                    cin.getline(nameBuffer, 512);
//                    username = nameBuffer;
//                    username = AppRecFacialValidateString(username);
//                    
//                    if(!strcmp(username.c_str(), "CANCEL"))
//                        break;
//                    
//                    if(strcmp(username.c_str(), "AppFailed"))
//                        saveFace(username);
//                    else
//                        std::cout << "\n\t>> Invalid username.\n\tOnly LETTERS are allowed.\n\n";
//                    
//                    std::cout << "\n\t>> Press Enter to continue...";
//                    getchar();
//                    
//                    break;
                
                case '2':
//                    generateLBPfile();
                    

                    
                    
                default:
                    break;
            }
            
            break;
            
        case ResetDatabase:
            AppRecFacialResetDatabase();
            break;
        default:
        {
            std::cout << "\t>> Option Not Available.\n";
            std::getchar();
            return AppFailed;
        }
    }

	return AppSuccess;
}

/**
 * @author Valberto
 * @brief Finalize the application saving the new updated database.
 *
 * @return (int) successful application.
 */
int AppRecFacialFinalize(void)
{
	T0 = clock();
	std::cout << AppRecFacialGetTime() << "\t>> Finalizing application...\n";
	LOGFILE << AppRecFacialGetTime() << "   Finalizing application...\n";

	UM.printRegisters();
	delete FR;
	delete FRsub;

	T = (1000*(clock() - T0))/CLOCKS_PER_SEC;

	std::cout << AppRecFacialGetTime() << "\t>> Application finalized.\n";
	LOGFILE << AppRecFacialGetTime() << "   Application finalized.\t" << "Time :" << T << endl;

	LOGFILE.close();

	return AppSuccess;
}






/**
 * @author Valberto
 * @brief Send a message by serial port.
 *
 * @param msg(string) message to be transmitted.
 * @return void
 */
void AppRecFacialSendMessage(string msg)
{
	string result;
	string message = "@@@CC306";

	message += msg;
	message += "&";

	fstream USB1;
        system("stty -F /dev/ttymxc0 9600 cs8 "); 
	USB1.open ("/dev/ttymxc0", fstream::in | fstream::out);	
	if (USB1.is_open()){
		cout << "Successfully opened communications to Modem" << endl;
		USB1 << message.c_str();
		cout << "Sending message... " << message.c_str()<< endl;
		USB1 >> result;
		USB1.close();
		cout << "Received the following back: " << result << endl;
		}
	else {
		cout << "Unsuccessful" << endl;
		}
}
	
/**
 * @author Valberto
 * @brief Open the door by serial port.
 *
 * @return void
 */
void AppRecFacialOpenDoor(void)
{
	string result;
	fstream USB1;
        system("stty -F /dev/ttymxc0 9600 cs8 "); 
	USB1.open ("/dev/ttymxc0", fstream::in | fstream::out);	
	if (USB1.is_open()){
		cout << "Successfully opened communications to Modem" << endl;
		USB1 << COMANDO;
		cout << "Sending command... " << COMANDO << endl;
		USB1 >> result;
		USB1.close();
		cout << "Received the following back: " << result << endl;
		}
	else {
		cout << "Unsuccessful" << endl;
		}
}

/**
 * @author Valberto
 * @brief Check if user is registred.
 *
 * @param userID(const char*) user name.
 * @return (bool) if user is registred or not.
 */
bool AppRecFacialCheckUserRegistration(const char* userID)
{
	for(int i = 0; i < UM.numOfUsers(); i++)
		if(!strcmp(userID, UM.users[i].userName.c_str()))
		  return true;

	return false;

}

/**
 * @author Valberto
 * @brief List all users in database.
 *
 * @return void
 */
void AppRecFacialListUsers(void)
{
	T0 = clock();

	std::cout << "\t>> USERS:\n\n";
	LOGFILE << AppRecFacialGetTime() << "   Listing users...\n";

	for(int i = 0; i < UM.numOfUsers(); i++)
	{
		std::cout << "\t>> " << i << " - " << UM.users[i].userName << endl;
		LOGFILE << AppRecFacialGetTime() << i << " - " << UM.users[i].userName << endl;
	}
	std::cout << endl << endl;
	LOGFILE << AppRecFacialGetTime() << "\tDone.\t" << "Time :" << T << endl;
	//std::cout << "\n\nPress Enter to continue...";
	//getchar();

}

/**
 * @author Valberto
 * @brief List all user enrollments.
 *
 * @param userID(const char*) user name.
 * @return void
 */
int AppRecFacialListEnrollments(int id)
{
	T0 = clock();
	if(id > UM.numOfUsers() - 1 || id < 0)
	{
		std::cout << "\t>> User is not subscribed.\n\n";
		LOGFILE << AppRecFacialGetTime() << "   User is not subscribed.\t" << "Time :" << endl;
		return AppFailed;
	}

	std::cout << "\n\t>> USER ENROLLMENTS:\n";
	LOGFILE << AppRecFacialGetTime() << "   Listing enrollments...\n";

	for(unsigned int j = 0; j < UM.users[id].enroll.size(); j++)
	{
		std::cout << endl << "\t>> " << j+1 << " - " << UM.users[id].enroll[j].date << "\n\t       " << UM.users[id].enroll[j].imageSample;
		LOGFILE << endl << "   " << j+1 << " - " << UM.users[id].enroll[j].date << "\n\t" << UM.users[id].enroll[j].imageSample;
	}

	T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
	std::cout << endl << endl;
	LOGFILE << AppRecFacialGetTime() << "   Done.\t" << "Time :" << T << endl;
	return AppSuccess;
	//std::cout << "\n\nPress Enter to continue...";
	//getchar();
}

/**
 * @author Valberto
 * @brief Reset database.
 *
 * @param msg(string) message to be transmited.
 * @return void
 */
void AppRecFacialResetDatabase(void)
{

	T0 = clock();

	std::cout << "\t>> Reseting database...\n";
	LOGFILE << AppRecFacialGetTime() << "   Reseting database...\n";

	while(UM.numOfUsers())	
		UM.removeUser(0);

	std::remove(databasePath);

	std::cout << "\t>> Database reset." << endl;
	LOGFILE << AppRecFacialGetTime() << "   Database reset." << endl;

	LOGFILE << AppRecFacialGetTime() << "   Done.\t" << "Time :" << T << endl;	
	//std::cout << "\nPress Enter to continue...";
	//getchar();
	
}

/**
 * @author Valberto
 * @brief Subscribe an user
 *
 * @param userID(const char*) user name.
 * @return void
 */
int AppRecFacialSubscribe(const char* userID)
{
	int Ttemp = clock();
	//Declarations
	CvCapture *capture;
	IplImage *frame;
	int frameCount;
	int faceCount;
	int noFaceCount;
	char nameBuffer[128];
	char addConfirmation = 'a';

	//Initialization
	frame = NULL;
	frameCount = 0;
	faceCount = 0;
	noFaceCount = 0;

	if(UM.numOfUsers() == MAX_USERS)
	{
		std::cout << "\n\t>> Database completely full.\n\t>> No more subcription allowed\n\n";
		return AppFailed;
	}

	std::cout << "\n\t>> Checking username and getting ID...\n";
	LOGFILE << AppRecFacialGetTime() << "   Checking username and getting ID...\n";

	if(AppRecFacialCheckUserRegistration(userID))
	{
		std::cout << "\t>> User already subscribed.\n";
		LOGFILE << AppRecFacialGetTime() << "   User already subscribed.\n";

		do
		{
			std::cout << "\t>> Add to same file? (y/n)\n\t";
			LOGFILE << AppRecFacialGetTime() << "\tAdd to same file? (y/n)\n";

			cin.getline(nameBuffer, 10);
			addConfirmation = nameBuffer[0];

			if(addConfirmation == 'y' || addConfirmation == 'Y')
			{
				std::cout << "\tdd confirmed.\n";
				LOGFILE << AppRecFacialGetTime() << "   Add confirmed.\n";

				break;
			}

			if(addConfirmation == 'n' || addConfirmation == 'N')
			{
				std::cout << "\t>> Exiting...\n";
				LOGFILE << AppRecFacialGetTime() << "   Exiting...\n";
				
				return AppFailed;
			}
		}while(addConfirmation != 'y' || addConfirmation != 'n' || addConfirmation != 'N' || addConfirmation != 'Y');
	}

	FRsub->clearFeatures();
	std::cout << "\n\t>> Stand still in front of camera...\n";
	//std::cout << "\nPress Enter to continue...";
	//getchar();

	std::cout << "\t>> Initializing camera...\n";
	LOGFILE << AppRecFacialGetTime() << "   Initializing camera...\n";
  T0 = clock();
	capture = cvCaptureFromCAM(setCam);
	T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
	LOGFILE << AppRecFacialGetTime() << "   Camera Initialized.\t" << "Time :" << T << endl;

	bool flag = false;
	//capture = cvCaptureFromCAM(setCam);
	IplImage *frameResized = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 3);
	while(!flag || frameCount <= MAX_FRAMES)
	{

		T0 = clock();
		LOGFILE << AppRecFacialGetTime() << "   Acquiring Frame...\n";

		frame = cvQueryFrame(capture);

		T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
		LOGFILE << AppRecFacialGetTime() << "   Frame Acquired.\t" << "Time :" << T << endl;

		if(frame)
		{
			cvResize(frame, frameResized);
			frameResized->origin = 0;
		  FRsub->setImage(frameResized);
		}

		else
		{
			std::cout << "\t>> Unable to capture frame.\n";
			LOGFILE << AppRecFacialGetTime() << "   Initializing camera...\n";
			std::cout << "\t>> Unable to capture frame.\n";
			break;		
		}

		T0 = clock();
		LOGFILE << AppRecFacialGetTime() << "\t>> Detecting face...\n";
		if(FRsub->detectFace())
		{
			T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
			std::cout << "\t>> Face Detected\n";
			LOGFILE << AppRecFacialGetTime() << "   Face Detected\t" << "Time :" << T << endl;
			
			T0 = clock();
			FRsub->computeLBP(LBP::LBP_82);
			faceCount++;

			T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
			LOGFILE << AppRecFacialGetTime() << "   Features extracted.\t" << "Time :" << T << endl;
		}
		
		else
		{
			std::cout << "\t>> No Face Detected\n";
			LOGFILE << AppRecFacialGetTime() << "   No Face Detected\n";

			noFaceCount++;
			if(noFaceCount >= 0.8*MAX_FRAMES)
			{
				std::cout << "\n\t>> Not enough faces to subscribe user\n\n";	
				LOGFILE << AppRecFacialGetTime() << "   Not enough faces to subscribe user\nProgram will now exit.\n\n";

				break;
			}
		}
	
		if(faceCount >= 0.2*MAX_FRAMES)
		{
			AppRecFacialUpdatePaths(userID);

			if(addConfirmation == 'y')
			{
				int label = AppRecFacialGetLabelFromDatabase(userID);
				UM.updateEnrollment(label - 1, __DATE__, samplePath, featPath, faceCount);
				FR->addToDatabase(FRsub->getDatabase(), label);
				UM.printRegisters();
			}
			else
			{
				UM.addUser(userID, FR->getMinDist(), __DATE__, samplePath, featPath, faceCount);
				UM.printRegisters();
				FR->addToDatabase(FRsub->getDatabase(), UM.numOfUsers());
			}
				

			std::cout << "\t>> Face successefully saved.\n\n";
			LOGFILE << AppRecFacialGetTime() << "   Face successefully saved.\n\n";
			T = clock() - Ttemp;
			LOGFILE << AppRecFacialGetTime() << "   Done.\t" << "Time :" << T << endl;
			cvReleaseCapture(&capture);
			return AppSuccess;
		}

		frameCount++;
	}

	T = clock() - Ttemp;
	LOGFILE << AppRecFacialGetTime() << "   Done.\t" << "Time :" << T << endl;
	cvReleaseCapture(&capture);
	return AppFailed;

}

/**
 * @author Valberto
 * @brief Unsubscribe an user
 *
 * @param userID(const char*) user name.
 * @return void
 */
int AppRecFacialRemoveUser(int index)
{
	bool flag = false;


	if(index > UM.numOfUsers()-1 || index < 0)
	{
		std::cout << "\t>> User is not subscribed.\n\n";
		LOGFILE << AppRecFacialGetTime() << "   User is not subscribed.\t" << "Time :" << endl;
		return AppFailed;
	}

	UM.removeUser(index);

	std::cout << "\t>> User successefully removed from database.\n\n";
	LOGFILE << AppRecFacialGetTime() << "   User successefully removed from database.\t" << "Time :" << T << endl;
	
	FR->clearFeatures();
	AppRecFacialInitDatabase();
	return AppSuccess;
}

/**
 * @author Valberto
 * @brief Remove an user enrollment.
 *
 * @param userID(const char*) user name.
 * @param position(int) position number of an enrollment from an user
 * @return void
 */
int AppRecFacialRemoveEnrollment(int userID, int position)
{
	T0 = clock();

	if(userID > UM.numOfUsers() - 1 || userID < 0)
	{
		std::cout << "\t>> User is not subscribed.\n\n";
		LOGFILE << AppRecFacialGetTime() << "   User is not subscribed.\t" << "Time :" << T << endl;
		return AppFailed;
	}

	if(position <= UM.numOfEnrollments(userID) && position > 0)
	{
		std::cout << "\t>> Removing enrollment: " << position << " from user: " << userID << endl;
	  LOGFILE << AppRecFacialGetTime() << "   Removing enrollment: " << position << " from user: " << userID << endl;
		UM.removeEnrollment(userID, position - 1);
		T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
		std::cout << "\t>> Enrollment successefully removed\n";
		LOGFILE << AppRecFacialGetTime() << "   Enrollment successefully removed.\t" << "Time :" << T << endl;
		return AppSuccess;	
	}
	else
	{
		std::cout << "\t>> Enrollment does not exist.\n";
		LOGFILE << AppRecFacialGetTime() << "   Enrollment does not exist.\t" << "Time :" << T << endl;
		return AppFailed;
	}
}

/**
 * @author Valberto
 * @brief Recognize 1 to 1.
 *
 * @param userID(const char*) user name.
 * @return void
 */
int AppRecFacialRecognizeOne(const char* userID)
{
	std::cout << "\t>> Checking username and getting ID...\n";
	LOGFILE << AppRecFacialGetTime() << "   Checking username and getting ID...\n";

	if(!AppRecFacialCheckUserRegistration(userID))
	{
		std::cout << "\t>> User is not subscribed.\n";
		//std::cout << "\nPress Enter to continue...";
		//getchar();
		return AppFailed;
	}

	int id = AppRecFacialGetLabelFromDatabase(userID);

	LOGFILE << AppRecFacialGetTime() << "   Done.\n";

	//declarations
	CvCapture *capture;
	IplImage *frame;
	int label;
	int frameCount, faceCount, noFaceCount;
	int IDValidation;
	int result;
	int nFaces = 0;
	bool addConfirmation = true;

	//initialization
	frame = NULL;
	frameCount = 0;
	noFaceCount = 0;
	faceCount = 0;
	label = 0;
	IDValidation = 0;
	FRsub->clearFeatures();

	std::cout << "\t>> Stand still in front of camera...\n";
	//std::cout << "\nPress Enter to continue...";
	//getchar();
	int Ttemp = clock();

	std::cout << "\t>> Initializing camera...\n";
	LOGFILE << AppRecFacialGetTime() << "   Initializing camera...\n";
  T0 = clock();
	capture = cvCaptureFromCAM(setCam);
	T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
	LOGFILE << AppRecFacialGetTime() << "   Camera Initialized.\t" << "Time :" << T << endl;

	std::cout << "\t>> Recognizing...\n";
	IplImage *frameResized = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 3);
	while(1)
	{
		T0 = clock();
		LOGFILE << AppRecFacialGetTime() << "   Acquiring Frame...\n";
	
		frame = cvQueryFrame(capture);
		
		cvResize(frame, frameResized);
		frameResized->origin = 0;
		
		FR->setImage(frameResized);
		
		T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
		LOGFILE << AppRecFacialGetTime() << "   Frame Acquired.\t" << "Time :" << T << endl;

		result = 0;

		LOGFILE << AppRecFacialGetTime() << "   Detecting Face...\n";

		T0 = clock();
		if(FR->detectFace())
		{
			T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
			LOGFILE << AppRecFacialGetTime() << "   Face Detected.\t" << "Time :" << T << endl;
			LOGFILE << AppRecFacialGetTime() << "   Technique:" << technique << "   Extracting Features...\n";
		

			T0 = clock();
			
			FR->computeLBP(LBP::LBP_82);
			
			FRsub->addFeatures(FR->getFeatures());

			T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
			LOGFILE << AppRecFacialGetTime() << "   Features extracted.\t" << "Time :" << T << endl;
			LOGFILE << AppRecFacialGetTime() << "   Recognizing...\n";

			T0 = clock();			
			IDValidation += FR->recognizeKNN(id);
			faceCount++;
			float *distResult = FR->getDistResult();
			
			T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
			std::cout << "\t>> Distance: " <<  distResult[0] << endl;
			LOGFILE << AppRecFacialGetTime() << "   Distance: " <<  distResult[0] << endl;
			LOGFILE << AppRecFacialGetTime() << "   Frame processed.\t" << "Time :" << T << endl;
		}

		else
		{ 
			std::cout << "\t>> No Face Detected.\n";
			LOGFILE << AppRecFacialGetTime() << "   No Face Detected.\n\n";

			noFaceCount++;
			if(noFaceCount >= 0.8*MAX_FRAMES)
			{
				std::cout << "\t>> Not enough faces to Recognize\n\t>> Program will now exit.\n\n";
				LOGFILE << AppRecFacialGetTime() << "   Not enough faces to Recognize\nProgram will now exit.\n\n";	
				std::cout << "\t>> Initializing camera...\n";
				cvReleaseCapture(&capture);
				return AppFailed;
			}
		}
	
		if(faceCount >= 0.2*MAX_FRAMES)
		{
			nFaces = (int)(0.2*MAX_FRAMES);

			std::cout << "\t>> " << IDValidation << "/" << nFaces << endl;
			LOGFILE << AppRecFacialGetTime() <<  "   " << IDValidation << "/" << nFaces << endl;
			break;
		}

	}
	
	if(nFaces > 0)
	{
		if(IDValidation > nFaces/2)
		{
			std::cout << "\t>> ACCESS GRANTED!\n\n";
			LOGFILE << AppRecFacialGetTime() << "   ACCESS GRANTED!\n\n";

			//AppRecFacialSendMessage("Identidade confirmada.");
			//AppRecFacialOpenDoor();

			if(IDValidation != nFaces)
				addConfirmation = false;

			if(addConfirmation)
			{
				AppRecFacialUpdatePaths(userID);
			
				int label = AppRecFacialGetLabelFromDatabase(userID);
				UM.updateEnrollment(label - 1, __DATE__, samplePath, featPath, faceCount);
				UM.printRegisters();			
				FR->addToDatabase(FRsub->getDatabase(), label);
			}

			T = clock() - Ttemp;
			LOGFILE << AppRecFacialGetTime() << "   Done.\t" << "Total Time :" << T << endl;
			
			cvReleaseCapture(&capture);
			return AppSuccess;
		}
		else
		{
			std::cout << "\t>> ACCESS DENIED!\n\n";
			LOGFILE << AppRecFacialGetTime() << "   ACCESS DENIED!\n\n";
			T = clock() - Ttemp;
			LOGFILE << AppRecFacialGetTime() << "   Done.\t" << "Total Time :" << T << endl;
			
			//AppRecFacialSendMessage("Acesso nao autorizado.");
			cvReleaseCapture(&capture);
			return AppFailed;
		}
	}

	T = clock() - Ttemp;
	LOGFILE << AppRecFacialGetTime() << "   Done.\t" << "Total Time :" << T << endl;
	//std::cout << "\nPress Enter to continue...";
	//getchar();

	return AppFailed;
}

/**
 * @author Valberto
 * @brief Recognize 1 to n.
 *
 * @param userID(const char*) user name.
 * @return void
 */
int AppRecFacialRecognizeAny()
{
	if(UM.numOfUsers() == 0)
	{
		std::cout << "\t>> Database empty.\n\t>> Please, subscribe user.\n";
		return AppFailed;
	}

	//declarations
	CvCapture *capture;
	IplImage *frame;
	bool addConfirmation = true;
	int label;
	int frameCount, faceCount, noFaceCount;
	int IDValidation;
	int result;
	int nFaces, nNoFaces;
	int resultValidation[MAX_USERS + 1];
	string identifiedUser;
		
	//initialization
	frame = NULL;
	frameCount = 0;
	noFaceCount = 0;
	faceCount = 0;
	label = 0;
	IDValidation = 0;
	nNoFaces = (int)0.8*MAX_FRAMES;
	nFaces = (int)0.2*MAX_FRAMES;
	FRsub->clearFeatures();
		
	for(int i = 0; i < MAX_USERS + 1; i++)
		resultValidation[i] = 0;

	std::cout << "\t>> Stand still in front of camera...\n";
	//std::cout << "\nPress Enter to continue...";
	//getchar();
	int Ttemp = clock();

	std::cout << "\t>> Initializing camera...\n";
	LOGFILE << AppRecFacialGetTime() << "   Initializing camera...\n";
  T0 = clock();
	capture = cvCaptureFromCAM(setCam);
	T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
	LOGFILE << AppRecFacialGetTime() << "   Camera Initialized.\t" << "Time :" << T << endl;

	std::cout << "\t>> Recognizing...\n";
	IplImage *frameResized = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 3);
	while(1)
	{
		T0 = clock();
		LOGFILE << AppRecFacialGetTime() << "   Acquiring Frame...\n";

		frame = cvQueryFrame(capture);
		
		cvResize(frame, frameResized);
		frameResized->origin = 0;
		FR->setImage(frameResized);

		T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
		LOGFILE << AppRecFacialGetTime() << "   Frame Acquired.\t" << "Time :" << T << endl;

		LOGFILE << AppRecFacialGetTime() << "   Detecting face...\n";

		T0 = clock();
		if(FR->detectFace())
		{
			T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
			LOGFILE << AppRecFacialGetTime() << "   Face Detected.\t" << "Time :" << T << endl;

			LOGFILE << AppRecFacialGetTime() << "   Technique:" << technique << "   Extracting Features...\n";

			T0 = clock();

			FR->computeLBP(LBP::LBP_82);
			
			FRsub->addFeatures(FR->getFeatures());

			T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
			LOGFILE << AppRecFacialGetTime() << "   Features extracted.\t" << "Time :" << T << endl;

			LOGFILE << AppRecFacialGetTime() << "   Done.\n";
			LOGFILE << AppRecFacialGetTime() << "   Recognizing...\n";

			T0 = clock();
			result = FR->recognizeKNN();
			resultValidation[result]++;

			faceCount++;

			float *distResult;
			distResult = FR->getDistResult();

			if(distResult[1] != distResult[3] || distResult[3] != distResult[5] || distResult[1] != distResult[5])
				addConfirmation = false;

			LOGFILE << AppRecFacialGetTime() << "   Frame processed.\t" << "Time :" << T << endl;
			std::cout <<  "\t>> Distances: " << endl;
			std::cout <<  "\t>> " << distResult[0] << " - " << distResult[1] << endl;
			std::cout <<  "\t>> " << distResult[2] << " - " << distResult[3] << endl;
			std::cout <<  "\t>> " << distResult[4] << " - " << distResult[5] << endl << endl;

			LOGFILE << AppRecFacialGetTime() << "   Distances: " << endl;
			LOGFILE << AppRecFacialGetTime() << "   " << distResult[0] << " - " << distResult[1] << endl;
			LOGFILE << AppRecFacialGetTime() << "   " << distResult[2] << " - " << distResult[3] << endl;
			LOGFILE << AppRecFacialGetTime() << "   " << distResult[4] << " - " << distResult[5] << endl << endl;;
			LOGFILE << AppRecFacialGetTime() << "   Done.\n";

		}

		else
		{ 
			std::cout << "\t>> No Face Detected.\n";
			LOGFILE << AppRecFacialGetTime() << "   No Face Detected.\n\n";
			noFaceCount++;
			if(noFaceCount >= 0.8*MAX_FRAMES)
			{
				std::cout << "\t>> Not enough faces to Recognize\n\t>> Program will now exit.\n\n";
				LOGFILE << AppRecFacialGetTime() << "   Not enough faces to Recognize\nProgram will now exit.\n\n";	

				cvReleaseCapture(&capture);
				return AppFailed;;
			}
		}
		
		if(faceCount >= 0.2*MAX_FRAMES)
		{
			int greater = resultValidation[0];
			int finalResult = 0;
			for(int i = 0; i < MAX_USERS + 1; i++)
			{
				if(greater < resultValidation[i])
				{
					greater = resultValidation[i];
					finalResult = i;
				}
				
			}

			if(finalResult > 0 && greater > nFaces/2)
			{
				identifiedUser = AppRecFacialGetStringFromDatabase(finalResult);

				std::cout << "\t ****************************************\n";
				std::cout << "\t IDENTIFIED USER -> " << identifiedUser;
				std::cout << "\n\t ****************************************\n";
				LOGFILE << AppRecFacialGetTime() << "   IDENTIFIED USER -> " << identifiedUser << endl;
	
				if(addConfirmation)
				{
					AppRecFacialUpdatePaths(identifiedUser.c_str());
				
					LOGFILE << AppRecFacialGetTime() << "   Updating database..." << endl;
					FR->addToDatabase(FRsub->getDatabase(), finalResult);
					UM.updateEnrollment(finalResult - 1, __DATE__, samplePath, featPath, faceCount);
					LOGFILE << AppRecFacialGetTime() << "   Saving database..." << endl;
					UM.printRegisters();
				}

				T = clock() - Ttemp;
				LOGFILE << AppRecFacialGetTime() << "   Done.\t" << "Time :" << T << endl;

				cvReleaseCapture(&capture);
				return AppSuccess;

			}
			else
			{
				std::cout << "\t   ****************************\n";
				std::cout << "\t   USER COULD NOT BE IDENTIFIED\n";
				std::cout << "\t   ****************************\n";
				LOGFILE << AppRecFacialGetTime() << "   USER COULD NOT BE IDENTIFIED";
				T = clock() - Ttemp;
				LOGFILE << AppRecFacialGetTime() << "   Done.\t" << "Time :" << T << endl;
				cvReleaseCapture(&capture);
				return AppFailed;
			}
			
			break;
		}

	}
		

	//std::cout << "\nPress Enter to continue...";
	//getchar();

	return AppFailed;
}

/**
 * @author Valberto
 * @brief Show camera output image.
 *
 * @param cam(int) camera type.
 * @return void
 */
void AppRecFacialShowCamOutput(int cam)
{
	CvCapture *capture = NULL;
	IplImage *camera = NULL;
	capture = cvCaptureFromCAM(cam);

	//FR = new FaceRecognition;
	IplImage *cameraResized = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 3);
	
	cvNamedWindow("camera", CV_WINDOW_AUTOSIZE);
	while(cvWaitKey(1) != 'q')
	{
		
		camera = cvQueryFrame(capture);
	
		cvResize(camera, cameraResized);
		FR->setImage(cameraResized);
		
        cvShowImage("camera", FR->getImage());
		FR->detectFace();
		//FR->detectFaceLBP();
        cvShowImage("face", FR->getImage());

		std::cout << T << endl;
		
	}
		
	cvReleaseCapture(&capture);
}

/**
 * @author Valberto
 * @brief Get an user label from database.
 *
 * @param userID(const char*) user name.
 * @return (int) label
 */
int AppRecFacialGetLabelFromDatabase(const char* userID)
{
	for(int i = 0; i < UM.numOfUsers(); i++)
	{
		if(strcmp(userID, UM.users[i].userName.c_str()))
			continue;
		return i + 1;
	}
	return -1;

}

/**
 * @author Valberto
 * @brief Concatene features database into just one file.
 *
 * @return (int) successful application.
 */
int AppRecFacialMergeUsers(void)
{
	if(UM.numOfUsers() == 0)
		return AppFailed;

	string buffer;
	
	FILE* mergedDatabase = fopen(mergedDataPath, "w");
	ifstream userFile;

	for(unsigned int i = 0; i < UM.numOfUsers(); i++) 
		for(unsigned int j = 0; j < UM.users[i].enroll.size(); j++)
		{
			userFile.open(UM.users[i].enroll[j].featuresFile.c_str(), ifstream::in);
			if(userFile.is_open())
			{
				while(!userFile.eof())
				{
					getline(userFile,buffer);
					
					if(!buffer.empty())
						fprintf(mergedDatabase, "%s %d", buffer.data(), i+1);
					
					if(!userFile.eof())
						fprintf(mergedDatabase, "\n");
				}
			}
			userFile.close();
		}	
	fclose(mergedDatabase);

	return AppSuccess;
}


/**
 * @author Valberto
 * @brief Gets the user name from a given id.
 *
 * @param id(int) user id.
 * @return (string) user name.
 */
string AppRecFacialGetStringFromDatabase(int id)
{
	string username = UM.users[id - 1].userName;
	return username;
}

/**
 * @author Valberto
 * @brief Gets time.
 *
 * @return (string) time (month-day-year-hour-minute-second).
 */
string AppRecFacialGetTime()
{
	time_t rawtime;
	struct tm * timeinfo;
	char timeBuffer [80];

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime (timeBuffer,80,"%b-%d-%Y_%H-%M-%S",timeinfo);
			
	return timeBuffer;
}

/**
 * @author Valberto
 * @brief Allows user to initialize database during runtime (necessary everytime database is changed).
 *
 * @return (int) failed/success flag to indicate if function worked correctly.
 */
int AppRecFacialInitDatabase(void)
{
	if(AppRecFacialMergeUsers())
	{		
		std::cout << "\t>> Loading Database...\n";
		LOGFILE << AppRecFacialGetTime() << "   Loading Database...\n";
		T0 = clock();

		FR->loadFeatures(mergedDataPath, nFeatures);
		FR->setKNN(MAX_USERS, nFeatures, normalize, 3);
		T = ((1000*(clock() - T0))/CLOCKS_PER_SEC)/CLOCKS_PER_SEC;
		std::cout << "\t>> Loading Complete.\n";
		LOGFILE << AppRecFacialGetTime() << "   Loading Complete.\t" << "Time: " << T << endl;;
	}
	else
	{
		FR->setKNN(MAX_USERS, nFeatures, normalize, 3);
		std::cout << "\t>> Database Empty.\n";
		LOGFILE << AppRecFacialGetTime() << "   Database Empty.\n";

		std::remove(mergedDataPath);

		T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
		
		return AppFailed;
	}

	std::remove(mergedDataPath);
	return AppSuccess;
}

/**
 * @author Valberto
 * @brief Used to set paths filename (data and time) for the files to be saved on Samples and Features folder.
 *
 * @return void
 */
void AppRecFacialUpdatePaths(const char* userID)
{
	string clockTime;
	clockTime =  AppRecFacialGetTime();
	
	strcpy(featPath, featPathInit);
	strcpy(samplePath, samplePathInit);
	
	sprintf(featPath, "%s%s_%s%s", featPath, userID, clockTime.c_str(), dataExt);
	FRsub->printFeatures(featPath);

	sprintf(samplePath, "%s%s_%s%s", samplePath, userID, clockTime.c_str(), sampleExt);
	
	if(FR->getImage() != NULL)
		cvSaveImage(samplePath, FR->getImage());
	else if(FRsub->getImage() != NULL)
		cvSaveImage(samplePath, FRsub->getImage());
	
}

/**
 * @author Valberto
 * @brief Analyses username string typed by user and, if valid changes to upper case for standardization.
 *
 * @return (string) Validated string or failed/success flag in form of string.
 */
string AppRecFacialValidateString(string username)
{
	bool ok = false;
	char letter;
	int i = 0;
	
	if(username.empty())
		return "AppFailed";

	while(i < username.length())
	{
		letter = username[i];

		if(letter != 32 && !ok) //if there is only "spaces" on string
			ok = true;

		if((letter < 64 || letter > 122) && letter != 32) //ascii codes from "A" to "z" including space
			return "AppFailed";
		username[i] = toupper(letter);
		i++;

		if(i == username.length())
		{
			letter = username[i-1];
			if(letter == 32)
				return "AppFailed";
		}
	}

	if(!ok)
		return "AppFailed";
	else
		return username;
}

void AppRecFacialClearScreen()
{

#if defined PC
	system("cls");
#else
	system("clear");
#endif


}

//vector<subject> loadYaleBFaces(string path)
//{
//    vector<subject> subjects;
//    vector<string> filenames;
//    
//    string imgPath;
//	bool isFaceImg = true;
//    
//    filenames = getFolderFiles(path, "jpg");
//    
//    FRsub->enableGammaCorrection(true);
//    FRsub->enableDoGFilter(true);
//    FRsub->enableEqualization(true);
//    
//    IplImage *img;
//    for (int i  = 0; i < filenames.size(); i++) {
//
//#ifdef __APPLE__
//        imgPath = path + filenames[i];
//#elif _WIN32
//		imgPath = filenames[i];
//#endif
//        subject sub;
//        
//        size_t pos = filenames[i].find("_");
//        
//        int label = 0;
//
//        label = std::stoi(filenames[i].substr(pos-2, pos));
//
//        img = cvLoadImage(imgPath.c_str(), 1);
//        if (img != NULL) {
//            FRsub->setImage(img);
//            
//            if (FRsub->detectFace(isFaceImg)) {
//                sub.img = cvCreateImage(cvGetSize(FRsub->getProcessedImage()), FRsub->getProcessedImage()->depth, FRsub->getProcessedImage()->nChannels);
//                
//                cvCopy(FRsub->getProcessedImage(), sub.img);
//                sub.label = label-1;
//                
//                subjects.push_back(sub);
//            }
//        }
//    }
//    
//    filenames.clear();
//    
//    return subjects;
//}
//
//vector<subject> loadYaleFaces(string path)
//{
//    vector<subject> subjects;
//    vector<string> filenames;
//    
//    string imgPath;
//    bool isFaceImg = true;
//    
//    filenames = getFolderFiles(path, "jpg");
//    
//    FRsub->enableGammaCorrection(true);
//    FRsub->enableDoGFilter(true);
//    FRsub->enableEqualization(true);
//    
//    IplImage *img;
//    for (int i  = 0; i < filenames.size(); i++) {
//#ifdef __APPLE__
//        imgPath = path + filenames[i];
//#elif _WIN32
//		imgPath = filenames[i];
//#endif
//        subject sub;
//        
//        size_t pos = filenames[i].find("_");
//        
//        int label = 0;
//
//        label = std::stoi(filenames[i].substr(pos-2, pos));
//
//        img = cvLoadImage(imgPath.c_str(), 1);
//        if (img != NULL) {
//            FRsub->setImage(img);
//            
//            if (FRsub->detectFace()) {
//                sub.img = cvCreateImage(cvGetSize(FRsub->getProcessedImage()), FRsub->getProcessedImage()->depth, FRsub->getProcessedImage()->nChannels);
//                
//                cvCopy(FRsub->getProcessedImage(), sub.img);
//                sub.label = label-1;
//                
//                subjects.push_back(sub);
//            }
//        }
//    }
//    
//    filenames.clear();
//    
//    return subjects;
//}
//
//vector<subject> loadORLFaces(string path)
//{
//    vector<subject> subjects;
//    vector<string> filenames;
//    
//    string imgPath;
//    bool isFaceImg = true;
//    
//    filenames = getFolderFiles(path, "jpg");
//    
//    IplImage *img;
//    for (int i  = 0; i < filenames.size(); i++) {
//        
//#ifdef __APPLE__
//        imgPath = path + filenames[i];
//#elif _WIN32
//        imgPath = filenames[i];
//#endif
//        subject sub;
//        
//        size_t pos = filenames[i].find("_");
//        
//        int label = 0;
//        
//        label = std::stoi(filenames[i].substr(pos-2, pos));
//        
//        img = cvLoadImage(imgPath.c_str(), 1);
//        if (img != NULL) {
//            FRsub->setImage(img);
//            
//            if (FRsub->detectFace()) {
//                sub.img = cvCreateImage(cvGetSize(FRsub->getProcessedImage()), FRsub->getProcessedImage()->depth, FRsub->getProcessedImage()->nChannels);
//                
//                cvCopy(FRsub->getProcessedImage(), sub.img);
//                sub.label = label-1;
//                
//                subjects.push_back(sub);
//            }
//        }
//    }
//    
//    filenames.clear();
//    
//    return subjects;
//}
//
//vector<subject> loadMyDataset(string path)
//{
//    vector<subject> subjects;
//    vector<string> filenames;
//    
//    string imgPath;
//   	bool isFaceImg = true;
//    
//    ifstream fin;
//#ifdef _WIN32
//    fin.open("FaceRec\\ReD\\regList.dat", std::ios::in);
//    string slash = "\\";
//#elif __APPLE__
//    fin.open("/Users/valbertoenoc/Dropbox/Trabalho/Faculdade/Monografia/FR/Datasets/ReD/regList.dat", std::ios::in);
//    string slash = "/";
//#endif
//    
//    vector<string> regList;
//    while (!fin.eof()) {
//        string name;
//        fin >> name;
//        
//        regList.push_back(name);
//    }
//    
//    IplImage *img;
//    
//    for (int x = 0; x < regList.size(); x++) {
//
//        string namePath = path + "Processed" + slash + regList[x] + slash;
//        filenames = getFolderFiles(namePath, "jpg");
//
//        for (int i  = 0; i < filenames.size(); i++) {
//#ifdef __APPLE__
//            imgPath = namePath + filenames[i];
//#elif _WIN32
//            imgPath = filenames[i];
//#endif
//            subject sub;
//            
//            int label = 0;
//            label = x;
//            
//            img = cvLoadImage(imgPath.c_str(), 1);
//            if (img != NULL) {
//                FRsub->setImage(img);
//                
//                if (FRsub->detectFace(isFaceImg)) {
//                    sub.img = cvCreateImage(cvGetSize(FRsub->getProcessedImage()), FRsub->getProcessedImage()->depth, FRsub->getProcessedImage()->nChannels);
//                    
//                    cvCopy(FRsub->getProcessedImage(), sub.img);
//                    sub.label = label;
//                    
//                    subjects.push_back(sub);
//                }
//            }
//        }
//        
//        filenames.clear();
//    }
//    
//
//    
//    return subjects;
//}

//void generateLBPfile()
//{
//
//#ifdef _WIN32
//    string datasetYalePath = "C:\\Users\\Valberto\\Dropbox\\Trabalho\\Faculdade\\Monografia\\FR\\Datasets\\yalefaces\\";
//	string featureYalePath = datasetYalePath + "features.dat";
//
//	string datasetYaleBPath = "C:\\Users\\Valberto\\Dropbox\\Trabalho\\Faculdade\\Monografia\\FR\\Datasets\\yalefacesB\\CroppedYale\\";
//	string featureYaleBPath = datasetYaleBPath + "features.dat";
//
//	string myDatasetPath = "C:\\Users\\Valberto\\Dropbox\\Trabalho\\Faculdade\\Monografia\\FR\\Datasets\\ReD\\";
//    string myFeaturePath = myDatasetPath + "features.dat";
//    
//#elif __APPLE__
//    string datasetORLPath = "/Users/valbertoenoc/Dropbox/Trabalho/Faculdade/Monografia/FR/Datasets/orl_faces/";
//    string featureORLPath = datasetORLPath + "features.dat";
//    
//    string datasetYalePath = "/Users/valbertoenoc/Dropbox/Trabalho/Faculdade/Monografia/FR/Datasets/yalefaces/";
//	string featureYalePath = datasetYalePath + "features.dat";
//
//	string datasetYaleBPath = "/Users/valbertoenoc/Dropbox/Trabalho/Faculdade/Monografia/FR/Datasets/yalefacesB/CroppedYale/";
//    string featureYaleBPath = datasetYaleBPath + "features.dat";
//    
//    string myDatasetPath = "/Users/valbertoenoc/Dropbox/Trabalho/Faculdade/Monografia/FR/Datasets/ReD/";
//    string myFeaturePath = myDatasetPath + "features.dat";
//#endif
//    
////	vector<subject> subjects = loadORLFaces(datasetORLPath);
////	vector<subject> subjects = loadYaleFaces(datasetYalePath);
////	vector<subject> subjects = loadYaleBFaces(datasetYaleBPath);
//    vector<subject> subjects = loadMyDataset(myDatasetPath);
//
//    
//    vector<IplImage*> imgs;
//    bool printLast = true;
//    for (size_t i = 0; i < subjects.size(); i++) {
//        
//        cvNamedWindow("asd");
//        cvShowImage("asd", subjects[i].img);
//        cvWaitKey(1);
//        
//        FRsub->clearFeatures();
//        FRsub->setLBPimage(subjects[i].img);
//        FRsub->computeLBP(LBP::LBP_82);
//
//		//remember to change output path for features file according to dataset being used*******
//        FRsub->printFeatures(myFeaturePath.c_str(), subjects[i].label, printLast);
//        
//        imgs.push_back(subjects[i].img);
//        
//    }
//    
//    cvDestroyAllWindows();
//    subjects.clear();
//    releaseImgVector(imgs);
//    
//    
//}


//void saveFace(string username)
//{
//	clock_t Ttemp = clock();
//	//Declarations
//	CvCapture *capture;
//	IplImage *frame;
//	int frameCount;
//	int faceCount;
//	int noFaceCount;
//	char filenameBuffer[512];
//	char key = 'a';
//
//	//Initialization
//	frame = NULL;
//	frameCount = 0;
//	faceCount = 0;
//	noFaceCount = 0;
//
//	std::cout << "\t>> Initializing camera...\n";
//	capture = cvCaptureFromCAM(setCam);
//
//	std::cout << "\n\t>> Stand still in front of camera...\n";
//	
//	bool flag = false;
//	IplImage *frameResized = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 3);
//	while(frameCount <= MAX_FRAMES)
//	{
//		T0 = clock();
//		
//		frame = cvQueryFrame(capture);
//
//		T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
//		
//		if(frame)
//		{
//			cvResize(frame, frameResized);
//			frameResized->origin = 0;
//			FRsub->setImage(frameResized);
//		}
//
//		else
//		{
//			std::cout << "\t>> Unable to capture frame.\n";
//			break;		
//		}
//
//		T0 = clock();
//		LOGFILE << AppRecFacialGetTime() << "\t>> Detecting face...\n";
//            
//        cvNamedWindow("camera", 1);
//        cvShowImage("camera", FRsub->getImage());
//        key = cvWaitKey(1);
//        
//        if(FRsub->detectFace())
//        {
//            cvNamedWindow("processed image", 1);
//            cvShowImage("processed image", FRsub->getProcessedImage());
//            
//            T = (1000*(clock() - T0))/CLOCKS_PER_SEC;
//            std::cout << "\t>> Face Detected\n";
//            faceCount++;
//            
//            
//            if (key == 's')
//            {
//#ifdef _WIN32
//                string rootDir = "FaceRec\\ReD\\";
//                string oriDir = rootDir + "Original\\";
//                string procDir = rootDir + "Processed\\";
//                string nameOriDir = oriDir + username + "\\";
//                string nameProcDir = procDir + username + "\\";
//                string unameDir = username + "\\";
//#elif __APPLE__
//                string rootDir = "FaceRec/ReD/";
//                string oriDir = rootDir + "Original/";
//                string procDir = rootDir + "Processed/";
//                string nameOriDir = oriDir + username + "/";
//                string nameProcDir = procDir + username + "/";
//                string unameDir = username + "/";
//#endif
//
//                
//                //verify if user already exists
//                bool isRegistered = verifyFilename(rootDir.c_str(), username);
//                
//                if (isRegistered) {
//                    
//                    //saving image to 'Original Images' folder
//                    sprintf(filenameBuffer, "%s%ssample%d.jpg", oriDir.c_str(),
//                            unameDir.c_str(), getLabelFromFolder(nameOriDir));
//                    
//                    cvSaveImage(filenameBuffer, FRsub->getImage());
//                    
//                    //saving image to 'Processed Images' folder
//                    sprintf(filenameBuffer, "%s%ssample%d.jpg", procDir.c_str(),
//                            unameDir.c_str(), getLabelFromFolder(nameProcDir));
//                    
//                    cvSaveImage(filenameBuffer, FRsub->getProcessedImage());
//                    
//                }
//                else {
//                    string makeDirCmd = "mkdir ";
//                    string mkdirOriCmd = makeDirCmd + nameOriDir;
//                    string mkdirProcCmd = makeDirCmd + nameProcDir;
//                    
//					//creating directory for new user
//					system(mkdirOriCmd.c_str());
//					system(mkdirProcCmd.c_str());
//
//					//saving image to 'Original Images' folder
//                    sprintf(filenameBuffer, "%s%ssample%d.jpg", oriDir.c_str(),
//                            unameDir.c_str(), 0);
//                    
//                    cvSaveImage(filenameBuffer, FRsub->getImage());
//                    
//                    //saving image to 'Processed Images' folder
//                    sprintf(filenameBuffer, "%s%ssample%d.jpg", procDir.c_str(),
//                            unameDir.c_str(), 0);
//                    
//                    cvSaveImage(filenameBuffer, FRsub->getProcessedImage());
//                }
//            }
//            
//        }
//        
//        else {
//            std::cout << "\t>> No Face Detected\n";
//        }
//        
//        frameCount++;
//    }
//
//
//	T = clock() - Ttemp;
//	std::cout << "\t>> Processing time: " << T << "\n";
//    cvDestroyAllWindows();
//	cvReleaseCapture(&capture);
//
//
//}

//size_t getLabelFromFolder(string dirName)
//{
//    vector<string> vecFilenames = getFolderFiles(dirName, "jpg");
//
//	//if number of files in folder is 0, it means the label can be 0.
//    if (vecFilenames.size() == 0) {
//        
//        string makeDirCmd = "mkdir " + dirName;
//        
//        //creating directory for new user
//        system(makeDirCmd.c_str());
//    
//		return 0;
//    }
//
//	//search for the next label to be assigned
//	size_t label;
//    label = vecFilenames.size();
//
//	return label;
//}
//
//bool verifyFilename(string regFilePath, string username)
//{
//    string rfPath = regFilePath + "regList.dat";
//    ifstream fin(rfPath, std::ios::in);
//
//    
//    string line;
//    if (fin.is_open()) {
//        
//        while (!fin.eof()) {
//            fin >> line;
//            
//            if (line == username) {
//                fin.close();
//                return true;
//            }
//        }
//
//    }
//    fin.close();
//    
//    ofstream fout(rfPath, std::ios::app);
//    
//    fout << username << std::endl;
//    fout.close();
//    
//    return false;
//
//}

//vector<string> getFolderFiles(string dir, string fileExt)
//{
//    vector<string> filenames;
//    char* filename;
//    
//#if defined WIN32
//    WIN32_FIND_DATA FindData;
//    HANDLE hFind;
//    
//    char directory[512];
//    char finalDir[512];
//    sprintf(directory, "%s", dir.c_str());
//    
//    strcat(directory, "*.");
//    strcat(directory, fileExt.c_str());
//    
//    hFind = FindFirstFile(directory, &FindData);
//    filename = FindData.cFileName;
//    sprintf(finalDir, "%s%s", dir.c_str(), filename);
//    filenames.push_back(finalDir);
//    
//    while (FindNextFile(hFind, &FindData))
//    {
//        filename = FindData.cFileName;
//        
//        sprintf(finalDir, "%s%s", dir.c_str(), filename);
//        filenames.push_back(finalDir);
//    }
//    
//#elif defined __APPLE__
//    DIR* dir_point = opendir(dir.c_str());
//    
//    if (dir_point == NULL) {
//        return filenames;
//    }
//    
//    dirent* entry = readdir(dir_point);
//    while (entry){									// if !entry then end of directory
//        if (entry->d_type == DT_DIR){				// if entry is a directory
//            std::string fname = entry->d_name;
//            if (fname != "." && fname != "..")
//                getFolderFiles(entry->d_name, fileExt);	// search through it
//        }
//        else if (entry->d_type == DT_REG){		// if entry is a regular file
//            std::string fname = entry->d_name;	// filename
//												// if filename's last characters are extension
//            if (fname.find(fileExt, (fname.length() - fileExt.length())) != std::string::npos)
//                filenames.push_back(fname);		// add filename to results vector
//        }
//        entry = readdir(dir_point);
//    }
//
//#endif
//    
//    return filenames;
//}
//
//void releaseImgVector(vector<IplImage*> imgs)
//{
//    for (int i = 0; i < imgs.size(); i++) {
//        cvReleaseImage(&imgs[i]);
//    }
//}

