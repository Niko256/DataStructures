#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <chrono>
#include "../include/WeakPtr.hpp"
#include "../include/SharedPtr.hpp"
#include "../include/UniquePtr.hpp"

template<typename T>
class Resource {
public:
    Resource(const std::string& name, const T& value, bool silent = false) : name_(name), value_(value), silent_(silent) {
        if (!silent_) {
            std::cout << "\nResource " << name_ << " created with value ";
            printValue(value_);
            std::cout << std::endl;
        }
    }

    ~Resource() {
        if (!silent_) {
            std::cout << "\nResource " << name_ << " destroyed" << std::endl;
        }
    }

    void print() const {
        std::cout << "Resource " << name_ << " has value ";
        printValue(value_);
        std::cout << std::endl;
    }

private:
    std::string name_;
    T value_;
    bool silent_;

    template<typename U>
    void printValue(const U& value) const {
        std::cout << value;
    }

    void printValue(const std::vector<int>& value) const {
        for (int v : value) {
            std::cout << v << " ";
        }
    }
};

template<typename T>
class SmartPointerManagerBase {
public:
    using SharedPtrVector = std::vector<SharedPtr<Resource<T>>>;
    using UniquePtrVector = std::vector<UniquePtr<Resource<T>>>;
    using WeakPtrVector = std::vector<WeakPtr<Resource<T>>>;

    size_t getSharedPtrsSize() const {
        return sharedPtrs.size();
    }

    void createSharedPtr() {
        std::string name;
        std::cout << "\nEnter resource name: ";
        std::cin >> name;
        clearInputBuffer();

        T value = createResourceValue<T>();
        sharedPtrs.push_back(SharedPtr<Resource<T>>(new Resource<T>(name, value)));
    }

