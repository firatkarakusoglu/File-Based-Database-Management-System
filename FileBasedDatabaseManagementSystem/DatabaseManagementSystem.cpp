//FILE BASED DATABASE MANAGEMENT SYSTEM
//FIRAT KARAKUSOGLU   
//firatkarakusoglu@gmail.com

#include "stdafx.h"
#include <stdio.h>
#include <cstdlib>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <stdlib.h>
#include <io.h>
#include "f_graph.h" //basic graphic and some additional functions

using namespace std;

//////////////////////////////////////
#pragma region RECORD MANAGER
#define PAGE_SIZE 80 //PAGE (BLOCK) SIZE IN BYTES
#define MAXLINE 256 //Maximum line length in catalog file
#define CACHE_SIZE 3 //Default cache size is 3 pages

char *oneRecord;

struct dbBlock{
	char block[PAGE_SIZE];
};

struct page{
	int pageId;
	dbBlock oneBlock;
}cache[CACHE_SIZE];//CACHE_SIZE (3 default) page caches are going to be hold in memory

FILE *databaseFile;
FILE *catalogFile;
FILE *bulkInsertFile;

//catalog information
int recordSize = 0;
int maxRecordsInOnePage = 0;
int totalNumberOfPages = 0;
int numberOfRecords = 0;
int numberOfAttributes = 0;
int attributeSizes[100];
int attributeTypes[100];
string attributeNames[100];

int currentPageId = -1;
int currentRecordId = -1;

//Creates file with a given (_fileName) name/location
void CreateStore(FILE *_file, char *_fileName)
{
	//if it does not exist , file is being created
	_file = fopen(_fileName,"w+b");
}
//Opens database file with -assign2.db- if not exists then file is created
void OpenDatabaseFile()
{
	//DATABASE  FILE
	//database file is getting checked to read
	databaseFile = fopen("assign2.db","r+b");
	if(databaseFile == NULL) CreateStore(databaseFile, "assign2.db");
}
//Opens cataloge file with -assign2.cat- if not exists then file is created
void OpenCatalogFile()
{
	//CATALOG  FILE
	//database file is getting checked to read
	catalogFile = fopen("assign2.cat","r+b");
	if(catalogFile == NULL) CreateStore(catalogFile, "assign2.cat");
}
//Opens cataloge file with -bulkInsert.txt- if not exists then file is created
void OpenBulkInsertFile()
{
	//CATALOG  FILE
	//database file is getting checked to read
	bulkInsertFile = fopen("bulkInsert.txt","r+b");
	if(bulkInsertFile == NULL) CreateStore(bulkInsertFile, "bulkInsert.txt");
}
//Calls OpenDatabaseFile() and OpenCatalogFile()
void OpenStore()
{
	OpenDatabaseFile();
	OpenCatalogFile();
}

//Closes database 'assign2.db' file
void CloseDatabaseFile()
{
	//DATABASE  FILE
	if(databaseFile != NULL) fclose(databaseFile);
}

//Closes catalog 'assign2.cat' file
void CloseCatalogFile()
{
	//CATALOG  FILE
	if(catalogFile != NULL) fclose(catalogFile);
}

//Closes 'bulkInsert.txt' file
void CloseBulkInsertFile()
{
	//BULK INSERT  FILE
	if(bulkInsertFile != NULL) fclose(bulkInsertFile);
}

//Calls CloseDatabaseFile() and CloseCatalogFile()
void CloseStore()
{
	CloseDatabaseFile();
	CloseCatalogFile();
}

//attribute types are read and saved into attributeTypes array
//0 = string //1 = integer //2 = float //3 = double
void AssignAttributeTypes(int attributeNumber,char *attributeType)
{
	int intAttributeType = -1;
	if( strcmp(attributeType,"string") == 1)  intAttributeType = 0;
	else if( strcmp(attributeType,"integer") == 1 ||  strcmp(attributeType,"int") == 1)  intAttributeType = 1;
	else if( strcmp(attributeType,"float") == 1)  intAttributeType = 2;
	else if( strcmp(attributeType,"double") == 1)  intAttributeType = 3;

	attributeTypes[attributeNumber] = intAttributeType;
}

//returns attribute types in human readable format
string GetAttributeType(int _attributeType)
{
	switch(_attributeType)
	{
		case 0: return "string";
		case 1: return "integer";
		case 2: return "float";
		case 3: return "double";
	}
}

void CalculateRecordSize();
void CalculateMaxRecordsInOnePage();
void GetTotalNumberOfPages();
void CalculateTotalNumberOfPages();

