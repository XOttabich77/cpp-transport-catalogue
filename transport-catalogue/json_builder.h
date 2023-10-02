#pragma once

#include <cassert>
#include <memory>
#include "json.h"

namespace json {

    class Builder;
    class AfterDictionary;
    class AfterKey;    
    class AfterArray;

    class BuilderContext {
    public:
        BuilderContext(Builder& builder);

        AfterArray StartArray();
        Builder& EndArray();
        Builder& Value(Node::Value value);
        AfterKey Key(std::string value);
        AfterDictionary StartDict();
        Builder& EndDict();
        Node Build();

    protected:
        Builder& builder_;
    };

    class AfterArray : public BuilderContext {
    public:           
        AfterKey Key(std::string value) = delete;
        Builder& EndDict() = delete;
        Node Build() = delete;
    };

    class AfterDictionary : public BuilderContext {
    public:        
        AfterArray StartArray() = delete;
        Builder& EndArray() = delete;
        Builder& Value(Node::Value value) = delete;
        AfterDictionary StartDict() = delete;
        Node Build() = delete;
    };
       
    class AfterKey : public BuilderContext {
    public:
        AfterKey(Builder& builder);
        AfterDictionary Value(Node::Value value);
        Builder& EndArray() = delete;
        AfterKey Key(std::string value) = delete;
        Builder& EndDict() = delete;
        Node Build() = delete;
    private:
        AfterDictionary v_context_{ builder_ };
    };
    

    using namespace std::string_literals;

    class Builder {
   
    private:
        AfterDictionary diccontext_{ *this };
        AfterKey  keycontext_{ *this };
        AfterArray arraycontext_{ *this };

    public:        
        Builder& Value(Node::Value value);               
        AfterArray StartArray();
        Builder& EndArray();
        AfterDictionary& StartDict();
        Builder& EndDict();
        AfterKey Key(std::string key);
        Node Build() const;

    private:
        Node root_ = nullptr;
        std::vector<std::unique_ptr<Node>> nodes_stack_;        
    };

}  // namespace json