#define VUL_DEFINE
#include "vul_xml.h"

int main( int argc, char **argv )
{
	// Open the XML file

	// Read XML file into a single long null-terminated string
	char *doc = "";

	// Parse it
	vul_node_t root;
	int ret = vul_xml_parse( &root, doc );

	// Print it
	vul_graph_dfs( &root, vul_xml_print_node, VUL_GRAPH_PRE );
	
	vul_graph_delete( &root );

	// Wait for input
	char buffer[ 1024 ];
	gets_s( buffer, 1024 );
}