//Opens catalog file
//Reads number of rows, number of attributes, attribute sizes, attribute names, attribute types
void ReadCatalogFile()
	{
		OpenCatalogFile();
		static const char fileName[] = "assign2.cat";
		if(catalogFile != NULL)
		{
			char line[MAXLINE];

			int lineNumber = 0;
			// reading a line
			while(fgets(line,sizeof line,catalogFile) != NULL)
			{
				int attributeNumber = numberOfAttributes-(lineNumber/4);

				//first line saves the number of rows
				if(lineNumber == 0) numberOfRecords = atoi(line);
				//second line saves the last attribute number
				//which is equal to the total number of attributes
				else if(lineNumber == 1) numberOfAttributes = atoi(line);
				//attribute is saved once every 4th row
				else if(lineNumber%4 == 0) attributeSizes[attributeNumber] = atoi(line);
				else if(lineNumber%4 == 2) attributeNames[attributeNumber-1] = line;
				else if(lineNumber%4 == 3) AssignAttributeTypes(attributeNumber-1, line);

				lineNumber++;
			}
		}
		CloseCatalogFile();

		//Calculating and Storing Record&Page Details
		CalculateRecordSize();
		CalculateMaxRecordsInOnePage();
		CalculateTotalNumberOfPages();

	}

//updates catalog file ,number of records information
void SaveNumberOfRecordsToCatFile()
{
	OpenCatalogFile();
	static const char fileName[] = "assign2.cat";
	char buffer[5];
	itoa(numberOfRecords,buffer,10);
	buffer[4]='\0';
	if(catalogFile != NULL)
	{
		fputs(buffer, catalogFile);
		fputs("\r",catalogFile);
	}
	CloseCatalogFile();
}

//calculates the record size by summing attribute sizes after reading catalog file
//record size is saved to "recordSize" global variable
void CalculateRecordSize()
{
	int oneRecordSize = 0;
	for(int i = 0;i < numberOfAttributes; i++)
	{
		oneRecordSize = oneRecordSize + attributeSizes[i];
	}
	recordSize = oneRecordSize+1;//1 is for the null terminating characters \0
}

//calculates the record size by summing attribute sizes after reading catalog file
void CalculateMaxRecordsInOnePage()
{
	//the last character is null character in page file
	int maxRecords = PAGE_SIZE / recordSize;
	maxRecordsInOnePage = maxRecords;
}

//Returns the number of blocks in database
//file size is divided by the block size
//check here if it returns correct number of pages
void CalculateTotalNumberOfPages()
{
	OpenDatabaseFile();
	if(filelength(fileno(databaseFile)) == 0) {totalNumberOfPages = 0;}
	else
	{
		totalNumberOfPages = (filelength(fileno(databaseFile)) / PAGE_SIZE);
	}
	CloseDatabaseFile();
}

//pageId starts with 0
void WritePageToDatabase(int _pageId)
{
	OpenDatabaseFile();
	dbBlock pageToWrite;
	//check cached pages
	for(int i=0;i<CACHE_SIZE;i++)
	{
		if(cache[i].pageId == _pageId)
		{
			pageToWrite = cache[i].oneBlock;
			fseek(databaseFile,_pageId*sizeof(struct dbBlock),SEEK_SET);
			fwrite(&pageToWrite,sizeof(struct dbBlock), 1, databaseFile);
			//flushall();
		}
	};
	CloseDatabaseFile();
}

//first cache is written to the database
//cached pages are moved up by one
//page is cached in the last cache item
void PushToCache(page pageToCache)
{
	bool isPushed = false;

	//if there is a blank item in cache items which has -1 pageId value
	//then page is placed into that space
	for(int i=CACHE_SIZE-1;i>=0;i--)
	{
		if(cache[i].pageId == -1) 
		{
			cache[i] = pageToCache;
			isPushed = true;
			break;
		};
	};

	//if there is no empty space for the page to cache
	//then first item is pushed to database
	//new page is pushed to the end of cache items
	if(!isPushed)
	{
		//this may return when cached page does not exist
		if(cache[0].pageId != -1) WritePageToDatabase(cache[0].pageId);//write the first cached page to file

		//move cached pages up
		for(int i=0;i<CACHE_SIZE-1;i++)
		{
			cache[i] = cache[i+1];
		};

		//put page into the last cached item
		cache[CACHE_SIZE-1] = pageToCache;
	}
}

