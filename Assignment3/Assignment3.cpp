//Copyright 2017, Bradley Peterson, Weber State University, All rights reserved.
// Carson Tyler		Assignment 3		February 9, 2018

#include <iostream>
#include <map>
#include <algorithm>
#include <list>
#include <vector>
#include <memory>

//To prevent those using g++ from trying to use a library
//they don't have
#ifndef __GNUC__
#include <conio.h>
#else
#include <stdio.h>
#endif

#include <sstream>

template <typename T> class DoublyLinkedList;

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::stringstream;

//************************************************************************
//A class I designed to help keep track of how much memory you allocate
//Do not modify, this is not part of your assignment, it just helps test it.
//For this to work, a class needs to inherit off of this one.
//Then this does the rest of the work, since it
//overloads new, new[], delete, and delete[].
//************************************************************************
class ManageMemory {
public:

	static std::size_t getTotalSize() {
		std::size_t total = 0;
		std::map<void *, std::size_t>::iterator iter;
		for (iter = mapOfAllocations.begin(); iter != mapOfAllocations.end(); ++iter) {
			total += iter->second;
		}
		return total;
	}

	//I overloaded the new and delete keywords so I could manually track allocated memory.
	void* operator new(std::size_t x) {
		void *ptr = ::operator new(x);
		mapOfAllocations[ptr] = x;
		return ptr;
	}
	void* operator new[](std::size_t x) {
		void *ptr = ::operator new[](x);
		mapOfAllocations[ptr] = x;
		return ptr;
	}
		void operator delete(void* x) {
		mapOfAllocations.erase(x);
		::operator delete(x);
	}
	void operator delete[](void* x) {
		mapOfAllocations.erase(x);
		::operator delete[](x);
	}
private:
	static std::map<void *, std::size_t> mapOfAllocations;
};
std::map<void *, std::size_t> ManageMemory::mapOfAllocations;


//******************
//The node class
//******************
template <typename T>
struct Node {
	T data{};
	shared_ptr< Node<T> > forward;
	shared_ptr< Node<T> > backward;
};



//******************
//The Iterator class
//******************
template <typename T>
class Iterator : public ManageMemory, public std::iterator<std::bidirectional_iterator_tag, T> {
	friend class DoublyLinkedList<T>;
private:
	bool rightCliffHanger{ false };
	shared_ptr< Node <T> > current;
public:
	T& operator*() const;
	Iterator<T> operator++();
	Iterator<T> operator++(int);
	Iterator<T> operator--();
	bool operator!=(Iterator<T> rhs);
	bool operator==(Iterator<T> rhs);
};

//***********************************
//The Iterator class methods
//***********************************

// operator*() method. Returns the current node.
template<typename T>
T& Iterator<T>::operator*() const {
	return current->data;
}

// prefix operator++() method. Increments the iterator to the next node in the list.
template<typename T>
Iterator<T> Iterator<T>::operator++() {
	if (rightCliffHanger == true)
		return *this;
	else if (current->forward == nullptr)
		rightCliffHanger = true;
	else
		current = current->forward;
	return *this;
}

// postfix operator++() method. Incrememts the iterator to the next node in the list.
template<typename T>
Iterator<T> Iterator<T>::operator++(int) {
	auto temp = *this;
	if (rightCliffHanger == true)
		return *this;
	else
		current = current->forward;
	return temp;
}

// operator--() method. Decrements the iterator to the previous node in the list.
template<typename T>
Iterator<T> Iterator<T>::operator--() {
	if (rightCliffHanger == true)
		rightCliffHanger = false;
	else {
		auto temp = *this;
		current = current->backward;
	}
	return *this;
}

// operator!=() method. Checks to see if the rightCliffHanger and current nodes are equivalent.
template<typename T>
bool Iterator<T>::operator!=(Iterator<T> rhs) {
	bool temp = false;
	if (this->rightCliffHanger == rhs.rightCliffHanger && this->current == rhs.current)
		temp;
	else
		temp = true;
	return temp;
}

template<typename T>
bool Iterator<T>::operator==(Iterator<T> rhs) {
	bool temp = true;
	if (this->rightCliffHanger == rhs.rightCliffHanger && this->current == rhs.current)
		temp;
	else
		temp = false;
	return temp;
}


