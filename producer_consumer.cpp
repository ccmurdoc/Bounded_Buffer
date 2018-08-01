/*
 * Bounded buffer (producer/consumer) console Application based on threads
 * created using the Win32 API.
 * By: Chantal Murdock & James Lopez
 */


#include <windows.h>
#include <process.h>
#include <iostream>
#include <time.h>
#include <chrono>
#include <fstream>
using namespace std;


#define NUM_THREADS    4
#define    BUF_SIZE    50


/// Function Prototypes

DWORD WINAPI Producer(LPVOID param);
DWORD WINAPI Consumer(LPVOID param);


/// Global Semaphores
// The first and last parameters identify a security
// attribute and a name for the semaphore.
//
//  The second and third parameters indicate the initial
//  value and maximum value of the semaphore
HANDLE    mutex = CreateSemaphore(0, 1, 1, 0);            // mutual exclusion (binary)
HANDLE    space = CreateSemaphore(0, BUF_SIZE, BUF_SIZE, 0);    // space available in buffer
HANDLE    element = CreateSemaphore(0, 0, BUF_SIZE, 0);        // element avalable in buffer


/// Global buffer variables
int    buffer[BUF_SIZE];
int    front = 0;
int    back = 0;

///Variable to calculate the Turn Around Time
int total_consume;
int total_produce;
int sleepTime;

/// Variable for printing output file into text
ofstream file;


int main()
{   cout<<"\n Please wait. Processing.........."<<endl;
    //Opening the output file
    file.open("producer_consumer.txt");

    DWORD    ThreadId;
    HANDLE    threads[NUM_THREADS];
    int    params[] = {0,1,2,3,4};

    // Starting Loop to test different Sleep times
    for (int num=1; num<5; num++){

    // Start of clock to calculate Turn Around Time at the End
    auto begin = chrono::high_resolution_clock::now();
    cout<<"__________________________________"<<endl;
    file<<"__________________________________"<<endl;
    cout<<"Test Case: "<<num;
    file<<"Test Case: "<<num;

    // create and start threads
    for (int i = 1; i <5; i++){
        threads[i] = CreateThread(0, 0, Producer, &params[i], 0, &ThreadId);}
    for (int i = 2; i <3; i++){
        threads[i] = CreateThread(0, 0, Consumer, &params[i], 0, &ThreadId);}


    // waiting for child threads to terminate
    for (int i = 1; i < NUM_THREADS; i++){
        // parameter value INFINITE indicates that
        // we will wait an infinite amount
        // of time for the lock to become available.
        WaitForSingleObject(threads[i], INFINITE);}

        // Stopping clock since processes are finished terminating
        auto end = chrono::high_resolution_clock::now();
        auto dur = end - begin;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

    // Display output on console
    cout<<"\n Turn Around Time: "<<ms<<" ms";
    cout<<"\n__________________________________"<<endl;

    // Writing output in text file named: "producer_consumer.txt"
    file<<"\n Turn Around Time: "<<ms<<" ms";
    file<<"\n__________________________________"<<endl;
    }

    // Explanation of results, displayed console
    cout<<"\n                 Results Explanation        "<<endl;
    cout<<"\n There are three indicators : mutex, space, and element."
    "\n The process waits for the appropriate semaphore to read"
    "\n from the shared buffer to ensure a suitable buffer slot"
    "\n exists. After the handle is gathered, the process uses"
    "\n it to read from the buffer slot, then updates the"
    "\n indicator and signals to the correct semaphore to inform"
    "\n the other process bank of the changes in the buffer structure."
    "\n The space semaphore will mutually exclude any process in the"
    "\n same bank from modifying the indicator associated with that"
    "\n bank when another process is already using it. Which is known"
    "\n as process blocking. The extra blocking causes the system"
    "\n response delay. When the buffer-array is full the producer"
    "\n cannot put any items into the buffer because there is no room."
    "\n So, the producer waits until the consumer clears space. When"
    "\n the scheduler runs the consumer next it will remove an element"
    "\n from the buffer, and then send a signal to the producer. The"
    "\n producer receives the signal, and will awake and begin"
    "\n processing again since space in the buffer is now available."
    "\n\n The random sleep time indicates a counter variable whose"
    "\n contents change to reflect the amount of free buffer slots."
    "\n Thus, depending on the random sleep time, there will yield a"
    "\n difference in producers and consumers because of the difference"
    "\n in sleep vs awaken at given time upon the buffer space."<<endl;

     cout<<"\n **************************************************************";
     cout<<"\n *  These results can also be viewed in this project folder   *"
    "\n *  under the file name: producer_consumer.txt                *"<<endl;
    cout<<" **************************************************************"<<endl;

    //Writing to text file
    file<<"\n                 Results Explanation        "<<endl;
    file<<"\n There are three indicators : mutex, space, and element."
    "\n The process waits for the appropriate semaphore to read"
    "\n from the shared buffer to ensure a suitable buffer slot"
    "\n exists. After the handle is gathered, the process uses"
    "\n it to read from the buffer slot, then updates the"
    "\n indicator and signals to the correct semaphore to inform"
    "\n the other process bank of the changes in the buffer structure."
    "\n The space semaphore will mutually exclude any process in the"
    "\n same bank from modifying the indicator associated with that"
    "\n bank when another process is already using it. Which is known"
    "\n as process blocking. The extra blocking causes the system"
    "\n response delay. When the buffer-array is full the producer"
    "\n cannot put any items into the buffer because there is no room."
    "\n So, the producer waits until the consumer clears space. When"
    "\n the scheduler runs the consumer next it will remove an element"
    "\n from the buffer, and then send a signal to the producer. The"
    "\n producer receives the signal, and will awake and begin"
    "\n processing again since space in the buffer is now available."
    "\n\n The random sleep time indicates a counter variable whose"
    "\n contents change to reflect the amount of free buffer slots."
    "\n Thus, depending on the random sleep time, there will yield a"
    "\n difference in producers and consumers because of the difference"
    "\n in sleep vs awaken at given time upon the buffer space."<<endl;
}

