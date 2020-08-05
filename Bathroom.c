#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<pthread.h>
#include<semaphore.h>

//create semaphores and count variables

sem_t empty; //semaphore to represent the state of the bathroom whether it is empty or occupied
sem_t man; //semaphore to represent that a man is using the bathroom
sem_t woman; //semaphore to represent that a woman is using the bathroom
sem_t man_count; //semaphore to indicate the maximum number of simultaneous allowable men in the bathroom
sem_t woman_count; //semaphore to indicate the maximum number of simultaneous allowable men in the bathroom
sem_t Barrier; // a semaphore to ensure fairness between men and women to avoid starvation
int W_count = 0 ; //number of women
int M_count = 0 ; //number of men
int Number_of_using_bathroom_males; // Number of times the men are going to use the bathroom
int Number_of_using_bathroom_females; //number of times the women are going to use the bathroom

void man_wants_to_enter ()
{
    sem_wait(&Barrier);
    sem_wait(&man); // main inside
    M_count = M_count + 1; // increment number of men 
    if(M_count == 1){
        sem_wait(&empty);} // raise the sign that there is a male inside
    sem_post(&man); // man exits
    sem_post(&Barrier);
    sem_wait(&man_count); // men are inside and did not exceed maximum capacity
}
void woman_wants_to_enter ()
{
    sem_wait(&Barrier);
    sem_wait(&woman);
    W_count = W_count + 1 ;
    if(W_count == 1){
        sem_wait(&empty);}
    sem_post(&woman);
    sem_post(&Barrier);
    sem_wait(&woman_count);
}
void man_leaves ()
{
    //printf("Man id # %d leaves the bathroom\n", *i);
    sem_post(&man_count); // men exit
    sem_wait(&man); // man acquire the lock to decrement
    M_count = M_count - 1; // decerementing (critical section)
    if(M_count==0){
        sem_post(&empty);} // no men are inside
    sem_post(&man); // man releases the lock 

}
void woman_leaves ()
{
   // printf("Woman # %d leaves the bathroom\n",*i);
    sem_post(&woman_count);
    sem_wait(&woman);
    W_count = W_count - 1 ;
    if(W_count==0){
        sem_post(&empty);}
    sem_post(&woman);


}
void men (int * i)
{
    int j;
    for ( j =0; j < Number_of_using_bathroom_males ; j++) // looping multiple times according to Number_of_using_bathroom_males
    {
    man_wants_to_enter();
    printf("Man # %d is in the bathroom\n", *i);
    sleep(2);
    printf("Man # %d leaves the bathroom\n", *i);
    man_leaves();
    
    
    }
    pthread_exit(0);
}
void women(int * i)
{
    int j;
    for ( j =0; j < Number_of_using_bathroom_females ; j++) // looping multiple times according to Number_of_using_bathroom_females
    {
    woman_wants_to_enter();
   printf("Woman  # %d is in the bathroom\n", *i);
    sleep(2);
    
    printf("Woman # %d leaves the bathroom\n",*i);
    woman_leaves();
    
   
  }
  pthread_exit(0);
}



int main ()
{
    
    sem_init(&empty, 0, 1); 
    sem_init(&man, 0, 1); 
    sem_init(&woman, 0, 1); 
    sem_init(&man_count, 0, 5); // Maximum number of males in the bathroom = 5
    sem_init(&woman_count, 0, 5); // Maximum number of females in the bathroom = 5
    sem_init(&Barrier, 0, 1); 

    int w , m, max ; // male and females threads counts and the max between both
    printf( "Please enter m:");
    scanf("%d", &m);
    printf( "Please enter w:");
    scanf("%d",&w);
    printf("please enter the number of times each man enters the bathroom:");
    scanf("%d",&Number_of_using_bathroom_males);
    printf("please enter the number of times each woman enters the bathroom:");
    scanf("%d",&Number_of_using_bathroom_females);



    pthread_t  *m_tid , *w_tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    m_tid = ((pthread_t*)malloc(m*sizeof(pthread_t)));
    w_tid = ((pthread_t*)malloc(w*sizeof(pthread_t)));
    // m_tid = ((pthread_t*)malloc((m+w)*sizeof(pthread_t)));
     if (m >= w) 
        max=m;
    else 
        max=w;


    int i ;
    int success_m , success_w;

    int * tmids , *tfids ;
    tmids = ((int*)malloc(m*sizeof(int)));
    tfids = ((int*)malloc(w*sizeof(int)));
    for (i=0;i<max;i++)
    {

       if (i < m)
       {     //then create man thread
       tmids[i]=i;
       success_m = pthread_create(&m_tid[i],&attr, (void *) (int *) men , &tmids[i]); //create threads and tmids[i] is used for indexing the threads
       if(success_m!=0)
       { //create returns 0 if thread creation succeeded
           printf("Man thread creation failed\n");
           exit(EXIT_FAILURE);        
       }
       }

       
       if (i < w)
       {  //then create female thread
       tfids[i]=i;
         success_w = pthread_create(&w_tid[i],&attr, (void*)(int*) women , &tfids[i]); //create female thread and tfids[i] is used for indexing the threads
       if(success_w!=0)
       {//create returns 0 if thread creation succeeded
           printf("Woman thread creation failed\n");
           exit(EXIT_FAILURE);        
       }
       }
   }
    

    for (i = 0 ; i < m ; i++)
    {
        pthread_join(m_tid[i],NULL);
    }
    int j;
    for (j = 0 ; j < w ; j++)
    {
        pthread_join(w_tid[j],NULL);
    }

return 0;
}