//****************************
//The DoublyLinkedList class
//****************************
template <typename T>
class DoublyLinkedList : public ManageMemory {

public:

	//public members of the DoublyLinkedList class
	DoublyLinkedList();
	~DoublyLinkedList();
	string getStringFromList();

	void insertFirst(const T&);
	void insertLast(const T&);
	void deleteFirst();
	void deleteLast();

	Iterator<T> begin();
	Iterator<T> end();
	//TODO: Implement a begin() and end() method

protected:
	shared_ptr< Node<T> > first;
	shared_ptr< Node<T> > last;
	unsigned int count;
};

template <typename T>
DoublyLinkedList<T>::DoublyLinkedList() {
	count = 0;
	first = nullptr;
	last = nullptr;
}

template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
	while (first) {
		first->backward.reset();
		first = first->forward;
	}
	last.reset();
}

template <typename T>
void DoublyLinkedList<T>::insertLast(const T& item) {


	if (!first) {
		//Scenario #1 - The list is empty
		first = make_shared< Node<T> >();
		first->data = item;
		last = first;
	}
	else {
		//Scenario #2 - The list has at least one item
		shared_ptr< Node<T> > tempPtr = make_shared< Node<T> >();
		tempPtr->data = item;
		last->forward = tempPtr;
		tempPtr->backward = last;
		last = tempPtr;
	}
	count++;
}
template <typename T>
void DoublyLinkedList<T>::insertFirst(const T& item) {
	if (!first) {
		//Scenario #1 The list is empty
		first = make_shared< Node<T> >();
		first->data = item;
		last = first;
	}
	else {
		//Scenario #2 The list has at least one item in it
		shared_ptr< Node<T> > tempPtr = make_shared< Node<T> >();
		tempPtr->data = item;
		tempPtr->forward = first;
		first->backward = tempPtr;
		first = tempPtr;
	}
	count++;
}

template <typename T>
void DoublyLinkedList<T>::deleteFirst() {

	//Empty list
	if (!first) {
		cout << "List is already empty" << endl;
		return;
	}

	//Scenario 1 or more nodes
	first = first->forward;

	if (!first) {
		//Must have been only 1 node in the list
		last.reset();
	}
	else {
		//first->backward = nullptr;
		first->backward.reset();
	}

	count--;

}

template <typename T>
void DoublyLinkedList<T>::deleteLast() {

	if (!first) {
		//empty list scenario
		cout << "The list is empty" << endl;
	}
	else {
		last = last->backward;
		if (!last) {
			//1 node scenario
			first.reset();
		}
		else {
			//2 or more node scenario
			last->forward.reset();
		}
		count--;
	}
}

//This method helps return a string representation of all nodes in the linked list
template <typename T>
string DoublyLinkedList<T>::getStringFromList() {
	stringstream ss;
	if (first == nullptr) {
		ss << "The list is empty.";
	}
	else {


		shared_ptr< Node<T> > currentNode = first;
		ss << currentNode->data;
		currentNode = currentNode->forward;

		while (currentNode != nullptr) {
			ss << " " << currentNode->data;
			currentNode = currentNode->forward;
		}
	}
	return ss.str();
}

//***********************************
//The DoublyLinkedList class methods
//***********************************

// begin() method. Sets the iterator to the beginning of the Doubly Linked List
template<typename T>
Iterator<T> DoublyLinkedList<T>::begin() {
	Iterator<T> retVal;
	if (this->count == 0)
		retVal.rightCliffHanger = true;
	else
		retVal.rightCliffHanger;

	retVal.current = this->first;
	return retVal;
}

// end() method. Sets the iterator to the end of the Double Linked List
template<typename T>
Iterator<T> DoublyLinkedList<T>::end() {
	Iterator<T> retVal;
	retVal.rightCliffHanger = true;
	retVal.current = this->last;
	return retVal;
}



