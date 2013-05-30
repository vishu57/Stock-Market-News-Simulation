/** File "BST.h", by KWR for CSE250, Fall 2009---updating 1996--2000
    versions to pass-in function objects via templates rather than via
    constructor arguments.  Thus it follows the K-W text's general practice.
    CHANGES from the K-W representation, all as in the "standard STL tree":

    () extra "parent" field in nodes---better support for iterator class
    () parent links also remove need for " *& " reference-to-link parameters
    () NULL pointers replaced by pointers to a node called NIL
    () no inheritance from an unsorted tree class
    () provides iterator class (inorder, bi-directional), so STL-conformant

    Most of the text's code doesn't care about these changes and carries
    over almost-verbatim, just needing to link the "parent" field correctly.
    Public code is *inlined* for readability, private helpers are below class.

    CHANGES beyond STL:
    () third template arg for ItemMatch, making a "multiset++"---by which I
       mean one can treat like a set at item-level, multiset at key-level.
    () a prettier "sideways" string conversion that shows the tree links.
 */

#ifndef BST_H_
#define BST_H_

#include <iostream>
#include <stdexcept>
#include <string>
#include <functional> // for std::less, std::equal_to defaults
                      // defaults are compatible with STL, but IMHO dubious...

using std::string;
using std::cerr;
using std::cout;
using std::endl;


template <class I, class LessThan = std::less<I>,
                   class ItemMatch = std::equal_to<I> >
class BST {
public:               // this & the next line not needed on timberlake, but
   class iterator;    // seem to be needed on Macintosh g++ and other systems
private:
   friend class iterator;
   friend class Node;
   class Node {                    
      friend class BST;   ///ANSI made this necessary in 1998.
      friend class iterator;
      I data;
      Node* left;
      Node* right;
      Node* parent;    // added mainly for iterator, which text doesn't code

      Node() : data(I()), left(NULL), right(NULL), parent(NULL) { }
      Node(I item, Node* left, Node* right, Node* parent)
       : data(item), left(left), right(right), parent(parent)
      { }
      virtual ~Node() { }
   };

   //----------------------------Class Fields------------------------------
   
   Node* NIL;          //in place of raw NULL pointers
   Node* root;
   const LessThan lessThan;
   const ItemMatch itemMatch;
   
   /** STL boolean field to decide between set/multiset and map/multimap.
       When false, code behaves as in K-W textbook---no duplicate keys.
       When true, code allows multiple items with the same key.
    */
   bool insert_always;

   /** Switch between using inorder predecessor or successor in erase.
    */
   bool usePred;

   //-----------------------Private Helper Functions-----------------------

   /** Insert item into portion of tree below the link to a local root.
       If insert_always, allow item to be duplicate, and return true.
       If not && item's key is already present, *don't insert*, return false.
       IMPT: Since it holds the link by-reference, works even if the link
       goes to NIL---which is when you actually create the new node.  Like
       text---unlike STL---does not also return iterator to node with item.
       Hence the delegating public "insert" has to call find(item) redundantly.
       The extra "parent" parameter helps build the new node's parent link.
       REQ: parent is source of link2localRoot.
    */
   bool insert(Node*& link2localRoot, const I& item, Node* parent);

   bool erase(Node*& link2localRoot, Node* parent);

   /** Minimum and maximum node below a given node; min/maxBelow(NIL) = NIL;
    */
   Node* minBelow(Node* x) const;
   Node* maxBelow(Node* x) const;

   /** Return next node in sorted order; on last node, returns NIL 
       Delegatee of the iterator operator++.  Ditto for operator--.
    */
   Node* inorderSuccessor(const Node* x) const;
   Node* inorderPredecessor(const Node* x) const;

   size_t subtreeSize(Node* n) const {
      return (n == NIL) ? 0 : 1 + subTreeSize(n->left) + subTreeSize(n->right);
   }

   /** used by str()*/
   static string spaces(int n) {
      return n > 0 ? string(" "+spaces(n-1)) : string("");
   }

public:

   class iterator {
      friend class BST;
      friend class BST::Node;
      const BST* theTree;      //so iterator class has a handle on "root" cell
      Node* currentNode;

      iterator(const BST* attachedTree, Node* atNode)
       : theTree(attachedTree), currentNode(atNode) 
      { }

   public:
      /** Why is this the only public constructor? Because the other ones
       *  have implementation-specific info, namely "BST"!
       *  Clients are expected to use "BST.begin()" to make new iterators.
       *  Hence IMPT to disable the zero-param constructor by defining this!
       */
      iterator(const iterator& other) 
       : theTree(other.theTree), currentNode(other.currentNode) 
      { }
     
