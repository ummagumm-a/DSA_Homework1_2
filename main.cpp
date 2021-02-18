/**
 * DSA Homework 1.3 in Innopolis University
 *
 * Author - Sinii Viacheslav
 */

#include <iostream>
#include <vector>
#include <bitset>
#include <memory>
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

    // this exception is thrown in case if there are no elements in a requested range
    class NoElementsInTheRange : public exception
    {
        virtual const char* what() const throw()
        {
            return "There are no elements in this range!";
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

    // this exception is thrown in case if a user tries to insert not the greatest element using addLast() method
    class InvalidUseOfAddLastException : public exception
    {
        virtual const char* what() const throw()
        {
            return "Don't use addLast() with this element!";
        }
    };
}

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
    // default constructor for LinkedSortedList
    UniqueItemsList()
    {
        // make a fictitious first node
        // we assume that value of this node is always less then the value of the first item
        auto emptyNode = new Node();
        first = emptyNode;
    }

    // add a new element to the List
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

        // go through each node until we find the one that is greater than the item
        auto currentNode = first;
        while (currentNode->next != nullptr)
        {
            currentNode = currentNode->next;
            // if similar element is already in the list - don't do anything
            if (currentNode->data == item) return;
        }

        // insert the item at the end
        auto newNode = new Node();
        *newNode = { item, currentNode->next };
        currentNode->next = newNode;
        last = newNode;
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

//template <typename T>
//class List
//{
//private:
//    struct Node
//    {
//        T data;
//        struct Node *next{};
//    };
//
//    // pointers on the first(dummy) and the last (for faster access to the greatest element) elements
//    Node* first = nullptr;
//    Node* last = nullptr;
//    unsigned int listSize = 0;
//public:
//    // default constructor for LinkedSortedList
//    List()
//    {
//        // make a fictitious first node
//        // we assume that value of this node is always less then the value of the first item
//        auto emptyNode = new Node();
//        first = emptyNode;
//    }
//
//    // add a new element to the List
//    // O(n) - worst case
//    void add(T item)
//    {
//        // if the list is empty we simply insert the item
//        if (listSize++ == 0) // increase the size of the list
//        {
//            // create new node
//            auto newNode = new Node();
//            newNode->data = item;
//            newNode->next = nullptr;
//            // update pointers
//            first->next = newNode;
//            last = newNode;
//            return;
//        }
//
//        // insert the item at the very end
//        auto newNode = new Node();
//        *newNode = { item, nullptr};
//        last->next = newNode;
//        last = newNode;
//
//    }
//
//    // return an element at index i
//    // O(n) - worst case
//    T get(int i)
//    {
//        // if the list is empty - there is nothing to return
//        if (listSize == 0) throw exceptions::NoElementsInTheListException();
//
//        return getNode(i)->data;
//    }
//
//    // this function returns the size of the list
//    // O(1) - worst case
//    int size()
//    {
//        return listSize;
//    }
//private:
//    // get the node at index i
//    // O(n) - worst case
//    struct Node* getNode(int i)
//    {
//        i++;
//        // raise an exception if i is invalid
//        if (i < 0 || i >= listSize) throw exceptions::IndexOutOfBoundsException();
//
//        // go to i'th element
//        auto *currentNode = first;
//
//        for (int j = 0; j < i; ++j) {
//            currentNode = currentNode->next;
//        }
//
//        return currentNode;
//    }
//};

template <typename keyType, typename valueType>
class Map
{
    virtual std::optional<valueType*> get(keyType key) = 0;
    virtual void put(keyType key, valueType value) = 0;
    virtual int size() = 0;
};

template <typename keyType, typename valueType>
class HashTable : Map<keyType, valueType>
{
private:
    struct Item
    {
        keyType h_key;
        bool empty = true;
        valueType data;
        Item* next = nullptr;
    };

    vector<Item*> table;
    int tableSize = 50000;
    int nElementsInTable = 0;
public:
    HashTable()
    {
        table.reserve(tableSize);
        for (int i = 0; i < tableSize; ++i) {
            table.push_back(new Item());
        }
    }

    explicit HashTable(int n)
    {
        tableSize = n;
        table.reserve(tableSize);
        for (int i = 0; i < tableSize; ++i) {
            table.push_back(new Item());
        }
    }

    std::optional<valueType*> get(keyType key) {
        auto current = table[hashFunction(key)];

        while (current->next != nullptr && current->h_key != key)
            current = current->next;

        if (current->h_key == key) return &current->data;
        else return {};
    };

    void put(keyType key, valueType value)
    {
        collisionHandler(key, value);
        nElementsInTable++;
    }

    int size()
    {
        return nElementsInTable;
    }
public:
    int hashFunction(keyType key)
    {
        return compressionFunction(hashCode(key));
    }

    // this is the function for computing hash code of a key
    // it implements polynomial accumulation using Horner’s rule
    int hashCode(keyType key)
    {
        int code = 0;
        if (typeid( keyType ) == typeid( string ))
        {
            for (int i = 0; i < key.length(); ++i) {
                code = 7 * code + key[i];
            }
        }

        return abs(code);
    }

    // this is the compression function
    // it implements MAD
    int compressionFunction(int code)
    {
        return ((3 * code + 17) % 160009) % tableSize;
    }

    // coalesced hashing
    void collisionHandler(keyType key, valueType value)
    {
        int hf = hashFunction(key);
        if (table[hf]->empty)
        {
            table[hf]->h_key = key;
            table[hf]->data = value;
            table[hf]->empty = false;
        }
        else
        {
            for (int i = tableSize - 1; i >= 0 ; --i)
            {
                if (table[i]->empty)
                {
                    table[i]->h_key = key;
                    table[i]->data = value;
                    table[i]->empty = false;
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

struct receipt
{
    string date;
    string time;
    string id;
    double cost;
    string item_title;

    void show()
    {
        cout << "Date: " << date << endl;
        cout << "Time: " << time << endl;
        cout << "ID: " << id << endl;
        cout << "cost: " << cost << endl;
        cout << "Item Title: " << item_title << endl;
    }
};

struct perDataInfo
{
    UniqueItemsList<string>* uniqueIds;
    double totalCost;
};

struct Cafe
{
private:
    UniqueItemsList<string> dates;
    HashTable<string, perDataInfo> receipts_by_dates;
public:
    void takeOrder()
    {
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

//        new_receipt->show();

        addReceipt(new_receipt);
    }

    void addReceipt(const receipt* new_receipt)
    {
        auto res = receipts_by_dates.get(new_receipt->date);
        if ( res )
        {
            (*res)->uniqueIds->add(new_receipt->id);
            (*res)->totalCost += new_receipt->cost;
        }
        else
        {
            auto list = new UniqueItemsList<string>();
            list->add(new_receipt->id);
            perDataInfo info = { list, new_receipt->cost };
            receipts_by_dates.put(new_receipt->date, info);
        }
    }

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
    int n = 5;
    cin >> n;
    HashTable<string, int> hashTable(n);

    Cafe cafe;

    for (int i = 0; i < n; ++i) {
        cafe.takeOrder();
    }
    cafe.showDates();

//    hashTable.put("a", new int(0));
//    hashTable.put("b", new int(1));
//    cout << *hashTable.get("b") << endl;
//    cout << *hashTable.get("c") << endl;


    return 0;
}
