/* File Synopsis:
 * This file contains functions that test the PQArray on large data sets or use it as a method of sorting values.
 * The pqSort function uses the PQArray/PQHeap for the purpose of sorting a vector. The topK function
 * inputs a stream of DataPoints and returns the top k values from the stream as a vector. The purpose
 * of these functions is to compare the efficiency of PQArray and PQHeap, so there are also many tests
 * and time trials.
 */


#include "pqclient.h"
#include "pqarray.h"
#include "pqheap.h"
#include "vector.h"
#include "strlib.h"
#include <sstream>
#include "testing/SimpleTest.h"
using namespace std;

/* Function Synopsis:
 * pqSort takes in one parameter which is a vector of DataPoints to be sorted. Nothing is returned
 * since the vector is passed in by reference allowing pqSort to modify it. This function is used
 * to compare the efficiency of PQHeap and PQArray.
 */
void pqSort(Vector<DataPoint>& v) {
    PQHeap pq; //changed to answer Q14

    /* Using the Priority Queue data structure as a tool to sort, neat! */

    /* Add all the elements to the priority queue. */
    for (int i = 0; i < v.size(); i++) {
        pq.enqueue(v[i]);
    }

    /* Extract all the elements from the priority queue. Due
     * to the priority queue property, we know that we will get
     * these elements in sorted order, in order of increasing priority
     * value. Store elements back into vector, now in sorted order.
     */
    for (int i = 0; !pq.isEmpty(); i++) {
        v[i] = pq.dequeue();
    }
}



/* Function synopsis:
 * The sortVec function is a helper function for topK which sorts the array through a series of swaps
 * when an element is just added to the vector. This function is void so it does not return any value,
 * and it takes in two parameters which are the vector of DataPoints to sort and the index of the recently
 * added data point.
 */
void sortVec(Vector<DataPoint>& toSort, DataPoint justAdded){
    for(int i = toSort.size()-2; i>=0; i--){//sorts the elements in descending order
        if(toSort[i].priority < justAdded.priority){
            DataPoint tmp = toSort[i];
            toSort[i] = toSort[i+1];
            toSort[i+1] = tmp;
        }
        else{
            break;
        }
    }
}
/* Function Synopsis:
 * firstKofTopK is a helper function for topK which takes in the first k values from the stream,
 * adds them to the vector of data points, and sorts them. The function then returns the fully sorted
 * vector of the first k amount of data points. There are two parameters which are
 * the stream that is passed by reference, and an integer that holds the value of k.
 */
Vector<DataPoint> firstKofTopK(istream& stream, int k){
    Vector<DataPoint> firstVals;
    DataPoint cur;
    while(firstVals.size()<k && stream >> cur){//adds first k vals from stream into the vector
        firstVals.add(cur);
        sortVec(firstVals, cur);
    }
    return firstVals;
}

/* Function synopsis:
 * topK is a function which takes in a stream of DataPoints and an integer k. topK returns a vector
 * of the k largest priority values inputted from the stream.
 */
Vector<DataPoint> topK(istream& stream, int k) {
    Vector<DataPoint> largestVals = firstKofTopK(stream, k);
    DataPoint cur;
    while(stream >> cur){
        if(cur.priority<largestVals[k-1].priority){
            continue; //cur is smaller than smallest val in largestVals, so don't save it
        }
        largestVals.add(cur);//adds element that is one of the largest k elements
        sortVec(largestVals, cur);//sorts the vector putting cur in its sorted spot
        largestVals.remove(largestVals.size()-1);//removes smallest element from the vector to keep it at size k
    }

    return largestVals;
}



/* * * * * * Test Cases Below This Point * * * * * */

/* Helper function that, given a list of data points, produces a stream from them. */
stringstream asStream(const Vector<DataPoint>& dataPoints) {
    stringstream result;
    for (const DataPoint& pt: dataPoints) {
        result << pt;
    }
    return result;
}

/* Helper function that, given range start and stop, produces a stream of values. */
stringstream asStream(int start, int stop) {
    stringstream result;
    for (int i = start; i <= stop; i++) {
        DataPoint pt = { "", double(i) };
        result << pt;
    }
    return result;
}

/* Helper function to fill vector with n random DataPoints. */
void fillVector(Vector<DataPoint>& vec, int n) {
    vec.clear();
    for (int i = 0; i < n; i++) {
        DataPoint pt = { "", randomReal(0, 100) };
        vec.add(pt);
    }
}


STUDENT_TEST("Sorting a vector using pq sort 1 of 5"){
    int size = 10000;
        Vector<DataPoint> v;
        fillVector(v, size);
        TIME_OPERATION(size, pqSort(v));
}

STUDENT_TEST("Sorting a vector using pq sort 2 of 5"){
    int size = 20000;
        Vector<DataPoint> v;
        fillVector(v, size);
        TIME_OPERATION(size, pqSort(v));
}

STUDENT_TEST("Sorting a vector using pq sort 3 of 5"){
    int size = 40000;
        Vector<DataPoint> v;
        fillVector(v, size);
        TIME_OPERATION(size, pqSort(v));
}

