/**
 * @autor Wojciech Gruszka
 * @email wojciech@gruszka.eu
 *
 */
#include <unordered_set>
#include <iostream>
#include "PriorityList.hpp"

PriorityList::PriorityList(const PriorityList& pList) : PriorityList() {
    operator+=(pList);
}

PriorityList::PriorityList(std::initializer_list<long> pList) : PriorityList() {
    for(auto it = pList.begin();it != pList.end(); it++)
        insertRef(*it, mSize, 0);
}

void PriorityList::pushBack(long pVal) {
    insertRef(pVal, mSize, 0);
}

void PriorityList::pushFront(long pVal) {
    insertRef(pVal, 0, 1 + mHead->ref_cnt);
}

void PriorityList::insert(long pVal, int pIdx) {
    insertRef(pVal, pIdx, 0);
}

long PriorityList::getByIdx(int pIdx) {
    return operator[](pIdx);
}

long& PriorityList::operator[](int pIdx) {
    if(pIdx < 0 || pIdx >= mSize)
        throw std::out_of_range("List index out of range");

    auto it = begin();
    for(int i = 0; i != pIdx; it++, i++);

    it->ref_cnt++;
    sortNearNode(it.getNode());
    return *it;
}

int PriorityList::find(long pVal) {
    int i = 0;
    for(auto it = begin(); it != end(); it++, i++)
        if(*it == pVal) {
            it->ref_cnt++;
            sortNearNode(it.getNode());
            return i;
        }
    return -1;
}

void PriorityList::removeByIdx(int pIdx) {
    if(pIdx < 0 || pIdx >= mSize)
        throw std::out_of_range("List index out of range");
    auto it = begin();
    for(int i = 0; i != pIdx; it++, i++);
    removeElement(it.getNode());

}

void PriorityList::removeOneByValue(long pVal) {
    auto it = begin();
    for(; it != end() && *it != pVal; it++);
    if(it != end())
        removeElement(it.getNode());
}

void PriorityList::removeAllByValue(long pVal) {
    return removeByRange(pVal, pVal);
}

void PriorityList::removeByRange(long pLVal, long pRVal) {
    Node* node = mHead;
    while(node != nullptr) {
        if(node->data >= pLVal && node->data <= pRVal)
            node = removeElement(node);
        else
            node = node->next;
    }
}

void PriorityList::removeAll() {
    Node* node = mHead;
    while(node != nullptr)
        node = removeElement(node);
}

void PriorityList::removeDuplicates() {
    using Set = std::unordered_set<long>;
    Set values;
    Node* node = mHead;
    while (node != nullptr) {
        if(values.find(node->data) != values.end())
            node = removeElement(node);
        else{
            values.insert(node->data);
            node = node->next;
        }
    }
}

PriorityList& PriorityList::operator+=(const long pVal) {
    pushBack(pVal);
    return (*this);
}

PriorityList& PriorityList::operator-=(const long pVal) {
    removeOneByValue(pVal);
    return (*this);
}

PriorityList::iterator PriorityList::begin() const {
  return PriorityList::iterator(mHead);
}

PriorityList::iterator PriorityList::end() const {
    if(mTail != nullptr)
        return PriorityList::iterator(mTail->next);
    return PriorityList::iterator(nullptr);
}

PriorityList::constIterator PriorityList::cBegin() const {
  return PriorityList::constIterator(mHead);
}

PriorityList::constIterator PriorityList::cEnd() const {
    if(mTail != nullptr)
        return PriorityList::constIterator(mTail->next);
    return PriorityList::constIterator(nullptr);
}

int PriorityList::length() const {
  return this->mSize;
}

PriorityList PriorityList::operator+(const PriorityList& rhs) const {
    PriorityList res(*this);
    res.operator+=(rhs);
    return res;
}

PriorityList& PriorityList::operator+=(const PriorityList& rhs) {
    for(auto it = rhs.cBegin(); it != rhs.cEnd(); it++) {
            insertRef(it->data, mSize, it->ref_cnt);
    }
    return *this;
}

