#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

#define READY 0
#define RUNNING 1
#define STOPPED 2
#define EXITED 3

struct process_descriptor{
	char *exe_name;
	char *exe_path;
	int priority;
	pid_t pid;
	int execution_status;
	double elapsed_time;
	double workload_time;
};

struct node{
	struct process_descriptor pd;
	struct node *prev;
	struct node *next;
};

struct processes_queue{
	struct node *head;
	struct node *tail;
};


void enqueueProcess(struct processes_queue *queue, struct node *new);

void dequeueProcess(struct processes_queue *queue);

void removeNode(struct processes_queue *queue, struct node *n);


double get_wtime(void);


void sigchldHandler(int signal);


void FCFS();

void SJF();

void RR();

void PRIO();


struct processes_queue queue;
struct processes_queue terminated_processes;
unsigned int quantum;
int flag;
double t0;


int main(int argc, char *argv[]){
	t0 = get_wtime();
	char *file_arg;
	queue.head = NULL;
	queue.tail = NULL;
	terminated_processes.head = NULL;
	terminated_processes.tail = NULL;


	if(argc == 3){
		file_arg = (char*) malloc(sizeof(char) * (strlen(argv[2])+1));
		strcpy(file_arg, argv[2]);
	}else if(argc == 4){
		quantum = atoi(argv[2]);
		file_arg = (char*) malloc(sizeof(char) * (strlen(argv[3])+1));
		strcpy(file_arg, argv[3]);
	}else{
		printf("\nWrong Arguments\n");
		printf("Scheduler exits\n\n");
		return 0;
	}
	

	FILE *file = fopen(file_arg, "r");
	if(file != NULL){
		char line[256];
		char first_field[128];
		char second_field[128];
		char exe_name[128];
		char exe_path[128];
		char *token;

		while (fgets(line, sizeof(line), file)) {
			token = strtok(line, "\t");
			strcpy(first_field, token);
			strcpy(exe_path, token);
			token = strtok(NULL, "\t");
			strcpy(second_field, token);

			token = strtok(first_field, "/");
			while(token != NULL){
				strcpy(exe_name, token);
				token = strtok(NULL, "/");
			}
					
			struct process_descriptor pr_descr;
			pr_descr.exe_name = (char*) malloc(sizeof(char) * (strlen(exe_name)+1));
			strcpy(pr_descr.exe_name, exe_name);
			pr_descr.exe_path = (char*) malloc(sizeof(char) * (strlen(exe_path)+1));
			strcpy(pr_descr.exe_path, exe_path);
			pr_descr.priority = atoi(second_field);
			pr_descr.execution_status = READY;
			pr_descr.elapsed_time = get_wtime();

			struct node *new = (struct node*) malloc(sizeof(struct node));
			new->pd = pr_descr;

			enqueueProcess(&queue, new);
		}
	}else{
		printf("\nFile Not Found\n");
		printf("Scheduler Exits\n\n");
		return 0;
	}


	if(strcmp(argv[1], "FCFS") == 0)
		FCFS();
	else if(strcmp(argv[1], "SJF") == 0)
		SJF();
	else if(strcmp(argv[1], "RR") == 0)
		RR();
	else if(strcmp(argv[1], "PRIO") == 0)
		PRIO();
	else{
		printf("\nWrong policy\n");
		printf("Scheduler exits\n\n");
		return 0;
	}


	struct node *temp = terminated_processes.head;
	while(temp != NULL){
		struct process_descriptor pr_descr = temp->pd;
		printf("\nPID %d - CMD: %s\n", pr_descr.pid, pr_descr.exe_name);
		printf("                        Elapsed Time = %.3lf secs\n", pr_descr.elapsed_time);
		printf("                        Workload Time = %.3lf secs\n", pr_descr.workload_time);
		temp = temp->next;
	}
	printf("WORKLOAD TIME: %.3lf seconds\n\n", get_wtime() - t0);

	return 0;
}



