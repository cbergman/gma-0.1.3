void init_prob_window( int length );
void destroy_prob_window( );

int fill_window( FILE * output, int range, char *chr, int pos, char base, int flag, int qual, int threshold );
int move_window( FILE * output, int range, char *chr, int pos, char base, int flag, int qual, int threshold );
void clean_windows( FILE * output, char *org_chr, int org_pos, int length, int threshold );
//void clean_windows( FILE * output, char *org_chr, int org_pos , int qual, int length );

