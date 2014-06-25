
/** @file ElementNotExist.h
 * Thrown when an required element does not exist
 * For example, iter.next(); while iter.hasNext() == false
 */

#ifndef __ELEMENTNOTEXIST_H
#define __ELEMENTNOTEXIST_H

#include <string>

class ElementNotExist {
public: ElementNotExist() {}
    ElementNotExist(std::string msg) : msg(msg) {}
    std::string getMessage() const { return msg; }
private:
    std::string msg;
}
;
#endif /* __ELEMENTNOTEXIST_H */
