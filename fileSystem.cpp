#include<iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib> // for system()


struct Node {
	bool is_directory ;
	std :: string name ;
	std :: map< std :: string , Node* > children ;
	std :: vector < std :: string > versions ;

	Node(const std :: string & name , bool isDir = false ) : name(name) , is_directory(isDir) {}
};

class FileSystem {
private:

	std :: string current_path ;
	Node * root ;
	Node * current_directory ;

	vector< std :: string > splitPath( const string & path ) const
	{
		if ( path.empty() ) return {} ;

		std :: vector< std :: string> result ;

		std :: size_t  start = 0 , end = 0 ;

		while ( (end = path.find('/' , start )  ) != std :: string :: npos )
		{
			if ( start != end )
				result.push_back( path.substr( start , end - start  ) ) ;

			start = end + 1 ;
		}

		if ( start != (size_t) path.size() )
			result.push_back( path.substr(start) ) ;

		return result ;
	}

	Node * getParent( Node *& current , Node *& node  ) const
	{
		if ( current == node )
		{
			return current ;
		}

		for ( auto & child : current->children )
		{
			if ( child.second == node )
				return current ;
		}

		for ( auto & child : current -> children )
		{
			if ( child.second -> is_directory )
			{
				Node * found =  getParent( child.second , node ) ;

				if ( found != nullptr )
					return found ;
			}
		}

		return nullptr ;
	}

	Node * findNode( const std :: string & path )
	{
		if ( path.empty() || path.front() == '/' )
			return nullptr ;

		Node * current = current_directory ;
		std :: vector< std :: string> components = splitPath( path ) ;

		for ( auto & component : components )
		{
			if ( component == ".." )
			{
				current = getParent( root , current ) ;
			}
			else if ( current->children.count( component ) )
			{
				current = current->children[component] ;
			}
			else if ( component != "/" )
			{
				return nullptr ;
			}
		}
		return current ;
	}

	Node * findDirectory( const std :: string & path )
	{
		std :: vector< std :: string> components = splitPath(path)  ;

		Node * current = current_directory ;

		for ( auto & component : components )
		{
			if ( component == ".." )
			{
				current = getParent(root , current) ;
				setCurrentPath(component) ;
			}
			else if ( current->children.find(component) != current->children.end() )
			{
				current = current->children[component] ;
				setCurrentPath(component) ;
			}
			else if ( component != "." ) {
				return nullptr ;
			}
		}

		return current ;

	}

	Node * copyNode( Node *& source )
	{
		Node * new_node = new Node( source -> name , source -> is_directory ) ;

		for ( auto &  child : source -> children )
		{
			new_node -> children[child.first] = copyNode( child.second ) ;
		}

		return new_node ;
	}

	void deleteNode( Node *& source )
	{
		if ( !source )
			return ;

		for ( auto & child : source -> children )
		{
			deleteNode( child.second ) ;
		}

		source -> children.clear() ;

		delete source ;
		// source = nullptr ;
	}

	void saveStateHelper( Node *& source , ofstream & file , const std :: string & path )
	{
		if ( !source )
			return ;

		file << path << source->name << " " << (source->is_directory ? "directory" : "file" ) << std :: endl ;

		for ( auto & child : source -> children )
		{
			saveStateHelper( child.second , file ,  path + source->name + (path.empty() ? "" : "/" ) ) ;
		}
	}

	void mkdirFromPath( const std :: string & path )
	{
		Node * current = root ;
		std :: vector< std :: string > components = splitPath( path ) ;

		for ( auto &component : components )
		{
			if ( current->children.find( component ) == current->children.end() )
			{
				current -> children[component] = new Node(component , true ) ;
			}
			current = current->children[component] ;
		}
	}

	void touchFromPath ( const std :: string & path )
	{
		Node * current = root ;
		std :: vector< std :: string > components = splitPath( path ) ;

		for ( size_t i = 0 ; i < components.size() - 1 ; i++ )
		{
			current = current->children[ components[i] ] ;
		}

		current->children[components.back()] = new Node( components.back() , false ) ;
	}

