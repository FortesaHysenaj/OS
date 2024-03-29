/*Consider a system consisting of processes P1, P2, ..., Pn, each of which has a unique priority
number. Write a monitor that allocates three identical printers to these processes, using the
priority numbers for deciding the order of allocation. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#define _BSD_SOURCE
#include <sys/time.h>
#include <pthread.h>
#include <queue>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <string>

#define BSIZE 8 // Buffer size
#define PWT 2 // Producer wait time limit
#define CWT 10 // Consumer wait time limit
#define RT 10 // Program run-time in seconds

using namespace std;
struct pnode{
	int id;
	int val;
};

pthread_mutex_t mutex1, m;

bool cmpfunc(struct pnode a, struct pnode b){
	if(a.val==b.val)return a.id<b.id;	
	return a.val>b.val;
}

class monitor {
	public:
		queue<int> printer;
		pthread_cond_t pro;
		void wait(int id, int *printer_no){
			pthread_mutex_lock(&m);
			if(printer.empty()) {
				printf("\t\tProcess %d blocked\n\n", (int)id);
				pthread_mutex_unlock(&m);
				pthread_cond_wait(&pro, &mutex1);
				pthread_mutex_lock(&m);
			}

			*printer_no = printer.front();
			printer.pop();
			pthread_mutex_unlock(&m);
		}
		void signal(int printer_no){
			pthread_mutex_lock(&m);
			printer.push(printer_no);
			pthread_cond_signal(&pro);
			pthread_mutex_unlock(&m);
		}
		monitor(){
			printer.push(1);
			printer.push(2);
			printer.push(3);
			pthread_cond_init(&pro, NULL);
		}
};

monitor obj;

void *callThread(void *arg) {
	int id = *(int *)arg;
	int printer_no;
	printf("Process %d requests printer\n\n", (int)id);
	obj.wait(id, &printer_no);
	printf("Process %d assigned printer %d\n\n", (int)id, printer_no);
	sleep(rand() % 10 + 1);
	obj.signal(printer_no);
	printf("Process %d release printer %d\n\n", (int)id, printer_no);
	return 0;
}

int main() {
	int n, i, no;
	cout << "Enter no. of processes: ";
	cin >> n;
	cout << "Enter priority for each process: "<<endl;
	pnode arr[1001];
	for(i = 0; i < n; i++){
		cout << "Process " << i+1 << "-";
		arr[i].id=i+1;
		cin >> arr[i].val;
	}
	sort(arr, arr+n, cmpfunc);
	cout << "\tProcesses prioritywise: " << endl;
	for(i = 0; i < n; i++){
		cout<<"\tProcess "<<arr[i].id<<endl;
	}
	pthread_t process[1000001];
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&m, NULL);
	int x = 0;
	while(1) {
		pthread_create(&process[x], NULL, &callThread, &(arr[x].id));
		x=(x+1)%n;
		sleep(2);
	}
	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&m);
	pthread_exit(NULL);
	return 0;
}