      virtual ~iterator() { }

//----------------------STL methods-----------------------------

      const iterator& operator= (const iterator& other) {
         theTree = other.theTree;
         currentNode = other.currentNode;
         return *this;
      }
      bool operator== (const iterator& rhs) const {
         return (theTree == rhs.theTree && currentNode == rhs.currentNode);
      }
      bool operator!= (const iterator& rhs) const {
          return !(operator==(rhs));
      }
      iterator operator++ () {    //pre-increment, as in ++it
         currentNode = theTree->inorderSuccessor(currentNode);
         return (*this);
      }
      iterator operator++ (int) { //post-increment, as in it++
         iterator temp = *this;
         currentNode = theTree->inorderSuccessor(currentNode);
         return temp;
      }
      iterator operator-- () {    //pre-decrement, as in --it
         currentNode = theTree->inorderPredecessor(currentNode);
         return (*this);
      }

      iterator operator-- (int) { //post-decrement, as in --it
         iterator temp = *this;
         currentNode = theTree->inorderPredecessor(currentNode);
         return temp;
      }

      I& operator* () const {       //same "security hole" as with Valli...
         return currentNode->data;  //I had a note about this in 2000 too.
      }

   }; //end of class iterator.  Forward & reverse begin/end methods:

   iterator begin() const {  
      Node* x = minBelow(root);   // recall root == NIL for empty tree
      return iterator(this, x);
   }
   iterator end() const {
      return iterator(this,NIL);
   }
   iterator rbegin() const {
      Node* x = maxBelow(root);   // recall root == NIL for empty tree
      return iterator(this, x);
   }
   iterator rend() const {
      return iterator(this,NIL);
   }


/*-------------------------------------------------------------------------
  CLASS INVARIANTS: Every BST has a non-null NIL node.
  Tree is empty iff its root pointer == NIL; always root.parent == NIL.  Any
  link in the tree stands for the subtree rooted at the node it goes to, and
  so an Empty Subtree is identified with a NIL "left" or "right" pointer.
  Also, NIL.parent == root  (used by the Standard Template Library code).

  Hence, VERY IMPT: All pointers not going to nodes have value NIL.

  In a nonempty (sub-)tree, all nodes are sorted in non-descending order
  of keys according to the passed-in "lessThan" function, going from left-to- 
  right in "in-order".  If duplicate keys are allowed, this implies that 
  items with the same key are consecutive according to the private
  successor(Node n) function.  
*///-----------------------------------------------------------------------
   
   BST() : NIL(new Node()), root(NIL), lessThan(LessThan()), 
      itemMatch(ItemMatch()), insert_always(true), usePred(true)
   {
      NIL->left = NIL;
      NIL->right = NIL;
      NIL->parent = root;
   }

   explicit BST(bool allowDuplicateKeys, bool usePred = true)
    : NIL(new Node())
    , root(NIL)
    , lessThan(LessThan())
    , itemMatch(ItemMatch())
    , insert_always(allowDuplicateKeys)
    , usePred(usePred)
   {
      NIL->left = NIL;
      NIL->right = NIL;
      NIL->parent = root;
   }

   /** Destruct tree by erasing all nodes.  Because erase uses the
       inorder /predecessor/ to patch up after an erase, the iterator
       remains formally valid when it marches onto the next node.
       Use of the post++ idiom makes the code short as well as safe.
    */
   virtual ~BST() {
      while (!empty()) {
         erase(begin());
      }
   } 
   

   bool empty() const { return root == NIL; }

   int size() const { return subtreeSze(root); }


   /** First node whose data is >= item, i.e. !lessThan(data,item),
       returned iterator is end() if no such node.
       Strategy is to go down left when equality is possible.
       The exit node (leaf or elbow) found this way is either the
       leftmost node with the item or its inorder predecessor.
    */
   iterator lower_bound(const I& item) const {
      Node* curr = root;
      Node* trailer = NIL;  //INV: trailer == curr.parent
      while (curr != NIL) { //INV: if loop entered, trailer stays a real node.
         trailer = curr;
         curr = lessThan(curr->data, item) ? curr->right : curr->left;
      } // now trailer is on exit node
      if (trailer != NIL && lessThan(trailer->data, item)) {
         return iterator(this, inorderSuccessor(trailer));
      } else {
         return iterator(this, trailer);
      }
   }

