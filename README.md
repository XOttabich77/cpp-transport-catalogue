cpp-transport-catalogue
Транспортный справочник.
•	Принимает на вход запросы в формате JSON двух видов: для формирования базы данных и для обработки маршрутов и выдает ответ в виде SVG-файла, визуализирующего остановки и маршруты.
•	Находит кратчайший маршрут между остановками.
•	Для разделения вычислений выполнена сериализация базы данных базы по средством Google Protobuf.
Формат JSON для формирования базы данных:
Установки:
{
  "serialization_settings": {
    "file": "transport_catalogue.db"
  },
  "routing_settings": {
    "bus_wait_time": 2,
    "bus_velocity": 30
  },
  "render_settings": {
    "width": 1200,
    "height": 500,
    "padding": 50,
    "stop_radius": 5,
    "line_width": 14,
    "bus_label_font_size": 20,
    "bus_label_offset": [
      7,
      15
    ],
    "stop_label_font_size": 18,
    "stop_label_offset": [
      7,
      -3
    ],
    "underlayer_color": [
      255,
      255,
      255,
      0.85
    ],
    "underlayer_width": 3,
    "color_palette": [
      "green",
      [
        255,
        160,
        0
      ],
      "red"
    ]
  }
Описание остановок и маршрутов:
"base_requests": [
    {
      "type": "Bus",
      "name": "14",
      "stops": [
        "Улица Лизы Чайкиной",
        "Электросети",
        "Ривьерский мост",
        "Гостиница Сочи",
        "Кубанская улица",
        "По требованию",
        "Улица Докучаева",
        "Улица Лизы Чайкиной"
      ],
      "is_roundtrip": true
    },
    {
      "type": "Bus",
      "name": "24",
      "stops": [
        "Улица Докучаева",
        "Параллельная улица",
        "Электросети",
        "Санаторий Родина"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Bus",
      "name": "114",
      "stops": [
        "Морской вокзал",
        "Ривьерский мост"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Stop",
      "name": "Улица Лизы Чайкиной",
      "latitude": 43.590317,
      "longitude": 39.746833,
      "road_distances": {
        "Электросети": 4300,
        "Улица Докучаева": 2000
      }
    },
    {
      "type": "Stop",
      "name": "Морской вокзал",
      "latitude": 43.581969,
      "longitude": 39.719848,
      "road_distances": {
        "Ривьерский мост": 850
      }
    }
Запросы к базе данных:
 
  {
      "serialization_settings": {
          "file": "transport_catalogue.db"
      },
      "stat_requests": [
          {
              "id": 218563507,
              "type": "Bus",
              "name": "14"
          },
          {
              "id": 508658276,
              "type": "Stop",
              "name": "Электросети"
          },
          {
              "id": 1964680131,
              "type": "Route",
              "from": "Морской вокзал",
              "to": "Параллельная улица"
          },
          {
              "id": 1359372752,
              "type": "Map"
          }
      ]
  }
Вывод:
[
    {
        "curvature": 1.60481,
        "request_id": 218563507,
        "route_length": 11230,
        "stop_count": 8,
        "unique_stop_count": 7
    },
    {
        "buses": [
            "14",
            "24"
        ],
        "request_id": 508658276
    },
    {
        "items": [
            {
                "stop_name": "РњРѕСЂСЃРєРѕР№ РІРѕРєР·Р°Р»",
                "time": 2,
                "type": "Wait"
            },
            {
                "bus": "114",
                "span_count": 1,
                "time": 1.7,
                "type": "Bus"
            },
            {
                "stop_name": "Р РёРІСЊРµСЂСЃРєРёР№ РјРѕСЃС‚",
                "time": 2,
                "type": "Wait"
            },
            {
                "bus": "14",
                "span_count": 4,
                "time": 6.06,
                "type": "Bus"
            },
            {
                "stop_name": "РЈР»РёС†Р° Р”РѕРєСѓС‡Р°РµРІР°",
                "time": 2,
                "type": "Wait"
            },
            {
                "bus": "24",
                "span_count": 1,
                "time": 2.2,
                "type": "Bus"
            }
        ],
        "request_id": 1964680131,
        "total_time": 15.96
    },
    {
        "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n  <polyline points=\"125.25,382.708 74.2702,281.925 125.25,382.708\" fill=\"none\" stroke=\"green\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <polyline points=\"592.058,238.297 311.644,93.2643 74.2702,281.925 267.446,450 317.457,442.562 365.599,429.138 367.969,320.138 592.058,238.297\" fill=\"none\" stroke=\"rgb(255,160,0)\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <polyline points=\"367.969,320.138 350.791,243.072 311.644,93.2643 50,50 311.644,93.2643 350.791,243.072 367.969,320.138\" fill=\"none\" stroke=\"red\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"green\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"green\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">14</text>\n  <text fill=\"rgb(255,160,0)\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">14</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <text fill=\"red\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"50\" y=\"50\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <text fill=\"red\" x=\"50\" y=\"50\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <circle cx=\"267.446\" cy=\"450\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"317.457\" cy=\"442.562\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"125.25\" cy=\"382.708\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"350.791\" cy=\"243.072\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"365.599\" cy=\"429.138\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"74.2702\" cy=\"281.925\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"50\" cy=\"50\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"367.969\" cy=\"320.138\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"592.058\" cy=\"238.297\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"311.644\" cy=\"93.2643\" r=\"5\" fill=\"white\"/>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"267.446\" y=\"450\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Р“РѕСЃС‚РёРЅРёС†Р° РЎРѕС‡Рё</text>\n  <text fill=\"black\" x=\"267.446\" y=\"450\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Р“РѕСЃС‚РёРЅРёС†Р° РЎРѕС‡Рё</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"317.457\" y=\"442.562\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РљСѓР±Р°РЅСЃРєР°СЏ СѓР»РёС†Р°</text>\n  <text fill=\"black\" x=\"317.457\" y=\"442.562\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РљСѓР±Р°РЅСЃРєР°СЏ СѓР»РёС†Р°</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РњРѕСЂСЃРєРѕР№ РІРѕРєР·Р°Р»</text>\n  <text fill=\"black\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РњРѕСЂСЃРєРѕР№ РІРѕРєР·Р°Р»</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"350.791\" y=\"243.072\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РџР°СЂР°Р»Р»РµР»СЊРЅР°СЏ СѓР»РёС†Р°</text>\n  <text fill=\"black\" x=\"350.791\" y=\"243.072\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РџР°СЂР°Р»Р»РµР»СЊРЅР°СЏ СѓР»РёС†Р°</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"365.599\" y=\"429.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РџРѕ С‚СЂРµР±РѕРІР°РЅРёСЋ</text>\n  <text fill=\"black\" x=\"365.599\" y=\"429.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РџРѕ С‚СЂРµР±РѕРІР°РЅРёСЋ</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Р РёРІСЊРµСЂСЃРєРёР№ РјРѕСЃС‚</text>\n  <text fill=\"black\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Р РёРІСЊРµСЂСЃРєРёР№ РјРѕСЃС‚</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"50\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РЎР°РЅР°С‚РѕСЂРёР№ Р РѕРґРёРЅР°</text>\n  <text fill=\"black\" x=\"50\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РЎР°РЅР°С‚РѕСЂРёР№ Р РѕРґРёРЅР°</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РЈР»РёС†Р° Р”РѕРєСѓС‡Р°РµРІР°</text>\n  <text fill=\"black\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РЈР»РёС†Р° Р”РѕРєСѓС‡Р°РµРІР°</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РЈР»РёС†Р° Р›РёР·С‹ Р§Р°Р№РєРёРЅРѕР№</text>\n  <text fill=\"black\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">РЈР»РёС†Р° Р›РёР·С‹ Р§Р°Р№РєРёРЅРѕР№</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"311.644\" y=\"93.2643\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Р­Р»РµРєС‚СЂРѕСЃРµС‚Рё</text>\n  <text fill=\"black\" x=\"311.644\" y=\"93.2643\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Р­Р»РµРєС‚СЂРѕСЃРµС‚Рё</text>\n</svg>",
        "request_id": 1359372752
    }

Использованы технологии:
С++17
JSON
SVG
CMake
PROTOBUF