    void createSharedPtrFromExisting() {
        if (sharedPtrs.empty()) {
            std::cout << "\nNo existing SharedPtrs\n";
            return;
        }

        std::cout << "\nEnter index of existing SharedPtr (0-" << sharedPtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < sharedPtrs.size()) {
            sharedPtrs.push_back(sharedPtrs[index]);
            std::cout << "\nSharedPtr created to existing resource\n";
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void getResource() {
        if (sharedPtrs.empty()) {
            std::cout << "\nNo SharedPtrs\n";
            return;
        }

        std::cout << "\nEnter index of SharedPtr (0-" << sharedPtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < sharedPtrs.size()) {
            printResource(sharedPtrs[index]);
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void getUseCount() {
        if (sharedPtrs.empty()) {
            std::cout << "No SharedPtrs\n";
            return;
        }

        std::cout << "\nEnter index of SharedPtr (0-" << sharedPtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < sharedPtrs.size()) {
            std::cout << "\nUse count: " << sharedPtrs[index].use_count() << std::endl;
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void resetSharedPtr() {
        if (sharedPtrs.empty()) {
            std::cout << "No SharedPtrs\n";
            return;
        }

        std::cout << "\nEnter index of SharedPtr to reset (0-" << sharedPtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < sharedPtrs.size()) {
            sharedPtrs[index].reset();
            std::cout << "\nSharedPtr reset\n";
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void listAllSharedPtrs() {
        if (sharedPtrs.empty()) {
            std::cout << "No SharedPtrs\n";
            return;
        }

        for (size_t i = 0; i < sharedPtrs.size(); ++i) {
            std::cout << "\nSharedPtr " << i << ": ";
            printResource(sharedPtrs[i]);
        }
    }

    void removeSharedPtr() {
        if (sharedPtrs.empty()) {
            std::cout << "No SharedPtrs\n";
            return;
        }

        std::cout << "\nEnter index of SharedPtr to remove (0-" << sharedPtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < sharedPtrs.size()) {
            sharedPtrs.erase(sharedPtrs.begin() + index);
            std::cout << "\nSharedPtr removed\n";
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void createMultipleSharedPtrs(int count) {
        T value = createResourceValue<T>();

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < count; ++i) {
            sharedPtrs.push_back(SharedPtr<Resource<T>>(new Resource<T>("MultiShared", value, true)));
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "\nTime to create " << count << " SharedPtrs: " << elapsed.count() << " s\n";

        std::vector<std::shared_ptr<Resource<T>>> stlSharedPtrs;
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < count; ++i) {
            stlSharedPtrs.push_back(std::make_shared<Resource<T>>("MultiSTLShared", value, true));
        }
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "\nTime to create " << count << " std::shared_ptrs: " << elapsed.count() << " s\n";
    }

    void removeCreatedSharedPtrs(size_t originalSize) {
        sharedPtrs.resize(originalSize);
    }

    void createUniquePtr() {
        std::string name;
        std::cout << "\nEnter resource name: ";
        std::cin >> name;
        clearInputBuffer();

        T value = createResourceValue<T>();
        uniquePtrs.push_back(UniquePtr<Resource<T>>(new Resource<T>(name, value)));
    }

    void getUniquePtrResource() {
        if (uniquePtrs.empty()) {
            std::cout << "\nNo UniquePtrs\n";
            return;
        }

        std::cout << "\nEnter index of UniquePtr (0-" << uniquePtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < uniquePtrs.size()) {
            uniquePtrs[index]->print();
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void releaseUniquePtr() {
        if (uniquePtrs.empty()) {
            std::cout << "\nNo UniquePtrs\n";
            return;
        }

        std::cout << "\nEnter index of UniquePtr to release (0-" << uniquePtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < uniquePtrs.size()) {
            uniquePtrs[index].release();
            std::cout << "\nResource released\n";
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void resetUniquePtr() {
        if (uniquePtrs.empty()) {
            std::cout << "\nNo UniquePtrs\n";
            return;
        }

        std::cout << "\nEnter index of UniquePtr to reset (0-" << uniquePtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < uniquePtrs.size()) {
            uniquePtrs[index].reset();
            std::cout << "\nUniquePtr reset\n";
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void createMultipleUniquePtrs(int count) {
        T value = createResourceValue<T>();

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < count; ++i) {
            uniquePtrs.push_back(UniquePtr<Resource<T>>(new Resource<T>("MultiUnique", value, true)));
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "\nTime to create " << count << " UniquePtrs: " << elapsed.count() << " s\n";

        std::vector<std::unique_ptr<Resource<T>>> stlUniquePtrs;
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < count; ++i) {
            stlUniquePtrs.push_back(std::make_unique<Resource<T>>("MultiSTLUnique", value, true));
        }
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "\nTime to create " << count << " std::unique_ptrs: " << elapsed.count() << " s\n";
    }

    void createWeakPtr() {
        if (sharedPtrs.empty()) {
            std::cout << "\nNo existing SharedPtrs\n";
            return;
        }

        std::cout << "\nEnter index of existing SharedPtr (0-" << sharedPtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < sharedPtrs.size()) {
            weakPtrs.push_back(sharedPtrs[index]);
            std::cout << "\nWeakPtr created from SharedPtr\n";
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void lockWeakPtr() {
        if (weakPtrs.empty()) {
            std::cout << "\nNo WeakPtrs\n";
            return;
        }

        std::cout << "\nEnter index of WeakPtr (0-" << weakPtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < weakPtrs.size()) {
            SharedPtr<Resource<T>> lockedPtr = weakPtrs[index].lock();
            if (lockedPtr) {
                lockedPtr->print();
            } else {
                std::cout << "\nWeakPtr expired\n";
            }
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void getWeakPtrUseCount() {
        if (weakPtrs.empty()) {
            std::cout << "No WeakPtrs\n";
            return;
        }

        std::cout << "\nEnter index of WeakPtr (0-" << weakPtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < weakPtrs.size()) {
            std::cout << "\nUse count: " << weakPtrs[index].use_count() << std::endl;
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void checkWeakPtrExpired() {
        if (weakPtrs.empty()) {
            std::cout << "No WeakPtrs\n";
            return;
        }

        std::cout << "\nEnter index of WeakPtr (0-" << weakPtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < weakPtrs.size()) {
            std::cout << "\nExpired: " << (weakPtrs[index].expired() ? "Yes" : "No") << std::endl;
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void createMultipleWeakPtrs(int count) {
        if (sharedPtrs.empty()) {
            std::cout << "\nNo existing SharedPtrs\n";
            return;
        }

        std::cout << "\nEnter index of existing SharedPtr (0-" << sharedPtrs.size() - 1 << "): ";
        int index;
        std::cin >> index;
        clearInputBuffer();

        if (index >= 0 && index < sharedPtrs.size()) {
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < count; ++i) {
                weakPtrs.push_back(sharedPtrs[index]);
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "\nTime to create " << count << " WeakPtrs: " << elapsed.count() << " s\n";

            std::weak_ptr<Resource<T>> stlWeakPtr;
            start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < count; ++i) {
                stlWeakPtr = std::make_shared<Resource<T>>("MultiSTLShared", T(), true);
            }
            end = std::chrono::high_resolution_clock::now();
            elapsed = end - start;
            std::cout << "\nTime to create " << count << " std::weak_ptrs: " << elapsed.count() << " s\n";
        } else {
            std::cout << "\nInvalid index\n";
        }
    }

    void clearInputBuffer() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    SharedPtrVector sharedPtrs;

    template<typename U>
    U createResourceValue() {
        if constexpr (std::is_same_v<U, std::vector<int>>) {
            std::vector<int> value;
            std::cout << "\nEnter resource value (comma-separated integers): ";
            std::string input;
            std::cin >> input;
            clearInputBuffer();
            std::stringstream ss(input);
            int val;
            while (ss >> val) {
                value.push_back(val);
                if (ss.peek() == ',') ss.ignore();
            }
            return value;
        } else {
            U value;
            std::cout << "\nEnter resource value: ";
            std::cin >> value;
            clearInputBuffer();
            return value;
        }
    }

private:
    UniquePtrVector uniquePtrs;
    WeakPtrVector weakPtrs;

    void printResource(const SharedPtr<Resource<T>>& ptr) {
        if (ptr) {
            ptr->print();
        } else {
            std::cout << "\nNo resource managed by SharedPtr\n";
        }
    }
};




template<typename T>
class SmartPointerManager : public SmartPointerManagerBase<T> {};

class SmartPointerManagerInt : public SmartPointerManager<int> {};
class SmartPointerManagerString : public SmartPointerManager<std::string> {};
class SmartPointerManagerVectorInt : public SmartPointerManager<std::vector<int>> {};
class SmartPointerManagerChar : public SmartPointerManager<char> {};
class SmartPointerManagerBool : public SmartPointerManager<bool> {};
class SmartPointerManagerDouble : public SmartPointerManager<double> {};



void sharedPtrMenu() {
    SmartPointerManagerInt managerInt;
    SmartPointerManagerString managerString;
    SmartPointerManagerVectorInt managerVectorInt;
    SmartPointerManagerChar managerChar;
    SmartPointerManagerBool managerBool;
    SmartPointerManagerDouble managerDouble;

    size_t originalSizeInt = managerInt.getSharedPtrsSize();
    size_t originalSizeString = managerString.getSharedPtrsSize();
    size_t originalSizeVectorInt = managerVectorInt.getSharedPtrsSize();
    size_t originalSizeChar = managerChar.getSharedPtrsSize();
    size_t originalSizeBool = managerBool.getSharedPtrsSize();
    size_t originalSizeDouble = managerDouble.getSharedPtrsSize();

    while (true) {
        std::cout << "\nSharedPtr Menu:\n";
        std::cout << "\n1.  ➤   Create SharedPtr\n";
        std::cout << "\n2.  ➤   Create SharedPtr to Existing Resource\n";
        std::cout << "\n3.  ➤   Get Resource\n";
        std::cout << "\n4.  ➤   Use Count\n";
        std::cout << "\n5.  ➤   Reset\n";
        std::cout << "\n6.  ➤   List All SharedPtrs\n";
        std::cout << "\n7.  ➤   Remove SharedPtr\n";
        std::cout << "\n8.  ➤   Create Multiple SharedPtrs\n";
        std::cout << "\n9.  ➤   Remove Created SharedPtrs\n";
        std::cout << "\n10. ➤   Back to Main Menu\n";
        std::cout << "\nEnter your choice: ";

        int choice;
        std::cin >> choice;
        managerInt.clearInputBuffer();

        if (choice == 1) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.createSharedPtr();
            } else if (type == 2) {
                managerString.createSharedPtr();
            } else if (type == 3) {
                managerVectorInt.createSharedPtr();
            } else if (type == 4) {
                managerChar.createSharedPtr();
            } else if (type == 5) {
                managerBool.createSharedPtr();
            } else if (type == 6) {
                managerDouble.createSharedPtr();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 2) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.createSharedPtrFromExisting();
            } else if (type == 2) {
                managerString.createSharedPtrFromExisting();
            } else if (type == 3) {
                managerVectorInt.createSharedPtrFromExisting();
            } else if (type == 4) {
                managerChar.createSharedPtrFromExisting();
            } else if (type == 5) {
                managerBool.createSharedPtrFromExisting();
            } else if (type == 6) {
                managerDouble.createSharedPtrFromExisting();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 3) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.getResource();
            } else if (type == 2) {
                managerString.getResource();
            } else if (type == 3) {
                managerVectorInt.getResource();
            } else if (type == 4) {
                managerChar.getResource();
            } else if (type == 5) {
                managerBool.getResource();
            } else if (type == 6) {
                managerDouble.getResource();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 4) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.getUseCount();
            } else if (type == 2) {
                managerString.getUseCount();
            } else if (type == 3) {
                managerVectorInt.getUseCount();
            } else if (type == 4) {
                managerChar.getUseCount();
            } else if (type == 5) {
                managerBool.getUseCount();
            } else if (type == 6) {
                managerDouble.getUseCount();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 5) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.resetSharedPtr();
            } else if (type == 2) {
                managerString.resetSharedPtr();
            } else if (type == 3) {
                managerVectorInt.resetSharedPtr();
            } else if (type == 4) {
                managerChar.resetSharedPtr();
            } else if (type == 5) {
                managerBool.resetSharedPtr();
            } else if (type == 6) {
                managerDouble.resetSharedPtr();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 6) {
            std::cout << "\nSharedPtrs (int): ";
            managerInt.listAllSharedPtrs();

            std::cout << "\nSharedPtrs (string): ";
            managerString.listAllSharedPtrs();

            std::cout << "\nSharedPtrs (vector<int>): ";
            managerVectorInt.listAllSharedPtrs();

            std::cout << "\nSharedPtrs (char): ";
            managerChar.listAllSharedPtrs();

            std::cout << "\nSharedPtrs (bool): ";
            managerBool.listAllSharedPtrs();

            std::cout << "\nSharedPtrs (double): ";
            managerDouble.listAllSharedPtrs();
        } else if (choice == 7) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.removeSharedPtr();
            } else if (type == 2) {
                managerString.removeSharedPtr();
            } else if (type == 3) {
                managerVectorInt.removeSharedPtr();
            } else if (type == 4) {
                managerChar.removeSharedPtr();
            } else if (type == 5) {
                managerBool.removeSharedPtr();
            } else if (type == 6) {
                managerDouble.removeSharedPtr();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 8) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            int count;
            std::cout << "\nEnter number of SharedPtrs to create: ";
            std::cin >> count;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.createMultipleSharedPtrs(count);
            } else if (type == 2) {
                managerString.createMultipleSharedPtrs(count);
            } else if (type == 3) {
                managerVectorInt.createMultipleSharedPtrs(count);
            } else if (type == 4) {
                managerChar.createMultipleSharedPtrs(count);
            } else if (type == 5) {
                managerBool.createMultipleSharedPtrs(count);
            } else if (type == 6) {
                managerDouble.createMultipleSharedPtrs(count);
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 9) {
            managerInt.removeCreatedSharedPtrs(originalSizeInt);
            managerString.removeCreatedSharedPtrs(originalSizeString);
            managerVectorInt.removeCreatedSharedPtrs(originalSizeVectorInt);
            managerChar.removeCreatedSharedPtrs(originalSizeChar);
            managerBool.removeCreatedSharedPtrs(originalSizeBool);
            managerDouble.removeCreatedSharedPtrs(originalSizeDouble);
        } else if (choice == 10) {
            break;
        } else {
            std::cout << "\nInvalid choice\n";
        }
    }
}

void uniquePtrMenu() {
    SmartPointerManagerInt managerInt;
    SmartPointerManagerString managerString;
    SmartPointerManagerVectorInt managerVectorInt;
    SmartPointerManagerChar managerChar;
    SmartPointerManagerBool managerBool;
    SmartPointerManagerDouble managerDouble;

    while (true) {
        std::cout << "\nUniquePtr Menu:\n";
        std::cout << "\n1.  ➤   Create UniquePtr\n";
        std::cout << "\n2.  ➤   Get Resource\n";
        std::cout << "\n3.  ➤   Release\n";
        std::cout << "\n4.  ➤   Reset\n";
        std::cout << "\n5.  ➤   Create Multiple UniquePtrs\n";
        std::cout << "\n6.  ➤   Back to Main Menu\n";
        std::cout << "\nEnter your choice: ";

        int choice;
        std::cin >> choice;
        managerInt.clearInputBuffer();

        if (choice == 1) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.createUniquePtr();
            } else if (type == 2) {
                managerString.createUniquePtr();
            } else if (type == 3) {
                managerVectorInt.createUniquePtr();
            } else if (type == 4) {
                managerChar.createUniquePtr();
            } else if (type == 5) {
                managerBool.createUniquePtr();
            } else if (type == 6) {
                managerDouble.createUniquePtr();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 2) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.getUniquePtrResource();
            } else if (type == 2) {
                managerString.getUniquePtrResource();
            } else if (type == 3) {
                managerVectorInt.getUniquePtrResource();
            } else if (type == 4) {
                managerChar.getUniquePtrResource();
            } else if (type == 5) {
                managerBool.getUniquePtrResource();
            } else if (type == 6) {
                managerDouble.getUniquePtrResource();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 3) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.releaseUniquePtr();
            } else if (type == 2) {
                managerString.releaseUniquePtr();
            } else if (type == 3) {
                managerVectorInt.releaseUniquePtr();
            } else if (type == 4) {
                managerChar.releaseUniquePtr();
            } else if (type == 5) {
                managerBool.releaseUniquePtr();
            } else if (type == 6) {
                managerDouble.releaseUniquePtr();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 4) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.resetUniquePtr();
            } else if (type == 2) {
                managerString.resetUniquePtr();
            } else if (type == 3) {
                managerVectorInt.resetUniquePtr();
            } else if (type == 4) {
                managerChar.resetUniquePtr();
            } else if (type == 5) {
                managerBool.resetUniquePtr();
            } else if (type == 6) {
                managerDouble.resetUniquePtr();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 5) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            int count;
            std::cout << "\nEnter number of UniquePtrs to create: ";
            std::cin >> count;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.createMultipleUniquePtrs(count);
            } else if (type == 2) {
                managerString.createMultipleUniquePtrs(count);
            } else if (type == 3) {
                managerVectorInt.createMultipleUniquePtrs(count);
            } else if (type == 4) {
                managerChar.createMultipleUniquePtrs(count);
            } else if (type == 5) {
                managerBool.createMultipleUniquePtrs(count);
            } else if (type == 6) {
                managerDouble.createMultipleUniquePtrs(count);
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 6) {
            break;
        } else {
            std::cout << "\nInvalid choice\n";
        }
    }
}

