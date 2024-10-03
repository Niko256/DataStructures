#pragma once


#include "/home/nikolay/projects/algo/labs/2/src/mutable_array.h"
#include "/home/nikolay/projects/algo/labs/2/src/exceptions.h"
#include "/home/nikolay/projects/algo/labs/2/src/Sequence.h"
#include "Stack.hpp"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <memory>

template<typename T>
class Persistent_Stack {
private:
    struct Node {
        T value;
        std::shared_ptr<Node> next;

        Node(const T& value, const std::shared_ptr<Node>& next) : value(value), next(next) {}
    };

    std::shared_ptr<Node> top_Node;

    std::vector<Persistent_Stack> versions;

public:
    Persistent_Stack() : top_Node(nullptr) {
        versions.push_back(*this);
    }

    Persistent_Stack(const T& value, const Persistent_Stack& previous)
        : top_Node(std::make_shared<Node>(value, previous.top_Node)) {
            versions.push_back(Persistent_Stack(*this));
        }

    explicit Persistent_Stack(std::shared_ptr<Node> topNode) : top_Node(topNode) {}

    bool is_Empty() const {
        return top_Node == nullptr;
    }

    T top() const {
        if (is_Empty()) {
            throw SEQUENCE_EXCEPTION{ERRORS::Stack_is_empty};
        }
        return top_Node->value;
    }

    Persistent_Stack push(const T& value) {
        auto new_version = Persistent_Stack(value, *this);
        versions.push_back(new_version);
        return new_version;
    }

    Persistent_Stack pop() {
        if (is_Empty()) {
            throw SEQUENCE_EXCEPTION{ERRORS::Stack_is_empty};
        }

        auto last_version = Persistent_Stack(top_Node->next);
        versions.push_back(last_version);
        return last_version;
    }

    size_t size() const {
        size_t cnt = 0;
        std::shared_ptr<Node> curr = top_Node;
        while (curr) {
            cnt++;
            curr = curr->next;
        }
        return cnt;
    }

    bool empty() const {
        return is_Empty();
    }

    Persistent_Stack clear() const {
        versions.push_back(Persistent_Stack());
        return Persistent_Stack();
    }

    std::vector<T> get_elements() const {
        std::vector<T> elements;
        std::shared_ptr<Node> current = top_Node;
        while (current != nullptr) {
            elements.push_back(current->value);
            current = current->next;
        }
        return elements;
    }

    Persistent_Stack& get_version(const size_t version_index) const {
        if (version_index >= versions.size()) {
            throw SEQUENCE_EXCEPTION{ERRORS::Index_Out_of_range};
        }
        return versions[version_index];
    }
};

