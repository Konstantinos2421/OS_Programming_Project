#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>

typedef struct {
    long mtype;
    double mtext;
}mymesg;

double get_wtime(void){
	struct timeval t;
	gettimeofday(&t, NULL);
	return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}

double f(double x){
	return log(x)*sqrt(x);
}

int main(int argc, char *argv[]){
	double a = 1.0;
	double b = 4.0;
	
	int *queues = (int*)malloc(sizeof(int));
	int queues_array_size = 1;

	unsigned long n;
    printf("Insert the number of processes to be executed: ");
    scanf("%ld", &n);

	const double dx = (b-a)/n;
	double S = 0;
	double t0 = get_wtime();

	pid_t main_process_id = getpid();
	int queue_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
	queues[0] = queue_id;

	unsigned long i = 0;
	while(i<n){
		double xi = a + (i + 0.5)*dx;

		mymesg mesg;
		mesg.mtype = 1;
		mesg.mtext = f(xi);
		int signal = msgsnd(queue_id, &mesg, sizeof(mesg.mtext), IPC_NOWAIT);

		if(signal == -1) {
			queues = realloc(queues, (queues_array_size + 1)* sizeof(int));
			queue_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
			queues[queues_array_size] = queue_id;
			queues_array_size++;
			msgsnd(queue_id, &mesg, sizeof(mesg.mtext), IPC_NOWAIT);
		}
		
		i++;
		vfork();
	}

	if(getpid() != main_process_id){
		_exit(0);
	}

	mymesg received_mesg;
    ssize_t received_mesg_size;

	for(int j = 0; j < queues_array_size; j++){
		int finished = 0;

		while(!finished) {

			received_mesg_size = msgrcv(queues[j], &received_mesg, sizeof(received_mesg.mtext), 1, IPC_NOWAIT);
			
			if(received_mesg_size < 0) {
				finished = 1;
			}else{
				S += received_mesg.mtext;
			}

		}
	}

	S *= dx;
	double t1 = get_wtime();
	printf("Time=%lf seconds, Result=%.8f\n", t1-t0, S);
	return 0;
}
