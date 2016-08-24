#define VUL_DEFINE
#include "vul_file.h"
#include "vul_timer.h"

void vul_test_file_monitor( )
{
	vul_file_watch w = vul_file_monitor_change( "testfile.txt" );
	while( 1 ) {
		if( vul_file_monitor_check( w ) ) {
			puts("File changed!");
		} else {
			puts(".");
			vul_sleep( 100 );
		}
	}
	vul_file_monitor_stop( w );
}

void vul_test_file_monitor_blocking( )
{
	while( 1 ) {
		if( vul_file_monitor_wait( "testfile.txt" ) ) {
			puts( "File changed!" );
		} else {
			puts(".");
		}
	}
}

int main()
{
   printf("These are interavtive tests for now!\n" );
   // @TODO(thynn): Write tests for the other functions of this file, and:
   // Test the above by poking the file from other threads, and checking we get
   // the results we want!
   return 0;
}