//***********************************
//TODO, complete the ourReverse function.
//Note that begin and end here are are iterators (The T is just saying it can be any kind of iterator).
//Our tools are ***ONLY***
//++, --, !=, *, ==
//For example, these three lines do a swap:
///----
//auto temp = *end;
//*end = *begin;
//*begin = temp;
//--
//++begin;
//--end;
template <typename T>
void ourReverse(T begin, T end) {

	int count = 1;
	int temp = 0;
	--end;
	T copyEnd = end;
	T copyBeg = begin;
	while (copyBeg != copyEnd) {
		count++;
		++copyBeg;
	}

	copyBeg = begin;
	if (count % 2 == 1) {
		while (copyBeg != copyEnd) {
			iter_swap(copyEnd, copyBeg);
			++copyBeg;
			--copyEnd;
		}
	}
	else if (count % 2 == 0) {
		while (temp != (count / 2)) {
			iter_swap(copyBeg, copyEnd);
			++copyBeg;
			--copyEnd;
			temp++;
		}
	}

}


//----------------------------------------------------------------------------------------------------------------------------------------
//This helps with testing, do not modify.
bool checkTest(string testName, string whatItShouldBe, string whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else {
		cout << "****** Failed test " << testName << " ****** " << endl << "     Output was " << whatItIs << endl << "     Output should have been " << whatItShouldBe << endl;
		return false;
	}
}

//This helps with testing, do not modify.
bool checkTest(string testName, int whatItShouldBe, int whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else {
		cout << "****** Failed test " << testName << " ****** " << endl << "     Output was " << whatItIs << endl << "     Output should have been " << whatItShouldBe << endl;
		return false;
	}
}

//This helps with testing, do not modify.
bool checkTestMemory(string testName, int whatItShouldBe, int whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else {
		cout << "***Failed test " << testName << " *** " << endl << "  There are " << whatItIs << " bytes of memory yet to be reclaimed!" << endl;
		return false;
	}
}


//This helps with testing, do not modify.
void testIteratorFundamentals() {
	cout << "|||### You need to comment in testIteratorFundamentals() when it's ready and remove this ###|||" << endl;
	
	DoublyLinkedList<int> d;

	//Our test data should have all even numbers from 2 to 20
	for (int i = 2; i <= 20; i += 2) {
	d.insertLast(i);
	}

	//Get an iterator which points at the beginning of the list
	Iterator<int> iter = d.begin();
	
	//Test the operator* operator
	checkTest("testIteratorFundamentals #1", 2, *iter);
	
	//Test the ++ prefix increment opreator
	++iter;
	checkTest("testIteratorFundamentals #2", 4, *iter);
	
	//Test the != operator
	//reset them back to the start
	iter = d.begin();
	Iterator<int> anotherIter = d.begin();
	if (iter != anotherIter) {
	cout << "****** Failed testIteratorFundamentals #3 ****** " << endl << "     The two iteraters held the same data." << endl;
	}
	else {
	cout << "Passed testIteratorFundamentals #3" << endl;
	}

	//Again test the != operator
	++iter;
	if (iter != anotherIter) {
	cout << "Passed testIteratorFundamentals #4" << endl;
	}
	else {
	cout << "****** Failed testIteratorFundamentals #4 ****** " << endl << "     The two iteraters held different data." << endl;
	}
	
	//Test the ++postfix increment
	iter = d.begin(); //reset it back to the start
	anotherIter = iter++;  //anotherIter should be at the data 2

	checkTest("testIteratorFundamentals #5", 4, *iter);
	checkTest("testIteratorFundamentals #6", 2, *anotherIter);

	checkTest("testIteratorFundamentals #7", "2 4 6 8 10 12 14 16 18 20", d.getStringFromList());
	
}