void weakPtrMenu() {
    SmartPointerManagerInt managerInt;
    SmartPointerManagerString managerString;
    SmartPointerManagerVectorInt managerVectorInt;
    SmartPointerManagerChar managerChar;
    SmartPointerManagerBool managerBool;
    SmartPointerManagerDouble managerDouble;

    while (true) {
        std::cout << "\nWeakPtr Menu:\n";
        std::cout << "\n1.  ➤   Create SharedPtr\n";
        std::cout << "\n2.  ➤   Create WeakPtr from SharedPtr\n";
        std::cout << "\n3.  ➤   Lock WeakPtr\n";
        std::cout << "\n4.  ➤   Use Count\n";
        std::cout << "\n5.  ➤   Expired\n";
        std::cout << "\n6.  ➤   Create Multiple WeakPtrs\n";
        std::cout << "\n7.  ➤   Back to Main Menu\n";
        std::cout << "\nEnter your choice: ";

        int choice;
        std::cin >> choice;
        managerInt.clearInputBuffer();

        if (choice == 1) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            std::string name;
            std::cout << "\nEnter resource name: ";
            std::cin >> name;
            managerInt.clearInputBuffer();

            if (type == 1) {
                int value = managerInt.createResourceValue<int>();
                managerInt.sharedPtrs.push_back(SharedPtr<Resource<int>>(new Resource<int>(name, value)));
            } else if (type == 2) {
                std::string value = managerString.createResourceValue<std::string>();
                managerString.sharedPtrs.push_back(SharedPtr<Resource<std::string>>(new Resource<std::string>(name, value)));
            } else if (type == 3) {
                std::vector<int> value = managerVectorInt.createResourceValue<std::vector<int>>();
                managerVectorInt.sharedPtrs.push_back(SharedPtr<Resource<std::vector<int>>>(new Resource<std::vector<int>>(name, value)));
            } else if (type == 4) {
                char value = managerChar.createResourceValue<char>();
                managerChar.sharedPtrs.push_back(SharedPtr<Resource<char>>(new Resource<char>(name, value)));
            } else if (type == 5) {
                bool value = managerBool.createResourceValue<bool>();
                managerBool.sharedPtrs.push_back(SharedPtr<Resource<bool>>(new Resource<bool>(name, value)));
            } else if (type == 6) {
                double value = managerDouble.createResourceValue<double>();
                managerDouble.sharedPtrs.push_back(SharedPtr<Resource<double>>(new Resource<double>(name, value)));
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 2) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.createWeakPtr();
            } else if (type == 2) {
                managerString.createWeakPtr();
            } else if (type == 3) {
                managerVectorInt.createWeakPtr();
            } else if (type == 4) {
                managerChar.createWeakPtr();
            } else if (type == 5) {
                managerBool.createWeakPtr();
            } else if (type == 6) {
                managerDouble.createWeakPtr();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 3) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.lockWeakPtr();
            } else if (type == 2) {
                managerString.lockWeakPtr();
            } else if (type == 3) {
                managerVectorInt.lockWeakPtr();
            } else if (type == 4) {
                managerChar.lockWeakPtr();
            } else if (type == 5) {
                managerBool.lockWeakPtr();
            } else if (type == 6) {
                managerDouble.lockWeakPtr();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 4) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.getWeakPtrUseCount();
            } else if (type == 2) {
                managerString.getWeakPtrUseCount();
            } else if (type == 3) {
                managerVectorInt.getWeakPtrUseCount();
            } else if (type == 4) {
                managerChar.getWeakPtrUseCount();
            } else if (type == 5) {
                managerBool.getWeakPtrUseCount();
            } else if (type == 6) {
                managerDouble.getWeakPtrUseCount();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 5) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.checkWeakPtrExpired();
            } else if (type == 2) {
                managerString.checkWeakPtrExpired();
            } else if (type == 3) {
                managerVectorInt.checkWeakPtrExpired();
            } else if (type == 4) {
                managerChar.checkWeakPtrExpired();
            } else if (type == 5) {
                managerBool.checkWeakPtrExpired();
            } else if (type == 6) {
                managerDouble.checkWeakPtrExpired();
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 6) {
            std::cout << "\nEnter resource type (1 - int, 2 - string, 3 - vector<int>, 4 - char, 5 - bool, 6 - double): ";
            int type;
            std::cin >> type;
            managerInt.clearInputBuffer();

            int count;
            std::cout << "\nEnter number of WeakPtrs to create: ";
            std::cin >> count;
            managerInt.clearInputBuffer();

            if (type == 1) {
                managerInt.createMultipleWeakPtrs(count);
            } else if (type == 2) {
                managerString.createMultipleWeakPtrs(count);
            } else if (type == 3) {
                managerVectorInt.createMultipleWeakPtrs(count);
            } else if (type == 4) {
                managerChar.createMultipleWeakPtrs(count);
            } else if (type == 5) {
                managerBool.createMultipleWeakPtrs(count);
            } else if (type == 6) {
                managerDouble.createMultipleWeakPtrs(count);
            } else {
                std::cout << "\nInvalid type\n";
            }
        } else if (choice == 7) {
            break;
        } else {
            std::cout << "\nInvalid choice\n";
        }
    }
}



int main() {
    while (true) {
        std::cout << "\nMain Menu:\n";
        std::cout << "\n1.  ➤   SharedPtr Menu\n";
        std::cout << "\n2.  ➤   WeakPtr Menu\n";
        std::cout << "\n3.  ➤   UniquePtr Menu\n";
        std::cout << "\n4.  ➤   Exit\n";
        std::cout << "\nEnter your choice: ";

        int choice;
        std::cin >> choice;
        SmartPointerManager<int> managerInt;
        managerInt.clearInputBuffer();

        if (choice == 1) {
            sharedPtrMenu();
        } else if (choice == 2) {
            weakPtrMenu();
        } else if (choice == 3) {
            uniquePtrMenu();
        } else if (choice == 4) {
            break;
        } else {
            std::cout << "\nInvalid choice\n";
        }
    }

    return 0;
}
