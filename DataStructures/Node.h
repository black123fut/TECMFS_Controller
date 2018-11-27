#ifndef TECMFS_CONTROLLER_NODE_H
#define TECMFS_CONTROLLER_NODE_H


template <typename T>

class Node {

private:
    Node<T> *next;
    T data;
public:
    int x;
    int y;

    Node(T data){
        this->data = data;
        next = nullptr;
    }

    void setNext(Node<T> *next){
        this->next = next;
    }

    Node<T>* getNext(){
        return next;
    }

    T getData(){
        return data;
    }

    void setData(T data){
        this->data=data;
    }

    ~Node(){
        next = nullptr;
        data = nullptr;
    }

};



#endif //TECMFS_CONTROLLER_NODE_H