	void loadStateHelper( ifstream & file) {

		deleteNode( root ) ;

		root = new Node("/" , true ) ;
		current_directory = root ;
		current_path = "" ;

		std :: string line ;

		while ( std :: getline( file , line ) )
		{
			std :: istringstream  iss( line ) ;

			std :: string full_path ;
			std :: string type ;

			iss >> full_path >> type ;

			if ( type == "directory" )
			{
				mkdirFromPath( full_path ) ;
			}
			else
			{
				touchFromPath( full_path ) ;
			}
		}
	}

	void createVersion( Node *& file , const std :: string & content )
	{
		if ( !file-> is_directory )
		{
			file -> versions.push_back( content ) ;
		}
	}

public:

	FileSystem() {
		root = new Node("/" , true ) ;
		current_directory = root ;
		current_path = "" ;
	}

	~FileSystem() {
		deleteNode( root ) ;
	}

	void setCurrentPath( const std :: string & path )
	{
		if ( path == "" )
		{
			return ;
		}
		else if ( path == "/" )
		{
			current_path = "" ;
		}
		else if ( path == ".." )
		{
			std :: size_t index = current_path.find_last_of('/') ;

			if ( index != string :: npos )
				current_path.erase( index  ) ;
		}
		else {
			current_path = current_path + "/" +  path ;
		}
	}

	std :: string currentDirectory()
	{
		return current_path.empty() ? "/" : current_path ;
	}

	void mkdir( const std :: string & name )
	{
		try {
			if ( current_directory->children.find(name) != current_directory->children.end() )
				throw runtime_error("Directory Already Exist." )  ;

			Node * new_directory = new Node(name , true ) ;

			current_directory->children[name] = new_directory ;
		}
		catch ( std :: exception & e ) {
			std :: cerr << "Exception : " << e.what() << std :: endl  ;
		}
	}

	void cd( const std :: string & path ) {

		if ( path.empty() ) {
			return ;
		}
		else if ( path == "/") {
			current_directory = root ;
			setCurrentPath(path) ;
		}
		else if (path == "..")
		{
			current_directory = getParent(root , current_directory) ;
			setCurrentPath(path) ;
		}
		else
		{
			Node * target_directory = findDirectory( path ) ;

			try {
				if ( target_directory == nullptr )
					throw runtime_error("Directory Not Found . ") ;

				current_directory = target_directory ;
			}
			catch ( std :: exception & e )
			{
				std :: cerr << "Exception : " << e.what() << std :: endl ;
			}
		}
	}

	void ls( const std :: string & path = "" )
	{
		Node * target_directory  ;

		if ( path.empty() )
			target_directory = current_directory ;
		else
		{
			target_directory = findDirectory( path ) ;
		}

		try {
			if ( !target_directory )
				throw runtime_error("Directory Not Found") ;

			for ( auto & entry : target_directory->children ) {
				cout << entry.first << ' ' ;
			}
			std :: cout << std :: endl ;
		}
		catch ( exception & e )
		{
			std :: cerr << "Exception : " << e.what() << std :: endl ;
		}
	}

	void touch(const std :: string & file_name , const std :: string & content = "" )
	{
		try {

			if ( current_directory->children.find(file_name) != current_directory->children.end() )
			{
				Node * file = current_directory->children[file_name] ;

				if ( !file->is_directory) {
					createVersion( file , content ) ;
					std :: cout << "File " << file_name << " modified" << std :: endl ;
				}
				else
				{
					throw runtime_error("A directory with same name exits") ;
				}
			}
			else {
				Node * new_file = new Node( file_name , false ) ;
				new_file -> versions.push_back( content ) ;
				current_directory -> children[file_name] = new_file ;

				std :: cout << "File " << file_name << " created" << std :: endl ;
			}
		}
		catch ( std :: exception & e )
		{
			std :: cerr << e.what() << std :: endl  ;
		}
	}

