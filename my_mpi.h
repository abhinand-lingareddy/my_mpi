#define MPI_SUCCESS 1
#define MPI_ERR 0
#define MPI_COMM_WORLD 1
#define MPI_CHAR 1

typedef struct MPI_Status {

}MPI_Status;

int MPI_Init(int *argc, char ***argv);
int MPI_Comm_size ( int comm, int *size_ptr );
int MPI_Comm_rank ( int comm, int *rank_ptr );

int MPI_Send( void *buf, int count, int datatype, int dest,
                     int tag, int comm );
int MPI_Recv( void *buf, int count, int datatype, int source,
                     int tag, int comm, int *status );

int MPI_Finalize( void );
