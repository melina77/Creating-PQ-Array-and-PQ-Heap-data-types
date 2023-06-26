/*
 * File Synopsis:
 * The implementation of the Priority Queue Heap is shown here. This file contains 14 functions which are used as helper functions, for testing purposes, or the priority queue methods.
 * In addition, the allocator and deallocator for the priority queue are also in this file. The priority queue heap is essentially a more efficient approach to somewhat sorting an
 * array of elements compared to the PQArray approach of sorting each individual element.
 */

#include "pqheap.h"
#include "error.h"
#include "random.h"
#include "strlib.h"
#include "datapoint.h"
#include "testing/SimpleTest.h"
using namespace std;

const int INITIAL_CAPACITY = 10;
const int NONE = -1; // used as sentinel index


/*
 * Synopsis: This is the allocator for the priority queue heap. It initializes the array of elements
 * and other essential variables for the priority queue heap.
 */
PQHeap::PQHeap(){
    _numAllocated = INITIAL_CAPACITY;
    _elements = new DataPoint[_numAllocated](); // allocated zero'd memory
    _numFilled = 0;
}

/*
 * Synopsis: This is the deallocator for the priority queue heap. It deletes the leftover array of elements to prevent memory leaks.
 */
PQHeap::~PQHeap() {
    delete[] _elements;
}

/* Function Synopsis:
 * This helper function performs a simple swap of elements within the array for the priority queue.
 * It takes in the parameters of indexes to swap and has no return type since it directly edits the
 * array.
 */
void PQHeap::swap(int indexA, int indexB) {
    DataPoint tmp = _elements[indexA];
    _elements[indexA] = _elements[indexB];
    _elements[indexB] = tmp;
}


/* Function Synopsis:
 * This helper function takes in one parameter which is an index of an element that was just added
 * or switched around within the priority queue. It runs a series of checks to ensure that the
 * parent-child relationships within the prioirity queue are valid. A boolean representing whether or not
 * the priority queue is properly sorted is returned.
 */
bool PQHeap::validateHeap(int indexJustAdded){
    if(indexJustAdded!=0){
        int parent = getParentIndex(indexJustAdded);
        if(_elements[parent].priority > _elements[indexJustAdded].priority){
            return false;
        }
    }
    if(2*indexJustAdded+1 < size()){//left child exists
        int leftChild = getLeftChildIndex(indexJustAdded);
        if(_elements[leftChild].priority < _elements[indexJustAdded].priority){
            return false;
        }
    }
    if(2*indexJustAdded+2 < size()){//right child exists
        int rightChild = getRightChildIndex(indexJustAdded);
        if(_elements[rightChild].priority < _elements[indexJustAdded].priority){
            return false;
        }
    }
    return true;
}

/*
 * Function Synopsis:
 * This function adds its parameter DataPoint to the end of the priority queue array. It then calls
 * helper functions to properly rearrange the array to ensure it is sorted. Nothing is returned
 * since this is a void function, but the array of elements is directly modified.
 */
void PQHeap::enqueue(DataPoint elem) {
    if(_numFilled+1 > _numAllocated){//will not be able to add another element without reaching array size
        enlargeSize();
    }

    _elements[_numFilled] = elem;//adds element to last index
    int temp = _numFilled;

    while(!validateHeap(temp)){//the heap is not in order because of the element just added
        int toSwap = getParentIndex(temp);
        swap(temp, toSwap);
        temp = toSwap;
    }

    _numFilled++;

}


/*
 * Function Synopsis:
 * This is a helper function for enqueue which doubles the size of allocated spaces in the array
 * when it is called. The function takes in no parameters and returns no value since it directly
 * edits the array.
 */
void PQHeap::enlargeSize(){
    DataPoint* newPQ = new DataPoint[_numAllocated*2];//creates new array with twice the memory of the current array
    for(int i = 0; i<size(); i++){
        newPQ[i] = _elements[i];//transfers all data values from the original array to the new one
    }
    delete[] _elements;//deallocates the memory from the previous array
    _elements = newPQ;
    _numAllocated *= 2;

}

/*
 * Function Synopsis:
 * This function returns the top-most (highest priority) element in the priority queue without removing
 * it from the array. It takes in no parameters.
 */
DataPoint PQHeap::peek() const {
    if (isEmpty()) {
        error("PQueue is empty!");
    }
    return(_elements[0]);
}

/*
 * This function removes and returns the highest priority element from the priority queue. There are
 * no parameters.
 */
DataPoint PQHeap::dequeue() {
    DataPoint front = peek();//element at index 0 is stored
    DataPoint toReplaceFront = _elements[_numFilled-1];
    _elements[0] = toReplaceFront;//replaces element at first index with last element (it will become empty anyways)
    int temp = 0;//sets starting value for the while loop, changes as the value being altered moves across the vector

    while(!validateHeap(temp)){
        int rightChild = getRightChildIndex(temp);
        int leftChild = getLeftChildIndex(temp);
        if(_elements[rightChild].priority < _elements[leftChild].priority){//right child needs to move up, its priority is smaller than the left child's priority
            swap(temp, rightChild);
            temp = rightChild;
        }
        else{//left child needs to move up, its priority is smaller than the right child's priority
            swap(temp, leftChild);
            temp = leftChild;
        }
    }

    _numFilled--;//The priority queue size decrememnts by 1 since the frontmost item is removed and returned
    return front;
}

