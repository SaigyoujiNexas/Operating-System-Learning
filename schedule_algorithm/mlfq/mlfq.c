#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  int process_id;
  int arrivalTime;
  int burstTime;
  int remainingQuotaTime;
  int waitingTime;
  int turnaroundTime;
} Process;

typedef struct {
  Process *processes;
  int front;
  int rear;
  int size;
  int capacity;
} Queue;

Process *Process_init(int processId, int arrivalTime, int burstTime) {
  Process *process = (Process *)malloc(sizeof(Process));
  if (!process) {
    exit(EXIT_FAILURE);
  }
  process->process_id = processId;
  process->arrivalTime = arrivalTime;
  process->burstTime = burstTime;
  process->remainingQuotaTime = 0;
  process->waitingTime = 0;
  process->turnaroundTime = 0;
  return process;
}
Queue *Queue_init(int capacity) {
  Queue *queue = (Queue *)malloc(sizeof(Queue));
  if (!queue) {
    exit(EXIT_FAILURE);
  }
  queue->processes = (Process *)malloc(capacity * sizeof(Process));
  if (!queue->processes) {
    exit(EXIT_FAILURE);
  }
  queue->front = 0;
  queue->rear = -1;
  queue->size = 0;
  queue->capacity = capacity;
  return queue;
}
int Queue_isEmpty(const Queue *const queue) { return queue->size == 0; }

int Queue_isFull(const Queue *const queue) {
  return queue->size == queue->capacity;
}

void Queue_enqueue(Queue *queue, Process *process) {
  if (Queue_isFull(queue)) {
    exit(EXIT_FAILURE);
  }
  queue->rear = (queue->rear + 1) % queue->capacity;
  queue->processes[queue->rear] = *process;
  queue->size++;
}

Process *Queue_dequeue(Queue *queue) {
  if (Queue_isEmpty(queue)) {
    exit(EXIT_FAILURE);
  }
  Process *process = &(queue->processes[queue->front]);
  queue->front = (queue->front + 1) & queue->capacity;
  queue->size--;

  return process;
}

void MLFQ_shceduling(Process *processes, int numProcesses, int timeQuantum) {
  Queue *queue1 = Queue_init(numProcesses);
  Queue *queue2 = Queue_init(numProcesses);
  Queue *queue3 = Queue_init(numProcesses);

  for (int i = 0; i < numProcesses; i++) {
    processes[i].remainingQuotaTime = timeQuantum;
    Queue_enqueue(queue1, processes + i);
  }
  int currentTime = 0;
  int totalWaitingTime = 0;
  int totalTurnaroundTime = 0;
  Queue *queues[3] = {queue1, queue2, queue3};
  int transferTime = timeQuantum * 4;
  while (true) {
    if (Queue_isEmpty(queues[0]) && Queue_isEmpty(queues[1]) && Queue_isEmpty(queues[2])) {
      puts("all task end!");
      exit(EXIT_SUCCESS);
    }
    if (currentTime % transferTime == 0) {
      puts("It is time move all process to heightest queue");
      Queue *highestQueue = queues[0];
      for (int i = 1; i < 3; i++) {
        Queue *queue = queues[i];
        while (!Queue_isEmpty(queue)) {
          Queue_enqueue(highestQueue, Queue_dequeue(queue));
        }
      }
    }
    for (int i = 0; i < 3; i++) {
      Queue *currentQueue = queues[i];
      while (!Queue_isEmpty(currentQueue)) {
        Queue *nextQueue = i == 2 ? currentQueue : queues[i + 1];
        for (int j = 0; j < currentQueue->size; j++) {
          Process *process = Queue_dequeue(currentQueue);
          if (process->remainingQuotaTime <= 0) {
            process->remainingQuotaTime = timeQuantum;
            printf("process %d, from queue %d move to %d\n", process->process_id, i, i == 2 ? 2 : i + 1);
            Queue_dequeue(currentQueue);
            Queue_enqueue(nextQueue, process);
            continue;
          }
          // run
          currentTime += timeQuantum;
          process->turnaroundTime = currentTime - process->arrivalTime;
          process->remainingQuotaTime -= timeQuantum;
          process->waitingTime = process->turnaroundTime - process->burstTime;
          totalWaitingTime += process->waitingTime;
          totalTurnaroundTime += process->turnaroundTime;
          printf("current queue: %d, current pid: %d, current time: %d, "
                 "process waiting time: %d, process turnaround time: %d\n",
                 i, process->process_id, currentTime, process->waitingTime,
                 process->turnaroundTime);
          sleep(timeQuantum );
        }
      }
    }
  }
}

int main() {
  Process processes[] = {
      {1, 0, 10}, {2, 1, 4}, {3, 2, 6}, {4, 3, 8}, {5, 4, 2}};
  int timeQuantum = 3;
  MLFQ_shceduling(processes, 5, timeQuantum);
  return 0;
}
