/* 
 * Gagandip  Singh 
 * CSE 250 project 2 - 5/2/2013
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> 

#include "ChainedHash.h"
#include "Stock.h"
#include "Heap.h"
#include "StringExtra.h"

#include "HiResTimer.h"
#include <iomanip>

using namespace std;



//--------------------------Command Strings----------------------------------
//Data lines beginning with these tokens are taken as commands not data.
//Extra arguments expected in command-file commented at right after "+".

const string ADD_TOKEN = "add?";                     //+data for stock to add
const string TOP_VOLUME_TOKEN = "printTopByVolume?"; //+int k
const string TOP_PERCENT_CHANGE_TOKEN =              //absolute change
"printTopByPercentChange?";                          //+int k
const string TOP_PERCENT_UP_TOKEN   = "printTopByPercentUp?";   //+int k
const string TOP_PERCENT_DOWN_TOKEN = "printTopByPercentDown?"; //+int k
const string TOP_MOMENTUM_TOKEN     = "printTopByMomentum?";    //+int k
const string TOP_TREND_TRADES_TOKEN = "printTopByTrendTrades?"; //+int k
//extra credit
//const string TOP_TREND_SHARES_TOKEN = "printTopByTrendShares?";  //+int k

/** Print all stocks, sorted by ticker symbol
 */
const string PRINT_ALL_TOKEN = "printAll?";          //no extra argument

/** Pause run (& print out to screen) to set watch for timing next ops.
 Will be required on final timing runs.
 */
const string PAUSE_TOKEN = "pause?";

/** Print out a message just to help see where in the data file you are.
 */
const string TYPEOUT_TOKEN = "typeout?";


//-------------Optional command strings may help you debug--------------

/** Print out a few specified stocks, for debug help to see if they
 updated correctly.  Not required in the final runs---"printAll?" will
 take care of that.
 */
const string QUOTE_THESE_TOKEN = "quoteThese?";      //+list-of-stocks

/** Print out data structure underlying masters, for debugging */
const string PRINT_DS_TOKEN = "printDS?";

/** Version of "typeout" which you could turn off with a Boolean flag,
 analogous to if you had a Boolean flag to skip all "cerr << ___"
 debugging statements in other code, and to how C++ has a global
 option to turn off all "assert(...)" checks (which we didn't cover).
 */
const string WHISPER_TOKEN = "whisper?";

// End of required and suggested tokens---you're free to add others...



//------------------------------------------------------------------------

