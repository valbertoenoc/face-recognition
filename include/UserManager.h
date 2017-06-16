
#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <highgui.h>
#include <string>
#include <vector>
#include <cxcore.h>
#include <stdio.h>

using namespace std;

struct Enrollment
{
	string date;
	string imageSample;
	string featuresFile;
	int numOfSamples;
};

struct User
{
  vector <Enrollment> enroll;
	float distance;
	string userName;
};
class UserManager
{
  public:
		UserManager();
		UserManager(char* filename);
		~UserManager();

		void setDatabaseFilename(char* filename);
		void loadRegisters();
		void printRegisters();
	
		void addUser(string userName, float distance, string date, string imageSample, string featuresFile,	int numOfSamples);
		void removeUser(int userID);
		void removeEnrollment(int userID, int enrollNumber);
		void updateEnrollment(int userID, string date, string imageSample, string featuresFile,	int numOfSamples);

		int numOfUsers();
		int numOfEnrollments();
		int numOfEnrollments(int userId);

		int maxEnrollment;
  	char* databaseFilename;
		float distance;
	  vector <User> users;

	

};
#endif