/// Create producers
DWORD WINAPI Producer(LPVOID param)
{
    int    id = * (DWORD *)param;

    srand(time(0) + id);

    for (int i = 1; i < 13; i++)
    {
        int    prod = rand() % 300;
        Sleep(prod);

        // Waits till the objects are in non-signaled state

        WaitForSingleObject(space, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        buffer[front] = prod;
        front = (front + 1) % BUF_SIZE;


        // Amount of threads allowed to access the shared resource
        ReleaseSemaphore(mutex, 1, 0);
        ReleaseSemaphore(element, 1, 0);

        total_produce= prod + i++;
        sleepTime = prod;

    }
    return 0;
}


/// Create consumers
DWORD WINAPI Consumer(LPVOID param)
{
    int    id = * (DWORD *)param;

    srand(time(0) + id);

    for (int i = 1; i < 13; i++)
    {
        int    consume;
        Sleep(rand() % 507);

        //  Waits till the objects are in non-signaled state
        WaitForSingleObject(element, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        consume = buffer[back];
        back = (back + 1) % BUF_SIZE;

        total_consume = consume + i;


        //Amount of threads allowed to access the shared resource
        ReleaseSemaphore(mutex, 1, 0);
        ReleaseSemaphore(space, 1, 0);

    }

    //Displays output to console & text file
    cout<<"\n Producers: "<<total_produce;
    file<<"\n Producers: "<<total_produce;
    cout<<"\n Consumers: "<<total_consume;
    file<<"\n Consumers: "<<total_consume;
    cout<<"\n Sleep Time: "<<sleepTime<<" ms";
    file<<"\n Sleep Time: "<<sleepTime<<" ms";

    return 0;
}