//cached pages are saved to the database
void SaveCachedPagesToDatabase(void)
{
	OpenDatabaseFile();
	dbBlock pageToWrite;
	for(int i=0;i<CACHE_SIZE;i++)
	{
		pageToWrite = cache[i].oneBlock;
		fseek(databaseFile,cache[i].pageId*sizeof(struct dbBlock),SEEK_SET);
		fwrite(&pageToWrite,sizeof(struct dbBlock), 1, databaseFile);
		//flushall();
	};
	CloseDatabaseFile();
}

//pageId starts with 0
page GetPageFromDatabase(int _pageId)
{
	OpenDatabaseFile();
	page pageReaded;
	dbBlock blockReaded;
	if(_pageId >= 0 && _pageId < totalNumberOfPages)
	{
		fseek(databaseFile,_pageId*sizeof(struct dbBlock),SEEK_SET);
		fread(&blockReaded, sizeof(struct dbBlock), 1,databaseFile);
	}
	pageReaded.oneBlock = blockReaded;
	pageReaded.pageId = _pageId;
	CloseDatabaseFile();
	return pageReaded;
}

//gets the next page from the database
page GetNextPageFromDatabase()
{
	currentPageId++;
	return GetPageFromDatabase(currentPageId);
}

//gets the previous previous from the database
page GetPrevPageFromDatabase()
{
	currentPageId--;
	return GetPageFromDatabase(currentPageId);
}

//gets the last page from the database
page GetLastPageFromDatabase()
{
	return GetPageFromDatabase(totalNumberOfPages-1);
}

//Calculates page id by dividing the row's number to max records in one page
int CalculatePageId(int _recordId)
{
	int recordPageId = ( _recordId / maxRecordsInOnePage);
	return recordPageId;
}

//Calculates record order in a page
//record order is based on 1
int CalculateRecordOrderInPage(int _recordId)
{
	int recordOrderInPage = _recordId % maxRecordsInOnePage;
	return recordOrderInPage+1;
}

//returns true if number of pages are not enough to record all the records
bool isNewPageRequired(int _recordId)
{
	int possibleMaxRecords = totalNumberOfPages * maxRecordsInOnePage;
	//record id is 0 based, so we add 1
	if(possibleMaxRecords < (_recordId + 1)) return true;
	else return false;
}

//returns true if related page is in cache already
bool isPageInCache(int _pageId)
{
	for(int i=0;i<CACHE_SIZE;i++)
	{
		if(cache[i].pageId == _pageId) return true;
	}

	return false;
}

//pulling file into cache to fill the cache with the data and make it ready
void StartCache()
{
	for(int i=0;i<CACHE_SIZE;i++)
	{
		if(totalNumberOfPages>i) cache[i] = GetPageFromDatabase(i);
		else cache[i].pageId = -1;
	};
}

//Checks record (size and type) 
bool CheckRecord(char * _recordValue, int _recordOrder)
{
		int recordSize = attributeSizes[_recordOrder];
		int recordType = attributeTypes[_recordOrder];

		if(sizeof(_recordValue) > recordSize) return false;
		else if(recordType == 1)//integer
		{
			if(atoi(_recordValue) ==0 ) return false;
		}
		else if(recordType == 2)//float
		{
			if(atof(_recordValue) ==0 ) return false;
		}
		else if(recordType == 3)//double
		{
			if(atol(_recordValue) ==0 ) return false;
		}
		return true;
}

//initilizes the record, places empty lines and null character at end
char* InitializeRecord(char *_record)
{
	oneRecord = (char*)malloc(recordSize);
	for(int i=0;i<recordSize;i++)
	{
		oneRecord[i]=' ';
	}
	oneRecord[recordSize-1] = '\0';
	return oneRecord;
}

//initilizes the attribute, places empty lines and null character at end
void InitializeAttribute(char *_attribute)
{
	//_attribute = (char*)malloc(MAXLINE);
	for(int i=0;i<MAXLINE;i++)
	{
		_attribute[i]=' ';
	}
	_attribute[MAXLINE-1] = '\0';
}

void SaveNewRecordToCache();
void DisplayCurrentRecord();