void enqueueProcess(struct processes_queue *queue, struct node *new){
	if(queue->head == NULL && queue->tail == NULL){
		queue->head = new;
		queue->tail = new;
		new->prev = NULL;
		new->next = NULL;
	}else{
		new->next = NULL;
		new->prev = queue->tail;
		queue->tail->next = new;
		queue->tail = new;
	}
}

void dequeueProcess(struct processes_queue *queue){
	if(queue->head != queue->tail){
		queue->head = queue->head->next;
		queue->head->prev->next = NULL;
		queue->head->prev = NULL;
	}else{
		queue->head = NULL;
		queue->tail = NULL;
	}
}

void removeNode(struct processes_queue *queue, struct node *n){
	if(n->prev == NULL){
		dequeueProcess(queue);
	}else if(n->next == NULL){
		queue->tail = queue->tail->prev;
		queue->tail->next->prev = NULL;
		queue->tail->next = NULL;
	}else{
		n->prev->next = n->next;
		n->next->prev = n->prev;
		n->prev = NULL;
		n->next = NULL;
	}
}


double get_wtime(void){
	struct timeval t;
	gettimeofday(&t, NULL);
	return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}


void sigchldHandler(int signal){
	struct process_descriptor pr_descr = queue.head->pd;
	flag = 1;

	pr_descr.elapsed_time = get_wtime() - pr_descr.elapsed_time;
	pr_descr.workload_time = get_wtime() - t0;
	pr_descr.execution_status = EXITED;

	struct node *new = (struct node*) malloc(sizeof(struct node));
	new->pd = pr_descr;
	dequeueProcess(&queue);
	enqueueProcess(&terminated_processes, new);

}


void FCFS(){
	struct process_descriptor pr_descr;

	while(queue.head != NULL && queue.tail != NULL){
		pr_descr = queue.head->pd;

		if(vfork() == 0){
			pr_descr.pid = getpid();
			pr_descr.execution_status = RUNNING;
			pr_descr.elapsed_time = get_wtime();

			execl(pr_descr.exe_path, pr_descr.exe_name, (char*)0);
			exit(0);
		}
		
		wait(NULL);
		pr_descr.execution_status = EXITED;
		pr_descr.elapsed_time = get_wtime() - pr_descr.elapsed_time;
		pr_descr.workload_time = get_wtime() - t0;

		struct node *new = (struct node*) malloc(sizeof(struct node));;
		new->pd = pr_descr;
		enqueueProcess(&terminated_processes, new);
		dequeueProcess(&queue);
	}
}

void SJF(){
	struct processes_queue sorted_queue;
	sorted_queue.head = NULL;
	sorted_queue.tail = NULL;

	struct process_descriptor pr_descr;
	struct node *temp;
	int i = 0;

	while(queue.head != NULL && queue.tail != NULL){
		temp = queue.head;

		while(temp != NULL){
			pr_descr = temp->pd;
			
			if(pr_descr.priority == i){
				struct node *new = (struct node*) malloc(sizeof(struct node));
				new->pd = pr_descr;
				enqueueProcess(&sorted_queue, new);

				struct node *n = temp;
				temp = temp->next;
				removeNode(&queue, n);
			}else{
				temp = temp->next;
			}
		}
		i++;
	}
	
	queue = sorted_queue;
	FCFS();
}