STUDENT_TEST("Sorting a vector using pq sort 4 of 5"){
    int size = 80000;
        Vector<DataPoint> v;
        fillVector(v, size);
        TIME_OPERATION(size, pqSort(v));
}

STUDENT_TEST("topK: time trial with changing n") {
    int startSize = 200000;
    int k = 10;
    for (int n = startSize; n < 10*startSize; n *= 2) {
        Vector<DataPoint> input;
        fillVector(input, n);
        stringstream stream = asStream(input);
        TIME_OPERATION(n, topK(stream, k));
    }
}

STUDENT_TEST("topK: time trial with changing k") {
    int SizeN = 200000;
    int StartK = 100;
    for (int k = StartK; k < 10*StartK; k *= 2) {
        Vector<DataPoint> input;
        fillVector(input, SizeN);
        stringstream stream = asStream(input);
        TIME_OPERATION(SizeN, topK(stream, k));
    }
}

PROVIDED_TEST("pqSort: vector of random elements") {
    setRandomSeed(137); //good idea to set seed here so that any "randomized" values in the entire test case follow this seed

    Vector<DataPoint> input;
    Vector<double> expected;
    fillVector(input, 100);
    for (DataPoint dp : input) {
        expected.add(dp.priority);
    }
    expected.sort();

    pqSort(input);
    for (int i = 0; i < input.size(); i++) {
        EXPECT_EQUAL(input[i].priority, expected[i]);
    }
}

PROVIDED_TEST("pqSort: time trial") {
    int startSize = 1000;
    for (int n = startSize; n < 10*startSize; n *= 2) {
        Vector<DataPoint> v;
        fillVector(v, n);
        TIME_OPERATION(n, pqSort(v));
    }
}


/* Constant used for sizing the tests below this point. */
const int kMany = 100000;

PROVIDED_TEST("topK: stream 0 elements, ask for top 1") {
    stringstream emptyStream = asStream({});
    Vector<DataPoint> expected = {};
    EXPECT_EQUAL(topK(emptyStream, 1), expected);
}

PROVIDED_TEST("topK: stream 1 element, ask for top 1") {
    stringstream stream = asStream({ { "" , 1 } });
    Vector<DataPoint> expected = { { "" , 1 } };
    EXPECT_EQUAL(topK(stream, 1), expected);
}

PROVIDED_TEST("topK: small hand-constructed input") {
    Vector<DataPoint> input = { { "A", 1 }, { "B", 2 }, { "C", 3 }, { "D", 4 } };

    stringstream stream = asStream(input);
    Vector<DataPoint> expected = { { "D", 4 }, { "C", 3 } };
    EXPECT_EQUAL(topK(stream, 2), expected);

    stream = asStream(input);
    expected = { { "D", 4 }, { "C", 3 }, { "B", 2 }, { "A", 1 } };
    EXPECT_EQUAL(topK(stream, 5), expected);
}

PROVIDED_TEST("topK: stream many elements, ask for top 1") {
    stringstream stream = asStream(1, kMany);
    Vector<DataPoint> expected = { { "" , kMany } };
    EXPECT_EQUAL(topK(stream, 1), expected);
}

PROVIDED_TEST("topK: stream many elements, ask for top 5") {
    stringstream stream = asStream(1, kMany);
    Vector<DataPoint> expected = {
        { "" , kMany     },
        { "" , kMany - 1 },
        { "" , kMany - 2 },
        { "" , kMany - 3 },
        { "" , kMany - 4 },
    };
    EXPECT_EQUAL(topK(stream, 5), expected);
}

PROVIDED_TEST("topK: stress test, many elements, random values") {
    Vector<double> expected;
    Vector<DataPoint> points;
    fillVector(points, 10000);
    for (DataPoint dp : points) {
        expected.add(dp.priority);
    }
    sort(expected.begin(), expected.end(), greater<double>());

    stringstream stream = asStream(points);
    int k = 10;
    Vector<DataPoint> result = topK(stream, k);
    EXPECT_EQUAL(result.size(), k);
    for (int i = 0; i < k; i++) {
        EXPECT_EQUAL(result[i].priority, expected[i]);
    }
}

PROVIDED_TEST("topK: stress test - many elements, ask for top half") {
    stringstream stream = asStream(1, kMany);
    Vector<DataPoint> result = topK(stream, kMany/2);
    EXPECT_EQUAL(result.size(), kMany/2);
    EXPECT_EQUAL(result[0].priority, kMany);
    EXPECT_EQUAL(result[result.size()-1].priority, kMany - result.size() + 1);
}

PROVIDED_TEST("topK: time trial") {
    int startSize = 200000;
    int k = 10;
    for (int n = startSize; n < 10*startSize; n *= 2) {
        Vector<DataPoint> input;
        fillVector(input, n);
        stringstream stream = asStream(input);
        TIME_OPERATION(n, topK(stream, k));
    }
}