//Opens bulk insert file
//Reads attribute sizes, attribute names, attribute types
void ReadBulkInsertFile()
	{
		OpenBulkInsertFile();
		//if(bulkInsertFile != NULL)
		{
			bool isErrorInRecords = false;
			char line[MAXLINE];

			int lineNumber = 0;
			// reading a line
			
			while(fgets(line,sizeof line,bulkInsertFile) != NULL && isErrorInRecords == false)
			{
				
				char attributeValue[MAXLINE];
				InitializeAttribute(attributeValue);

				oneRecord = (char*)malloc(recordSize);
				oneRecord = InitializeRecord(oneRecord);
				char newRecord[MAXLINE];
				int previousAttributeSize = 0;
				int lineCharacterCounter=0;
				int attributeCharacterCounter=0;

				for(int i=0;i<numberOfAttributes;i++)
				{
						if(i>0) previousAttributeSize += attributeSizes[i-1];
						int attributeSize = attributeSizes[i];
						int attributeType = attributeTypes[i];
						attributeCharacterCounter=0;
						//cin >>attributeValue;
						while(line[lineCharacterCounter] != ',' && lineCharacterCounter<sizeof(line))
						{
							attributeValue[attributeCharacterCounter] = line[lineCharacterCounter];
							lineCharacterCounter++;
							attributeCharacterCounter++;
						}
						lineCharacterCounter++;

						//attribute value is in check
						if(!CheckRecord(attributeValue, i)) 
						{
							gf.warning("Error In Records!");
							isErrorInRecords = true;
							break;
						}

						strncpy(&oneRecord[previousAttributeSize], attributeValue,attributeSizes[i]);
				}
				lineNumber++;
				SaveNewRecordToCache();
				gf.showRightSide("New Record Has Been Inserted!",1);
				DisplayCurrentRecord();
				gf.wait(2);
			}
		}
		CloseBulkInsertFile();

	}

//looking for the appropriate cached page and saves records there
void SaveNewRecordToCache()
{
	//number of records is increased by one
	currentRecordId = numberOfRecords++;
	currentPageId = CalculatePageId(currentRecordId);
	int recordOrderInPage = CalculateRecordOrderInPage(currentRecordId);

	if(isNewPageRequired(currentRecordId))
	{
		 page newPage;
		 //setting the page id property of the page
		 newPage.pageId = currentPageId;
		 //oneRecord is the new record has gotten from user
		 //copying into newpage's block
		 int startPosition = (recordOrderInPage-1)*recordSize;
		 for(int i=0;i<recordSize;i++)
		 {
			 newPage.oneBlock.block[startPosition]= oneRecord[i];
			 startPosition++;
		 }

		 PushToCache(newPage);
		 totalNumberOfPages++;
	}
	else
	{
		//if page is not in the cache then database is read and page is placed into cache
		if(!isPageInCache(currentPageId))
		{
			if(currentPageId < totalNumberOfPages)
			{
				PushToCache(GetPageFromDatabase(currentPageId));
			}
		}
		
		//checking the cache in reverse order
		for(int i=CACHE_SIZE-1;i>=0;i--)
		{
			if(cache[i].pageId == currentPageId)
			{
				int startPosition = (recordOrderInPage-1)*recordSize;
				for(int j=0;j<recordSize;j++)
				{
					cache[i].oneBlock.block[startPosition]= oneRecord[j];
					startPosition++;
				}
			}
		}
	}

}

//record is written into oneRecord global variable
bool GetRecord(int _recordId)
{
	oneRecord = (char*)malloc(recordSize);
	oneRecord = InitializeRecord(oneRecord);

	if(numberOfRecords > _recordId)
	{
		//gf.showRightSide("Looking for the record with record id: ");cout<<_recordId+1;

		//number of records is increased by one
		currentRecordId = _recordId;
		currentPageId = CalculatePageId(currentRecordId);
		int recordOrderInPage = CalculateRecordOrderInPage(currentRecordId);

		//if page is not in the cache then database is read and page is placed into cache
		if(!isPageInCache(currentPageId))
		{
			//gf.showRightSide("Record is not in the cache");
			if(currentPageId < totalNumberOfPages)
			{
				//gf.showRightSide("Page has been brought from database, page id: ");cout<<currentPageId+1;
				PushToCache(GetPageFromDatabase(currentPageId));
			}
		}

		//gf.showRightSide("Record has been found in the cache");
		//checking the cache in reverse order to achieve the page faster in case it just cached from database
		for(int i=CACHE_SIZE-1;i>=0;i--)
		{
			if(cache[i].pageId == currentPageId)
			{
				//gf.showRightSide("Record is set as the current record");

				int startPosition = (recordOrderInPage-1)*recordSize;
				for(int j=0;j<recordSize;j++)
				{
					oneRecord[j] = cache[i].oneBlock.block[startPosition];
					startPosition++;
				}
			}
		}
		return true;
	}
	return false;
}

