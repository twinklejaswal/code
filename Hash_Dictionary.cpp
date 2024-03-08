// Implementation file for the Dictionary class
// Stores data in a hash table 
// By Twinkle Jaswal 
// Program 2 Part 2

#include "Dictionary.h"
#include <vector>
#include<algorithm>

//constructor 
Dictionary::Dictionary(){
    size = tableSizeArray[tableSizeArrIndex];  
    myHashTable = new Entry*[size]{nullptr};
    occupied = 0; 
}

//assignment operator 
Dictionary& Dictionary::operator=(const Dictionary& rhs)
{
    if (this != &rhs)
    {
        clear();
        copy(rhs);
    }
    return *this;
}

//adds an entry to the dictionary by calling private insert function 
void Dictionary::addEntry(Entry* anEntry)
{
   insert(anEntry);
}

//Checks if the hash table is empty 
bool Dictionary::isEmpty() const{
    for(int i =0; i<size; i++){
        if(myHashTable[i] != nullptr)
        {
            return false; 
        }
    }
    return true; 
}

//Finds an entry based off of the key 
//returns nullptr if doesn't find the word 
Entry* Dictionary::findEntry(string key) const
{
    int hashIndex = hash(key, size); 
    int offset = hash(key, primeNumArray[primeArrIndex]) + 1;
           
    while(myHashTable[hashIndex]!=nullptr){
        if(myHashTable[hashIndex]->getKey() == key){
           return myHashTable[hashIndex];  
        }
        hashIndex = (hashIndex+offset)%size; 
    }

    return nullptr; 

}

//Prints concordance in order 
//copies the dynamic array of pointers to a vector and uses built in sort to sort the vector 
//uses the vector to print the contents in order 
void Dictionary::printDictionaryInOrder(ostream& outputStream) const
{
    vector<Entry*> myHashCopy; 
    for(int index = 0; index < size; index++){      //makes copy of myHashTable
       if(myHashTable[index]!=nullptr){
            myHashCopy.push_back(myHashTable[index]);
       } 
    } 

    struct{         //custom comparing function to sort the string 
        bool operator()(Entry* strKey1, Entry* strKey2) const{          //adopted from cppreference
            return strKey1->getKey() < strKey2->getKey(); 
        }
    }
    customCompare;

    std::sort(myHashCopy.begin(), myHashCopy.end(), customCompare);
    
    //print the sorted vector 
    for(Entry* curPtr : myHashCopy){
        curPtr->printEntry(outputStream);  
    }
    
}

//Prints the contents in the hash table 
void Dictionary::printDictionaryKeys(ostream& outputStream) const
{
    for(int index = 0; index < size; index++){
        if(myHashTable[index] == nullptr){
            outputStream << index << " " << "\n"; 
        } else{
            outputStream << index << " " << myHashTable[index]->getKey() << "\n" ; 
        }
    }
}

//private helper function for the destructor and assignment operator 
void Dictionary::clear()
{
    for(int i =0; i < size; i++){
       if(myHashTable[i] !=nullptr){
        delete myHashTable[i]; 
       } 
    }
    delete [] myHashTable; 
    myHashTable = nullptr; 
}

//private helper function for the copy constructor and assignment operator 
void Dictionary::copy(const Dictionary& orig){
    this->size = orig.size; 
    this->occupied = orig.occupied; 
    this->myHashTable = new Entry*[size]{nullptr}; 
    
    for(int i =0; i < size; i++){
       if(orig.myHashTable[i] != nullptr){
            Entry* newData = new Entry(*(orig.myHashTable[i]));
            this->myHashTable[i] = newData; 
       }
    }
}

//private function to insert a word into the dictionary 
//if hash table is almost half full, then calls the rehash function 
void Dictionary::insert(Entry* value)
{ 
   if(occupied < size/2){
        int hashIndex = hash(value->getKey(), size); 
        if(myHashTable[hashIndex] == nullptr){
            myHashTable[hashIndex] = value;
            occupied++;  
        }else{
            primeArrIndex == tableSizeArrIndex; 
            int offset = hash(value->getKey(), primeNumArray[primeArrIndex]) + 1; 
            while(myHashTable[hashIndex] != nullptr){ 
                hashIndex = (hashIndex+offset)%size; 
                if(myHashTable[hashIndex] == nullptr){
                    myHashTable[hashIndex] = value; 
                    occupied++; 
                    return; 
                }
            }
        }

   } else{
        rehash(); 
        insert(value); 
   }
}

//string into a hash value 
unsigned int Dictionary::hash(const string& key, int tableSize) const{
    unsigned int hashVal =0; 
    for(char ch:key){
        hashVal = 37 * hashVal + ch; 
    }
    return hashVal%tableSize; 
}

//if the table is half full then chooses the next size from tableSize and 
//rehashes the table 
void Dictionary::rehash(){
    tableSizeArrIndex++; 
    int oldSize = size; 
    size = tableSizeArray[tableSizeArrIndex]; 
    primeArrIndex = tableSizeArrIndex; 
    occupied = 0; 
    Entry** savedTable = myHashTable; 
    myHashTable = new Entry*[size]{nullptr};

    for(int i = 0; i < oldSize; i++){
        if(savedTable[i] != nullptr){
            Entry* currValue = savedTable[i]; 
            insert(currValue); 
        }
    } 
    delete [] savedTable; 
    savedTable = nullptr;

}