/*
 * Function Synopsis:
 * This function returns a boolean representing whether or not the array of the priority queue is empty.
 * There are no parameters and the array is not edited.
 */
bool PQHeap::isEmpty() const {
    if(size() == 0){
        return true;
    }
    return false;
}

/*
 * Function Synopsis:
 * This function returns an integer representing the size of the array storing the priority queue's elments.
 * The array is not edited, and there are no parameters.
 */
int PQHeap::size() const {
    return _numFilled;
}

/*
 * Function Synopsis:
 * This function clears the existing array containing the priority queue's elements. The priority queue
 * now contains no elements. There are no parameters and nothing is returned.
 */
void PQHeap::clear() {
    _numFilled = 0;
}

/*
 * Function Synopsis:
 * This function is used for testing purposes, it prints a message then prints out the contents of each
 * array index of the priority queue. Nothing is returned and the only parameter is the string of the message
 * to print.
 */
void PQHeap::printDebugInfo(string msg) const {
    cout << msg << endl;
    for (int i = 0; i < size(); i++) {
        cout << "[" << i << "] = " << _elements[i] << endl;
    }
}

/*
 * Function Synopsis:
 * This function validates the order of the priority queue's array contents by iterating through each
 * index and checking the priority order. An error is thrown containing a description of the index that
 * breaks the order, otherwise this function runs without stopping or returning any values if the array
 * is correctly sorted.
 */
void PQHeap::validateInternalState() const {
    for(int i = 0; i<size(); i++){
        if(getRightChildIndex(i) > -1 ){//the right child exists
            if((_elements[getRightChildIndex(i)].priority < _elements[i].priority)){//checks priority
                error("The priority of index " + integerToString(getRightChildIndex(i)) + " has an incorrect priority relationship to its parent.");
            }
        }
        if(getLeftChildIndex(i) > -1){//the left child exists
            if((_elements[getLeftChildIndex(i)].priority < _elements[i].priority)){//checks priority
                error("The priority of index " + integerToString(getLeftChildIndex(i)) + " has an incorrect priority relationship to its parent.");
            }
        }
    }
}

/*
 * Function Synopsis:
 * This helper function calculates the index of the element that is the parent of the
 * specified child index. If this child has no parent, the sentinel value NONE is returned. The only
 * parameter is the index of the child in the array.
 */
int PQHeap::getParentIndex(int child) const {
    if(child == 0){
        return NONE;
    }
    int parentIndex = int(((child-1)/2) + 0.5);//previously had it as child-1/2
    return parentIndex;
}

/*
 * Function Synopsis:
 * This function calculates the index of the element that is the left child of the
 * specified parent index. If this parent has no left child, the sentinel value NONE is returned.
 * The only parameter is the index of the parent.
 */
int PQHeap::getLeftChildIndex(int parent) const {
    if(!(parent*2+1 < size())){
        return NONE;
    }
    int leftChild = 2*parent+1;
    return leftChild;
}

/*
 * Function Synopsis:
 * This function calculates the index of the element that is the right child of the
 * specified parent index. If this parent has no right child, the sentinel value NONE is returned.
 * The only parameter is the index of the parent.
 */
int PQHeap::getRightChildIndex(int parent) const {
    if(!(parent*2+2 < size())){
        return NONE;
    }
    int rightChild = 2*parent+2;
    return rightChild;
}


/* * * * * * Test Cases Below This Point * * * * * */


STUDENT_TEST("Enqueuing & Dequeuing to PQHeap"){
    PQHeap pq;
    Vector<DataPoint> toAdd = {
        { "H", 7 }, { "W", 29 }, { "J", 4 }, { "K", 7 }, { "G", 11 },
        { "V", 1 }, { "T", 9 }, { "S", 8 }, { "O", 6 } };
    pq.validateInternalState();
    for (DataPoint dp : toAdd) {
        pq.enqueue(dp);
        pq.validateInternalState();
    }
    while (!pq.isEmpty()) {
        pq.dequeue();
        pq.validateInternalState();
    }
}

STUDENT_TEST("Enqueuing & Dequeuing single element to PQHeap 2"){
    PQHeap pq;
    Vector<DataPoint> toAdd = {
        { "H", 7 }};
    pq.validateInternalState();
    for (DataPoint dp : toAdd) {
        pq.enqueue(dp);
        pq.validateInternalState();
    }
    while (!pq.isEmpty()) {
        pq.dequeue();
        pq.validateInternalState();
    }
}

STUDENT_TEST("Enqueuing & Dequeuing two elements to PQHeap 2"){
    PQHeap pq;
    Vector<DataPoint> toAdd = {
        { "H", 7 }, { "W", 29 }};
    pq.validateInternalState();
    for (DataPoint dp : toAdd) {
        pq.enqueue(dp);
        pq.validateInternalState();
    }
    while (!pq.isEmpty()) {
        pq.dequeue();
        pq.validateInternalState();
    }
}

