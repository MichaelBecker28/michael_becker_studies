/**
 * appserver.c Bank Server (Fine-grained)
 * CprE 308 Project 2
 * 
 * This implementation uses fine-grained locking with one mutex for each account.
 * I'll be simulating a banking systems that handles account operations. It allows the user
 * to check balances and transfer funds. 
 * 
 * Michael Becker
 */

 #include <stdio.h>//usual needed
 #include <stdlib.h>
 #include <string.h>
 #include <pthread.h>
 #include <unistd.h>
 #include <sys/time.h>
 #include "Bank.h" //need for account operations
 
 //defining the limits of the program
 #define MAX_LINE 256
 #define MAX_TRANS 10  // Maximum number of accounts in a transaction
 
 /* Structure for a transaction */
 struct trans {
     int account_id;   // Account ID
     int amount;   // Amount to add (positive) or subtract (negative)
 };
 
 /* Structure for a request */
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
     struct request *head;  // Head of the queue
     struct request *tail;  // Tail of the queue
     int jobs_in_da_q;          // Number of jobs in the queue
     int done;              // a flag for server stopping
     pthread_mutex_t lock;  // mutex lock for the que
     pthread_cond_t cond;   // variable for wroker threads
 };
 
 /* Global variables */
 int bank_account_total;          // Number of bank accounts
 pthread_mutex_t *account_mutexes; // Array of mutexes for the accounts
 struct queue request_queue;  // Queue for pending requests
 FILE *output_file;         // Output file for results
 pthread_mutex_t file_lock; // Mutex for protecting file output
 
 /* Function prototypes */
 void initialize_server(int n_accounts);
 void cleanup_server();
 void *worker_thread(void *arg);
 void process_check(struct request *req);
 int process_transaction(struct request *req);
 void add_request(struct request *req);
 struct request *get_request();
 
 /**
  * This is the main function, and needs to initialize the server, create 
  * threads, and needs to handle the user commands.
  */
 int main(int argc, char *argv[]) {
     int thread_count_workers;// number of threads to create

     pthread_t *workers;//creating an array to store thread IDS
     char input_line[MAX_LINE];//reading user commands and this is the buffer
     char *token;
     int i;//loop
 
     /* Check command line arguments and make sure they are correct*/
     /* Validate command line parameters */
if (argc != 4) {
    //checking if it is not equal to 4 and throws an error if not.
    fprintf(stderr, "Error: wrong number of arguments dude\n");
    exit(1);
}
 
     /* Parsing the command line args
     need to convert the string args to ints 
      */
     thread_count_workers = atoi(argv[1]);//worker thread count to int
     bank_account_total = atoi(argv[2]);//account count to int
     


     ///////////////////////////////////////////////////////////
     /* STARTING THE SERVER SETTING UP BANKS*/
     initialize_server(bank_account_total);
     
     /* need to open the output file */

     output_file = fopen(argv[3], "w");//opening file in write mode because we need to write to that file.
     //argv[3] refers to the output file path.

     //error for output
     if (output_file == NULL) {
         perror("Error opening the file");
         cleanup_server();// need for error handling, clean up the resources and destroy locks on accounts and free memory.
         exit(1);
     }
 
     // Create worker threads 

     workers = (pthread_t *)malloc(thread_count_workers * sizeof(pthread_t));
     for (i = 0; i < thread_count_workers; i++) {
         if (pthread_create(&workers[i], NULL, worker_thread, NULL) != 0) {
             perror("Error creating worker thread");
             cleanup_server();
             exit(1);
         }
     }
 
     /* Process user input  into a cont loop*/
     int next_id = 1;  // Next request ID counter, increases with request
     
     while (1) {
        //infinite loop
         if (fgets(input_line, MAX_LINE, stdin) == NULL) {
             break;  // end of file
         }
         
         // Removes the new line from the input 

         input_line[strcspn(input_line, "\n")] = '\0';
         

         /* Need to parse the input into words that are separated by spaces*/

         token = strtok(input_line, " ");
         if (token == NULL) {
             continue;  // User presses enter woo
         }
 
         /* Determines the process considering the type of command. */
         if (strcmp(token, "CHECK") == 0) {

             /* Handle the CHECK command */
             token = strtok(NULL, " ");// get the account id
             if (token == NULL) {
                 fprintf(stderr, "Invalid CHECK format\n");
                 continue;
             }
            //Chang the account ID string to interger and make sure it is valid.
             int account_id = atoi(token);
             //if statement 
             if (account_id < 1 || account_id > bank_account_total) {
                 fprintf(stderr, "Invalid account ID");
                 continue;
             }
 
             /* Create a CHECK request object. Processed by a worker thread*/
             struct request *req = (struct request *)malloc(sizeof(struct request));
             req->next = NULL;
             //Assign unique ID and counter
             req->request_id = next_id++;
             //0 means check 
             req->request_type = 0;  
             req->check_account_id = account_id;
             
             gettimeofday(&req->starttime, NULL);// record and get the time of submission.

 
             /* Add to queue and respond to the user */
             add_request(req);
             printf("ID %d\n", req->request_id);//display the assigned request ID to user
         } 
         else if (strcmp(token, "TRANS") == 0) {
             /* Handle TRANS command
             The format:
             TRANS <acct1> <amount1> <acct2> <amount2>
              */

             struct request *req = (struct request *)malloc(sizeof(struct request));
             //
             req->next = NULL;
             req->request_id = next_id++;
             req->request_type = 1;  // TRANS op type
             req->transactions = (struct trans *)malloc(MAX_TRANS * sizeof(struct trans));
             req->numba_of_transactions = 0;
             gettimeofday(&req->starttime, NULL);
 
             /* Parse transaction pairs: account_id amount from command 
             format: account_id amount account_id */
             int count = 0;
             while ((token = strtok(NULL, " ")) != NULL && count < MAX_TRANS * 2) {
                 if (count % 2 == 0) {
                     // the positions that are even have the Account ID 
                     int account_id = atoi(token);
                     if (account_id < 1 || account_id > bank_account_total) {
                         fprintf(stderr, "Invalid account ID: %d\n", account_id);
                         break;//breaks if invalid account is hit
                     }
                     req->transactions[count/2].account_id = account_id;
                 } else {
                     // Odd contains transactions 
                     req->transactions[count/2].amount = atoi(token);
                    //update the transaction count
                     req->numba_of_transactions = (count + 1) / 2;
                 }
                 count++;
             }
 
             if (req->numba_of_transactions > 0) {
                 // Add to queue and respond to user 
                 add_request(req);//add request to the queue

                 printf("ID %d\n", req->request_id);//display the assigned ID
             } else {
                 // For invalid transaction format 
                 free(req->transactions);//free array
                 free(req);//free request
                 fprintf(stderr, "Invalid TRANS command format\n");//error handling
             }
         } 
         else if (strcmp(token, "END") == 0) {
             // Handle END command to shut the server down
              
             pthread_mutex_lock(&request_queue.lock);//Lock the queue before modifying it

             request_queue.done = 1;//set the shutdown flag
             pthread_cond_broadcast(&request_queue.cond);
             //wake the worker threads
             pthread_mutex_unlock(&request_queue.lock);//release lock
             break;//exit
         } 
         else {
             fprintf(stderr, "Unknown command");
         }
     }
 
     /* Wait for worker threads to finish */
     for (i = 0; i < thread_count_workers; i++) {
         pthread_join(workers[i], NULL);
     }
 
     // Clean and exit 
     fclose(output_file);
     free(workers);
     cleanup_server();
     return 0;
 }
 
 /**
  * Initialize the server set up accounts
  */
 void initialize_server(int n_accounts) {
     //Initialize bank accounts
     if (initialize_accounts(n_accounts) == 0) {
         fprintf(stderr, "Error initializing the accounts!!!\n");
         exit(1);
     }
 
     //Initialize account locks. Create one for each account 
     //allows ops on different accounts to run same "parallel"
     account_mutexes = (pthread_mutex_t *)malloc(n_accounts * sizeof(pthread_mutex_t));
     for (int i = 0; i < n_accounts; i++) {
         pthread_mutex_init(&account_mutexes[i], NULL);
     }
 
     // start/initialize the request que
     request_queue.head = NULL;//empty queue to start
     request_queue.tail = NULL;
     request_queue.jobs_in_da_q = 0;
     //server not down yet
     request_queue.done = 0;
     pthread_mutex_init(&request_queue.lock, NULL);//mutex for ops
     pthread_cond_init(&request_queue.cond, NULL);//variable for signals
 
     //Initialize file lock, protexts output file ops
     pthread_mutex_init(&file_lock, NULL);
 }
 
 /**
  * Clean up the server - Destroys locks and frees memory
  */
 void cleanup_server() {
     //Clean up account locks
     for (int i = 0; i < bank_account_total; i++) {
         pthread_mutex_destroy(&account_mutexes[i]);
     }
     free(account_mutexes);
 
     //Clean queue
     pthread_mutex_destroy(&request_queue.lock);
     pthread_cond_destroy(&request_queue.cond);
 
     // destroy file lock
     pthread_mutex_destroy(&file_lock);
 
     
     free_accounts();
 }
 
 /**
  * Worker thread function
  * get the requests from the queu and process them.
  */
 void *worker_thread(void *arg) {
     struct request *req;//pointer to hold request.
 
     while (1) {
         //Get a request from the queue
         req = get_request();
         if (req == NULL) {
             break;  // No more requests and server is shutting down
         }
 
         /* Process the request */
         if (req->request_type == 0) {
             //check
             process_check(req);
         } else {
             //trans
             process_transaction(req);
         }
 
         
         if (req->request_type == 1) {
             free(req->transactions);
         }
         free(req);
     }
 
     return NULL;
 }
 
 /**
  * Process a CHECK request --> get the account balance
  */
 void process_check(struct request *req) {
     int balance;
 
     //Lock the account to stop other threads from modifying it
     pthread_mutex_lock(&account_mutexes[req->check_account_id - 1]);
 
     //Read the account balance
     balance = read_account(req->check_account_id);
 
     //unlock the account
     pthread_mutex_unlock(&account_mutexes[req->check_account_id - 1]);
 
     //Records the time for performance tracking and such
     gettimeofday(&req->endtime, NULL);
 
     //Now need to write it to the output file in the format specified...
     pthread_mutex_lock(&file_lock);
     fprintf(output_file, "%d BAL %d TIME %ld.%06ld %ld.%06ld\n",//this format :(
             req->request_id, balance,
             req->starttime.tv_sec, req->starttime.tv_usec,
             req->endtime.tv_sec, req->endtime.tv_usec);
    //Make sure the result it written
     fflush(output_file);
     pthread_mutex_unlock(&file_lock);
 }
 
 /**
  * Process a TRANS request, managing transfers between accounts
  * will return 1 if it works and 0 if the account has insufficient funds
  */
 int process_transaction(struct request *req) {
     int i, account_id;

     int *account_ids;   //Unique account ids

     int *og_balance;//self explanatory- original balances

     int super_special_accounts = 0;     // number of unique accounts counter
     int broke_bank_id = -1;//tracks if account has insufficient funds
     
     /* Get the unique accounts, 
     then need to lock each account once 
     */

     account_ids = (int *)malloc(req->numba_of_transactions * sizeof(int));
     
     // Copy account IDs
     for (i = 0; i < req->numba_of_transactions; i++) {
         int id = req->transactions[i].account_id;
         
         // Check if this ID is a dupe
         int is_duplicate = 0;

         for (int j = 0; j < super_special_accounts; j++) {
             if (account_ids[j] == id) {
                 is_duplicate = 1;

                 break;//very interesting stuff!
             }
         }
         
         if (!is_duplicate) {//if it is not add it to the list
             account_ids[super_special_accounts++] = id;
         }
     }
     
     // Sort account IDs to lock in order 
     for (i = 0; i < super_special_accounts; i++) {

         for (int j = i + 1; j < super_special_accounts; j++) {

             if (account_ids[i] > account_ids[j]) {
                 int temp = account_ids[i];
                 account_ids[i] = account_ids[j];

                 account_ids[j] = temp;
             }
         }
     }
     
     //prep space --> og balances
     og_balance = (int *)malloc(super_special_accounts * sizeof(int));
 
     //lock in order
     for (i = 0; i < super_special_accounts; i++) {
         pthread_mutex_lock(&account_mutexes[account_ids[i] - 1]);
     }
 
     //read og balances
     for (i = 0; i < super_special_accounts; i++) {
         og_balance[i] = read_account(account_ids[i]);
     }
     
     // new balances and check for insufficient funds 
     int *new_BAL = (int *)malloc(super_special_accounts * sizeof(int));
     //start with the original
     for (i = 0; i < super_special_accounts; i++) {
         new_BAL[i] = og_balance[i];
     }
     //apply each trans and check if account would go negative
     for (i = 0; i < req->numba_of_transactions; i++) {
         account_id = req->transactions[i].account_id;
         int amount = req->transactions[i].amount;
         
         // Find the account in our list 
         for (int j = 0; j < super_special_accounts; j++) {
             if (account_ids[j] == account_id) {
                 new_BAL[j] += amount;
                 
                 //neg
                 if (new_BAL[j] < 0) {
                     broke_bank_id = account_id;
                     break;
                 }
                 break;
             }
         }
         //stop checking
         if (broke_bank_id != -1) {
             break;
         }
     }
 
     // If all accounts have funds update them
     if (broke_bank_id == -1) {
         for (i = 0; i < super_special_accounts; i++) {
             write_account(account_ids[i], new_BAL[i]);
         }
     }
 
     // unlock all accounts in reverse order 
     for (i = super_special_accounts - 1; i >= 0; i--) {
         pthread_mutex_unlock(&account_mutexes[account_ids[i] - 1]);
     }
 
     
     gettimeofday(&req->endtime, NULL);
 
     //Write to output file 
     pthread_mutex_lock(&file_lock);
     if (broke_bank_id == -1) {

         fprintf(output_file, "%d OK TIME %ld.%06ld %ld.%06ld\n",//Yes! the modulus op maybe be least favorite key on keyboard
                 req->request_id,
                 req->starttime.tv_sec, req->starttime.tv_usec,
                 req->endtime.tv_sec, req->endtime.tv_usec);

     } else {
         fprintf(output_file, "%d ISF %d TIME %ld.%06ld %ld.%06ld\n",//:(

                 req->request_id, broke_bank_id,
                 req->starttime.tv_sec, req->starttime.tv_usec,
                 req->endtime.tv_sec, req->endtime.tv_usec);
     }

     fflush(output_file);
     pthread_mutex_unlock(&file_lock);
 
     // Free alloc mem
     free(account_ids);
     free(og_balance);
     free(new_BAL);
     
     return (broke_bank_id == -1);
 }
 
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
     
     //Update the size counter
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
     //return the request for processing
     return req;
 }