/**
 * DSA Homework 1.3 in Innopolis University
 *
 * Author - Sinii Viacheslav
 */

/*
 * test case:
 * try to get element that is not in the table
 * try to add receipts all with the same date/id/zero cost
 */

#include <iostream>
#include <vector>
#include <optional>
#include <iomanip>
#include <exception>

using namespace std;

// a bunch of exceptions
namespace exceptions
{
    // this exception is thrown in case if user tries to reach element by an invalid index
    class IndexOutOfBoundsException : public exception
    {
        virtual const char* what() const throw()
        {
            return "Index out of bounds!";
        }
    };

    // this exception is thrown in case if a user tries to access elements in an empty list
    class NoElementsInTheListException : public exception
    {
        virtual const char* what() const throw()
        {
            return "The list is empty, dummy!";
        }
    };
}

// a list which contains only unique elements
template <typename T>
class UniqueItemsList
{
private:
    struct Node
    {
        T data;
        struct Node *next{};
    };

    // pointers on the first(dummy) and the last (for faster access to the greatest element) elements
    Node* first = nullptr;
    Node* last = nullptr;
    unsigned int listSize = 0;
public:
    // default constructor for UniqueItemsList()
    UniqueItemsList()
    {
        // make a fictitious first node
        // we assume that value of this node is always less then the value of the first item
        auto emptyNode = new Node();
        first = emptyNode;
    }

    // add a new (only unique) element to the List
    // O(n) - worst case
    void add(T item)
    {
        // if the list is empty we simply insert the item
        if (listSize == 0) // increase the size of the list
        {
            // create new node
            auto newNode = new Node();
            newNode->data = item;
            newNode->next = nullptr;
            // update pointers
            first->next = newNode;
            last = newNode;
            listSize++;
            return;
        }

        // go through each node
        auto currentNode = first;
        while (currentNode->next != nullptr)
        {
            currentNode = currentNode->next;
            // if similar element is already in the list - don't do add the new one
            if (currentNode->data == item) return;
        }

        // insert the item at the end
        auto newNode = new Node();
        *newNode = { item, currentNode->next };
        currentNode->next = newNode;
        last = newNode;
        // increase the size of the list
        listSize++;
    }

    // return an element at index i
    // O(n) - worst case
    T get(int i)
    {
        // if the list is empty - there is nothing to return
        if (listSize == 0) throw exceptions::NoElementsInTheListException();

        return getNode(i)->data;
    }

    // this function returns the size of the list
    // O(1) - worst case
    int size()
    {
        return listSize;
    }
private:
    // get the node at index i
    // O(n) - worst case
    struct Node* getNode(int i)
    {
        i++;
        // raise an exception if i is invalid
        if (i < 0 || i > listSize) throw exceptions::IndexOutOfBoundsException();

        // go to i'th element
        auto *currentNode = first;

        for (int j = 0; j < i; ++j) {
            currentNode = currentNode->next;
        }

        return currentNode;
    }
};

// abstract base class for all Maps
template <typename keyType, typename valueType>
class Map
{
    virtual std::optional<valueType*> get(keyType key) = 0;
    virtual void put(keyType key, valueType value) = 0;
};

// HashTable implementation of a Map
// uses coalesced hashing
template <typename keyType, typename valueType>
class HashTable : Map<keyType, valueType>
{
private:
    // struct which defines contents of each cell of the HashTable
    struct Item
    {
        keyType h_key;
        valueType data;
        Item* next = nullptr;
        bool empty = true;
    };

    vector<Item*> table;
    int tableSize = 50000;
    int nElementsInTable = 0;
public:
    // default constructor for HastTable
    HashTable()
    {
        table.reserve(tableSize);
        for (int i = 0; i < tableSize; ++i) {
            table.push_back(new Item());
        }
    }

    // constructor which takes into account how many elements there will be in the table
    explicit HashTable(int n)
    {
        tableSize = n;
        table.reserve(tableSize);
        for (int i = 0; i < tableSize; ++i) {
            table.push_back(new Item());
        }
    }

    // get the element with key 'key'
    // if there is no such key in the table - indicate it with nullopt
    // O(n) - worst case
    std::optional<valueType*> get(keyType key) {
        // take an element at index corresponding to the hash value of key
        auto current = table[hashFunction(key)];

        // go through each element until we find the one with the same key
        while (current->next != nullptr && current->h_key != key)
            current = current->next;

        // if key is indeed the same - return the value
        if (current->h_key == key) return &current->data;
        // if we just went to the end and didn't find the one with the same key -
        // indicate it with nullopt
        else return {};
    };