//fill the oneRecord variable with the first record's values
bool GetFirstRecord()
{
	return GetRecord(0);
}

//fill the oneRecord variable with the next record's values
bool GetNextRecord()
{
	return GetRecord(currentRecordId+1);
}

//fill the oneRecord variable with the previous record's values
bool GetPrevRecord()
{
	return GetRecord(currentRecordId-1);
}

//deletes the record with a given record id
bool DeleteRecord(int _recordId)
{
	if(numberOfRecords>_recordId)
	{
		//number of records is increased by one
		currentRecordId = _recordId;
		currentPageId = CalculatePageId(currentRecordId);
		int recordOrderInPage = CalculateRecordOrderInPage(currentRecordId);

		//if page is not in the cache then database is read and page is placed into cache
		if(!isPageInCache(currentPageId))
		{
			if(currentPageId < totalNumberOfPages)
			{
				PushToCache(GetPageFromDatabase(currentPageId));
			}
		}

		//checking the cache in reverse order to achieve the page faster in case it just cached from database
		for(int i=CACHE_SIZE-1;i>=0;i--)
		{
			if(cache[i].pageId == currentPageId)
			{
				int startPosition = (recordOrderInPage-1)*recordSize;
				for(int j=0;j<recordSize;j++)
				{
					cache[i].oneBlock.block[startPosition]= ' ';
					startPosition++;
				}
			}
		}
		return true;
	}
	return false;
}

//deletes the current records' values
bool DeleteCurrentRecord()
{
	if(currentRecordId != -1)
	{
		DeleteRecord(currentRecordId);
		return true;
	}
	return false;
}

//finds record with a given id and updates with given values
bool UpdateRecord(int _recordId, char * _newRecordValue)
{
	if(numberOfRecords > _recordId)
	{
		//number of records is increased by one
		currentRecordId = _recordId;
		currentPageId = CalculatePageId(currentRecordId);
		int recordOrderInPage = CalculateRecordOrderInPage(currentRecordId);

		//if page is not in the cache then database is read and page is placed into cache
		if(!isPageInCache(currentPageId))
		{
			if(currentPageId < totalNumberOfPages)
			{
				PushToCache(GetPageFromDatabase(currentPageId));
			}
		}

		//checking the cache in reverse order to achieve the page faster in case it just cached from database
		for(int i=CACHE_SIZE-1;i>=0;i--)
		{
			if(cache[i].pageId == currentPageId)
			{
				int startPosition = (recordOrderInPage-1)*recordSize;
				for(int j=0;j<recordSize;j++)
				{
					cache[i].oneBlock.block[startPosition] = _newRecordValue[j];
					startPosition++;
				}
			}
		}
		return true;
	}
	return false;
}

//updates the current record with given values
bool UpdateCurrentRecord(char * _newRecordValue)
{
	if(currentRecordId != -1)
	{
		UpdateRecord(currentRecordId, _newRecordValue);
		return true;
	}
	return false;
}

//displays current record in human readable format
void DisplayCurrentRecord()
{
	if(currentRecordId!= -1)
	{
		int attributeBegin = 0;
		for(int i=0;i<numberOfAttributes;i++)
		{
			string attributeName = attributeNames[i];
			int attributeSize = attributeSizes[i];
			//int attributeType = attributeTypes[i];

			gf.showRightSide("Attribute Name: ");cout<<attributeName; 
			gf.showRightSide("Attribute Value: ");
		
			for(int j=0;j<attributeSize;j++)
			{
				cout<<oneRecord[attributeBegin++];
			}
		}
	}
	else
	{
		gf.showRightSide("Current Record Is Empty!");
	}
}
#pragma endregion RECORD MANAGER
//////////////////////////////////////

//////////////////////////////////////
#pragma region DISPLAY MANAGER

short menuId = 1;
int displaySecondMenu(void);

