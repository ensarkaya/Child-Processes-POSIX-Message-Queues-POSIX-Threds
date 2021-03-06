/*
	@author: Ensar Kaya
	@date 28.10.19
	project1
*/
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/ipc.h>
#include <sys/msg.h>
// Merges two subarrays of arr[]. 
// First subarray is arr[l..m] 
// Second subarray is arr[m+1..r] 
void merge(int arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m;   
    // create temp arrays
    int L[n1], R[n2]; 
    // Copy data to temp arrays L[] and R[] 
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1+ j];  
    // Merge the temp arrays back into arr[l..r]
    i = 0; // Initial index of first subarray 
    j = 0; // Initial index of second subarray 
    k = l; // Initial index of merged subarray 
    while (i < n1 && j < n2) 
    { 
        if (L[i] <= R[j]) 
        { 
            arr[k] = L[i]; 
            i++; 
        } 
        else
        { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
    // Copy the remaining elements of L[], if there are any 
    while (i < n1) 
    { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    }  
    // Copy the remaining elements of R[], if there are any 
    while (j < n2) 
    { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
}  
// l is for left index and r is right index of the 
//  sub-array of arr to be sorted 
void mergeSort(int arr[], int l, int r) 
{ 
    if (l < r) 
    { 
        // Same as (l+r)/2, but avoids overflow for 
        // large l and h 
        int m = l+(r-l)/2; 
  
        // Sort first and second halves 
        mergeSort(arr, l, m); 
        mergeSort(arr, m+1, r); 
  
        merge(arr, l, m, r); 
    } 
}  
// UTILITY FUNCTIONS 
// Function to print an array 
void printArray(int A[], int size) 
{ 
    int i; 
    for (i=0; i < size; i++) 
        printf("%d ", A[i]); 
    printf("\n"); 
} 
int numberOfDigits(int number)
{
	int count=0;
	while(number != 0)
	{
		number /= 10;
		++count;	
	}
	return count;
}

void operation(char * input, char * output, const int k)
{
	//Open file
	int rfd=open(input, O_RDONLY, 0);
	if (rfd < 0){
   		printf("Open failed..1");
		exit(2);
	};
	//Read file and store it
	int * temp = (int *)malloc(1000000*sizeof(int));
	int n_read;
	int cnt=0, nsofar =0;
	char num[1];	
	int ctrl = 1;
	int nodcnt=0;
	while( (n_read = read(rfd,num,1) ) > 0)
	{		
		if(num[0] == '\n' || num[0] == '\t' || num[0] == ' '){
			if(ctrl==1)
			{
				temp[cnt] = nsofar;
				cnt++; 
				nodcnt += numberOfDigits(nsofar);
				nsofar=0;		
			}
			ctrl=0;			
		}
		else{
			nsofar *= 10;		
			nsofar += atoi(num);
			ctrl = 1;
		}	
	};
	//printf(" nodcnt:%d, cnt:%d ",nodcnt,cnt);	
	int * arr = (int *)malloc(cnt*sizeof(int));
	for(int j = 0; j<cnt;j++)
		arr[j]=temp[j];
	free(temp);
	
	//printArray(arr,cnt);
	//Close
	close(rfd);
	//Sort
	mergeSort(arr, 0, cnt-1); 
	//printArray(arr,cnt);

	//NEW
	char temp_array[nodcnt+cnt];
	int nod = numberOfDigits(arr[cnt-k]);
	int p=0;
	for(int l=0; l < k; l++)
	{
		int slct = arr[cnt-k+l];
		//printf("slct:%d\n",slct);
		p+=sprintf(&temp_array[p],"%d ", slct);
		//sprintf(&temp_array[])	
		nod += numberOfDigits(slct);	
	}

	//Return	
	strcpy(output,temp_array);
	//printf(" out:%s\n",output);	
	//Close
	free(arr);	
}
struct mesg_buffer{
	long mesg_type;
	char mesg_text[6000];
} message;
int main(int argc, char *argv[]) {
	struct timeval startTime, endTime;    	
	long elapsedTime;
	gettimeofday(&startTime, NULL);
	// k , N , # of N arg, output
	if(argc < 5 && argc > 9){
		printf("Wrong number of arguments \n");
		exit(1);
	}
	int k, n;

	k = atoi(argv[1]);
	n = atoi(argv[2]);
	if(k<1 || k>1000){
		printf("K is out of bounds \n");
		exit(1);	
	}
	if(n<1 || n>5){
		printf("N is out of bounds \n");
		exit(1);	
	}
	
	int * temp = (int *)malloc(n*k*sizeof(int));
	for(int i=0;i<n;i++){
		char* infile = argv[(3+i)];
		int pid = fork();
		if(pid == 0){
			//printf("child %d\n", i);
			char text[6000]="0";
			operation(infile,text,k);//burdayım
			//printf("text1: %s\n",text);			
			key_t key;
			int msgid;			
			//ftok() to generate unique key
			if((key = ftok("/home/ensar/Desktop/project1"
			, i))==(key_t)-1){
				perror("IPC error: ftok");
				exit(1);			
			}				
			//printf("Key is: %d\n",(int)key);
			msgid = msgget(key, 0666| IPC_CREAT);
			message.mesg_type=1;
			//printf("ID is: %d\n",msgid);
			strcpy(message.mesg_text,text);
			//printf("message.mesg_text: %s\n",message.mesg_text);
			msgsnd(msgid, &message, 
			sizeof(message),0); 
			//printf("Data send is: %s\n",message.mesg_text);
			_exit(0);
		}
		wait(NULL);
	}
	for(int l=0;l<n;l++)
	{
		//Main process
		//Read msg and store it
		//printf("parent %d\n", l);
		int msgid;
		key_t key;
		key = ftok("/home/ensar/Desktop/project1", l);
		msgid = msgget(key,0666|IPC_CREAT);		
		msgrcv(msgid,&message, 
		sizeof(message),1,0);
		//printf("Data received: %s\n",message.mesg_text);
		//temp[l] = atoi(message.mesg_text);
		//NEW
		int cnt=0,nsofar=0;
		for(int i=0;message.mesg_text[i]!='\0'; i++)
		{
			char num[1];
			num[0] =message.mesg_text[i];
			//printf("num:%d\n",num[0]);
			if(num[0] == '\n' || num[0] == '\t' || num[0] == ' '){
					temp[cnt+(l*k)] = nsofar;
					//printf("n:%d\n",nsofar);
					cnt++; nsofar=0;
					
			}
			else{
				nsofar *= 10;		
				nsofar +=atoi(num);
			}
		}
		//NEW END
		//printf("temp: %d\n", temp[l]);		
	}
	mergeSort(temp,0, (n*k)-1);
	//printArray(temp, n*k);
	int ofd=open(argv[3+n],O_RDWR|O_CREAT, 0777);
	if (ofd < 0){
   		printf("Open failed..2");
		exit(2);
	};
	//NEW
	for(int l=0; l <k; l++)
	{
		//printf("slct:%d\n",temp[(n*k)-l-1]);
		int slct = temp[(n*k)-l-1];
		int nod = numberOfDigits(slct);
		char temp_array[nod];	
		sprintf(temp_array,"%d", slct);	
		//Write
		write(ofd,temp_array,nod);
		write(ofd, "\n", strlen("\n"));
	}
	//Close
	close(ofd);	
	free(temp);
	gettimeofday(&endTime, NULL);
	elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000000 +
	endTime.tv_usec - startTime.tv_usec;
	printf("findtopk_mqueue in ms: %ld\n",  elapsedTime);
	return(0);
}

