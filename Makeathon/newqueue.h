#define _IntQueue_h

class IntQueue
{
  public:
    IntQueue();
    void zero(int values[], int max_length);
    bool pushOn(int in);
    int popOff();
    bool queueEmpty();
  private:
    int values[20];
    int queueSize;
};

IntQueue::IntQueue(){
  zero(values, 20);
  queueSize = 0;
};

void IntQueue::zero(int values[], int max_length){
  for(int i = 0; i < max_length; i++){
    values[i] = 0;
  }
}

bool IntQueue::pushOn(int in){
  values[queueSize] = in;
  queueSize++;
}

int IntQueue::popOff(){
  int val = values[0];
  for (int i = 1; i < queueSize; i++){
    values[i-1] = values[i];
  }
  values[queueSize] = 0;
  queueSize--;
  return val;
}

bool IntQueue::queueEmpty(){
  if (queueSize == 0) return true;
  return false;
}
