#include<iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>


using namespace std ;


class FileSystem {
private:

	struct Node {

		string name ;
		bool isDirectory ;
		map< string , Node* > children ;

		Node(const std :: string & name , bool isDir = false ) : name(name) , isDirectory(isDir) {}
	};

	Node * root ;
	Node * currentDirectory ;

public:

	FileSystem() {
		root = new Node("/" , true ) ;
		currentDirectory = root ;
	}

	~FileSystem() {

	}

	void mkdir( const std :: string & directoryName )
	{
		try {

			if ( currentDirectory->children.find(directoryName) != currentDirectory->children.end() )
				throw " Directory alreay Exist" ;

			Node * newDirectory = new Node( directoryName , true ) ;
			currentDirectory -> children[directoryName] = newDirectory ;
		}
		catch ( std :: exception & e )
		{
			std :: cerr << e.what() << endl ;
		}
	}

	void cd(const std :: string & path) {

	}

	// List the current content of specfied directory
	void ls( const std :: string & path = "" )
	{
		Node * targetDirectory = nullptr ;

		if ( path.empty() )
		{
			targetDirectory = currentDirectory ;
		}
		else {
			// targetDirectory = findDirectory(path) ;
		}

		try {
			if ( !targetDirectory)
				throw "Directory Not Found" ;

			for ( auto & entry : targetDirectory->children )
			{
				cout << entry.first << ' ' ;
			}
			cout << endl ;
		}
		catch ( std ::  exception & e ) {
			std :: cerr << e.what() << endl ;
		}
	}

	// Create a new Empty File

	void touch( const std :: string & fileName ) {

		try {
			if ( fileName.empty() )
				throw "File Name is empty" ;

			if ( currentDirectory->children.find(fileName) != currentDirectory->children.end() )
				throw "File Already Exist" ;

			Node * newFile = new Node(fileName , false ) ;
			currentDirectory->children[fileName] = newFile ;
		}
		catch ( std :: exception & e )
		{
			std :: cerr << e.what() << endl ;
		}
	}

};