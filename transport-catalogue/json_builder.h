#pragma once

#include <cassert>
#include <memory>
#include "json.h"

namespace json {

    class Builder;
    class AfterDictionary;  
    class AfterKey;

    class BuilderContext {
    public:
        BuilderContext(Builder& builder);
    protected:
        Builder& builder_;
    };

    class InArray : public BuilderContext {
    public:
        InArray Value(Node::Value value);
        InArray StartArray();
        AfterDictionary StartDict();
        Builder& EndArray();       
    };

    class AfterDictionary : public BuilderContext {
    public:
        AfterKey Key(std::string key);
        Builder& EndDict();      
    };
        
    class AfterValue : public BuilderContext {
    public:
        AfterKey Key(std::string value);
        Builder& EndDict();
    };

    class AfterKey : public BuilderContext {
    public:
        AfterKey(Builder& builder) : 
            BuilderContext(builder) 
        {}
        AfterValue Value(Node::Value value);     
        InArray StartArray();
        AfterDictionary StartDict();     
    private:
        AfterValue v_context_{ builder_ };
       
    };

    using namespace std::string_literals;

    class Builder {
   
    private:
        AfterDictionary diccontext_{ *this };
        AfterKey  keycontext_{ *this };
        InArray arraycontext_{ *this };

    public:        
        Builder& Value(Node::Value value);               
        InArray StartArray();
        Builder& EndArray();
        AfterDictionary& StartDict();
        Builder& EndDict();
        AfterKey& Key(std::string key);
        Node Build() const;

    private:
        Node root_ = nullptr;
        std::vector<std::unique_ptr<Node>> nodes_stack_;        
    };

}  // namespace json