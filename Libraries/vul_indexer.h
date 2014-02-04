// Indexes all files in a directory/subdirectory into a tree.
// Can be used to check if tree has changes/rescan; stores in nodes
// what change is seen. Can be searched.

/* Define either of these. Agressive does allocations for you with the given (de/)allocator,
   while passive does not handle memory at all. */
#define VUL_INDEXER_AGGRESSIVE
//#define VUL_INDEXER_PASSIVE

#include "vul_file.h"
#include "vul_compress.h"
#include "vul_string.h"
#include "vul_datatypes.h"

enum {
	VUL_INDEXER_NEW = 1,
	VUL_INDEXER_DELETED = 2,
	VUL_INDEXER_UPDATED = 4
} vul_indexer_state_flags;

typedef struct
{
	int ( *deallocator )( void *ptr );
	void *data;
} vul_indexer_data;

typedef struct
{
	// Data
	vul_indexer_data *data;
	// Metadata
	char *name;
	char *full_path;
	unsigned long last_altered;
	// @TODO: Hash if wanted.
	unsigned int state_flags;
} vul_indexer_content;

#define vul__loader unsigned int (*loader)

/**
 * Searches the tree for an occurance of file_name. 
 * Will search the subtrees in priority_list first before searching the rest of the tree.
 * If none is found, returns NULL
 * If one is found, return that node.
 * If multiple are found, the first found one is returned, thus priority_list is followed.
 */
vul_indexer_node *vul_indexer_find( vul_indexer_node *root, const char *file_name, const char **priority_list = NULL, unsigned int priority_list_count = 0 );
/**
 * Iterates through the tree looking for changes to the files. Returns the smallest subtree
 * that contains all changes, and marks which changes are found in the flags parameter.
 */
vul_indexer_data *vul_indexer_is_tree_out_dated( vul_indexer_node *root, unsigned int *flags );

/**
 * Uses the supplied loader to load the data for the given node. Marks the node as loaded.
 */
vul_indexer_data *vul_indexer_load( vul_indexer_node *node, vul__loader( vul_indexer_node *n, void *d ), void *out_data );