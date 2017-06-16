#include "FaceRecognition.h"
#include "KNN.h"
#include "AppRecFacial.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <wiringPi.h>

using namespace std;

/**@mainpage
*@authors Tarique Cavalcante / Valberto Enoc / Thomaz Maia
*
*@brief This project consists of an API for Face Recognition methods. 
*
* <p>Some functionalities was developed to serve as an example of use.</p>
*
* <p>1 - Recognition 1 by 1 (ID validation)</p>
* <p>2 - Recognition 1 by N (ID identification)</p>
* <p>3 - Database management through .xml file</p>
*
*/

/**
 * @brief Application that recognizes subscribed users, it loads database from the file generated on "generateDatabase()"
 * @details it checks if user to be recognized is similar to subscribed users and than it confirms user identity
 */
void Help()
{
	cout<<"Usage: \n"<<"\t./FaceRecognition -cam [device] -app [application] OR\n";
	cout<<"\t./FaceRecognition -gen\n";
	cout<<"\t-cam sets the capture device to usb camera\n";
	cout<<"\t[device] can be 0, 1 or 2,corresponding to the camera\n"; 
	cout<<"\t\t connected on /dev/video[device]\n\n";
	cout<<"\t-app sets which aplication to be used.\n";	
	cout<<"\t[application] can be 1, 2 corresponding to the application.\n";

}

void menuHeader(){
	AppRecFacialClearScreen();
	printf("     _____              ____                            _ _   _                \n");
	printf("    |  ___|_ _  ___ ___|  _ \\ ___  ___ ___   __ _ _ __ (_) |_(_) ___  _ __     \n");
	printf("    | |_ / _` |/ __/ _ \\ |_) / _ \\/ __/ _ \\ / _` | '_ \\| | __| |/ _ \\| '_ \\    \n");
	printf("    |  _| (_| | (_|  __/  _ <  __/ (_| (_) | (_| | | | | | |_| | (_) | | | |   \n");
	printf("    |_|  \\__,_|\\___\\___|_| \\_\\___|\\___\\___/ \\__, |_| |_|_|\\__|_|\\___/|_| |_|   \n");
	printf("                                            |___/               v1.0 Release 03\n");
	//printf("--------------------------------------------------------------------------------\n");
	printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
}

void menuHelp(int menu)
{
	menuHeader();

	switch(menu)
	{
		case Subscribe:
			printf("                              --- Subscribe Menu ---                            \n");
			printf("\tUsage: \n");
			printf("\t1. Position user in front of camera. \n");
			printf("\t2. Type user name. \n");
			printf("\t3. Verify user posture. \n");
			printf("\t4. Press Enter. \n");
			printf("\t(*) Type 'CANCEL' to cancel operation. \n\n");
			printf("\t>> Type your name: ");
			break;

		case RecognizeOne:
			printf("                       --- Validate Identification Menu ---                            \n");
			printf("\tUsage: \n");
			printf("\t1. Type your name. \n");
			printf("\t2. Position yourself in front of camera. \n");
			printf("\t3. Press Enter. \n");
			printf("\t(*) Type 'CANCEL' to cancel operation. \n\n\n");
			printf("\t>> Type your name: ");
			break;
				
		case RecognizeAny:
			printf("                           --- Recognize Face Menu ---                            \n");
			printf("\tUsage: \n");
			printf("\t1. Position yourself in front of camera. \n");
			printf("\t2. Press Enter.");
			
			break;

		case RemoveUser:
			printf("                          --- Unsubscribe User Menu ---                            \n");
			printf("\n\tUsage: \n");
			printf("\t1. Type user index. \n");
			printf("\t2. Press Enter. \n\n");
			printf("\t(*) Type 'CANCEL' to cancel operation. \n\n");

			AppRecFacialListUsers();
			
			printf("\t>> Type user index: ");
			break;

		case RemoveEnrollment:
			printf("                         --- Remove Enrollment Menu ---                            \n");
			printf("\tUsage: \n");
			printf("\t1. Type user index. \n");
			printf("\t2. Type enrollment index. \n\n");

			AppRecFacialListUsers();
						
			break;
		
		case ListEnrollments:
			printf("                          --- List Enrollment Menu ---                            \n");
			printf("\tUsage: \n");
			printf("\t1. Type user index. \n");
			printf("\t2. Type enrollment index. \n\n");
						
			AppRecFacialListUsers();

			printf("\t>> Type user index: ");
			break;

		case ResearchAndDevelopment:
//			printf("                          --- Research and Development Menu ---                            \n");
//            printf("\n\t1 - Save Faces");
//            printf("\n\t2 - GenerateLBP");
//			printf("\n\t>> Choose the mode: ");
//			break;


		case ListUsers:
			printf("                            --- Subscribed Users ---                            \n\n");
						
			break;
	}


}