    // put an element to the table
    // O(n) - worst case
    void put(keyType key, valueType value)
    {
        // find an index for the element and place it there
        collisionHandler(key, value);
        // increase the amount of items in the table
        nElementsInTable++;
    }
public:
    // my hash function
    // O(1) - worst case
    int hashFunction(keyType key)
    {
        return compressionFunction(hashCode(key));
    }

    // this is the function for computing hash code of a key
    // it implements polynomial accumulation using Horner’s rule
    // O(1) - worst case
    int hashCode(keyType key)
    {
        int code = 0;
        if (typeid( keyType ) == typeid( string ))
        {
            // if the key is of type string
            // then the chars in that string would be the coefficients in the polynomial
            for (int i = 0; i < key.length(); ++i) {
                code = 7 * code + key[i];
            }
        }

        // abs in case of overflow
        return abs(code);
    }

    // this is the compression function
    // it implements MAD
    // O(1) - worst case
    int compressionFunction(int code)
    {
        return ((3 * code + 17) % 160009) % tableSize;
    }

    // coalesced hashing
    // O(n) - worst case
    void collisionHandler(keyType key, valueType value)
    {
        int hf = hashFunction(key);
        // if a cell is empty - simply insert new element there
        if (table[hf]->empty)
        {
            table[hf]->h_key = key;
            table[hf]->data = value;
            table[hf]->empty = false;
        }
        else // if a cell is not empty
        {
            for (int i = tableSize - 1; i >= 0 ; --i)
            {
                // find the first empty element starting from the end of the table
                if (table[i]->empty)
                {
                    // insert the element there
                    table[i]->h_key = key;
                    table[i]->data = value;
                    table[i]->empty = false;

                    // link this collided element to the ones with the same
                    // result of hashFunction for its keys
                    auto current = table[hf];

                    while (current->next != nullptr)
                        current = current->next;

                    current->next = table[i];

                    break;
                }
            }
        }
    }
};

// struct which defines contents of each receipt
struct receipt
{
    string date;
    string time;
    string id;
    double cost;
    string item_title;
};

// struct which defines all relevant information for each date
struct infoPerDate
{
    UniqueItemsList<string>* uniqueIds;
    double totalCost;
    vector<receipt> receipts;
};

// class which defines the work of cafe
class Cafe
{
private:
    // this list contains all unique dates in the input
    UniqueItemsList<string> dates;
    HashTable<string, infoPerDate> receipts_by_dates;
public:
    // function which splits the order into the fields of receipt structure
    void takeOrder()
    {
        // fill the receipt struct with the data from the input
        auto new_receipt = new receipt();
        cin >> new_receipt->date;
        dates.add(new_receipt->date);
        cin >> new_receipt->time;
        cin >> new_receipt->id;

        string s_cost;
        cin >> s_cost;
        new_receipt->cost = stod(s_cost.substr(1));

        getchar();
        getline(cin, new_receipt->item_title);

        // add that receipt to the table
        addReceipt(new_receipt);
    }

    // add new receipt to the table
    // the key is the date
    // the value is infoPerDate struct
    void addReceipt(const receipt* new_receipt)
    {
        auto res = receipts_by_dates.get(new_receipt->date);
        if ( res ) // if the element with the same date is already in the table
        {
            // update it's value
            (*res)->uniqueIds->add(new_receipt->id);
            (*res)->totalCost += new_receipt->cost;
            (*res)->receipts.push_back(*new_receipt);
        }
        else // if the element with this date hasn't been added to the table yet.
        {
            // make a new infoPerDate struct for that date
            auto list = new UniqueItemsList<string>();
            list->add(new_receipt->id);
            infoPerDate info = { list, new_receipt->cost, { *new_receipt } };
            // add it to the table
            receipts_by_dates.put(new_receipt->date, info);
        }
    }

    // show the information about each date -
    // number of unique id's and total cost of items
    void showDates()
    {
        for (int i = 0; i < dates.size(); ++i)
        {
            cout << dates.get(i) << " ";
            auto info = receipts_by_dates.get(dates.get(i));

            cout << fixed << setprecision(2)
                << "$" << (*info)->totalCost << " "
                << (*info)->uniqueIds->size() << endl;
        }

    }
};

int main() {
    int n;
    cin >> n;
    HashTable<string, int> hashTable(n);

    Cafe cafe;

    for (int i = 0; i < n; ++i) {
        cafe.takeOrder();
    }
    cafe.showDates();


    return 0;
}
