#include "json_builder.h"

using namespace json;


BuilderContext::BuilderContext(Builder& builder) : builder_(builder) {}

InArray InArray::Value(Node::Value value) {
    builder_.Value(value);
    return *this;
}

InArray InArray::StartArray() { return builder_.StartArray(); }

AfterDictionary InArray::StartDict() { return builder_.StartDict(); }

Builder& InArray::EndArray() { return builder_.EndArray(); }

AfterKey AfterDictionary::Key(std::string key) { return builder_.Key(key); }

Builder& AfterDictionary::EndDict() { return builder_.EndDict(); }

AfterKey AfterValue::Key(std::string value) { return builder_.Key(value); }

Builder& AfterValue::EndDict() { return builder_.EndDict(); }

AfterValue AfterKey::Value(Node::Value value) {
    builder_.Value(value);
    return v_context_;
}

InArray AfterKey::StartArray() { return builder_.StartArray(); }

AfterDictionary AfterKey::StartDict() { return builder_.StartDict(); }


Builder& json::Builder::Value(Node::Value value)
{
    if (!root_.IsNull()) {
        throw std::logic_error("Node is empty"s);
    }
    std::unique_ptr<Node> ptr;
    visit([&ptr](auto&& val) { ptr = std::make_unique<Node>(val); }, value);
    if (nodes_stack_.empty()) {
        root_ = *ptr.release();
        return *this;
    }
    if (nodes_stack_.back()->IsArray()) {
        nodes_stack_.back()->AsArray().emplace_back(*ptr.release());
        return *this;
    }
    if (nodes_stack_.back()->IsString()) {
        std::string key = nodes_stack_.back().release()->AsString();
        nodes_stack_.pop_back();
        nodes_stack_.back()->AsMap().emplace(key, *ptr.release());

        return *this;
    }
    throw std::logic_error("Error setting the value"s);
}

InArray json::Builder::StartArray()
{
    if (!root_.IsNull()) {
        throw std::logic_error("a node exists"s);
    }
    if (!(nodes_stack_.empty() || nodes_stack_.back()->IsArray() ||
        nodes_stack_.back()->IsString())) {
        throw std::logic_error("nodes is not empty"s);
    }
    auto ptr = std::make_unique<Node>();
    *ptr = json::Array{};
    nodes_stack_.push_back(std::move(ptr));
    return arraycontext_;
}

Builder& json::Builder::EndArray()
{
    if (nodes_stack_.empty()) {
        throw std::logic_error("array problem : Stack empty"s);
    }
    if (nodes_stack_.back()->IsArray()) {
        auto arr = std::move(nodes_stack_.back());
        nodes_stack_.pop_back();
        if (nodes_stack_.empty()) {
            root_ = *arr.release();
            return *this;
        }
        else if (nodes_stack_.back()->IsArray()) {
            nodes_stack_.back()->AsArray().push_back(*arr.release());
            return *this;
        }
        else if (nodes_stack_.back()->IsString()) {
            std::string key = nodes_stack_.back()->AsString();
            nodes_stack_.pop_back();
            nodes_stack_.back()->AsMap().emplace(key, *arr.release());
            return *this;
        }
    }
    throw std::logic_error("array not close"s);
}

AfterDictionary& json::Builder::StartDict()
{
    if (!root_.IsNull()) {
        throw std::logic_error("a node exists"s);
    }
    if (!(nodes_stack_.empty() || nodes_stack_.back()->IsArray() ||
        nodes_stack_.back()->IsString())) {
        throw std::logic_error("nodes is not empty"s);
    }

    auto ptr = std::make_unique<Node>();
    *ptr = json::Dict{};
    nodes_stack_.push_back(std::move(ptr));
    return diccontext_;
}

Builder& json::Builder::EndDict()
{
    if (nodes_stack_.empty()) {
        throw std::logic_error("dictionary problem : stack empty"s);
    }

    if (nodes_stack_.back()->IsMap()) {
        auto dic = std::move(nodes_stack_.back());
        nodes_stack_.pop_back();

        if (nodes_stack_.empty()) {
            root_ = *dic.release();
            return *this;
        }
        else if (nodes_stack_.back()->IsArray()) {
            nodes_stack_.back()->AsArray().push_back(*dic.release());
            return *this;
        }
        else if (nodes_stack_.back()->IsString()) {
            std::string key = nodes_stack_.back()->AsString();
            nodes_stack_.pop_back();
            nodes_stack_.back()->AsMap().emplace(key, *dic.release());
            return *this;
        }
    }
    throw std::logic_error("dictionary not close"s);
}

AfterKey& json::Builder::Key(std::string key)
{
    if (nodes_stack_.empty() || !root_.IsNull()) {
        throw std::logic_error("empty or null"s);
    }
    if (!(nodes_stack_.back()->IsMap())) {
        throw std::logic_error("nodes is not map"s);
    }
    auto ptr = std::make_unique<Node>();
    *ptr = std::move(key);
    nodes_stack_.push_back(std::move(ptr));;
    return keycontext_;
}

Node json::Builder::Build() const
{
    if (root_.IsNull()) {
        throw std::logic_error("Node empty"s);
    }
    if (!nodes_stack_.empty()) {
        throw std::logic_error("Not fineshed"s);
    }
    return root_;
}
