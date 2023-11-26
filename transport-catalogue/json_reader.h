#pragma once
#include "json.h"
#include "map_renderer.h"
#include "router.h"
#include "transport_router.h"
#include "transport_catalogue.h"

namespace transport {
	namespace json_reader {

		class Json_Reader
		{
		public:
			void LoadJson(std::istream& input);

			const json::Array& GetBaseRequest() const { return base_requests_; }
			const json::Array& GetStatRequest() const { return stat_requests_; }
			const json::Dict& GetRenderSetting() const { return render_settings_; }
			const json::Dict& GetRoutingSetting() const { return routing_settings_; }
            const json::Dict& GetSerializationSetting()const {return serialization_settings_;}
            void SetRenderSetting(json::Dict render_settings){render_settings_= std::move(render_settings);}
            void SetRoutingSettings(json::Dict routing_settings){routing_settings_ = std::move(routing_settings);}

		private:
			json::Array base_requests_;
			json::Array stat_requests_;
			json::Dict  render_settings_;
            json::Dict routing_settings_;
            json::Dict serialization_settings_;
		};
		TransportCatalogue LoadBaseRequest(const Json_Reader& db);
        std::pair <router::Time, router::Speed> GetRoutSetting(const json::Dict& request);
        json::Array DoRequest(const transport::TransportCatalogue& catalog,
                              std::vector<graph::Edge<double>>& edges,
                              std::vector<std::vector<size_t>>& incidence_lists,
                              const Json_Reader&);

	} // end of namespase json_reader
} //end of namespace transport 
