#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t registers;   /* semaphore for register */
sem_t customers; /* semaphore for customers */


#define CUSTOMER_NUMBER 25 /*number of customers*/
#define REGISTER_NUMBER 5  /*number of registers*/
#define CUSTOMER_ARRIVAL_TIME_MIN 1 /*minimum customer arrival time*/
#define CUSTOMER_ARRIVAL_TIME_MAX 3 /*maximum customer arrival time*/
#define COFEE_TIME_MIN 1 /*minimum coffee time*/
#define COFEE_TIME_MAX 5 /*maximum coffee time*/


void customerFun(void *count); /*function to send a customer to a register*/
void registerFun(void *count); /*function for serving a customer */

int customerId[CUSTOMER_NUMBER]; /*using arrays to assign ids to registers and customers*/
int registerID[REGISTER_NUMBER]; 


int activeCustomer = 0; /* customer that is ready to be served */
int *waitingCustomers; 

int main(int argc, char **args)
{
   sem_init(&customers, 0, 0); /*initializing semaphores for registers and customers*/
   sem_init(&registers, 0, 0);


   waitingCustomers = (int *)malloc(sizeof(int) * CUSTOMER_NUMBER); /* allocating memory for each customer*/

    if(CUSTOMER_ARRIVAL_TIME_MIN>CUSTOMER_ARRIVAL_TIME_MAX ||COFEE_TIME_MIN>COFEE_TIME_MAX){ /* checking for logical errors */
        printf("Min value is greater than max value\n");
        return EXIT_FAILURE;
    }

    if (CUSTOMER_NUMBER < 1){
      printf("No customer to be served\n");
      return EXIT_FAILURE;
    }

    if (REGISTER_NUMBER < 1){
      printf("No avaliable registers\n");
      return EXIT_FAILURE;
    }
   

   for (size_t i = 0; i < CUSTOMER_NUMBER; i++) /* putting ids in arrays */
   {
      customerId[i]=i;
      waitingCustomers[i]=i;
   }

   for (size_t i = 0; i < REGISTER_NUMBER; i++) 
   {
      registerID[i]=i;
   }
   

   pthread_t registerArr[REGISTER_NUMBER], customerArr[CUSTOMER_NUMBER]; /*threads */
 

   /* create register threads */
   for (int i = 0; i < REGISTER_NUMBER; i++)
   {
      pthread_create(&registerArr[i], NULL, (void *)registerFun, (void *)&registerID[i]);
   }


   /* create customers threads */
   
   for (int i = 0; i < CUSTOMER_NUMBER; i++)
   {
      int t= SleepFunction(CUSTOMER_ARRIVAL_TIME_MIN,CUSTOMER_ARRIVAL_TIME_MAX); /* waiting for a random time for customer arrival*/
      sleep(t);

      pthread_create(&customerArr[i], NULL, (void *)customerFun,(void *)&customerId[i] ); /*arrival of the customer*/
      printf("CUSTOMER %d IS CREATED AFTER  %d SECONDS.\n",customerId[i],t);
        
   }

   for (int i = 0; i < CUSTOMER_NUMBER; i++)
   {
      pthread_join(customerArr[i], NULL); /*joining threads after the customers have been served*/
   }
  
   
   printf("\n----------------ALL CUSTOMERS HAVE BEEN SERVED !----------------\n\n");
   return EXIT_SUCCESS;
}


void customerFun(void *count){

    sem_post(&registers); /*unlocking a register*/
    sem_wait(&customers); /*prevention of a customer to come to the same register*/

    pthread_exit(0); /* closing the customer thread after the serving is complete */
}

void registerFun(void *count){ 
   int nextCustomer, customer_identity;

    while (1)
    {

      /* controlling which customer to go to a register to be served */
      activeCustomer = (activeCustomer) % CUSTOMER_NUMBER;
      nextCustomer = activeCustomer;
      customer_identity = waitingCustomers[nextCustomer];
      activeCustomer++;
      sem_wait(&registers); /* locking the register after customer has come*/
      

      printf("CUSTOMER %d GOES TO REGISTER  %d .\n",customer_identity,*(int*)count);
      int t = SleepFunction(COFEE_TIME_MIN,COFEE_TIME_MAX); /* waiting a random time for coffee */
      sleep(t);
      sem_post(&customers); /* unlocking the register after the coffee has been served */

      printf("CUSTOMER %d FINISHED BUYING FROM REGISTER %d AFTER %d  SECONDS .\n",customer_identity,*(int*)count,t);
    }
    

}
int SleepFunction(int min , int max){ /* determining how long to wait for random time functions */
      srand(time(0));  
      return (rand() % (max - min +1)) + min;
}