int main(int argc, char* argv[])
{


	// DEFININDO PORTAS DE GPIO**
	wiringPiSetup();
	int rele = 0, BUTTON = 2, R = 8, G =7 , B =9 ;
	pinMode(rele, OUTPUT);
	pinMode(R, OUTPUT);
	pinMode(G, OUTPUT);
	pinMode(B, OUTPUT);
	pinMode(BUTTON, INPUT);

	digitalWrite(rele, HIGH);
	digitalWrite(R, HIGH);
	digitalWrite(G, HIGH);
	digitalWrite(B, HIGH);

	unsigned long previousMillis = 0;
	// *****************************

	//generateDatabase(); 
	//int app = RecognizeAny;
	char nameBuffer[512];
	int gui = 0;
	int cam = 0;
	bool run = true;

	if (argc>1 && (strcmp (argv[1],"-cam")==0))
	{ 
		cam=atoi(argv[2]);
		cout<<"\tUsing camera on /dev/video"<<cam<<endl;
	}

    if(gui){
        if (AppRecFacialInit()) {
            AppRecFacialShowCamOutput(cam);
        }
    }
    else
	{
		int *param = new int[8];
		param[0] = 950; // Distance Threshold
		param[1] = 1;   // DoG
		param[2] = 1;   // Gamma
		param[3] = 1;   // Equalize
		param[4] = cam; // Camera                                  
		param[5] = 15;  // Num. m�x frames
	
		  if(!AppRecFacialInit())
			{
				AppRecFacialConfigure(param);
				char a;
				bool flag = true;
				while(flag)
				{
					menuHeader();
					std::cout << "\t>> Database Empty.\n\t>> Subscribe User? (y/n)\n\t";
					cin.getline(nameBuffer, 10);
					a = nameBuffer[0];

					if(a == 'y' || a == 'Y')
					{
						string username;

						menuHelp(Subscribe);
												
						cin.getline(nameBuffer, 512);
						username = nameBuffer;						
						username = AppRecFacialValidateString(username);

						if(!strcmp(username.c_str(), "CANCEL") || !strcmp(username.c_str(), "cancel"))
							break;

						if(strcmp(username.c_str(), "AppFailed"))
						{
							if(AppRecFacialProcess(Subscribe, username.c_str()))
								flag = false;
							else
								std::cout << "\t>> Try again.\n";
						}
						else
							std::cout << "\n\t>> Invalid username: only LETTERS are allowed.\n\n";

					}
					if(a == 'n' || a == 'N')
					{
						std::cout << "\t>> Application will now exit.";				
						flag = false;
						run = false;
					}	

					if(a != 'y' && a != 'n')
						std::cout << "\n\t>> Invalid command.\n";
						
				}
			}
			
			else
				AppRecFacialConfigure(param);

		  while (run)
		  {
				menuHeader();
				printf("                              --- Subscribe Menu ---                            ");
				printf("\n\t1 - Subscribe user");
				printf("\n\t2 - Validate ID");
				printf("\n\t3 - Recognize face");					  
				printf("\n\t4 - Remove user");
                printf("\n\t5 - Remove enrollment");
				printf("\n\t6 - List users");
				printf("\n\t7 - List enrollments");
                printf("\n\t8 - R&D");
				printf("\n\t9 - Finalize application");
				printf("\n\t>> Choose the mode: ");

				char app;
				string username;				

				cin.getline(nameBuffer, 512);
				app = nameBuffer[0];

				switch (app)
				{			
					case '1': 
						menuHelp(Subscribe);
					
						cin.getline(nameBuffer, 512);
						username = nameBuffer;
						username = AppRecFacialValidateString(username);

						if(!strcmp(username.c_str(), "CANCEL"))
							break;

						if(strcmp(username.c_str(), "AppFailed"))
							AppRecFacialProcess(Subscribe, username.c_str());
						else
							std::cout << "\n\t>> Invalid username.\n\tOnly LETTERS are allowed.\n\n";
													
						std::cout << "\n\t>> Press Enter to continue...";
						getchar();

						break;

				  case '2': 
						menuHelp(RecognizeOne);

						cin.getline(nameBuffer, 512);
						username = nameBuffer;
						username = AppRecFacialValidateString(username);

						AppRecFacialProcess(RecognizeOne, username.c_str());
						std::cout << "\n\t>> Press Enter to continue...";
						getchar();
						break;

				  case '3': 
						menuHelp(RecognizeAny);
						
						getchar();
						if (AppRecFacialProcess(RecognizeAny)) 
						{
							std::cout << "Delay de 3 segundos!";
							delay(3000);
							AppRecFacialProcess(RecognizeAny); 
							std::cout << "\n\tSEGUNDO PROCESSO!";
						}

						getchar();
						
						break;		

				  case '4': 
						menuHelp(RemoveUser);
						
						int index;
						scanf("%d", &index);//cin.getline(nameBuffer, 512);
						//index = (int)nameBuffer[0] - 48;

						AppRecFacialProcess(RemoveUser, username.c_str(), index);
						std::cout << "\n\t>> Press Enter to continue...";
						getchar();
						getchar();
						break;

				  case '5': 
						menuHelp(RemoveEnrollment);												

						printf("\t>> Type user index: ");
						scanf("%d", &index);//cin.getline(nameBuffer, 512);
						//index = (int)nameBuffer[0] - 48;
						if(AppRecFacialProcess(ListEnrollments, 0, index))
						{
							printf("\t>> Type enrollment index: ");
							int position;
							scanf("%d", &position);//cin.getline(nameBuffer, 10);
							//position = (int)nameBuffer[0] - 48;

							AppRecFacialProcess(RemoveEnrollment, 0, index, position);							
						}

						std::cout << "\n\t>> Press Enter to continue...";
						getchar();
						getchar();
							break;

				  case '6': 
						menuHelp(ListUsers);
						AppRecFacialProcess(ListUsers);

						std::cout << "\n\t>> Press Enter to continue...";
						getchar();
						break;

				  case '7': 
						menuHelp(ListEnrollments);
						//cin.getline(nameBuffer, 512);
						scanf("%d", &index);//index = (int)nameBuffer[0] - 48;

                        AppRecFacialProcess(ListEnrollments, 0, index);
						std::cout << "\n\t>> Press Enter to continue...";
						getchar();
						getchar();
						break;
                        
                  case '8':
						menuHelp(ResearchAndDevelopment);
                        
                        AppRecFacialProcess(ResearchAndDevelopment);

						break;

					case '&': 
						AppRecFacialProcess(ResetDatabase);
						break;

					case '9': 
						AppRecFacialFinalize();
                        run = false;
						break;	

				  default: std::cout << "\n\n\t>> Invalid command.\n";
						break;
				}
			}
		}
		return 0;
}

			