//This helps with testing, do not modify.
void testIteratorIncrement() {
	
	DoublyLinkedList<int> *d = new DoublyLinkedList<int>;

	//Our test data should have all even numbers from 2 to 20
	for (int i = 2; i <= 20; i += 2) {
	d->insertLast(i);
	}

	//Get an iterator which points at the beginning of the list
	Iterator<int> iter = d->begin();

	//Test that it does point to the first
	checkTest("testIteratorsIncrement #1", 2, *iter);

	//Test that our Iterator can move forward;
	++iter;	checkTest("testIteratorsIncrement #2", 4, *iter);


	//Test that our Iterator can move forward again;
	++iter;
	checkTest("testIteratorsIncrement #3", 6, *iter);

	//move it some more
	for (int i = 0; i < 6; i++) {
	++iter;
	}
	checkTest("testIteratorsIncrement #4", 18, *iter);

	//Hit the end
	++iter;
	checkTest("testIteratorsIncrement #5", 20, *iter);

	//Verify we move the iterator past the end without crashing
	++iter;
	checkTest("testIteratorsIncrement #6", "did not crash", "did not crash");

	delete d;
	
}

//This helps with testing, do not modify.
void testIteratorDecrement() {
	
	DoublyLinkedList<int> *d = new DoublyLinkedList<int>;

	//Our test data should have all even numbers from 2 to 20
	for (int i = 2; i <= 20; i += 2) {
	d->insertLast(i);
	}

	//Get an Iterator which points at the end of the list
	Iterator<int> iter = d->end();
	--iter;  //We have to do a decrement otherwise it crashes

	//Test that it does point to the first
	checkTest("testIteratorsDecrement #1", 20, *iter);

	//Test that our Iterator can move forward;
	--iter;
	checkTest("testIteratorsDecrement #2", 18, *iter);

	//move it some more
	for (int i = 0; i < 7; i++) {
	--iter;
	}
	checkTest("testIteratorsDecrement #3", 4, *iter);

	//Hit the end
	--iter;
	checkTest("testIteratorsDecrement #4", 2, *iter);

	//Now go back forward
	++iter;
	checkTest("testIteratorsDecrement #5", 4, *iter);

	delete d;
	
}

//This helps with testing, do not modify.
void testIterationTricky() {
	
	DoublyLinkedList<int> myListOneNode;

	myListOneNode.insertLast(42);
	cout << "TestIterationTricky test #1, the next line should display 42" << endl;
	stringstream ss;
	//see if we can just iterator through one item.
	for (auto i : myListOneNode) {
	cout << i << " ";
	ss << i << " ";
	}
	cout << endl;
	checkTest("TestIterationTricky test #1", "42 ", ss.str());
	ss.str("");

	DoublyLinkedList<int> myListEmpty;
	cout << "TestIterationTricky test #2, the next line shouldn't display anything" << endl;
	//see if we can just iterator through one item.
	for (auto i : myListEmpty) {
	cout << i << " ";
	ss << i << " ";
	}
	cout << endl;
	checkTest("TestIterationTricky test #2", "", ss.str());
	ss.str("");
	
}

