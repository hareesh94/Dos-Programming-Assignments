
#include <xinu.h>

pid32 producer_id;
pid32 consumer_id;
pid32 timer_id;

int32 consumed_count = 0;
const int32 CONSUMED_MAX = 100;

/* Define your circular buffer structure and semaphore variables here */
#define SIZE 10
int queue[SIZE], rear = -1, front = -1, item;
sid32 semid;
sid32 full;
sid32 empty;

/* Place your code for entering a critical section here */
void mutex_acquire(sid32 mutex)
{
	wait(mutex);
}

/* Place your code for leaving a critical section here */
void mutex_release(sid32 mutex)
{
	signal(mutex);
}

/* Place the code for the buffer producer here */
process producer(void)
{     int k;
	while (1){
		mutex_acquire(empty);
		   if ((front == rear + 1) && (front == 0 && rear == SIZE - 1))
			printf("\n\nbuffer is full.");
                else
		{
                   k=rand();
                   if (rear == -1)
				   {
					   front = 0;
                          rear=0;
                   }
		
                 else if (rear == SIZE-1)
                   rear=0;
                  else
                    rear++;
                 mutex_acquire(semid);
                 queue[rear]=k;
                kprintf("\nItem produced:%d\n",k);
                mutex_release(semid);
				mutex_release(full);


}
}

	return OK;
}

/* Place the code for the buffer consumer here */
process consumer(void)
{    int item;
	while (1){
		mutex_acquire(full);
		if (rear == -1)
		kprintf("\n\nQueue is empty");
		else
		{
			mutex_acquire(semid);
			item=queue[front];
			mutex_release(semid);
			if(front == rear)
			{    
				rear = -1;
				front=-1;
				
			}
			else if (front == SIZE-1)
				front=0;
			else
				front++;

			kprintf("\n\nItem consumed:%d",item);
			consumed_count++;
			mutex_release(empty);
		  /* Every time your consumer consumes another buffer element,
	       * make sure to include the statement:
	     
	       * this will allow the timing function to record performance */
}	      /* */
}
	return OK;

}


/* Timing utility function - please ignore */
process time_and_end(void)
	{
		int32 times[5];
		int32 i;

		for (i = 0; i < 5; ++i)
		{
			times[i] = clktime_ms;
			yield();

			consumed_count = 0;
			while (consumed_count < CONSUMED_MAX * (i + 1))
			{
				yield();
			}

			times[i] = clktime_ms - times[i];
		}

		kill(producer_id);
		kill(consumer_id);

		for (i = 0; i < 5; ++i)
		{
			kprintf("TIME ELAPSED (%d): %d\n", (i + 1) * CONSUMED_MAX, times[i]);
		}
	}

process	main(void)
{
	recvclr();
	semid = semcreate(1);
	full = semcreate(0);
	empty = semcreate(1);
    /* Create the shared circular buffer and semaphores here */
	producer_id = create(producer, 4096, 50, "producer", 0);
	consumer_id = create(consumer, 4096, 50, "consumer", 0);
	timer_id = create(time_and_end, 4096, 50, "timer", 0);
	resched_cntl(DEFER_START);
	resume(producer_id);
	resume(consumer_id);
	/* Uncomment the following line for part 3 to see timing results */
	resume(timer_id);
	resched_cntl(DEFER_STOP);
	


	return OK;
}
