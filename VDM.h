#pragma once
#include <string>
#include <iostream>
#include <stdexcept>
using namespace std;

// MyClasses.h

#pragma once

#include <string>

class VDMException : public exception{
private:
    std::string message;

public:
    VDMException(std::string msg) : message(msg) {}

    std::string& getMessage() {
        return message;
    }
};

// class InvariantCheck {
// public:
//     virtual bool inv() = 0;
// };


class VDM {
public:
    template <typename T>
    void invTest(T& sysObject) {
        if (!sysObject.inv()) {
            throw VDMException("VDMException Invariant Violation");
        }
    }

    void preTest(bool preCondition) {
        if (!preCondition) {
            throw VDMException("VDMException PreTest Violation");
        }
    }

    template <typename T>
    bool uniqueExists(T& sysObject, string Id) {
      return sysObject.isUnique(Id);
    }

};




// class VDMException {
//   private:
//     string message;

//   public:
//     VDMException(string msg);

//     string& getMessage();
// };

// class InvariantCheck {
//   public:
//     virtual bool inv() = 0;
// };

// class VDM {
//   public:
//     template <typename T>
//     void invTest(T& sysObject);
//     void preTest(bool preCondition);
// };