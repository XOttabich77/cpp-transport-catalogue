#pragma once
#include "json.h"
#include "map_renderer.h"
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
		private:
			json::Array base_requests_;
			json::Array stat_requests_;
			json::Dict  render_settings_;
		};
		TransportCatalogue LoadBaseRequest(const Json_Reader& db);
		json::Array DoRequest(const transport::TransportCatalogue& catalog, const Json_Reader&);

	} // end of namespase json_reader
} //end of namespace transport 