int main(int argc, char** argv) 
{
    //Use command-line arguments to get data-file name, much as before
    //but without the extra "n" parameter, since now we will always
    //read the whole data file.  Once you've read in a fileName:

    if (argc == 1 || argc > 2) //argc must == 2 (1 for progname, 1 for input-file)
    {
        cerr << "Usage: " << argv[0] << " input-file" << endl;
        return 0;
    }

    string filename = argv[1];

    ifstream IN(filename.c_str(), ios::in); //NB: this syntax can vary a little
    // ofstream outf("output.txt");

    // if (!outf)
    // {
    //     cout << "failed to write to output.txt" << endl;
    //     exit(EXIT_FAILURE);
    // }
    if (!IN)
    {
        cout << "could not find " << filename << endl;
        exit(EXIT_FAILURE);
    }
    //ENS: top two if's ensure an infile and outfile are available

    //-----------------------------------------------------------------------------------------------------

    //one chained-hashtable of stock and six different heaps of six different proxy types
    MorphedValli<Stock> table;
    Heap<Proxy_byVolume> heap_byVolume;
    Heap<Proxy_byPercentUp> heap_byPercentUp;
    Heap<Proxy_byPercentDown> heap_byPercentDown;
    Heap<Proxy_byPercentChange> heap_byPercentChange;
    Heap<Proxy_byMomentum> heap_byMomentum;
    Heap<Proxy_byTrend> heap_byTrend;

    //-----------------------------------------------------------------------------------------------------    

    //ofstream OUT...      //to save results in a named output file
    //ostream OUT = cout;  //print results to screen

    string result = ""; //save output so it only prints when asked, e.g.
    //when you have a "pause?" line.
    int k;  //number of top stocks to be fetched, read on each line.

    //--------- timing code ---------
    double et;
    HiResTimer timer;
    timer.reset();
    //--------- timing code ---------

    string line; 
    while (getline(IN, line))
    {
        line = trim(line);   //forgives whitespace before command token
        
        if (startsWith(line,ADD_TOKEN))
        {
            string stockData = trim(line.substr(ADD_TOKEN.length()));
            int i = (int) stockData.find(' ');
            string stockName = stockData.substr(0, i);
            int sloc = (int) stockData.find('s');
            string totNumOfSharesStr = stockData.substr((i+1), sloc);
            int totalNumOfShares = atoi(totNumOfSharesStr);
            double pricePerShare = atof(stockData.substr(sloc+1, stockData.length()));
            //cout << "stock is: " << stockName << " with " << totalNumOfShares << " shares. price per share is: " << pricePerShare << endl;
            //Etc.:---you might want to parse the inserts & trades in other ways
            
            /* If your client reads a line of the form "add? NCF 1000000s100",
             it should do the following: It should create a stock object and
             insert it into the chained-hashtable. It should also create six
             different proxy objects and insert them into the six different
             heaps, like this: 
            */
            
            Stock stock(stockName, totalNumOfShares, pricePerShare);
            Stock* stock_ptr = &*table.insert(stock);
            // "insert" returns an interator.
            // "*" dereferences that iterator and returns a stock.
            // "&" returns the address of that stock.
            heap_byVolume.push(stock_ptr);
            heap_byPercentUp.push(stock_ptr);
            heap_byPercentDown.push(stock_ptr);
            heap_byPercentChange.push(stock_ptr);
            heap_byMomentum.push(stock_ptr);
            heap_byTrend.push(stock_ptr);
            
        }
        else if (startsWith(line,QUOTE_THESE_TOKEN))
        {
            //optional---mainly as a debugging help
            
        }
        else if ( startsWith(line, TOP_VOLUME_TOKEN)
                   || startsWith(line, TOP_PERCENT_CHANGE_TOKEN)
                   || startsWith(line, TOP_PERCENT_UP_TOKEN)
                   || startsWith(line, TOP_PERCENT_DOWN_TOKEN)
                   || startsWith(line, TOP_MOMENTUM_TOKEN)
                   || startsWith(line, TOP_TREND_TRADES_TOKEN) )
        {
            //extract k first---we assume at least one space precedes it.
            int i = (int) line.find(' '); //== whitespace after command token
            
            //see code in "StringExtra.h"---note "trim" trims whitespaces only
            k = atoi(trim(line.substr(i+1)));
            
            //now get requested "Top k" list:
            if (startsWith(line, TOP_VOLUME_TOKEN))
            {
                /*If your client reads a line "PrintTopByVolume 5", it should
                 do the following three steps: First, it should make the heap
                 "heap_byVolume". Second, it should pop out 5 items of that
                 heap and print them. Third, it should push those 5 items
                 back into the heap: */
                
                heap_byVolume.make_heap();
                vector<Proxy_byVolume> top;
                cout << "-- Top By Volune --" << endl; 
                for (size_t i = 0; i < k && heap_byVolume.size() > 0; i++)
                {
                    top.push_back(heap_byVolume.pop());
                    const Stock& stock = *top[top.size() - 1];
                    // "top[top.size() - 1]" is a proxy.
                    // "*" dereferences that proxy and returns a stock.
                    
                    // Print stock.
                    cout << i << ". " <<  stock.name << ", " << stock.capital << endl;
                }
                for (size_t i = 0; i < top.size(); i++){
                    heap_byVolume.push(top[i]);
                }
            }
            else if (startsWith(line, TOP_PERCENT_CHANGE_TOKEN))
            {
                heap_byPercentChange.make_heap();
                vector<Proxy_byPercentChange> top;
                cout << "-- Top By Percent Change --" << endl;
                for (size_t i = 0; i < k && heap_byPercentChange.size() > 0; i++)
                {
                    top.push_back(heap_byPercentChange.pop());
                    const Stock& stock = *top[top.size() - 1];
                    // "top[top.size() - 1]" is a proxy.
                    // "*" dereferences that proxy and returns a stock.
                    
                    // Print stock.
                    cout << i << ". " <<  stock.name << ", " << stock.PercentChange() << endl;
                }
                for (size_t i = 0; i < top.size(); i++){
                    heap_byPercentChange.push(top[i]);
                }
                
            }
            else if (startsWith(line, TOP_PERCENT_UP_TOKEN))
            {
                heap_byPercentUp.make_heap();
                vector<Proxy_byPercentUp> top;
                cout << "-- Top By Percent Up --" << endl;
                for (size_t i = 0; i < k && heap_byPercentUp.size() > 0; i++)
                {
                    top.push_back(heap_byPercentUp.pop());
                    const Stock& stock = *top[top.size() - 1];
                    // "top[top.size() - 1]" is a proxy.
                    // "*" dereferences that proxy and returns a stock.
                    
                    // Print stock.
                    cout << i << ". " <<  stock.name << ", " << stock.PercentUp() << endl;
                }
                for (size_t i = 0; i < top.size(); i++){
                    heap_byPercentUp.push(top[i]);
                }
            }
            else if (startsWith(line, TOP_PERCENT_DOWN_TOKEN))
            {
                heap_byPercentDown.make_heap();
                vector<Proxy_byPercentDown> top;
                cout << "-- Top By Percent Down --" << endl;
                for (size_t i = 0; i < k && heap_byPercentDown.size() > 0; i++)
                {
                    top.push_back(heap_byPercentDown.pop());
                    const Stock& stock = *top[top.size() - 1];
                    // "top[top.size() - 1]" is a proxy.
                    // "*" dereferences that proxy and returns a stock.
                    
                    // Print stock.
                    cout << i << ". " <<  stock.name << ", " << stock.PercentDown() << endl;
                }
                for (size_t i = 0; i < top.size(); i++){
                    heap_byPercentDown.push(top[i]);
                }
            }
            else if (startsWith(line, TOP_MOMENTUM_TOKEN)){
                heap_byMomentum.make_heap();
                vector<Proxy_byMomentum> top;
                cout << "-- Top By Momentem --" << endl;
                for (size_t i = 0; i < k && heap_byMomentum.size() > 0; i++)
                {
                    top.push_back(heap_byMomentum.pop());
                    const Stock& stock = *top[top.size() - 1];
                    // "top[top.size() - 1]" is a proxy.
                    // "*" dereferences that proxy and returns a stock.
                    
                    // Print stock.
                    cout << i << ". " <<  stock.name << ", " << stock.Momentum() << endl;
                }
                for (size_t i = 0; i < top.size(); i++){
                    heap_byMomentum.push(top[i]);
                }
            }
            else if (startsWith(line, TOP_TREND_TRADES_TOKEN))
            {
                heap_byTrend.make_heap();
                vector<Proxy_byTrend> top;
                cout << "-- Top By Momentem --" << endl;
                for (size_t i = 0; i < k && heap_byTrend.size() > 0; i++)
                {
                    top.push_back(heap_byTrend.pop());
                    const Stock& stock = *top[top.size() - 1];
                    // "top[top.size() - 1]" is a proxy.
                    // "*" dereferences that proxy and returns a stock.
                    
                    // Print stock.
                    cout << i << ". " <<  stock.name << ", " << stock.Trend() << endl;
                }
                for (size_t i = 0; i < top.size(); i++){
                    heap_byTrend.push(top[i]);
                }
            }
            else
            {
                //no-op, impossible case; just here to make adding cases easier
            }
        
        }
        else if (startsWith(line, PRINT_ALL_TOKEN))
        {
            /*
            [Code to iterate, sort, and print in ticker-symbol sorted order.]
            [In Spring 2000 we had to work around a Sun CC compiler bug by adding
             interaction, e.g. cout << "Enter y to go on..."; getline(cin,...);
             Many of you do this on your home systems already in order to stop
             a window from closing before you see the output.  Our grading
            scripts require no-interaction, as with CSE250 all last year.]
             */
            
        }
        else if (startsWith(line, PAUSE_TOKEN))
        {
            /*
            [For the moment, this CAN be interactive---which may help you debug.
             For the final 12/4 submission you must later we will ask you
             to remove interaction lines here as well.]
            */
            
            // REST deals with the debugging tokens, i.e. OPTIONAL, recommended...
            
        }
        else if (startsWith(line, PRINT_DS_TOKEN))
        {
            /*...
            ... [you can call your hash table's own str() method---maybe the
                 one from your Valli will work without change!  This might help
                 more for debugging than just printing items a-la printAll?] */
            
        }
        else if (startsWith(line, TYPEOUT_TOKEN))
        {
            //...[print the rest of the line after the token]...
        }
        else if (startsWith(line, WHISPER_TOKEN))
        {
            //...[print if a certain bool field is switched on]...
        }
        else if (line == "")
        {
            //can ignore, or maybe output a note about reading a blank line
        }
        else //!! Line is a stock-transaction line, so update stock info
        {
            int i = (int) line.find(' '); ///assume whitespace after ticker symbol
            string stockSymbol = line.substr(0, i);
            
            int sloc = (int) line.find('s');
            string totNumOfSharesStr = line.substr((i+1), sloc);
            int totalNumOfShares = atoi(totNumOfSharesStr);
            double pricePerShare = atof(line.substr(sloc+1, line.length()));

            
            /* [Etc.: hash it, process the trade, update fields as-needed.
            Reminder---you need not do any heap fix-ups immediately---wait
            until the client hits a "printTopStocksBy... k" request to reheapify.
            Figuratively, the /less/ you need to do after a trade, the better!]
             */
            
            /* If your client reads a line of the form "NCF 1000s150" 
             (without "add?"), it should look for the stock "NCF" in 
             the chained-hashtable, and call process_trade on that stock. 
             Notice that the heaps don't need to be involved: 
             */
                
            Stock* stock_ptr = &*table.find(stockSymbol);
            // "find" returns an interator.
            // "*" dereferences that iterator and returns a stock.
            // "&" returns the address of that stock.
            stock_ptr->process_trade(totalNumOfShares, pricePerShare);
            
        } //end of "if(startsWith(...))..."
        //Note: the while-loop test itself reads a line as a side-effect.
    }//end of outer while loop.  POST: EOF reached, so we're done.


    //--------- timing code ---------
    et = timer.elapsedTime();
    cout << fixed << setprecision(6) << "Time to process: "  << et << endl;
    //--------- timing code ---------

    cout << endl << "done." << endl;
    IN.close();

    return (0);
}  //end of main.




/* TIMING 

-- Chained Hash -- 
593717.114000
580311.093000
382078.210000
611363.324000
577891.117000
558888.317000
588612.037000
587584.965000
556573.007000
560698.352000
AVERAGE: 559,771.7536

-- Valli -- 
773383.334000
896898.214000
921933.444000
891646.205000
792990.160000
792258.414000
926751.180000
859849.352000
791610.626000
870519.657000
AVERAGE: 851,784.0586

The chained hash was 292,012.305 units faster 
in processing allstocks100k-30.data than the 
Valli data structure. 

Quickest run on Chained Hash: 382,078.210000
Slowest run on Chained Hash: 611,363.324000

Quickest run on Valli: 773,383.334000
Slowest run on Valli: 921,933.444000

There was some slight variation in the run times,
but nothing too alarming from our perspective.

You can definitely tell that the Chained Hash performs
better in this case. The Valli's quickest run could not
outperform Chained Hash's slowest. The quickest run
under Chained Hash was almost 1/3 of the average runtime for
Valli.

*/






















