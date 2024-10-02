#include<iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>


using namespace std ;


class FileSystem {
private:
	struct Node {

		bool is_directory ;
		std :: string name ;
		std :: map< string , Node* > children ;

		Node(const std :: string & name , bool isDir = false ) : name(name) , is_directory(isDir) {}
	};

	std :: string current_path ;
	Node * root ;
	Node * current_directory ;

	vector< string > splitPath( const string & path ) const
	{
		if ( path.empty() ) return {} ;

		std::vector<string> result ;

		size_t  start = 0 , end = 0 ;

		while ( (end = path.find('/' , start )  ) != string :: npos )
		{
			result.push_back( path.substr( start , end - start  ) ) ;
			start = end + 1 ;
		}
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

	Node * findNode( const string & path )
	{
		if ( path.empty() || path.front() == '/' )
			return nullptr ;

		Node * current = current_directory ;
		std::vector<string> components = splitPath( path ) ;

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

	Node * findDirectory( const string & path )
	{
		std::vector<std::string> components = splitPath(path)  ;

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

	void clearFileSystem(Node* node )
	{
		for ( auto & entry : node->children )
		{
			clearFileSystem( entry.second ) ;
		}

		delete node ;
	}

public:

	FileSystem() {
		root = new Node("/" , true ) ;
		current_directory = root ;
		current_path = "" ;
	}

	~FileSystem() {
		clearFileSystem( root ) ;
	}

	void setCurrentPath( const string & path )
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
			size_t index = current_path.find_last_of('/') ;

			if ( index != string :: npos )
				current_path.erase( index  ) ;
		}
		else {
			current_path = current_path + "/" +  path ;
		}
	}

	string currentDirectory()
	{
		return current_path.empty() ? "/" : current_path ;
	}

	void mkdir( const string & name )
	{
		try {
			if ( current_directory->children.find(name) != current_directory->children.end() )
				throw runtime_error("Directory Already Exist." )  ;

			Node * new_directory = new Node(name , true ) ;

			current_directory->children[name] = new_directory ;
		}
		catch ( exception & e ) {
			cerr << "Exception : " << e.what() << endl  ;
		}
	}

	void cd( const string & path ) {

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
			catch ( exception & e )
			{
				cerr << "Exception : " << e.what() << endl ;
			}
		}
	}

	void ls( const string & path = "" )
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
			cout << endl ;
		}
		catch ( exception & e )
		{
			cerr << "Exception : " << e.what() << endl ;
		}
	}

	void touch(const string & file_name )
	{
		if ( file_name.empty() )
		{
			return ;
		}
		else {

			if ( current_directory->children.count( file_name ) )
			{
				cout << file_name << " file Already Exist" << endl ;
			}
			else
			{
				Node * new_file = new Node(file_name , false ) ;

				current_directory -> children[file_name] = new_file ;
			}
		}
	}

	void mv( const string & source , const string & destination )
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

			cout << source << " Moved to " << destination << endl ;
		}
		catch ( std :: exception & e )
		{
			std :: cerr << "Error : " << e.what() << std :: endl ;
		}
	}

	void cp( const string & source , const string & destination )
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

			cout << source << " Copied to " << destination << endl ;
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
			std :: cerr << "Error : " << e.what() << endl ;
		}
	}


	vector< string > splitString( const string & input ) const {

		std::vector<string> tokens ;

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

		cout << "C:" << file.currentDirectory() << "$ " ;

		string command ;
		getline( cin , command ) ;

		if ( command == "" )
			continue ;

		std::vector< string > tokens = file.splitString( command ) ;

		string operation = tokens.front() ;
		tokens.erase( tokens.begin() ) ;

		// cout << tokens[0] << endl ;

		if ( operation == "exit" )
		{
			break ;
		}
		else if ( operation == "mk" && tokens.size() == 1 )
		{
			file.mkdir( tokens[0] ) ;
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
		else {
			cout << "Invalid command " << endl ;
		}


	}
	return 0 ;
}