PriorityList PriorityList::operator-(const PriorityList& rhs) const {
    PriorityList res(*this);
    res.operator-=(rhs);
    return res;
}

PriorityList& PriorityList::operator-=(const PriorityList& rhs) {
    for(auto it = rhs.cBegin(); it != rhs.cEnd(); it++)
        removeOneByValue(*it);
    return *this;
}

PriorityList& PriorityList::operator=(const PriorityList& rhs) {
    removeAll();
    operator+=(rhs);
    return *this;
}

bool PriorityList::operator==(const PriorityList& rhs) const {
  if(mSize != rhs.length())
    return false;

    auto li = cBegin();
    auto ri = rhs.cBegin();
    for(;li != cEnd() && ri != rhs.cEnd(); ++li, ++ri)
          if(li->data != ri->data || li->ref_cnt != ri->ref_cnt)
            return false;

    if(li == cEnd() && ri == rhs.cEnd())
      return true;
    return false;
 }

 bool PriorityList::operator!=(const PriorityList& rhs) const {
     return !(operator==(rhs));
 }

 void PriorityList::insertRef(long pData, int pIdx, unsigned int pRefCnt) {
     Node* node = new Node(pData, pRefCnt);
     if(mSize == 0) {
         mHead = node;
         mTail = node;
     }
     else if(pIdx == 0) {
         node->next = mHead;
         mHead->prev = node;
         mHead = node;
     } else if(pIdx >= mSize) {
         node->prev = mTail;
         mTail->next = node;
         mTail = node;
     } else {
         auto it = begin();
         for(int i = 0; i != pIdx; ++i, ++it);

         node->next = it.getNode();
         node->prev = it->prev;
         it->prev->next = node;
         it->prev = node;
     }
     mSize++;
     sortNearNode(node);
 }


void PriorityList::sortNearNode(PriorityList::Node *pNode) {
    if(pNode == nullptr)
        return;

    Node* ptr = pNode->prev;

    if(ptr == nullptr) //if pNode is at the beggingng of the list
        return;

    while(ptr != nullptr && (*ptr)<(*pNode))
        ptr = ptr->prev;

    if(ptr == pNode->prev) // No need to sort
        return;

    if(pNode->next != nullptr){
        pNode->next->prev = pNode->prev;
        pNode->prev->next = pNode->next;
    } else {
        pNode->prev->next = nullptr;
        mTail = pNode->prev;
    }

    if(ptr == nullptr) { //have to move pNode to the begining of the list
        pNode->next = mHead;
        pNode->prev = nullptr;
        mHead->prev = pNode;
        mHead = pNode;
    } else { //have to move pNode right after ptr
        pNode->next = ptr->next;
        pNode->prev = ptr;
        ptr->next->prev = pNode;
        ptr->next = pNode;
    }

}

PriorityList::Node* PriorityList::removeElement(PriorityList::Node* pNode) {
    if(pNode == nullptr)
        return nullptr;
    if(pNode->next == nullptr) {
         if(pNode->prev == nullptr) {
            mHead = nullptr;
            mTail = nullptr;
        } else {
            mTail = pNode->prev;
            pNode->prev->next = nullptr;
         }
     } else if (pNode->prev == nullptr) {
        mHead = pNode->next;
        pNode->next->prev = nullptr;
     }else {
        pNode->prev->next = pNode->next;
        pNode->next->prev = pNode->prev;
     }
     mSize--;

     Node* next = pNode->next;
     delete pNode;
     return next;
 }

std::ostream& operator<<(std::ostream& out, PriorityList& mList)
{
    for(auto it = mList.cBegin(); it != mList.cEnd(); it++)
    {
        out << it->data << "(" << it->ref_cnt << "), ";
        //out << it->data << "(" << it->ref_cnt << ")["<< it.getNode() << ", " << it->prev << ", " << it->next << "], ";
    }
    return out;
}
