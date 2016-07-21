#include <iostream>
#include <cstring>

using namespace std;


void printBook( struct Books *book );

struct Books {
    char title[50];
    char author[50];
    char subject[100];
    int book_id;
};

int main() {
    struct Books book1;
    struct Books book2;

    strcpy( book1.title ," Learn C++ Programming");
    strcpy( book1.author,"Bruce Lee");
    strcpy(book1.subject,"C++ Programming");
    book1.book_id = 1024;

    printBook(&book1);

    strcpy( book2.title ," Learn J2EE Programming");
    strcpy( book2.author,"Figo Xu");
    strcpy(book2.subject,"J2EE Programming");
    book2.book_id = 1024;
    printBook(&book2);

    cout << "Hello, World!" << endl;
    return 0;
}

void printBook(struct Books *book) {
    cout << "title:"<<book->title<<endl;
    cout << "author:"<<book->author<<endl;
    cout << "subject:"<<book->subject<<endl;
    cout <<"id:"<<book->book_id<<endl;
}