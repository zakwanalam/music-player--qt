#ifndef QUEUE_H
#define QUEUE_H

#include <QObject>
#include <QString>

struct Node {
    QString data;
    Node *next;
};

class Queue : public QObject {
    Q_OBJECT

public:
    explicit Queue(QObject *parent = nullptr);
    ~Queue();

    bool isEmpty() const;
    void enqueue(const QString &value);
    void dequeue();
    QString front() const;

private:
    Node *frontNode;
    Node *rearNode;
};

#endif // QUEUE_H