STUDENT_TEST("PQHeap complicated enqueue (many swaps) & dequeue") {
    PQHeap pq;
    Vector<DataPoint> input = {
        { "R", 4 }, { "A", 5 }, { "B", 3 }, { "K", 2 }, { "G", 2 },
        { "V", 36 }, { "T", 1 }, { "O", 8 }, { "S", 3 } };

    pq.validateInternalState();
    for (DataPoint dp : input) {
        pq.enqueue(dp);
        pq.validateInternalState();
    }
    while (!pq.isEmpty()) {
        pq.dequeue();
        pq.validateInternalState();
    }

}

STUDENT_TEST("PQHeap enqueue that requires resizing and dequeue") {
    PQHeap pq;
    Vector<DataPoint> input = {
        { "R", 4 }, { "A", 5 }, { "B", 3 }, { "K", 2 }, { "G", 2 },
        { "V", 36 }, { "T", 1 }, { "O", 8 }, { "S", 3 },{ "A", 5 },
        { "B", 3 }, { "K", 2 }, { "G", 2 }, { "V", 36 }, { "T", 1 },
        { "O", 8 }, { "S", 3 } };

    pq.validateInternalState();
    for (DataPoint dp : input) {
        pq.enqueue(dp);
        pq.validateInternalState();
    }
    while (!pq.isEmpty()) {
        pq.dequeue();
        pq.validateInternalState();
    }
}

STUDENT_TEST("Tries to dequeue from empty priority queue"){
    PQHeap pq;
    Vector<DataPoint> input = {
        { "R", 4 }};
    pq.enqueue(input[0]);
    pq.dequeue();
    EXPECT_ERROR(pq.dequeue());
}

STUDENT_TEST("PQHeap, test enlarge array memory") {
    for (int size = 5; size <= 500; size *= 5) {
        PQHeap pq;

        for (int i = 1; i <= size; i++) {
            pq.enqueue({"", double(i) });
        }
        pq.validateInternalState();

        for (int i = 1; i <= size; i++) {
            DataPoint expected = {"", double(i) };
            EXPECT_EQUAL(pq.dequeue(), expected);
        }
    }
}

STUDENT_TEST("PQHeap, dequeue, validate at every step") {
    PQHeap pq;

    pq.enqueue({ "e", 2.718 });
    pq.enqueue({ "pi", 3.14 });
    pq.enqueue({ "phi", 1.618 });

    for (int i = 0; i < 3; i++) {
        pq.dequeue();
        pq.validateInternalState();
    }
}

STUDENT_TEST("PQHeap: dequeue or peek on empty queue raises error") {
    PQHeap pq;
    DataPoint point = { "Programming Abstractions", 106 };

    EXPECT(pq.isEmpty());
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());

    pq.enqueue(point);
    pq.dequeue();
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());

    pq.enqueue(point);
    pq.clear();
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());
}


STUDENT_TEST("PQHeap: operations size/isEmpty/clear") {
    PQHeap pq;

    EXPECT(pq.isEmpty());
    pq.clear();
    EXPECT_EQUAL(pq.isEmpty(), pq.size() == 0);
    pq.enqueue({ "", 7 });
    EXPECT_EQUAL(pq.size(), 1);
    pq.enqueue({ "", 5 });
    EXPECT_EQUAL(pq.size(), 2);
    pq.enqueue({ "", 5 });
    EXPECT_EQUAL(pq.size(), 3);
    pq.clear();
    pq.validateInternalState();
    EXPECT(pq.isEmpty());
    EXPECT_EQUAL(pq.size(), 0);
}

PROVIDED_TEST("PQHeap example from writeup of PQArray") {
    PQHeap pq;

    pq.enqueue( { "Zoe", -3 } );
    pq.enqueue( { "Elmo", 10 } );
    pq.enqueue( { "Bert", 6 } );
    EXPECT_EQUAL(pq.size(), 3);
    pq.printDebugInfo("After enqueue 3 elements");

    pq.enqueue( { "Kermit", 5 } );
    EXPECT_EQUAL(pq.size(), 4);
    pq.printDebugInfo("After enqueue one more");

    DataPoint removed = pq.dequeue();
    DataPoint expected = { "Zoe", -3 };
    EXPECT_EQUAL(removed, expected);
    pq.printDebugInfo("After dequeue one");
}

PROVIDED_TEST("PQHeap example from writeup, validate each step") {
    PQHeap pq;
    Vector<DataPoint> input = {
        { "R", 4 }, { "A", 5 }, { "B", 3 }, { "K", 7 }, { "G", 2 },
        { "V", 9 }, { "T", 1 }, { "O", 8 }, { "S", 6 } };

    pq.validateInternalState();
    for (DataPoint dp : input) {
        pq.enqueue(dp);
        pq.validateInternalState();
    }
    while (!pq.isEmpty()) {
        pq.dequeue();
        pq.validateInternalState();
    }
}
