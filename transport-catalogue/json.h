#pragma once

#include <cassert>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

    class Node;
    // —охраните объ€влени€ Dict и Array без изменени€
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Ёта ошибка должна выбрасыватьс€ при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    class Node final : public Value {
    public:
        using variant::variant;
        
        const Value& GetValue() const { return *this; }
        
        const Array& AsArray() const;
        const Dict& AsMap() const;
        int AsInt() const;
        const std::string& AsString() const;
        bool AsBool() const;
        double AsDouble() const; 

        bool IsInt() const;
        bool IsDouble() const; 
        bool IsPureDouble() const; 
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        bool operator==(const Node& rhs) {
            return GetValue() == rhs.GetValue();
        }

        bool operator!=(const Node& rhs) {
            return !(*this == rhs);
        }
    };

    inline bool operator== (const Node& lhs, const Node& rhs) {
        return lhs.GetValue() == rhs.GetValue();
    }

    inline bool operator!= (const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };
    inline bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    inline bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

    Document Load(std::istream& input);
    void Print(const Document& doc, std::ostream& output);

}  // namespace json