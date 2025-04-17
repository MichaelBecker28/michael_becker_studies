/**
 * appserver-coarse.c - Multithreaded Bank Server (Coarse-grained version)
 * CprE 308 Project 2
 * 
 * This implementation uses coarse-grained locking with a single mutex for all accounts.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <pthread.h>
 #include <unistd.h>
 #include <sys/time.h>
 #include "Bank.h"
 
 #define MAX_LINE 256
 #define MAX_TRANS 10  // Maximum number of accounts in a transaction
 
 /* Structure for a transaction */
 struct trans {
     int account_id;   // Account ID
     int amount;   // Amount to add (positive) or subtract (negative)
 };
 
 /* Structure for a request */
 //now! lets explain the variables! yes! fun!
 struct request {
     struct request *next;  // Pointer to next request in queue
     int request_id;        // Unique ID for this request
     int request_type;      // 0 for CHECK, 1 for TRANS
     int check_account_id;      // Account ID for CHECK requests
     struct trans *transactions; // Array of transactions for TRANS requests
     int numba_of_transactions;         // Number of transactions
     struct timeval starttime;  // Request start time
     struct timeval endtime;    // Request end time
 };
 
 /* Structure for the request queue */
 struct queue {
     struct request *head;  // Head of the queue not the tail! right!
     struct request *tail;  // Tail of the queue ^ the head!
     int jobs_in_da_q;          // Number of jobs in the queue
     int done;              // Flag to indicate server shutdown
     pthread_mutex_t lock;  // Mutex for protecting the queue
     pthread_cond_t cond;   // Condition variable for worker threads
 };
 
 /* Global variables */
 int bank_account_num;           // Number of bank accounts
 pthread_mutex_t bank_lock;  // Single mutex for the entire bank
 struct queue request_queue; // Queue for pending requests
 FILE *output_file;          // Output file for results
 pthread_mutex_t file_lock;  // Mutex for protecting file output
 
 //functions & stuff!!
 void initialize_server(int n_accounts);
 void cleanup_server();
 void *worker_thread(void *arg);
 void process_check(struct request *req);
 int process_transaction(struct request *req);
 void add_request(struct request *req);
 struct request *get_request();
 
 /**
  * Main function - Initializes server, processes user input, and cleans up
  */
 int main(int argc, char *argv[]) {
     int thread_count_workers;
     pthread_t *workers;
     char input_line[MAX_LINE];
     char *token;
     int i;
 
      /* Check command line arguments and make sure they are correct*/
     /* Validate command line parameters */
if (argc != 4) {
    //checking if it is not equal to 4 and throws an error if not.
    fprintf(stderr, "Error: wrong number of arguments dude\n");
    exit(1);
}
 
     /* Parse command line arguments */
     thread_count_workers = atoi(argv[1]);
     bank_account_num = atoi(argv[2]);
     
     /* Initialize the server */
     initialize_server(bank_account_num);
     
     /* Open output file */
     output_file = fopen(argv[3], "w");
     if (output_file == NULL) {
         perror("Error opening output file");
         cleanup_server();
         exit(1);
     }
 
     // Create worker threads
     workers = (pthread_t *)malloc(thread_count_workers * sizeof(pthread_t));
     for (i = 0; i < thread_count_workers; i++) {
         if (pthread_create(&workers[i], NULL, worker_thread, NULL) != 0) {
             perror("Error creating worker thread");
//didn't know if it was necesary at first but definetly is
             cleanup_server();
             exit(1);
         }
     }
 
     /* Process user input */
     int next_id = 1;  // Next request ID
     
     while (1) {
         if (fgets(input_line, MAX_LINE, stdin) == NULL) {
             break;  // EOF
         }
         
         /* Remove trailing newline */
         input_line[strcspn(input_line, "\n")] = '\0';
         
         /* Tokenize the input line */
         token = strtok(input_line, " ");
         if (token == NULL) {
             continue;  // Empty line
         }
 
         /* Process based on command type */
         if (strcmp(token, "CHECK") == 0) {
             /* Handle CHECK command */
             token = strtok(NULL, " ");
             if (token == NULL) {
                 fprintf(stderr, "Invalid CHECK command format\n");
                 continue;
             }
 
             int account_id = atoi(token);
             if (account_id < 1 || account_id > bank_account_num) {
                 fprintf(stderr, "Invalid account ID: %d\n", account_id);
                 continue;
             }
 
             /* Create a CHECK request */
             struct request *req = (struct request *)malloc(sizeof(struct request));
             req->next = NULL;
             req->request_id = next_id++;
             req->request_type = 0;  // CHECK
             req->check_account_id = account_id;
             gettimeofday(&req->starttime, NULL);
 
             /* Add to queue and respond to user */
             add_request(req);
             printf("ID %d\n", req->request_id);
         } 
         else if (strcmp(token, "TRANS") == 0) {
             /* Handle TRANS command */
             struct request *req = (struct request *)malloc(sizeof(struct request));
             req->next = NULL;
             req->request_id = next_id++;
             req->request_type = 1;  // TRANS
             req->transactions = (struct trans *)malloc(MAX_TRANS * sizeof(struct trans));
             req->numba_of_transactions = 0;
             gettimeofday(&req->starttime, NULL);
 
             /* Parse transaction pairs: account_id amount ... */
             int count = 0;
             while ((token = strtok(NULL, " ")) != NULL && count < MAX_TRANS * 2) {
                 if (count % 2 == 0) {
                     /* Account ID */
                     int account_id = atoi(token);
                     if (account_id < 1 || account_id > bank_account_num) {
                         fprintf(stderr, "Invalid account ID: %d\n", account_id);
                         break;
                     }
                     req->transactions[count/2].account_id = account_id;
                 } else {
                     /* Amount */
                     req->transactions[count/2].amount = atoi(token);
                     req->numba_of_transactions = (count + 1) / 2;
                 }
                 count++;
             }
 
             if (req->numba_of_transactions > 0) {
                 /* Add to queue and respond to user */
                 add_request(req);
                 printf("ID %d\n", req->request_id);
             } else {
                 /* Invalid transaction format */
                 free(req->transactions);
                 free(req);
                 fprintf(stderr, "Invalid TRANS command format\n");
             }
         } 
         else if (strcmp(token, "END") == 0) {
             /* Handle END command - Signal shutdown */
             pthread_mutex_lock(&request_queue.lock);
             request_queue.done = 1;
             pthread_cond_broadcast(&request_queue.cond);
             pthread_mutex_unlock(&request_queue.lock);
             break;
         } 
         else {
             fprintf(stderr, "Unknown command!!");
         }
     }
 
     // we have to wait for worker threads to finish
     for (i = 0; i < thread_count_workers; i++) {
         pthread_join(workers[i], NULL);
     }
 
     // need to clean close, wrap up
     fclose(output_file);
     free(workers);
     cleanup_server();
     return 0;
 }
 
 /**
  * Initialize the server - Sets up accounts and synchronization primitives
  */
 void initialize_server(int n_accounts) {
     /* Initialize bank accounts */
     if (initialize_accounts(n_accounts) == 0) {
         fprintf(stderr, "Error initializing accounts\n");
         exit(1);
     }
 
     /* Initialize the bank lock (single mutex for all accounts) */
     pthread_mutex_init(&bank_lock, NULL);
 
     /* Initialize request queue */
     request_queue.head = NULL;
     request_queue.tail = NULL;
     request_queue.jobs_in_da_q = 0;
     request_queue.done = 0;
     pthread_mutex_init(&request_queue.lock, NULL);
     pthread_cond_init(&request_queue.cond, NULL);
 
     /* Initialize file lock */
     pthread_mutex_init(&file_lock, NULL);
 }
 
 /**
  * Clean up the server - Destroys locks and frees memory
  */
 void cleanup_server() {
     /* Clean up bank lock */
     pthread_mutex_destroy(&bank_lock);
 
     /* Clean up queue */
     pthread_mutex_destroy(&request_queue.lock);
     pthread_cond_destroy(&request_queue.cond);
 
     /* Clean up file lock */
     pthread_mutex_destroy(&file_lock);
 
     /* Free bank accounts */
     free_accounts();
 }
 
 /**
  * Worker thread function - Gets requests from the queue and processes them
  */
 void *worker_thread(void *arg) {
     struct request *req;
 
     while (1) {
         /* Get a request from the queue */
         req = get_request();
         if (req == NULL) {
             break;  // No more requests and server is shutting down
         }
 
         /* Process the request */
         if (req->request_type == 0) {
             /* CHECK request */
             process_check(req);
         } else {
             /* TRANS request */
             process_transaction(req);
         }
 
         /* Free the request */
         if (req->request_type == 1) {
             free(req->transactions);
         }
         free(req);
     }
 
     return NULL;
 }
 
 /**
  * Process a CHECK request - Get account balance
  */
 void process_check(struct request *req) {
     int balance;
 
     /* Lock the entire bank */
     pthread_mutex_lock(&bank_lock);
 
     /* Read the account balance */
     balance = read_account(req->check_account_id);
 
     /* Unlock the bank */
     pthread_mutex_unlock(&bank_lock);
 
     /* Record end time */
     gettimeofday(&req->endtime, NULL);
 
     /* Write result to output file */
     pthread_mutex_lock(&file_lock);
     fprintf(output_file, "%d BAL %d TIME %ld.%06ld %ld.%06ld\n",
             req->request_id, balance,
             req->starttime.tv_sec, req->starttime.tv_usec,
             req->endtime.tv_sec, req->endtime.tv_usec);
     fflush(output_file);
     pthread_mutex_unlock(&file_lock);
 }
 
 /**
  * Process a TRANS request it will return 1 if successful 0 if insufficient funds
  */
 int process_transaction(struct request *req) {
     int i, account_id;
     int broke_bank_id = -1;
     
     /* Lock the entire bank */
     pthread_mutex_lock(&bank_lock);
     
     /* Simplify by combining amounts for the same account */
     int *accounts = (int *)malloc(req->numba_of_transactions * sizeof(int));
     int *amounts = (int *)malloc(req->numba_of_transactions * sizeof(int));
     int super_special_accounts = 0;
     
     for (i = 0; i < req->numba_of_transactions; i++) {
         account_id = req->transactions[i].account_id;
         int amount = req->transactions[i].amount;
         
         //check if the account is in the list of super special accounts
         int found = 0;
         for (int j = 0; j < super_special_accounts; j++) {
             if (accounts[j] == account_id) {
                 amounts[j] += amount;
                 found = 1;
                 break;
             }
         }
         
         if (!found) {
             accounts[super_special_accounts] = account_id;
             amounts[super_special_accounts] = amount;
             super_special_accounts++;
         }
     }
     
     //check if they are poor
     for (i = 0; i < super_special_accounts; i++) {
         account_id = accounts[i];
         int balance = read_account(account_id);
         if (balance + amounts[i] < 0) {
             broke_bank_id = account_id;
             break;
         }
     }
 
     //If all the accounts have funds update them!
     if (broke_bank_id == -1) {

         for (i = 0; i < super_special_accounts; i++) {
             account_id = accounts[i];
             int balance = read_account(account_id);
             write_account(account_id, balance + amounts[i]);
         }
     }
 
    //Unlock the bank (note not looping for accounts)
     pthread_mutex_unlock(&bank_lock);
 
     
     gettimeofday(&req->endtime, NULL);
 
     //WRITE TO THE OUT PUT!!
     pthread_mutex_lock(&file_lock);
     if (broke_bank_id == -1) {
         fprintf(output_file, "%d OK TIME %ld.%06ld %ld.%06ld\n",
            //:(
                 req->request_id,
                 req->starttime.tv_sec, req->starttime.tv_usec,
                 req->endtime.tv_sec, req->endtime.tv_usec);
     } else {
         fprintf(output_file, "%d ISF %d TIME %ld.%06ld %ld.%06ld\n",
            //the pain of formatting!
                 req->request_id, broke_bank_id,
                 req->starttime.tv_sec, req->starttime.tv_usec,
                 req->endtime.tv_sec, req->endtime.tv_usec);
     }
     fflush(output_file);
     pthread_mutex_unlock(&file_lock);
 
     //free alloc mem 
     free(accounts);
     //note not og and new bal!
     free(amounts);
     
     return (broke_bank_id == -1);
 }

 //////////////////////////////////////
 //this next part is the same for both implementations

 //add the request to the queue
 void add_request(struct request *req) {
    //access to queue
     pthread_mutex_lock(&request_queue.lock);
     
     
     if (request_queue.head == NULL) {
//queu is empty set the head and the tail
         request_queue.head = req;
         request_queue.tail = req;
     } else {
         request_queue.tail->next = req;
         request_queue.tail = req;
     }

     //update the size counter
     request_queue.jobs_in_da_q++;
     
     // Signal worker thread
     pthread_cond_signal(&request_queue.cond);
     pthread_mutex_unlock(&request_queue.lock);
 }
 
 /**
  * Get a request from the queue
  * Returns NULL if there are no more requests and the server is shutting down
  */
 struct request *get_request() {
     struct request *req;
     
     pthread_mutex_lock(&request_queue.lock);
     
     // Wait until there is a job or the server is done 
     while (request_queue.jobs_in_da_q == 0 && !request_queue.done) {
         pthread_cond_wait(&request_queue.cond, &request_queue.lock);
     }
     
     // If there are no jobs and stuff just stop it the server is over with
     if (request_queue.jobs_in_da_q == 0 && request_queue.done) {
         pthread_mutex_unlock(&request_queue.lock);
         return NULL;
     }
     
     // Get the request from the queue 
     req = request_queue.head;
     request_queue.head = req->next;
     if (request_queue.head == NULL) {
         request_queue.tail = NULL;
     }
     
     request_queue.jobs_in_da_q--;
     
     pthread_mutex_unlock(&request_queue.lock);
     //return request for processing
     return req;
 }