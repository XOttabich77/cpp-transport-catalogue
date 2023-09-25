#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <string_view>
#include <sstream>
#include <optional>
#include <variant>

namespace svg {       
    struct Rgba {
        Rgba() = default;
        Rgba(uint8_t r, uint8_t g, uint8_t b, double o) :
            red(r),
            green(g),
            blue(b),
            opacity(o)
        {}
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity = 1.0;
    };
    struct Rgb {
        Rgb() = default;
        Rgb(uint8_t r, uint8_t g, uint8_t b) :
            red(r),
            green(g),
            blue(b)
        {}
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };
    template <typename T>
    std::string ToString(T val)
    {
        std::ostringstream os;
        os << val;
        return os.str();
    }

    using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;
    inline const std::string NoneColor{"none"};
    struct ColorResolve
    {
        std::ostream& os;
        void operator()(std::monostate) const {
            os << NoneColor;
        }
        void operator()(std::string str) const {
            os << str;
        }
        void operator()(Rgb rgb) const {
            os << "rgb(";
            os << static_cast<int>(rgb.red) << ",";
            os << static_cast<int>(rgb.green) << ",";
            os << static_cast<int>(rgb.blue) << ")";

        }
        void operator()(Rgba rgba) const {
            os << "rgba(";
            os << static_cast<int>(rgba.red) << ",";
            os << static_cast<int>(rgba.green) << ",";
            os << static_cast<int>(rgba.blue) << ",";
            os << rgba.opacity << ")";
        }
    };

    inline std::ostream& operator<<(std::ostream& os, Color color) {
        std::ostringstream strm;
        std::visit(ColorResolve{ strm }, color);
        os << strm.str();
        return os;
    }

    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }
        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    inline std::ostream& operator<<(std::ostream& os, StrokeLineCap value) {
        using namespace std::literals;
        std::string str;
        switch (value) {
        case StrokeLineCap::BUTT:
            str = "butt"s;
            break;
        case StrokeLineCap::ROUND:
            str = "round"s;
            break;
        case StrokeLineCap::SQUARE:
            str = "square"s;
        }
        os << str;
        return os;
    }

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };
    inline std::ostream& operator<<(std::ostream& os, StrokeLineJoin value) {
        using namespace std::literals;
        std::string str;
        switch (value)
        {
        case svg::StrokeLineJoin::ARCS:
            str = "arcs"s;
            break;
        case svg::StrokeLineJoin::BEVEL:
            str = "bevel"s;
            break;
        case svg::StrokeLineJoin::MITER:
            str = "miter"s;
            break;
        case svg::StrokeLineJoin::MITER_CLIP:
            str = "miter-clip"s;
            break;
        case svg::StrokeLineJoin::ROUND:
            str = "round"s;
            break;
        }
        os << str;
        return os;
    }

    template <typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return AsOwner();
        }
        Owner& SetStrokeColor(Color color) {
            stroke_color_ = std::move(color);
            return AsOwner();
        }
        Owner& SetStrokeWidth(double width) {
            width_ = width;
            return AsOwner();
        }
        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            line_cap_ = line_cap;
            return AsOwner();
        }
        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            line_join_ = line_join;
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;

            if (fill_color_) {
                out << " fill=\""sv << *fill_color_ << "\""sv;
            }
            if (stroke_color_) {
                out << " stroke=\""sv << *stroke_color_ << "\""sv;
            }
            if (width_) {
                out << " stroke-width=\""sv << ToString(*width_) << "\""sv;
            }
            if (line_cap_) {
                out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
            }
            if (line_join_) {
                out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
            }
        }

    private:
        Owner& AsOwner() {
            return static_cast<Owner&>(*this);
        }
        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> width_;
        std::optional<StrokeLineCap> line_cap_;
        std::optional<StrokeLineJoin> line_join_;
    };


    /*
    * Абстрактный базовый класс Object служит для унифицированного хранения
    * конкретных тегов SVG-документа
    * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
    */
    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    /*
     * Класс Circle моделирует элемент <circle> для отображения круга
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);
    private:
        void RenderObject(const RenderContext& context) const override;
        Point center_;
        double radius_ = 1.0;
    };

    /*
     * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline : public Object, public PathProps<Polyline> {
    public:
        Polyline& AddPoint(Point point);
    private:
        void RenderObject(const RenderContext& context) const override;
        std::string pointers_;
    };

    /*
     * Класс Text моделирует элемент <text> для отображения текста
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text : public Object, public PathProps<Text> {
    public:
        Text& SetPosition(Point pos);
        Text& SetOffset(Point offset);
        Text& SetFontSize(uint32_t size);
        Text& SetFontFamily(std::string font_family);
        Text& SetFontWeight(std::string font_weight);
        Text& SetData(std::string data);
    private:
        std::string MakeData(std::string& data);
        void RenderObject(const RenderContext& context) const override;
        Point pos_;
        Point offset_;
        uint32_t size_ = 1;
        std::optional <std::string> font_family_;
        std::optional <std::string> font_weight_;
        std::string data_;
    };

    class ObjectContainer {
    public:
        virtual ~ObjectContainer() = default;

        template <typename Obj>
        void Add(Obj obj) {
            objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
        }
        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    protected:
        std::vector< std::unique_ptr<svg::Object>> objects_;
    };

    class Document : public ObjectContainer {
    public:
        virtual void AddPtr(std::unique_ptr<Object>&& obj) override;
        void Render(std::ostream& out) const;
    };

    class Drawable {
    public:
        virtual ~Drawable() = default;
        virtual void Draw(ObjectContainer& container)const = 0;
    };

}  // namespace svg