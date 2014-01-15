#define ARGC_MAX 50

void analyzer( int argc, char * argv[] );
void reducer_pair( char *ref, int length, char *bam );
void reducer_single( char *ref, int length, int start_base_pos, char *bam );