int displayFirstMenu(void)  
{ 	
	menuId = 1;

	gf.makeSkeleton();
	gf.headerLine("Database Management System");
	gf.showLeftSide("   MENU - 1  ",1);
	gf.showLeftSide("1 Open Database"); 
	gf.showLeftSide("2 Get 1st Rec.");
	gf.showLeftSide("3 Get Next Rec.");
	gf.showLeftSide("4 Get Prev. Rec.");
	gf.showLeftSide("5 Get Nth Rec.");
	gf.showLeftSide("6 Insert Record");
	gf.showLeftSide("7 Bulk Insert");
	gf.showLeftSide("8 Delete Record");
	gf.showLeftSide("0 Next Menu");

	gf.footerLine("Enter your choice [0-9]: ");
	int intUserChoice = -1;
	cin>>intUserChoice;
	while((intUserChoice<1 || intUserChoice>8) & intUserChoice!=0 )
	{
		gf.warning("Error: Your choice should be [1-8]!");
		gf.footerLine("Enter your choice [1-8]: ");
		cin>>intUserChoice;
	};
	return intUserChoice; 
};
int displaySecondMenu(void)  
{ 
	menuId = 2;

	gf.makeSkeleton();
	gf.headerLine("Database Management System");
	gf.showLeftSide("   MENU - 2  ",1);
	gf.showLeftSide("9 Update Record");
	gf.showLeftSide("10 Find Record");
	gf.showLeftSide("11 Show Catalog");
	gf.showLeftSide("12 Get 1st Page");
	gf.showLeftSide("13 Get Next Page");
	gf.showLeftSide("14 Show Stats.");
	gf.showLeftSide("15 Commit Change");
	gf.showLeftSide("16 EXIT");
	gf.showLeftSide("0 Prev. Menu");

	gf.footerLine("Enter your choice [9-16]: ");
	int intUserChoice = -1;
	cin>>intUserChoice;

	while((intUserChoice<9 || intUserChoice>16) & intUserChoice!=0)
	{ 
		intUserChoice = -1;
		gf.warning("Error: Your choice should be [9-16]!");
		gf.footerLine("Enter your choice [9-16]: ");
		cin>>intUserChoice;
	};
	return intUserChoice;
};

void switchMenu(void)
{
	if(menuId==1){menuId=2;}
	else if (menuId==2){ menuId=1;}
}

//OPENING DATABASE
void menu1(void)
{
	OpenStore();
	ReadCatalogFile();
	StartCache();

	gf.showRightSide("DATABASE DETAILS",1);

	gf.showRightSide("Cataloge file has been read...");
	gf.showRightSide("Database file has been opened...");
	gf.showRightSide("Database file has been read...");

	gf.showRightSide("Database Name: assign2.db");
	gf.showRightSide("Catalog File Name: assign2.cat");

	gf.showRightSide("Page Size: "); cout<<PAGE_SIZE<<" Bytes";
	gf.showRightSide("Number of pages: "); cout<<totalNumberOfPages;
	gf.showRightSide("Number of records: "); cout<<numberOfRecords;
	gf.showRightSide("Number of attributes: "); cout<<numberOfAttributes;
};

//GET FIRST RECORD
void menu2(void)
{ 
	gf.showRightSide("FIRST RECORD",1);
	GetFirstRecord();
	DisplayCurrentRecord();
};

//GET NEXT RECORD
void menu3(void)
{ 
	gf.showRightSide("NEXT RECORD",1);
	GetNextRecord();
	DisplayCurrentRecord();
};

//GET NEXT RECORD
void menu4(void)
{ 
	gf.showRightSide("PREVIOUS RECORD",1);
	GetPrevRecord();
	DisplayCurrentRecord();
};

//GET Nth RECORD
void menu5(void)
{ 
	gf.showRightSide("Nth RECORD",1);
	int recordIdToDisplay = -1;
	gf.footerLine("Enter record id to display: "); cin>>recordIdToDisplay;
	if(recordIdToDisplay>0 && recordIdToDisplay<=numberOfRecords)
	{
		GetRecord(recordIdToDisplay-1);
		DisplayCurrentRecord();
	}
	else gf.warning("There is no such record in the system!");
};

