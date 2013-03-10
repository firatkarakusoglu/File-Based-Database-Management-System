File Based Database Management System
===========================

Introduction
------------
"File Based Database Management System" supports catalog
files, paging, caching and insert, select, update and delete operations.
Concurrency and large files (files that has size more than the memory) are not supported in simple
record manager.

Records and operation results are displayed in a human readable format by a good looking user
interface in console window.

![solution explorer](http://gitscc.codeplex.com/Project/Download/FileDownload.aspx?DownloadId=123874)

Features
--------

Data Structures
--------
The record manager uses two data files: the database file (with a .db extension) and the catalog file (with a .cat extension). The database file is a sequence of blocks that stores records, and the catalog file stores the structure of the database.
The information in the catalog file tells the file manager how to read the database file. It contains the number of attributes of a table, their types, their lengths, and their names. The catalog will also store statistics about the table. In this case, the only relevant statistic is the number of rows in the table.
The database will contain tuples with four attributes:

1.  student number - integer
2.	last name - 6 byte string, including NULLs
3.	first name - 5 byte string, including NULLs
4.	GPA - float

Together, a record should consume 4+6+5+4 = 19 bytes on a Windows PC.
The database file is made up of blocks. Assume each block is 80 bytes long (although in practice, they're much bigger). Each block contains entire rows of a table, so a block can contain at most 4 rows. Each row is encoded in binary.
Overview
When the database is initialized, a .cat file. The contents of the .cat file will be written by a hard-coded module to contain the structure described above. In practice, the contents of the .cat file (and therefore the structure of the database) would be user-defined, but I would like you to automatically write the structure to the .cat file. If I were to give you another database structure (e.g., add another attribute to the database), you should be able to handle it by just changing the .cat file.
The function of the structure in the .cat file is to a) tell the file manager how to interpret the data in the .db file, and b) to help validate inserts into the database. For interpreting the .db file, the .cat file tells how long rows are, and what the attributes are. Validation of inserts is the process of checking that the data the user is trying to insert fits the structure of the database. For example, an insert into the school database must contain four attributes.
Implementation Issues
Your system should have two major modules: an application manager and a record manager. The task of the application manager is to accept a request from the user and deliver it to the record manager by invoking an appropriate function or functions of the record manager. The record manager carries out the requested task possibly returning a record. Following this, the application manager displays the result on the screen.
Implementation of the Application Manager
Through a simple menu-driven interface, the application manager should present to the users the following capabilities:

1. Open database(dbname) - Open the database and initialize the necessary in-memory structures. The files we will use are dbname.db and dbname.cat. In case the database does not exist, create the files: The catalog should contain the structure, but the database should be empty. 

2. Get first record - Fetch the first record in the database and display it on the screen. Make the fetched record the current one. If there are no records in the database, display an appropriate message.

3. Get next record - Fetch the next record immediately following the current one (in the physical order in which the records are stored) and display it on the screen. If the current record is not known, display an appropriate message on the screen. If the current record is the last one in the physical order, display an appropriate message and stay positioned on the last record. Otherwise, if there is a next record, it becomes the current one. 

4. Get previous record - Fetch the previous record in the database (in the physical order) and display it on the screen. If the current record is not known, display an appropriate message on the screen. If the current record is the first one in the physical order, display an appropriate message and stay positioned on the first record. Otherwise, if there is a previous record, it becomes the current one. 

5. Get Nth record - Find the Nth record in the file. If there are fewer than N records in the database, then return an error.

6. Insert record - Insert a new record into the database. The inserted record becomes the current one. The database should accept a comma-delimited sequence of values, and the record manager should make sure that it is valid (i.e., has enough fields, and the fields are the correct type.) The inserted record should be placed in the last block of the database file.

7. Bulk insert records in file - This is like Insert, but takes a filename as an argument. The file contains a set of rows, each separated by new-lines, to be inserted into the table.

8. Delete record - Delete the current record from the database. Display an error if the current record is not known. Replace the "space" left by the current record with the last record in the file. This record becomes the current record.

9. Update record - Replace the current record with a new one. Stay positioned on the current record. 

10. Find record with first attribute value - Take a value for the first attribute, and find any records containing that attribute.

11. Show catalog file- Fetch the contents of the catalog file and display it on the screen in a readable format so that the user can understand the meaning of individual fields.

12. Get first page - Fetch the contents of the first data page in the database file (in the physical order in which the pages are stored in the file) and display it on the screen. The current record should be the first record of this page. If there are no records in the database, display an error message.

13. Get next page - Fetch the contents of the next data page immediately following the "currently page" and display it on the screen.

14. Show buf stats - Indicate the pages that are in the buffer, and whether they are dirty.

15. Commit changes - This function makes sure that all the changes you have made so far are written to disk. Note that if a block becomes empty, the record manager should shrink the database file by truncating that block.

16. Exit - Terminate the application after flushing all changes to the disk and closing the database.


The API (application programming interface) has the following functions:

1. CreateStore - create and initialize the file to hold the database. 
2. OpenStore - open the file containing the database. 
3. FirstRec - get first record in the physical order. 
4. NextRec - get next record in the physical order. 
5. PriorRec - get previous record in the physical order. 
6. InsertRec - insert a new record. 
7. DeleteRec - delete current record. 
8. UpdateRec - replace the current record with the new one. 
9. CloseStore - make sure all changes are propagated to disk and close the database.

These functions are used internally by the application manager to perform a function requested by the user. Note that the user interface differs slightly from the record manager's API. You may find it convenient to "extend" the record manager's API with few more functions. The precise syntax of the record manager's API is up to you. However, the semantics of functions 3 through 8 should match the definitions of the corresponding functions of the user interface.
As indicated earlier, the entire database should be maintained in a single file. As usual in actual DBMS implementations, the records should be grouped into fixed-sized blocks (pages). In other words, the file is logically divided into fixed-sized pages containing data records. Typically, the page size is either 1K, 2K, 4K or more bytes, but for this assignment fix the block size to exactly 80 bytes. This makes it easier to test your implementation.
Transfer of data between memory and disk should be block-based rather than record-based. This means that the record manager, when accessing a record, should first read from the file the entire block (page) in which the record resides, place the page in a memory buffer, and fetch the record there. Similarly, during an insertion, deletion, or an update, the page is first modified in memory, and then the entire page is written to the file. This arrangement requires that your record manager keeps one or more "page buffers" in memory at all times.
As suggested in the API above, you must maintain a main-memory buffer to store blocks from disk. You should have at least three, but do not implement more than four. One of these buffers should hold the catalog, and the rest should hold data.
Implementation of the Storage Manager

