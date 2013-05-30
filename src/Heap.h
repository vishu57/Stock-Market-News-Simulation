/* 
 * Gagandip  Singh 
 * CSE 250 project 2 - 5/2/2013
*/
#include <vector>
using std::vector;

template <typename T>
class Heap
{
public:
    vector <T> item;

    Heap()
        :item(vector<T>(1)){}

    int size() const
    {
        return (int) item.size() - 1; //-1 b/c 1st item of the vector is a dummy node
    }


    void fix_down(int index)
    {
        T temp = item[index];
        while (2*index <= size() )//while index has at least one child then intinally we take the index of that child
        {
            int child = 2*index;
            if (child < size() && item[child+1] > item[child]){
                child++;
            }
            if (temp >= item[child]){
                break;
            }
            item[index] = item[child];
            index = child;
        }
        item[index] = temp;
    }

    void make_heap()
    {
        for (int index = size()/2; index >= 1; index--) //start at last_parent
        {
            fix_down(index);
        }
    }

    T pop()
    {
        T temp = item[1];
        item[1] = item[size()];
        item.pop_back();
        if (size() > 0){
            fix_down(1);
        }
        return temp;
    }

    void push(T new_item)
    {
        item.push_back(new_item);
        //TODO: fix up
    }

};