//INSERT RECORD
void menu6(void)
{ 
	char attributeValue[MAXLINE];
	
	oneRecord = (char*)malloc(recordSize);
	oneRecord = InitializeRecord(oneRecord);

	char newRecord[MAXLINE];
	int previousAttributeSize = 0;
	for(int i=0;i<numberOfAttributes;i++)
	{
		string attributeName = attributeNames[i];
		
		if(i>0) previousAttributeSize += attributeSizes[i-1];
		int attributeSize = attributeSizes[i];
		int attributeType = attributeTypes[i];
		gf.showRightSide("INSERT RECORD",1);
		gf.showRightSide("Attribute Name: ");cout<<attributeName;
		gf.showRightSide("Attribute Type: ");cout<<GetAttributeType(attributeType);
		gf.showRightSide("Attribute Size: ");cout<<attributeSize;

		gf.footerLine("Enter value for the attribute: ");cin>>attributeValue;
		//cout << attributeName;// <<"Size: "<<attributeSize<<" (bytes): "<<"\nType: "<<attributeType<<"\nValue: ";
		//cin >>attributeValue;
		//flushall();
		while(!CheckRecord(attributeValue, i)) 
		{
			gf.warning("Enter attribute value again!");
			//cout << "\n enter again!";
			
			//cout << attributeName;// <<"Size: "<<attributeSize<<" (bytes): "<<"\nType: "<<attributeType<<"\nValue: ";
			//cin >>attributeValue;

			gf.footerLine("Enter value for the attribute: ");cin>>attributeValue;

		}
		strncpy(&oneRecord[previousAttributeSize], attributeValue,attributeSizes[i]);//&oneRecord[previousAttributeSize]
	}

	SaveNewRecordToCache();
};

//INSERT BULK RECORD
void menu7(void)
{
	int isUserSure = 0;

	gf.footerLine("Are you sure you want to bulk insert file [0/1]: ");cin>>isUserSure;
	if(isUserSure == 1)
	{
		ReadBulkInsertFile();
	}

}

//DELETE CURRENT RECORD
void menu8(void)
{ 
	gf.showRightSide("DELETE CURRENT RECORD",1);
	DisplayCurrentRecord();
	int isUserSure = 0;
	gf.footerLine("Are you sure you want to delete current value[0/1]: ");cin>>isUserSure;
	if(isUserSure == 1)
	{
		if(DeleteCurrentRecord()) 
		{
			gf.showRightSide("DELETE CURRENT RECORD",1);
			gf.showRightSide("Record Has Been Deleted Successfuly");
		}
		else gf.warning("Error Occured!");

	}
};

//UPDATE RECORD
void menu9(void)
{ 
	gf.showRightSide("UPDATE RECORD",1);
	DisplayCurrentRecord();

	int isUserSure = 0;
	gf.footerLine("Are you sure you want to update current value[0/1]: ");cin>>isUserSure;
	if(isUserSure == 1)
	{
		char attributeValue[MAXLINE];

		oneRecord = (char*)malloc(recordSize);
		oneRecord = InitializeRecord(oneRecord);

		char newRecord[MAXLINE];
		int previousAttributeSize = 0;
		for(int i=0;i<numberOfAttributes;i++)
		{
			string attributeName = attributeNames[i];

			if(i>0) previousAttributeSize += attributeSizes[i-1];
			int attributeSize = attributeSizes[i];
			int attributeType = attributeTypes[i];
			gf.showRightSide("UPDATE RECORD",1);
			gf.showRightSide("Attribute Name: ");cout<<attributeName;
			gf.showRightSide("Attribute Type: ");cout<<GetAttributeType(attributeType);
			gf.showRightSide("Attribute Size: ");cout<<attributeSize;

			gf.footerLine("Enter value for the attribute: ");cin>>attributeValue;
			//cout << attributeName;// <<"Size: "<<attributeSize<<" (bytes): "<<"\nType: "<<attributeType<<"\nValue: ";
			//cin >>attributeValue;
			//flushall();
			while(!CheckRecord(attributeValue, i)) 
			{
				gf.warning("Enter attribute value again!");
				//cout << "\n enter again!";

				//cout << attributeName;// <<"Size: "<<attributeSize<<" (bytes): "<<"\nType: "<<attributeType<<"\nValue: ";
				//cin >>attributeValue;

				gf.footerLine("Enter value for the attribute: ");cin>>attributeValue;
			}
			strncpy(&oneRecord[previousAttributeSize], attributeValue,attributeSizes[i]);//&oneRecord[previousAttributeSize]
		}

		//Updating current record
		UpdateCurrentRecord(oneRecord);
	}
};

//FIND RECORD
void menu10(void)
{
	gf.showRightSide("FIND RECORD",1);
	
	string valueToFind;
	gf.footerLine("Enter attribute value to find: ");cin>>valueToFind;
	
	int firstAttributeSize = attributeSizes[0];
	char *firstAttributeValue = (char*)malloc(MAXLINE);
	InitializeAttribute(firstAttributeValue);
	for(int i=0;i<numberOfRecords;i++)
	{
		GetRecord(i);

		for(int i=0;i<firstAttributeSize;i++)
		{
			firstAttributeValue[i]=oneRecord[i];
		}

		if(strncmp(valueToFind.c_str(),firstAttributeValue,firstAttributeSize)==0)
		{
			DisplayCurrentRecord();
		}
	}

}