   /** First node whose data is > item, i.e. lessThan(item,data), else end()
       Is exit node of right-leaning search, or its successor.
    */
   iterator upper_bound(const I& item) const {
      Node* curr = root;
      Node* trailer = NIL;  //INV: trailer == curr.parent
      while (curr != NIL) { //INV: if loop entered, trailer stays a real node.
         trailer = curr;
         curr = lessThan(item, curr->data) ? curr->left : curr->right;
      } // now trailer is on exit node
      if (trailer != NIL && lessThan(item, trailer->data)) {
         return iterator(this, trailer);
      } else {
         return iterator(this, inorderSuccessor(trailer));
      }
   }


   /** Iterator to node for which item ItemMatches the node's data.
       end() if no such item in tree, even if some nodes have the same /key/.
    */
   iterator find(const I& item) const {
      iterator itr2 = upper_bound(item);
      iterator itr1 = lower_bound(item);
//cout << "Seeking----: " << item.str() << endl;
//cout << "Lower bound: " << (*itr1).str() << endl;
//cout << "Upper bound: " << (*itr2).str() << endl;
      for ( ; itr1 != itr2; ++itr1) {
         if (itemMatch(item, *itr1)) {
            return itr1;
         } // else
      }
      return end();
   }


   /** Add item, maintaining CLASS INVS by keeping tree sorted. 
       For full STL conformance, should return std::pair<iterator, bool>.
    */
   iterator insert(const I& item) {
      bool inserted = insert(NIL->parent, item, NIL);//recall root->parent = NIL
      iterator itr = find(item);
      return itr;
   }


   //bool erase(const I& item);  //K-W text's prototype on p476, but non-STL.
   //Also redundant: use itr = find(item); if (itr != end()) { erase(itr); }


   /** Erase item referenced by iterator, error if iterator==end().
       Equivalent to the part on K-W page 477 after "else { // Found item".
       But in the case of two children, rather than call "replace_parent",
       we use the iterator operator-- to move "pos" to the node to delete.
       (If usePred is false, use pos++ instead, per p484, Prog Exercise 1.)
       We move its data up to the old position, and call on the new "pos".
       The call won't recurse further, because the predecessor (or successor)
       of a node with 2 children must have at most 1 child, under inorder.
    */
   void erase(iterator pos) {
      if (pos == end()) {
         throw std::invalid_argument("Attempt to erase the end");
      } 
      Node* current = pos.currentNode;
      if (current->left != NIL && current->right != NIL) {   // 2 children
         iterator moveData = usePred ? pos-- : pos++; //erase pos at new place
         *moveData = *pos;
         erase(pos);                    //pos now has <= 1 child, so done.
      } else {                          //we erase current node.
         Node* spliceTo = (current->left == NIL) ? current->right
                                                 : current->left;
         Node* dparent = current->parent;
         if (current == dparent->left) {
            dparent->left = spliceTo;   //unlinks current node
         } else {
            dparent->right = spliceTo;
         } 
         if (current == root) {         //extra fixups needed
            NIL->parent = root = spliceTo;
            //note that if this was the last node, root == NIL automatically
         }
         if (spliceTo != NIL) {
            spliceTo->parent = dparent;
         }
         delete (current);              //zaps iterator pos too.
      }
   }


   /** Erase *all* occurrences of item, using ItemMatch rather than key.
       Returns number of items erased---rather than bool like text pp 469,476.
    */
   size_t erase(const I& item) {
      size_t numErased = 0;
      iterator itr1 = lower_bound(item);
      iterator itr2 = upper_bound(item);
      while (itr1 != itr2) {
         if (itemMatch(item, *itr1)) {
            iterator delendum = itr1++;
            erase(delendum);   //itr1 stays valid since predecessor is moved
            numErased++;
	 } else {
            ++itr1;
         }
      }
      return numErased;
   }


/*-------------------------------------------------------------------------
Other Public Methods (non-STL), 
*///-----------------------------------------------------------------------

   /** Print subtree rooted at node "nat" by recursive inorder transversal.
       REC INV: if rhs and/or lhs is nonempty, it ends with a "\n".
       NOTE: Order is optically reversed, i.e. left side is down not up.
    */
   string str(Node* nat, int indent, int offset) const {
      string rhs = (nat->right==NIL) ?
            string("") : str(nat->right,indent+offset,offset);
      string lhs = (nat->left==NIL) ?
            string("") : str(nat->left,indent+offset,offset);
      return rhs + spaces(indent) + string("()") + nat->data.str()
            + string("\n") + lhs;
   }

