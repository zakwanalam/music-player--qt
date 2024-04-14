#include "Queue.h"

Queue::Queue(QObject *parent) : QObject(parent), frontNode(nullptr), rearNode(nullptr) {}

Queue::~Queue() {
    while (!isEmpty()) {
        dequeue();
    }
}

bool Queue::isEmpty() const {
    return frontNode == nullptr;
}

void Queue::enqueue(const QString &value) {
    Node *newNode = new Node;
    newNode->data = value;
    newNode->next = nullptr;

    if (isEmpty()) {
        frontNode = newNode;
        rearNode = newNode;
    } else {
        rearNode->next = newNode;
        rearNode = newNode;
    }
}

void Queue::dequeue() {
    if (isEmpty()) {
        return;
    }

    Node *temp = frontNode;
    frontNode = frontNode->next;

    if (frontNode == nullptr) {
        rearNode = nullptr;
    }

    delete temp;
}

QString Queue::front() const {
    if (isEmpty()) {
        // Handle empty queue error, you can throw an exception or return an empty string
        return QString(); // Return an empty string
    }

    return frontNode->data;
}
