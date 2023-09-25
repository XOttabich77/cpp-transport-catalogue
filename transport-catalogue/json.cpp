#include "json.h"

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;
            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (!input) {
                throw ParsingError("Array parsing error"s);
            }

            return Node(move(result));
        }

        Node LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // ��������� � parsed_num ��������� ������ �� input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // ��������� ���� ��� ����� ���� � parsed_num �� input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // ������ ����� ����� �����
            if (input.peek() == '0') {
                read_char();
                // ����� 0 � JSON �� ����� ���� ������ �����
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // ������ ������� ����� �����
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // ������ ���������������� ����� �����
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // ������� ������� ������������� ������ � int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // � ������ �������, ��������, ��� ������������,
                        // ��� ���� ��������� ������������� ������ � double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }


        Node LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // ����� ���������� �� ����, ��� ��������� ����������� �������?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // ��������� ����������� �������
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // ��������� ������ escape-������������������
                    ++it;
                    if (it == end) {
                        // ����� ���������� ����� ����� ������� �������� ����� �����
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // ������������ ���� �� �������������������: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // ��������� ����������� escape-������������������
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // ��������� ������� ������- JSON �� ����� ����������� ��������� \r ��� \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // ������ ��������� ��������� ������ � �������� ��� � �������������� ������
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(move(s));
        }

        Node LoadDict(istream& input) {
            Dict dict;

            for (char c; input >> c && c != '}';) {
                if (c == '"') {
                    std::string key = LoadString(input).AsString();
                    if (input >> c && c == ':') {
                        if (dict.find(key) != dict.end()) {
                            throw ParsingError("Duplicate key '"s + key + "' have been found");
                        }
                        dict.emplace(std::move(key), LoadNode(input));
                    }
                    else {
                        throw ParsingError(": is expected but '"s + c + "' has been found"s);
                    }
                }
                else if (c != ',') {
                    throw ParsingError(R"(',' is expected but ')"s + c + "' has been found"s);
                }
            }
            if (!input) {
                throw ParsingError("Dictionary parsing error"s);
            }
            return Node(std::move(dict));
        }

        string LoadLineString(istream& input) {
            string str;
            while (isalpha(input.peek())) {
                str += input.get();
            }

            return str;
        }
        Node LoadNull(istream& input) {
            if (auto str = LoadLineString(input); str == "null"sv) {
                return Node{ nullptr };
            }
            else {
                throw ParsingError("Failed to parse '"s + str + "' as null"s);
            }
        }
        Node LoadBool(istream& input) {
            const auto s = LoadLineString(input);
            if (s == "true"sv) {
                return Node{ true };
            }
            else if (s == "false"sv) {
                return Node{ false };
            }
            else {
                throw ParsingError("Failed to parse '"s + s + "' as bool"s);
            }
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            switch (c) {
            case '[':
                return LoadArray(input);
            case  '{':
                return LoadDict(input);
            case '"':                
                return LoadString(input);
            case 'n':
                input.putback(c);
                return LoadNull(input);
            case 't':
                [[fallthrough]];
            case 'f':
                input.putback(c);
                return LoadBool(input);
            default:
                input.putback(c);
                return LoadNumber(input);
            }

        }

    }  // namespace

    const Array& Node::AsArray() const {
        if (!IsArray()) {
            throw std::logic_error("Not an array"s);
        }
        return get<Array>(*this);
    }

    const Dict& Node::AsMap() const {
        if (!IsMap()) {
            throw std::logic_error("Not a map"s);
        }
        return get<Dict>(*this);
    }

    int Node::AsInt() const {
        if (!IsInt()) {
            throw std::logic_error("Not an int"s);
        }
        return get<int>(*this);
    }

    const string& Node::AsString() const {
        if (!IsString()) {
            throw std::logic_error("Not a string"s);
        }
        return get<string>(*this);
    }

    bool Node::AsBool() const
    {
        if (!IsBool()) {
            throw std::logic_error("Not a bool"s);
        }
        return get<bool>(*this);
    }

    double Node::AsDouble() const
    {
        if (!IsDouble()) {
            throw std::logic_error("Not a double"s);
        }
        return IsPureDouble() ? std::get<double>(*this) : AsInt();
    }

    bool Node::IsInt() const
    {
        return holds_alternative<int>(*this);
    }

    bool Node::IsDouble() const
    {
        return holds_alternative<int>(*this) || holds_alternative<double>(*this);
    }

    bool Node::IsPureDouble() const
    {
        return holds_alternative<double>(*this);;
    }

    bool Node::IsBool() const
    {
        return holds_alternative<bool>(*this);
    }

    bool Node::IsString() const
    {
        return holds_alternative<string>(*this);;
    }

    bool Node::IsNull() const
    {
        return holds_alternative<nullptr_t>(*this);;
    }

    bool Node::IsArray() const
    {
        return holds_alternative<Array>(*this);;
    }

    bool Node::IsMap() const
    {
        return holds_alternative<Dict>(*this);;
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    struct PrintContext {
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        PrintContext Indented() const {
            return { out, indent_step, indent_step + indent };
        }
    };

    void PrintNode(const Node& value, const PrintContext& ctx);

    template <typename Value>
    void PrintValue(const Value& value, const PrintContext& ctx) {
        ctx.out << value;
    }

    void PrintString(const std::string& value, std::ostream& out) {
        out.put('"');
        for (const char c : value) {
            switch (c) {
            case '\r':
                out << "\\r"sv;
                break;
            case '\n':
                out << "\\n"sv;
                break;
            case '"':
                [[fallthrough]];
            case '\\':
                out.put('\\');
                [[fallthrough]];
            default:
                out.put(c);
                break;
            }
        }
        out.put('"');
    }

    template <>
    void PrintValue<std::string>(const std::string& value, const PrintContext& ctx) {
        PrintString(value, ctx.out);
    }

    template <>
    void PrintValue<std::nullptr_t>(const std::nullptr_t&, const PrintContext& ctx) {
        ctx.out << "null"sv;
    }

    // � ������������ ������� PrintValue ��� ���� bool �������� value ���������
    // �� ����������� ������, ��� � � �������� �������.
    // � �������� ������������ ����� ������������ ����������:
    // void PrintValue(bool value, const PrintContext& ctx);
    template <>
    void PrintValue<bool>(const bool& value, const PrintContext& ctx) {
        ctx.out << (value ? "true"sv : "false"sv);
    }

    template <>
    void PrintValue<Array>(const Array& nodes, const PrintContext& ctx) {
        std::ostream& out = ctx.out;
        out << "[\n"sv;
        bool first = true;
        auto inner_ctx = ctx.Indented();
        for (const Node& node : nodes) {
            if (first) {
                first = false;
            }
            else {
                out << ",\n"sv;
            }
            inner_ctx.PrintIndent();
            PrintNode(node, inner_ctx);
        }
        out.put('\n');
        ctx.PrintIndent();
        out.put(']');
    }

    template <>
    void PrintValue<Dict>(const Dict& nodes, const PrintContext& ctx) {
        std::ostream& out = ctx.out;
        out << "{\n"sv;
        bool first = true;
        auto inner_ctx = ctx.Indented();
        for (const auto& [key, node] : nodes) {
            if (first) {
                first = false;
            }
            else {
                out << ",\n"sv;
            }
            inner_ctx.PrintIndent();
            PrintString(key, ctx.out);
            out << ": "sv;
            PrintNode(node, inner_ctx);
        }
        out.put('\n');
        ctx.PrintIndent();
        out.put('}');
    }

    void PrintNode(const Node& node, const PrintContext& ctx) {
        std::visit(
            [&ctx](const auto& value) {
                PrintValue(value, ctx);
            },
            node.GetValue());
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), PrintContext{ output });
    }

}  // namespace json