   /** Print subtree rooted at node "nat" by recursive preorder transversal.
       REC INV: if rhs and/or lhs is nonempty, it ends with a "\n".
       NOTE: This time order is correct top-down, i.e. root is first not last.
    */
   string preStr(Node* nat, int indent, int offset) const {
      string rhs = (nat->right==NIL) ?
            string("") : preStr(nat->right,indent+offset,offset);
      string lhs = (nat->left==NIL) ?
            string("") : preStr(nat->left,indent+offset,offset);
      return spaces(indent) + string("()") + nat->data.str()
            + string("\n") + lhs + rhs;
   }

   /** Print subtree rooted at node "nat" by recursive postorder transversal.
       REC INV: if rhs and/or lhs is nonempty, it ends with a "\n".
    */
   string postStr(Node* nat, int indent, int offset) const {
      string rhs = (nat->right==NIL) ?
            string("") : postStr(nat->right,indent+offset,offset);
      string lhs = (nat->left==NIL) ?
            string("") : postStr(nat->left,indent+offset,offset);
      return lhs + rhs + spaces(indent) + string("()") + nat->data.str()
            + string("\n");
   }

   

   /** Pretty-print tree with branches indented. Only info fields printed. */
   string str() const {
      int indent = 0;
      int offset = 4;
      return str(root,indent,offset);
   }

   string preStr() const { return preStr(root,0,4); }
   string postStr() const { return postStr(root,0,4); }

   virtual string whatami() const { return "BST<I>"; }

   void refresh(size_t r) { }

};  //end of class BST.



//---------------------------Helper Method Bodies----------------------
//Exhibited outside typename partly to show off the Armor-TemPlated Syntax!
//Convention(?): Double-indent (here 6 sp) means continuation of previous line

template<class I, class LessThan, class ItemMatch>
typename BST<I,LessThan,ItemMatch>::Node* 
      BST<I,LessThan,ItemMatch>::minBelow (
            typename BST<I,LessThan,ItemMatch>::Node* x) const {
   while (x->left != NIL) {
      x = x->left;    //OK since x is a ptr-to-const-data, not a const pointer
   }
   return x;
}

template<class I, class LessThan, class ItemMatch>
typename BST<I,LessThan,ItemMatch>::Node*
      BST<I,LessThan,ItemMatch>::maxBelow (
            typename BST<I,LessThan,ItemMatch>::Node* x) const {
   while (x->right != NIL) {
      x = x->right;
   }
   return x;
}


template<class I, class LessThan, class ItemMatch>
typename BST<I,LessThan,ItemMatch>::Node* 
      BST<I,LessThan,ItemMatch>::inorderSuccessor(
            const typename BST<I,LessThan,ItemMatch>::Node* x) const {
   if (x->right != NIL) {   // right subtree exists, find its leftmost node
      return minBelow(x->right);
   } else {                 // need to find first parent thru left link
      typename BST<I,LessThan,ItemMatch>::Node* y = x->parent;
      while (y != NIL && x == y->right) {
         x = y;
         y = y->parent;
      }
      return y; // if we zoomed back to root then x was max and this is NIL.
   }
}

template<class I, class LessThan, class ItemMatch>
typename BST<I,LessThan,ItemMatch>::Node*
      BST<I,LessThan,ItemMatch>::inorderPredecessor(
            const typename BST<I,LessThan,ItemMatch>::Node* x) const {
   if (x->left != NIL) {   // left subtree exists, find its rightmost node
      return maxBelow(x->left);
   } else {                // need to find first parent thru right link
      typename BST<I,LessThan,ItemMatch>::Node* y = x->parent;
      while (y != NIL && x == y->left) {
         x = y;
         y = y->parent;
      }
      return y; // if we zoomed back to root then x was min and this is NIL.
   }
}

/** REC INV: parent = link2localRoot->parent, i.e. parent is the source node.
    Inserts "leftishly", i.e. at the front of any equal range.
 */
template<class I, class LessThan, class ItemMatch>
bool BST<I,LessThan,ItemMatch>::insert(Node*& link2localRoot,
                                       const I& item, Node* parent) {
   if (link2localRoot == NIL) {
      link2localRoot = new Node(item, NIL, NIL, parent);
      if (parent == NIL) { root = link2localRoot; }
      return true;
   } else if (lessThan(link2localRoot->data, item)) {
      return insert(link2localRoot->right, item, link2localRoot);
   } else if (lessThan(item, link2localRoot->data) || insert_always) {
      return insert(link2localRoot->left, item, link2localRoot);
   } else {
      return false; 
   }
}
      
   

#endif