//SHOW CATALOG FILE
void menu11(void)
{ 
	gf.showRightSide("CATALOG FILE DETAILS",1);

	gf.showRightSide("Catalog File Name: assign2.cat");

	gf.showRightSide("Page Size: "); cout<<PAGE_SIZE<<" Bytes";
	gf.showRightSide("Number of pages: "); cout<<totalNumberOfPages;
	gf.showRightSide("Number of records: "); cout<<numberOfRecords;
	gf.showRightSide("Number of attributes: "); cout<<numberOfAttributes;

	
	for(int i=0;i<numberOfAttributes;i++)
	{
		string attributeName = attributeNames[i];

		int attributeSize = attributeSizes[i];
		int attributeType = attributeTypes[i];
		gf.showRightSide("Attribute Name:");cout<<attributeName;
		gf.showRightSide("Attribute Type:");cout<<GetAttributeType(attributeType);
		gf.showRightSide("Attribute Size:");cout<<attributeSize;
	}
};

//GET FIRST PAGE
void menu12(void)
{ 
	gf.showRightSide("GET FIRST PAGE",1);
	for(int i=0;i<maxRecordsInOnePage;i++)
	{
		GetRecord(i);
		gf.showRightSide("Page Id:"); cout<<currentPageId+1;
		gf.showRightSide("Record Id:"); cout<<currentRecordId+1;
		DisplayCurrentRecord();
	}
}

//GET NEXT PAGE
void menu13(void)
{ 
	gf.showRightSide("GET NEXT PAGE",1);
	int pageBegin = (currentPageId+1)*maxRecordsInOnePage;
	for(int i=0;i<maxRecordsInOnePage;i++)
	{
		GetRecord(pageBegin+i);
		gf.showRightSide("Page Id:"); cout<<currentPageId+1;
		gf.showRightSide("Record Id:"); cout<<currentRecordId+1;
		DisplayCurrentRecord();
	}
}

//SHOW BUFFER STATUS
void menu14(void)
{ 
	gf.showRightSide("SHOW BUFFER STATUS",1);

	int numberOfPagesInBuffer = 0;
	gf.showRightSide("Buffered Pages;"); 
	for(int i=0;i<CACHE_SIZE;i++)
	{
		if(cache[i].pageId != -1 ) 
		{
			gf.showRightSide("Page Id:"); cout<<cache[i].pageId+1;
		}
	}
}

//COMMIT CHANGES
void menu15(void)
{ 
	gf.showRightSide("COMMIT CHANGES",1);
	
	int isUserSure = 0;
	gf.footerLine("Are you sure you want to commit changes[0/1]: ");cin>>isUserSure;
	if(isUserSure == 1)
	{
		SaveCachedPagesToDatabase();
		SaveNumberOfRecordsToCatFile();

		gf.showRightSide("Changes have been committed to database."); 

	}
}


//EXIT
int menu16(void)
{ 
	gf.showRightSide("EXIT",1);
	
	int isUserSure = 0;
	gf.footerLine("Are you sure you want to exit[0/1]: ");cin>>isUserSure;
	if(isUserSure == 1)
	{
		SaveCachedPagesToDatabase();
		SaveNumberOfRecordsToCatFile();
		CloseStore();
		return 1;
	}
	return 0;
}

#pragma endregion DISPLAY MANAGER
//////////////////////////////////////

int main(int argc, char *argv[])
{
	 while(1==1)
	 {
		 if(menuId==1)
		 {
			 switch(displayFirstMenu())
			 {
				 case 1: menu1(); break;
				 case 2: menu2(); break;
				 case 3: menu3(); break;
				 case 4: menu4(); break;
				 case 5: menu5(); break;
				 case 6: menu6(); break;
				 case 7: menu7(); break;
				 case 8: menu8(); break;
				 case 0:  switchMenu();break;
			 };
		 }
		 else if(menuId==2)
		 {
			 switch(displaySecondMenu())
			 {
				 case 9: menu9(); break;
				 case 10: menu10(); break;
				 case 11: menu11(); break;
				 case 12: menu12(); break;
				 case 13: menu13(); break;
				 case 14: menu14(); break;
				 case 15: menu15(); break;
				 case 16: 
					 {
						 if(menu16())
						 {
							 return EXIT_SUCCESS;
							 break;
						 }
					 }
				 case 0: switchMenu(); break;
			 };
		 }
	 };
}