void RR(){
	struct sigaction sa;
    sa.sa_handler = sigchldHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

	struct process_descriptor pr_descr;
	struct timespec sleep_time = {(double)quantum/1000, 0};
	flag = 0;

	while(queue.head != NULL && queue.tail != NULL){
		pr_descr = queue.head->pd;

		if(pr_descr.execution_status == READY){
			
			if(vfork() == 0){
				pr_descr.pid = getpid();
				pr_descr.execution_status = RUNNING;
				pr_descr.elapsed_time = get_wtime();

				execl(pr_descr.exe_path, pr_descr.exe_name, (char*)0);
				exit(0);
			}
			
			nanosleep(&sleep_time, NULL);

			if(flag == 0){
				kill(pr_descr.pid, SIGSTOP);
				pr_descr.execution_status = STOPPED;
				struct node *new = (struct node*) malloc(sizeof(struct node));
				new->pd = pr_descr;
				dequeueProcess(&queue);
				enqueueProcess(&queue, new);
			}else{
				flag = 0;
			}
			
		}else if(pr_descr.execution_status == STOPPED){
			pr_descr.execution_status = RUNNING;
			kill(pr_descr.pid, SIGCONT);
			nanosleep(&sleep_time, NULL);
			
			if(flag == 0){
				kill(pr_descr.pid, SIGSTOP);
				pr_descr.execution_status = STOPPED;
				struct node *new = (struct node*) malloc(sizeof(struct node));
				new->pd = pr_descr;
				dequeueProcess(&queue);
				enqueueProcess(&queue, new);
			}else{
				flag = 0;
			}
		}
	}
}

void PRIO(){
	int *priorities_array = (int *) malloc(sizeof(int));
	int priorities_array_size = 1;

	struct processes_queue **queues_array = (struct processes_queue **) malloc(sizeof(struct processes_queue*));
	int queues_array_size = 1;
	queues_array[0] = (struct processes_queue*) malloc(sizeof(struct processes_queue*));
	queues_array[0]->head = NULL;
	queues_array[0]->tail = NULL;

	struct process_descriptor pr_descr;
	struct node *temp = queue.head;
	priorities_array[0] = temp->pd.priority;
	temp = temp->next;


	while(temp != NULL){
		int found = 0;
		pr_descr = temp->pd;
		
		for(int i=0; i<priorities_array_size; i++){
			if(priorities_array[i] == pr_descr.priority){
				found = 1;
				break;
			}
		}
		
		if(!found){
			queues_array = realloc(queues_array, (queues_array_size + 1)* sizeof(struct processes_queue*));
			queues_array[queues_array_size] = (struct processes_queue*) malloc(sizeof(struct processes_queue*));
			queues_array[queues_array_size]->head = NULL;
			queues_array[queues_array_size]->tail = NULL;
			queues_array_size++;

			priorities_array = realloc(priorities_array, (priorities_array_size + 1)* sizeof(int));
			priorities_array[priorities_array_size] = pr_descr.priority;

			for(int i=priorities_array_size; i>0; i--){
				if(priorities_array[i] < priorities_array[i-1]){
					int temp = priorities_array[i];
					priorities_array[i] = priorities_array[i-1];
					priorities_array[i-1] = temp;
				}else{
					break;
				}
			}
			priorities_array_size++;
		}
		
		temp = temp->next;
	}
	
	for(int i=0; i<priorities_array_size; i++){
		temp = queue.head;

		while(temp != NULL){
			pr_descr = temp->pd;
			
			if(pr_descr.priority == priorities_array[i]){
				struct node *new = (struct node*) malloc(sizeof(struct node));
				new->pd = pr_descr;
				enqueueProcess(queues_array[i], new);

				struct node *n = temp;
				temp = temp->next;
				removeNode(&queue, n);
			}else{
				temp = temp->next;
			}
		}
	}


	for(int i=0; i<priorities_array_size; i++){
		temp = queues_array[i]->head;
		
		if(temp->next == NULL){
			pr_descr = temp->pd;
			
			if(vfork() == 0){
				pr_descr.pid = getpid();
				pr_descr.execution_status = RUNNING;
				pr_descr.elapsed_time = get_wtime();

				execl(pr_descr.exe_path, pr_descr.exe_name, (char*)0);
				exit(0);
			}
			
			wait(NULL);
			double t2 = get_wtime();
			pr_descr.execution_status = EXITED;
			pr_descr.elapsed_time = get_wtime() - pr_descr.elapsed_time;
			pr_descr.workload_time = get_wtime() - t0;

			struct node *new = (struct node*) malloc(sizeof(struct node));
			new->pd = pr_descr;
			enqueueProcess(&terminated_processes, new);

		}else{
			queue = *queues_array[i];
			RR();
		}
	}
}
