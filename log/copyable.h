#ifndef COPYABLE_H
#define COPYABLE_H

/*
说明：此类是一个空类，就是为了表明继承此类的子类是可以复制的
*/

class copyable {
protected:
    copyable()=default;
    ~copyable()=default;
};


#endif