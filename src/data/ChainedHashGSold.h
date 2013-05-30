/*
Author: Gagandip Singh 3/29/2013
CSE 250 - University at Buffalo
A7
base code provided by TA Mahmoud Abo Khamis
*/

#ifndef LLALLI_H_
#define LLALLI_H_

#include "DList.h"
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

using std::vector;
using std::string;
using std::iostream;
using std::ostringstream;
using std::sort;


template <typename I>
class ChainedHash
{
/* Iterator Class */
public:
	class iterator
	{
		typename DList<I>::iterator list_iter;

		iterator(const typename DList<I>::iterator& l_iter):list_iter(l_iter){}

		friend class ChainedHash<I>;

		public:
			iterator(){}

			iterator(const iterator& other)
				:list_iter(other.list_iter){}

			iterator& operator=(const iterator& rhs){
				list_iter = rhs.list_iter;
				return *this;
			}

			I& operator*() const{
				return *list_iter;
			}

			iterator& operator++(){
				++list_iter;
				return *this;
			}

			iterator operator++(int){
				iterator oldMe = *this;
				list_iter++;
				return oldMe;
			}

			bool operator==(const iterator& rhs) const{
				return list_iter == rhs.list_iter;
			}

			bool operator!=(const iterator& rhs) const{
				return list_iter != rhs.list_iter;
			}
	};

/* Inits & searchVec for ChainedHash */
private:
	DList<I> list;
	vector<iterator> vec;
	size_t n, r;
    
    iterator searchVec(const I& item)
    {
        int left = 0;
        int right = (int)vec.size()-1;
        int mid = (right+left)/2;
        
        while(left != right){
            mid = (right+left)/2;
            if (mid == 0 || mid == vec.size()-1){
                //nothing else to search, we didnt find anything
                iterator r = vec.at(vec.size()-1);
                return r;
            }
            if (*vec.at(mid) == item){
                return vec.at(mid);
            }
            else if (item > *vec.at(mid)){
                left = mid+1;
            }
            else if (item < *vec.at(mid)){
                right = mid;
            }
        }
        return vec.at(mid);
    }

    struct myhasher // function object
	{
	    int hash;
	    void operator()(int element) { 
	        cout << "element: " << element << ", hash is: " << hash << endl;
	        hash = hash + element;
	    }
	}hasher;

/* ChainedHash */
public:
	/* constructor */
	explicit ChainedHash(size_t ratio = 50)
		:list(DList<I>()),
		vec(vector<iterator>()),
		n(0), 
		r(ratio){}

	/* destructor */
	~ChainedHash(){}

	iterator begin() const{
		return iterator(list.begin());
	}

	iterator end() const{
		return iterator(list.end());
	}
    
	iterator find(const I& item) {
        iterator left = searchVec(item);
		iterator right = end();

		for (iterator iter = left; iter != right; iter++)
		{
			if (*iter == item){
				return iter;
			}
			if (*iter > item){
				return end();
			}
		}

		return end();
	}
    
	iterator insert(const I& item){

		hasher(1);

		iterator left = begin();
		iterator right = end();

        if (vec.size() == 0){
            right = begin();
        }
        else{
            left = searchVec(item);
        }
        
        iterator iter = left;
		for (; iter != right; iter++)
			if (*iter >= item)
				break;
		iterator result(list.insert(iter.list_iter, item));
		if (++n > 2 * r * vec.size())
			refresh();
		return result;
	}
    
    void erase(iterator itr){
        list.erase(itr.list_iter);
    }
    
	size_t size() const{
		return n;
	}

	bool empty() const{
		return size() == 0;
	}

	string str() const{
		return list.toString();
	}

	void refresh(size_t newRatio){
		r = newRatio;
		vec.clear();
		size_t count = r;
		for (iterator iter = begin(); iter != end(); iter++, count++)
			if (count == r)
			{
				count = 0;
				vec.insert(vec.end(), iter);
			}
	}

	void refresh(){
		refresh(r);
	}

    string vectoString() {
		ostringstream OUT;
		typename vector<iterator>::iterator itr = vec.begin();
		while (itr != vec.end()) {
			OUT << ">" << **itr << "< ";
			++itr;
		}
		return OUT.str();
	}
    
	string toString() const {
		ostringstream OUT;
		iterator itr = begin();
		while (itr != end()) {
			OUT << ">" << *itr << "< ";
			++itr;
		}
		return OUT.str();
	}
};

#endif
