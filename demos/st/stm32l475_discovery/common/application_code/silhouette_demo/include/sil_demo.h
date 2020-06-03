#ifdef UNPRIV_API_BENCHMARK
void start_microbenchmark( void ) PRIVILEGED_FUNCTION;
#else
void start_microbenchmark( void );
#endif
void start_beebsbenchmark( void );
