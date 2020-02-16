#define _IntVector_h

class IntVector
{
  public:
    IntVector();
    void zero(int values[], int max_length);
    bool vecPushOn(int in);
    int getVectorSize();
    int vectorGet(int i);
  private:
    int values[20];
    int vectorSize;
};

IntVector::IntVector(){
  zero(values, 20);
  vectorSize = 0;
};

void IntVector::zero(int values[], int max_length){
  for(int i = 0; i < max_length; i++){
    values[i] = 0;
  }
}

bool IntVector::vecPushOn(int in){
  values[vectorSize] = in;
  vectorSize++;
}

int IntVector::getVectorSize(){
  return vectorSize;
}

int IntVector::vectorGet(int i){
  return values[i];
}