	void mv( const std :: string & source , const std :: string & destination )
	{
		try {

			Node * source_node = findNode( source ) ;

			if ( !source_node )
				throw runtime_error("Source Not Found") ;

			Node * destination_node = findNode( destination ) ;

			if ( !destination_node )
				throw runtime_error("Destination Not Found") ;

			if ( destination_node->children.count( source_node->name ) )
				throw runtime_error("Destination already contains a node with the same name") ;


			destination_node -> children [ source_node->name ] = source_node  ;

			if ( root == source_node ) return ;

			Node * source_parent = getParent( root , source_node ) ;
			source_parent->children.erase( source_node-> name ) ;

			std :: cout << source << " Moved to " << destination <<  std :: endl ;
		}
		catch ( std :: exception & e )
		{
			std :: cerr << "Error : " << e.what() << std :: endl ;
		}
	}

	void cp( const std :: string & source , const std :: string & destination )
	{
		try {

			Node * source_node = findNode( source ) ;

			if ( !source_node )
				throw runtime_error("Source Not Found") ;

			Node * destination_node = findNode( destination ) ;

			if ( !destination_node )
				throw runtime_error("Destination Not Found") ;

			if ( destination_node->children.count( source_node->name ) )
				throw runtime_error("Destination already contains a node with the same name") ;


			destination_node -> children [ source_node->name ] = copyNode(source_node)  ;

			if ( root == source_node ) return ;

			std :: cout << source << " Copied to " << destination << std :: endl ;
		}
		catch ( std :: exception & e )
		{
			std :: cerr << "Error : " << e.what() << std :: endl ;
		}
	}

	void rm( const std :: string & name ) {

		try {
			Node * target = findNode( name ) ;

			if ( !target )
				throw runtime_error("Path Not Found") ;

			Node * parent = getParent( root , target ) ;


			if ( parent == target )
				throw runtime_error("Could not remove") ;

			if ( parent->children.count(target->name) )
				parent->children.erase(target->name) ;

			deleteNode( target ) ;

			std :: cout << name << " Has been deleted " << std :: endl  ;

		}
		catch ( std :: exception & e )
		{
			std :: cerr << "Error : " << e.what() << std :: endl ;
		}
	}

	void saveState( const std :: string & file_path )
	{
		try {
			std :: ofstream file( file_path ) ;

			if ( !file.is_open() )
				throw runtime_error("Error opening file for saving") ;

			saveStateHelper( root , file , "" ) ;

			std :: cout << "Saved successfully" << std :: endl ;
			file.close() ;
		}
		catch ( std :: exception & e )
		{
			std :: cerr << e.what() << std :: endl ;
		}
	}

	void loadState( const std :: string & file_path )
	{
		try {
			std :: ifstream file( file_path ) ;

			if ( !file.is_open() )
				throw runtime_error("Error opening file for loading:") ;

			loadStateHelper( file ) ;
			std :: cout << "Loaded successfully " << std :: endl ;
		}
		catch ( std :: exception & e )
		{
			std :: cerr << e.what() << std :: endl ;
		}
	}

	void lsVersions( const std :: string & file_name )
	{
		try {
			Node * file = findNode( file_name ) ;

			if ( !file || file->is_directory )
				throw runtime_error("File not found or it is a directory") ;

			for ( size_t i = 0 ; i < file->versions.size() ; i++ )
			{
				std :: cout << "Version " << i + 1 << ": " << file->versions[i] << std :: endl ;
			}
		}
		catch ( std :: exception & e )
		{
			std :: cerr << e.what() << std :: endl ;
		}
	}

	void rollBack( const std :: string & file_name , const int version )
	{

		try {
			Node * file = findNode( file_name ) ;

			if ( !file || file -> is_directory ) {
				throw runtime_error("File not found or it is a directory") ;
			}

			if ( version > 0 && version <= file -> versions.size() ) {
				std :: cout << "Rolling back " << file_name << " to version " << version << ": " << file->versions[version - 1] << std :: endl ;
			}
			else {
				throw runtime_error("Version number is out of range") ;
			}
		}
		catch ( std :: exception & e ) {
			std :: cerr << e.what() << std :: endl ;
		}
	}