//This helps with testing, do not modify.
void testAlgorithms() {
	
	DoublyLinkedList<int> myList;

	//Our test data should have all even numbers from 2 to 20
	for (int i = 2; i <= 6; i += 2) {
	myList.insertLast(i);
	}
	myList.insertLast(100);
	for (int i = 8; i <= 12; i += 2) {
	myList.insertLast(i);
	}
	myList.insertLast(100);
	for (int i = 14; i <= 20; i += 2) {
	myList.insertLast(i);
	}
	stringstream ss;
	cout << "testAlgorithms test #1, this should display 2 4 6 100 8 10 12 100 14 16 18 20." << endl;
	for (auto i : myList) {
	cout << i << " ";
	ss << i << " ";
	}
	cout << endl;
	checkTest("testAlgorithms test #1", "2 4 6 100 8 10 12 100 14 16 18 20 ", ss.str());
	ss.str("");
	
	//Test the STL reverse algorithm on our iterator
	cout << "testAlgorithms test #2, this should display 20 18 16 14 100 12 10 8 100 6 4 2." << endl;
	reverse(myList.begin(), myList.end());
	for (auto i : myList) {
	cout << i << " ";
	ss << i << " ";
	}
	cout << endl;
	checkTest("testAlgorithms test #2", "20 18 16 14 100 12 10 8 100 6 4 2 ", ss.str());
	ss.str("");
	
	//Test our reverse algorithm
	cout << "testAlgorithms test #3, this should display 20 18 16 14 100 12 10 8 100 6 4 2." << endl;
	//Get it back in ascending order using the STL reverse algorithm

	reverse(myList.begin(), myList.end());
	//Now reverse it using our algorithm
	ourReverse(myList.begin(), myList.end());
	for (auto i : myList) {
	cout << i << " ";
	ss << i << " ";
	}
	cout << endl;
	checkTest("testAlgorithms test #3", "20 18 16 14 100 12 10 8 100 6 4 2 ", ss.str());
	ss.str("");
	
	//Test our reverse algorithm on an STL vector collection
	vector<int> v;
	for (int i = 2; i <= 20; i += 2) {
	v.push_back(i);
	}
	cout << "testAlgorithms test #4, this should display 20 18 16 14 12 10 8 6 4 2." << endl;
	ourReverse(v.begin(), v.end());
	for (auto i : v) {
	cout << i << " ";
	ss << i << " ";
	}
	cout << endl;
	checkTest("testAlgorithms test #4", "20 18 16 14 12 10 8 6 4 2 ", ss.str());
	ss.str("");

	//Now test an odd sized list
	DoublyLinkedList<int> oddList;
	for (int i = 2; i <= 6; i += 2) {
	oddList.insertLast(i);
	}
	oddList.insertLast(100);
	for (int i = 8; i <= 12; i += 2) {
	oddList.insertLast(i);
	}
	oddList.insertLast(100);
	for (int i = 14; i <= 18; i += 2) {
	oddList.insertLast(i);
	}
	cout << "testAlgorithms test #5, this should display 18 16 14 100 12 10 8 100 6 4 2." << endl;
	ourReverse(oddList.begin(), oddList.end());
	for (auto i : oddList) {
	cout << i << " ";
	ss << i << " ";
	}
	cout << endl;
	checkTest("testAlgorithms test #5", "18 16 14 100 12 10 8 100 6 4 2 ", ss.str());
	ss.str("");


	//Delete all of occurrence using an STL algorithm.
	cout << "testAlgorithms test #6, this should display 4 2 5 1 2." << endl;
	DoublyLinkedList<int> manyNines;
	manyNines.insertLast(9);
	manyNines.insertLast(9);
	manyNines.insertLast(4);
	manyNines.insertLast(2);
	manyNines.insertLast(9);
	manyNines.insertLast(9);
	manyNines.insertLast(5);
	manyNines.insertLast(1);
	manyNines.insertLast(9);
	manyNines.insertLast(2);
	manyNines.insertLast(9);
	manyNines.insertLast(9);
	auto beginIter = manyNines.begin();
	auto endIter = manyNines.end();
	endIter = remove(beginIter, endIter, 9);

	while (beginIter != endIter) {
	cout << *beginIter << " ";
	ss << *beginIter << " ";
	++beginIter;
	}
	cout << endl;
	checkTest("testAlgorithms test #6", "4 2 5 1 2 ", ss.str());
	ss.str("");
	
}


void pressAnyKeyToContinue() {
	cout << "\nPress any key to continue...";

	//Linux and Mac users with g++ don't need this
	//But everyone else will see this message.
#ifndef __GNUC__
	_getch();

#else
	int c;
	fflush(stdout);
	do c = getchar(); while ((c != '\n') && (c != EOF));
#endif
	cout << endl;
}

int main() {

	testIteratorFundamentals();
	checkTestMemory("Memory Leak/Allocation Test #1", 0, ManageMemory::getTotalSize());
	pressAnyKeyToContinue();

	testIteratorIncrement();
	checkTestMemory("Memory Leak/Allocation Test #2", 0, ManageMemory::getTotalSize());
	pressAnyKeyToContinue();

	testIteratorDecrement();
	checkTestMemory("Memory Leak/Allocation Test #3", 0, ManageMemory::getTotalSize());
	pressAnyKeyToContinue();

	testIterationTricky();
	checkTestMemory("Memory Leak/Allocation Test #4", 0, ManageMemory::getTotalSize());
	pressAnyKeyToContinue();

	testAlgorithms();
	checkTestMemory("Memory Leak/Allocation Test #5", 0, ManageMemory::getTotalSize());
	pressAnyKeyToContinue();

	return 0;
}


