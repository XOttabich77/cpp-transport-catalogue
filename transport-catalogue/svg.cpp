#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();
        RenderObject(context);
        context.out << std::endl;
    }
        
    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "  <circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    Polyline& Polyline::AddPoint(Point point)
    {
        if (!pointers_.empty()) {
            pointers_ += " "s;
        }
        pointers_ += ToString(point.x);
        pointers_ += ","s;
        pointers_ += ToString(point.y);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "  <polyline points=\""sv << pointers_ << "\""sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    Text& Text::SetPosition(Point pos)
    {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset)
    {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size)
    {
        size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family)
    {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight)
    {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data)
    {
        data_ = MakeData(data);
        return *this;
    }

    std::string Text::MakeData(std::string& data)
    {
        std::string result;
        for (char c : data) {
            switch (c)
            {
            case '"':
                result += "&quot"s;
                break;
            case '\'':
                result += "&apos";
                break;
            case '<':
                result += "&lt";
                break;
            case '>':
                result += "&gt";
                break;
            case '&':
                result += "&amp";
                break;
            default:
                result += c;
            }
        }
        return result;
    }

    void Text::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "  <text";
        RenderAttrs(context.out);
        out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y;
        out << "\" dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y;
        out << "\" font-size=\""sv << size_ << "\""sv;
        if (font_family_) {
            out << " font-family=\""sv << *font_family_ << "\""sv;
        }
        if (font_weight_) {
            out << " font-weight=\""sv << *font_weight_ << "\""sv;
        }
        out << ">"sv;
        out << data_;
        out << "</text>"sv;
    }

    void Document::AddPtr(std::unique_ptr<Object>&& obj)
    {
        objects_.emplace_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

        for (size_t i = 0; i < objects_.size(); i++) {
            objects_[i].get()->Render(out);
        }
        out << "</svg>"sv;
    }

}  // namespace svg