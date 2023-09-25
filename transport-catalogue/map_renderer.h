#pragma once

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
//class map_renderer
//{
//};
#include "json_reader.h"
#include "svg.h"
#include "transport_catalogue.h"

namespace renderer {

namespace detail {

    inline const double EPSILON = 1e-6;
  
    class SphereProjector {
    public:
        
        SphereProjector(const transport::TransportCatalogue& catalog, double max_width, double max_height, double padding);

        template <typename PointInputIt>
		SphereProjector(PointInputIt points_begin, PointInputIt points_end,
			double max_width, double max_height, double padding);
           

        // Проецирует широту и долготу в координаты внутри SVG-изображения
        svg::Point operator()(geo::Coordinates coords) const {
            return {
                (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_
            };
        }

    private:
        bool IsZero(double value) { return std::abs(value) < EPSILON; }

        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

	template<typename PointInputIt>
	inline SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        if (points_begin == points_end) {
            return;
        }

        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;
                
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;
                
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {                                      
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
           zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom) {
            zoom_coeff_ = *width_zoom;
        }
        else if (height_zoom) {
            zoom_coeff_ = *height_zoom;
        }
    }

}// end of namespace detail

	
		struct RenderSetting {
			double width;
			double height;
			double padding;
			double line_width;
			double stop_radius;
			int bus_label_font_size;
			svg::Point bus_label_offset;
			int stop_label_font_size;
			svg::Point stop_label_offset;
			svg::Color underlayer_color;
			double underlayer_width;
			std::vector<svg::Color>	color_palette;
		};
        struct StopOnMap {
            std::string name;
            svg::Point coordinate;
            bool operator < (const StopOnMap& rhs) const {
                return name < rhs.name;
            }
        };
		class MapRenderer  
		{
			public:
				MapRenderer(const transport::TransportCatalogue& db, const json::Dict& setting);
				void MakeMap();
                void RenderMap(std::ostream& out);
			private: 
				const transport::TransportCatalogue& db_;
				RenderSetting render_setting_;
                std::map< std::string, std::vector<StopOnMap> > buses_;

                void AddBusLine(svg::Document& map_route);
                void AddNameBus(svg::Document& map_route);
                void AddStopOnLine(svg::Document& map_route);
		};

	}//namespace map_renderer


