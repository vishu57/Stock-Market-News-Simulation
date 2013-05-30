/** File "DList.h", KWR edit of Koffman-Wolfgang Ch. 4 double linked-list
    example.  "Item_Type" has been changed to "T" for simplicity, and the use
    of templates has been made more explicit, rather than "typedef'ed away".
    For CSE250 Fall 2010.
 */
#ifndef DLIST_H_
#define DLIST_H_

#include <iostream>
#include <sstream>
#include <string>

using std::string;     //From now on we rule out "using namespace std;"
using std::iostream;   //in library files---only clients may do it.
using std::ostringstream;
using std::cout;
using std::cerr;
using std::endl;


template <typename I>  //REQ: I has I(), operator<<(iostream&, const I&)
class DList;


/** A DNode is the building block for a double-linked list.  Text uses struct
    to make data fields public---later (of course!) we will encapsulate it.
 */
template <typename I>
struct DNode {
  /** A pointer to the list I belong to---added by KWR.
      [Not needed or even good for your node class, *yes* should do for
      your iterator class.]
   */
  DList<I>* myList;

  /** A copy of the data   (KWR: we might use pointers to avoid copying)
   */
  I data;

  /** Pointers to previous and next DNode (KWR: note different order from text)
   */
  DNode<I>* prev;
  DNode<I>* next;

  DNode(DList<I>* myList, const I& item, DNode<I>* prev = NULL,
                                         DNode<I>* next = NULL)
   : myList(myList), data(item), next(next), prev(prev) 
  { }
};


template <typename I>    //Written by KWR. 
class DList {
   DNode<I>* firstNode;  //by convention, a "real" node
   DNode<I>* endNode;    //by convention, a dummy node. Use to end while loops.

   //NOTE: Your Valli class will have vector<DNode*> [optional *] elements.

   //CLASS INV: firstNode always points to first node, endNode to dummy node
   //CLASS INV: Empty list exactly when firstNode == endNode == the dummy node
   //CLASS INV: endNode never changes; we only insert before it.
 public:
   DList<I>() : firstNode(new DNode<I>(NULL, I(), NULL, NULL)), endNode(firstNode){
        firstNode->myList = endNode->myList = this;
    }

   /** Example of destructor added by KWR.  Using "cerr" guarantees 
       immediate screen output.
    */
   virtual ~DList<I>() {
      //cerr << "This list will self-destruct in 5 nanoseconds..." << endl;
      DNode<I>* current = endNode;
      while (current != firstNode) {
         //cerr << "Deleting: " << current->data << "; ";
         current = current->prev;
         delete(current->next);
      }
      //delete(current);  //do you dare comment this in?
      //cerr << "Deleting: " << current->data << "; done." << endl;
      delete(current);  //i.e. delete(firstNode);
   }

   void clear()
   {
	  //cerr << "This list will self-destruct in 5 nanoseconds..." << endl;
      DNode<I>* current = endNode;
      while (current != firstNode) {
         //cerr << "Deleting: " << current->data << "; ";
         current = current->prev;
         delete(current->next);
      }
      //delete(current);  //do you dare comment this in?
      //cerr << "Deleting: " << current->data << "; done." << endl;
      delete(current);  //i.e. delete(firstNode);

	  endNode = firstNode = new DNode<I>(this, I(), NULL, NULL);
   }

   class iterator {
      friend class DList<I>;
      friend struct DNode<I>;  //needed?

      DNode<I>* curr;

      explicit iterator(DNode<I>* given) : curr(given) { }

     public:
      //default copy-ctor and operator= are OK, no destructor needed either
	  iterator():curr(NULL){}

      I& operator*() const { return curr->data; }  //allows assignment
      //WARNING, by allowing assgt this allows breaking Valli's sortedness INV

      //REQ: curr != endNode, else may bomb
      //Alternative conventions are endNode->next == endNode, or == firstNode!
      iterator& operator++() { 
         curr = curr->next;
         return *this;   //reference to self is returned
      }
   
      //REQ: curr != endNode, else may bomb
      iterator operator++(int x) {
         iterator oldMe = *this;  //uses copy ctor
         curr = curr->next;
         return oldMe;   //COPY of old self is returned
      }

      bool operator==(const iterator& other) const {
         return curr == other.curr;
      }
      bool operator!=(const iterator& other) const {
         return curr != other.curr;
      }
   };

   DNode<I>* pbegin() const { return firstNode; }  
   DNode<I>* pend() const { return endNode; }

   iterator begin() const { return iterator(firstNode); }
   iterator end() const { return iterator(endNode); }
   //LATER we will "hide" the node implementation and return iterators instead.

   //This will CHANGE as well in Valli, to have just the inserted item
   void insert(DNode<I>* beforeMe, I newItem) {  
      DNode<I>* newNode = new DNode<I>(this,newItem,beforeMe->prev,beforeMe);
      if (beforeMe->prev != NULL) {   //i.e. if we're not inserting a new first
         beforeMe->prev->next = newNode;
         beforeMe->prev = newNode;  //don't forget!
      } else {
         beforeMe->prev = newNode;
         firstNode = newNode;
      }
   }

   iterator insert(iterator it_beforeMe, I newItem) {
	  DNode<I>* beforeMe = it_beforeMe.curr;
	  insert(beforeMe, newItem);
	  return iterator(beforeMe->prev);
   }
    
    void erase(DNode<I>* ptr)
    {
        if (ptr == endNode)//endNode is just a dummy node. You can NOT delete it!
            return;
        if (ptr == firstNode)
            firstNode = ptr->next;
        if (ptr->prev != NULL)
            ptr->prev->next = ptr->next;
        if (ptr->next != NULL)
            ptr->next->prev = ptr->prev;
        delete ptr;
    }
    
    void erase(iterator itr)
    {
        DNode<I>* ptr = itr.curr;
        erase(ptr);
    }

   size_t size() const {            //illustrates size_t (unsigned int) type
      size_t count = 0;
      DNode<I>* current = firstNode;
      while (current != endNode) {
         count++;
         current = current->next;
      }
      return count;
   }

   //change to use the iterator internally
   string toString() const {       //Node resemblance to code in Deque<I> class
      ostringstream OUT;
      //DNode<I>* current = firstNode;
      iterator itr = begin();
      //while (current != endNode) {
      while (itr != end()) {
         //OUT << current->data << " ";  //REQ of T having operator<< used here.
         //current = current->next;
         OUT << *itr << " ";
         ++itr;
      }
      return OUT.str();
   }

   string str() const {
      return toString();
   }

};
#endif