	void openFile( const std :: string & file_name ) {

		try {
			Node * file = findNode( file_name ) ;

			if ( !file || file->is_directory )
				throw std :: runtime_error("File not found or it is a directory ") ;

			// Platform-specific command to open the file
			// #if defined(_WIN32) || defined(_WIN64)
			//     command = "start " + file_name;
			// #elif defined(__APPLE__)
			//     command = "open " + file_name;
			// #elif defined(__linux__)
			//     command = "xdg-open " + file_name;
			// #else
			//     throw std::runtime_error("Unsupported platform");
			// #endif

			std :: string command = "notepad.exe " + file_name ;
			int result = system( command.c_str() ) ;

			if ( result != 0 )
			{
				throw std :: runtime_error("Failed to open file in editor") ;
			}

			// Wait until the user finishes editing (after closing the editor)
			std :: cout << "Please edit the file and save changes." << std :: endl;

			std :: ifstream ifs( file_name ) ; // this will create or system file

			if ( !ifs.is_open() ) {
				throw std :: runtime_error("Could not open file after editing") ;
			}

			std :: string new_content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ifs.close() ;

			createVersion( file , new_content) ;
			std :: cout << "Changed saved to " << file_name << std :: endl ;
		}
		catch ( std :: exception & e )
		{
			std :: cerr << e.what() << endl ;
		}
	}

	std :: vector< std :: string > splitString( const std :: string & input ) const {

		std :: vector< std :: string > tokens ;

		std :: stringstream iss( input ) ;

		std :: string token ;

		while ( iss >> token )
		{
			tokens.push_back( token ) ;
		}

		return tokens ;
	}

};

int main()
{

	FileSystem file ;

	while (1) {

		std :: cout << "C:" << file.currentDirectory() << " > " ;

		std :: string command ;
		getline( std :: cin , command ) ;

		if ( command == "" )
			continue ;

		std :: vector< std :: string > tokens = file.splitString( command ) ;

		string operation = tokens.front() ;
		tokens.erase( tokens.begin() ) ;

		// cout << tokens[0] << endl ;

		if ( operation == "exit" )
		{
			break ;
		}
		else if ( operation == "mkdir" && tokens.size() == 1 )
		{
			file.mkdir( tokens[0] ) ;
		}
		else if ( operation == "touch" && tokens.size() >= 1 )
		{
			file.touch( tokens[0] , tokens.size() > 1 ? tokens[1] : "" );
		}
		else if ( operation == "cd"  )
		{
			file.cd( !tokens.empty() ? tokens[0] : "" ) ;
		}
		else if ( operation == "ls" )
		{
			file.ls( tokens.size() ? tokens[0] : "" ) ;
		}
		else if ( operation == "mv" && tokens.size() == 2 )
		{
			file.mv( tokens[0] , tokens[1] ) ;
		}
		else if ( operation == "cp" && tokens.size() == 2 )
		{
			file.cp( tokens[0] , tokens[1] ) ;
		}
		else if ( operation == "rm" && tokens.size() == 1 )
		{
			file.rm( tokens[0] ) ;
		}
		else if ( operation == "save" && tokens.size() == 1 )
		{
			file.saveState( tokens[0] ) ;
		}
		else if ( operation == "load" && tokens.size() == 1 )
		{
			file.loadState( tokens[0] ) ;
		}
		else if ( operation == "lsversion" && tokens.size() == 1 )
		{
			file.lsVersions( tokens[0] );
		}
		else if ( operation == "revertversion" && tokens.size() == 1 )
		{
			file.rollBack( tokens[0] , stoi( tokens[1]) ) ;
		}
		else if ( operation == "open" )
		{
			file.openFile( tokens[0] ) ;
		}
		else
		{
			std :: cout << "Invalid command " << std :: endl ;
		}


	}
	return 0 ;
}
