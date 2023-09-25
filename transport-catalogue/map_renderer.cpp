#include "map_renderer.h"
#include <list>

using namespace std;
namespace renderer {
	namespace detail {
		svg::Color ColorArray(const json::Node& node);
		svg::Color ColorFromNode(const json::Node& node) {
			if (node.IsString()) {
				return node.AsString();
			}
			if (node.IsArray()) {
				return ColorArray(node);
			}
			assert(false && "Color not string not array");
		}
		svg::Color ColorArray(const json::Node& node) {
			json::Array color_array = node.AsArray();
			size_t size = color_array.size();
			assert(size > 2 && size < 5);
			int red = color_array[0].AsInt();
			int green = color_array[1].AsInt();
			int blue = color_array[2].AsInt();
			if (size == 4) {
				return svg::Rgba({ static_cast<uint8_t>(red),static_cast<uint8_t>(green),static_cast<uint8_t>(blue),color_array[3].AsDouble() });
			}
			return svg::Rgb({ static_cast<uint8_t>(red),static_cast<uint8_t>(green),static_cast<uint8_t>(blue) });
		}
		SphereProjector::SphereProjector(const transport::TransportCatalogue& catalog, double max_width, double max_height, double padding)
		{			
			list<geo::Coordinates> all_coordinate;
			for (const auto& bus : catalog.GetAllBus()) {
				for (auto stop : bus.stops) {
					all_coordinate.push_back(stop->coordinate);
				}
			}
			SphereProjector tmp{ all_coordinate.begin(), all_coordinate.end(), max_width, max_height, padding };
			swap(*this, tmp);
		}
	}
}

renderer::MapRenderer::MapRenderer(const transport::TransportCatalogue& db, const json::Dict& setting) :
	db_(db)
	{	
		render_setting_.width = setting.at("width"s).AsDouble();
		render_setting_.height = setting.at("height"s).AsDouble();
		render_setting_.padding = setting.at("padding"s).AsDouble();
		render_setting_.line_width = setting.at("line_width"s).AsDouble();
		render_setting_.stop_radius = setting.at("stop_radius"s).AsDouble();
		render_setting_.bus_label_font_size = setting.at("bus_label_font_size"s).AsInt();
		{
			const json::Array tmp = setting.at("bus_label_offset"s).AsArray();
			render_setting_.bus_label_offset = { tmp[0].AsDouble(),tmp[1].AsDouble()};
		}		
		render_setting_.stop_label_font_size = setting.at("stop_label_font_size"s).AsInt();
		{
			const json::Array tmp = setting.at("stop_label_offset"s).AsArray();
			render_setting_.stop_label_offset = { tmp[0].AsDouble(),tmp[1].AsDouble() };
		}
		render_setting_.underlayer_color = detail::ColorFromNode(setting.at("underlayer_color"s));
		render_setting_.underlayer_width = setting.at("underlayer_width"s).AsDouble();
					
		for (auto& node: setting.at("color_palette"s).AsArray()) {
			render_setting_.color_palette.push_back(detail::ColorFromNode(node));
		}
		
	}

void renderer::MapRenderer::RenderMap(std::ostream& out)
{
	svg::Document map_route;
	AddBusLine(map_route);
	AddNameBus(map_route);
	AddStopOnLine(map_route);
	map_route.Render(out);
}



void renderer::MapRenderer::MakeMap()
{	
	detail::SphereProjector screen_coordinate(db_, render_setting_.width, render_setting_.height, render_setting_.padding);
	for (const auto& bus : db_.GetAllBus()) {
		if (bus.stops.size() == 0) {
			continue;
		}
		vector< StopOnMap> tmp;
		for (const auto& stop : bus.stops) {
			tmp.push_back({stop->name, screen_coordinate({ stop->coordinate.lat, stop->coordinate.lng }) });
		}
		buses_[bus.name] = move(tmp);
	}
}

void renderer::MapRenderer::AddBusLine(svg::Document& map_route)
{
	int counter = 0;
	for (const auto& bus : buses_) {
		svg::Polyline route;
		for (const auto& stop : bus.second) {
			route.AddPoint(stop.coordinate);
		}
		route.SetStrokeColor(render_setting_.color_palette[ counter % render_setting_.color_palette.size() ])
			.SetStrokeWidth(render_setting_.line_width)
			.SetFillColor(svg::NoneColor)
			.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
			.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

		map_route.Add(route);
		++counter;
	}
	
}

void renderer::MapRenderer::AddNameBus(svg::Document& map_route)
{
	int counter = 0;
	for (const auto& bus : buses_) {
		svg::Text bus_background;
		bus_background.SetData(bus.first)
			.SetPosition(bus.second.front().coordinate)
			.SetOffset(render_setting_.bus_label_offset)
			.SetFontSize(render_setting_.bus_label_font_size)
			.SetFontFamily("Verdana"s)
			.SetFontWeight("bold"s);
				
		svg::Text bus_name = bus_background;
		bus_name.SetFillColor(render_setting_.color_palette[counter % render_setting_.color_palette.size()]);

		bus_background.SetFillColor(render_setting_.underlayer_color)
			.SetStrokeColor(render_setting_.underlayer_color)
			.SetStrokeWidth(render_setting_.underlayer_width)
			.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
			.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
		
		map_route.Add(bus_background);
		map_route.Add(bus_name);
		int back_stop = (bus.second.size() - 1) / 2;
		if (!db_.IsBusCircle(bus.first) && bus.second[0].name!= bus.second[back_stop].name) {			
			bus_background.SetPosition(bus.second[back_stop].coordinate);
			bus_name.SetPosition(bus.second[back_stop].coordinate);
			map_route.Add(bus_background);
			map_route.Add(bus_name);
		}
		++counter;
	}

}

void renderer::MapRenderer::AddStopOnLine(svg::Document& map_route)
{
	
	set<StopOnMap> all_stops;

	// Изображение остановки
	for (const auto& [name,stops] : buses_) {		
		all_stops.insert(stops.begin(), stops.end());		
	}
	for (auto stop : all_stops) {		
		svg::Circle stop_on_map;
		stop_on_map.SetCenter(stop.coordinate).
			SetRadius(render_setting_.stop_radius)
			.SetFillColor("white"s);

		map_route.Add(stop_on_map);
	}
	// Название остановки
	for (auto stop : all_stops) {
		svg::Text stop_background;
		stop_background.SetData(stop.name)
			.SetPosition(stop.coordinate)
			.SetOffset(render_setting_.stop_label_offset)
			.SetFontSize(render_setting_.stop_label_font_size)
			.SetFontFamily("Verdana"s);

		svg::Text stop_name = stop_background;
		stop_name.SetFillColor("black"s);

		stop_background.SetFillColor(render_setting_.underlayer_color)
			.SetStrokeColor(render_setting_.underlayer_color)
			.SetStrokeWidth(render_setting_.underlayer_width)
			.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
			.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

		map_route.Add(stop_background);
		map_route.Add(stop_name);
	}

}



